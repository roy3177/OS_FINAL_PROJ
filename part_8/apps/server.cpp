#include "server.hpp"

using std::string;
using std::vector;
using std::unordered_map;

// IO helpers
bool recv_all_lines(int fd, std::string &out)
{
    char buf[1024];
    out.clear();
    while (true)
    {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0) 
        {
            out.append(buf, buf + n);
            if (out.find("\nEND\n") != string::npos || out.rfind("\nEND") == out.size() - 4)
            {
                return true; // Handle END case
            } 
            if (out.find("\nEXIT\n") != string::npos || out == "EXIT\n")
            {
                return true; // Handle EXIT case
            }
        }
        else if (n == 0) 
        {
            return !out.empty();
        }
        else 
        {
            if (errno == EINTR)
            {
                continue;
            }
            return false;
        }
    }
}

void send_response(int fd, const std::string &body, bool ok)
{
    std::ostringstream oss; oss << (ok?"OK\n":"ERR\n") << body << "\nEND\n"; auto s = oss.str();
    (void)send(fd, s.c_str(), s.size(), 0);
}

// Leader–Follower state
namespace 
{
std::mutex g_mu;
std::condition_variable g_cv;
bool g_hasLeader = false;
bool g_shutdown = false;
}

void lf_server_loop(int srv_fd, int workers)
{
    auto worker = [srv_fd]() 
    {
        while (true) 
        {
            // Become leader
            std::unique_lock<std::mutex> lk(g_mu);
            g_cv.wait(lk, []
            {
                return !g_hasLeader || g_shutdown;
            });
            if (g_shutdown)
            {
               return; 
            } 
            g_hasLeader = true; // I'm the leader now
            lk.unlock();

            // Accept
            sockaddr_in cli{};
            socklen_t clilen = sizeof(cli);
            int fd = accept(srv_fd, (sockaddr*)&cli, &clilen);

            // Promote follower immediately
            lk.lock();
            g_hasLeader = false;
            lk.unlock();
            g_cv.notify_one();

            if (fd < 0) 
            {
                if (errno == EINTR)
                {
                continue;
                }
                std::perror("accept");
                return;
            }

            // Log client connect
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &cli.sin_addr, ipstr, sizeof(ipstr));
            std::cout << "[LF] Client connected from " << ipstr << ":" << ntohs(cli.sin_port) << "\n";

            handle_client(fd);

            // Log client disconnect
            std::cout << "[LF] Client disconnected" << "\n";
        }
    };

    std::vector<std::thread> pool;
    pool.reserve(workers);
    for (int i=0;i<workers;i++)
    {
        pool.emplace_back(worker);  
    } 

    // Kick the first leader
    {
        std::lock_guard<std::mutex> lk(g_mu); g_hasLeader=false;
    }
    g_cv.notify_one();

    for (auto &t: pool) t.join();
}

static string trim_cr(string s)
{
    if(!s.empty() && s.back()=='\r')
    {
      s.pop_back();
    }
    return s;
}

static string run_alg_or_error(const string& alg, const Graph& g, const unordered_map<string,int>& params, bool requestedDirected)
{
    // directed-required algorithms
    bool isDirectedAlg = (alg=="MAX_FLOW" || alg=="SCC");
    bool okForThisGraph = (requestedDirected && isDirectedAlg) || (!requestedDirected && !isDirectedAlg);
    if (!okForThisGraph) 
    {
        std::ostringstream er;
        er << "Error: cannot run " << alg << " on " << (requestedDirected?"directed":"undirected") << " graph";
        return er.str();
    }
    auto ptr = AlgorithmFactory::create(alg);
    if (!ptr)
    {
        return "Unsupported algorithm";
    } 
    return ptr->run(g, params);
}

static std::string serialize_graph_edges(const Graph& g, bool directed)
{
    const auto& cap = g.get_capacity();
    std::ostringstream out;
    int V = g.get_vertices();
    int count = 0;
    // First count edges for header
    if (directed) 
    {
        for (int u=0; u<V; ++u)
        {
            for (int v=0; v<V; ++v)
            {
                if (cap[u][v] > 0)
                {
                    ++count;
                }
            }
        } 
    }
    else 
    {
        for (int u=0; u<V; ++u)
        {
            for (int v=u+1; v<V; ++v)
            {
                if (cap[u][v] > 0 || cap[v][u] > 0)
                {
                    ++count;
                }
            }
        } 
    }
    out << "GRAPH " << V << " " << count << "\n";
    if (directed) 
    {
        for (int u=0; u<V; ++u)
        {
           for (int v=0; v<V; ++v)
           {
               if (cap[u][v] > 0) out << "EDGE " << u << " " << v << " " << cap[u][v] << "\n";
           }
        } 
    }
    else 
    {
        for (int u=0; u<V; ++u) for (int v=u+1; v<V; ++v) 
        {
            int w = cap[u][v] > 0 ? cap[u][v] : cap[v][u];
            if (w > 0) out << "EDGE " << u << " " << v << " " << w << "\n";
        }
    }
    return out.str();
}

void handle_client(int fd)
{
    while (true)
    {
        std::string req;
        if (!recv_all_lines(fd, req))
        {
            close(fd);
            return; 
        }
        if (req == "EXIT\n" || req.find("\nEXIT\n") != string::npos) 
        {
            send_response(fd, "BYE", true);
            close(fd);
            return; 
        }

        // Parse request
        std::istringstream iss(req);
        string line;
        string alg;
        int V=-1;
        int E=0; 
        int directed=0; 
        int randomFlag=0; 
        int seed=42; 
        int src=-1,sink=-1,k=-1; 
        int wmin=1,wmax=1;
        struct Edge
        {
            int u,v,w;
        };
        vector<Edge> edges;
        bool parse_error=false;
        string perr;
        while (std::getline(iss,line)) 
        {
            line = trim_cr(line);
            if (line=="END")
            {
               break; 
            } 
            if (line.rfind("ALG ",0)==0)
            {
                alg=line.substr(4); 
            }
            else if (line.rfind("RANDOM ",0)==0) 
            {
                std::istringstream ls(line);
                string t;
                ls>>t>>randomFlag; 
            }
            else if (line.rfind("DIRECTED ",0)==0) 
            {
                std::istringstream ls(line);
                string t;
                ls>>t>>directed; 
            }
            else if (line.rfind("V ",0)==0) 
            {
                std::istringstream ls(line);
                char t;
                ls>>t>>V; 
            }
            else if (line.rfind("E ",0)==0) 
            {
                std::istringstream ls(line);
                char t;
                ls>>t>>E; 
            }
            else if (line.rfind("SEED ",0)==0) 
            {
                std::istringstream ls(line);
                string t;
                ls>>t>>seed; 
            }
            else if (line.rfind("EDGE ",0)==0) 
            {
                std::istringstream ls(line);
                string t;
                int u,v,w=1;
                ls>>t>>u>>v;
                if (ls>>w){}
                edges.push_back({u,v,w});
            }
            else if (line.rfind("WMIN ",0)==0) 
            {
                std::istringstream ls(line);
                string t; ls>>t>>wmin;
            }
            else if (line.rfind("WMAX ",0)==0) 
            {
                std::istringstream ls(line);
                string t;
                ls>>t>>wmax; 
            }
            else if (line.rfind("PARAM ",0)==0) 
            {
                std::istringstream ls(line);
                string t,kstr; int val; ls>>t>>kstr>>val;
                if(kstr=="SRC")src=val;
                else if(kstr=="SINK")sink=val;
                else if(kstr=="K")k=val;
            }
            else if (line.empty()) 
            {
                continue; 
            }
            else 
            {
                parse_error=true;
                perr = string("Unknown directive: ")+line;
                break;
            }
        }
    if (parse_error) 
    {
        send_response(fd, perr, false);
        continue; 
    }
    if (V<=0) 
    {
        send_response(fd, "Missing/invalid V", false);
        continue; 
    }
    if (randomFlag && (E<0)) 
    {
        send_response(fd, "Missing/invalid E", false);
        continue; 
    }

        Graph g(V, directed!=0);
        if (!randomFlag) 
        {
            for (auto &e: edges)
            {
                g.addEdge(e.u,e.v,e.w);
            }
        }
        else 
        {
            // clamp E to max possible
            int maxE = directed? V*(V-1) : V*(V-1)/2;
            if (E>maxE)
            {
              E=maxE;  
            } 
            if (E<0)
            {
              E=0;  
            } 
            if (wmax < wmin)
            {
               std::swap(wmax, wmin); 
            } 
            g = generate_random_graph(V, E, seed, directed!=0, wmin, wmax);
        }

        unordered_map<string,int> params;
        if(src>=0)
        {
            params["SRC"]=src;
        }
        if(sink>=0)
        {
            params["SINK"]=sink;
        }
        if(k>=0)
        {
            params["K"]=k;
        }

        if (alg=="PREVIEW") 
        {
            // Just return the generated/parsed graph as an edge list
            auto body = serialize_graph_edges(g, directed!=0);
            send_response(fd, body, true);
        }
        else if (alg=="ALL") 
        {
            // Build the entire body and send once (simpler and robust)
            std::ostringstream body;
            string r1 = run_alg_or_error("MAX_FLOW", g, params, directed!=0);
            string r2 = run_alg_or_error("SCC", g, params, directed!=0);
            string r3 = run_alg_or_error("MST", g, params, directed!=0);
            string r4 = run_alg_or_error("CLIQUES", g, params, directed!=0);
            body << "RESULT MAX_FLOW=" << r1 << "\n";
            body << "RESULT SCC_COUNT=" << r2 << "\n";
            body << "RESULT MST_WEIGHT=" << r3 << "\n";
            body << "RESULT CLIQUES=" << r4 << "\n";
            send_response(fd, body.str(), true);
        }
        else 
        {
            auto out = run_alg_or_error(alg, g, params, directed!=0);
            send_response(fd, out, true);
        }
    }
}

int run_server(int argc, char *argv[])
{
    int port = PORT;
    if (argc>=2) 
    {
        int p=std::atoi(argv[1]);
        if(p>0)
        {
            port=p;
        }
    }
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv<0)
    {
        std::perror("socket");
        return 1;
    }
    int opt=1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(port);
    if (bind(srv, (sockaddr*)&addr, sizeof(addr))<0)
    {
        std::perror("bind");
        close(srv);
        return 1; 
    }
    if (listen(srv, 64)<0)
    {
        std::perror("listen");
        close(srv);
        return 1;
    }
    std::cout<<"[LF] Server listening on port "<<port<<" with Leader–Follower pool...\n";

    // Start LF loop with N workers
    int workers = std::max(4u, std::thread::hardware_concurrency());
    lf_server_loop(srv, workers);
    close(srv);
    return 0;
}

int main(int argc, char* argv[])
{
    return run_server(argc, argv);
}

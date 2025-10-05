#include "server.hpp"

int g_listen_fd = -1;

using std::string;
using std::vector;
using std::unordered_map;

// Helper function for receiving all lines from a socket
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

// Helper function for sending a response to a client
void send_response(int fd, const std::string &body, bool ok)
{
    std::ostringstream oss;
    oss << (ok ? "OK\n" : "ERR\n") << body << "\nEND\n";
    auto s = oss.str();
    (void)send(fd, s.c_str(), s.size(), 0);
}

// Leader–Follower state:
namespace 
{
    std::mutex g_mu;
    std::condition_variable g_cv;
    bool g_hasLeader = false;
    bool g_shutdown = false;
}


void lf_server_loop(int srv_fd, int workers)
{
    /*
    Worker routine run by each thread in the pool. Implements the Leader–Follower pattern:
    - Exactly one thread at a time becomes the "leader" and blocks in accept().
    - After accept() returns, the leader immediately promotes a follower (so another thread
      goes to accept the next connection) and then the former leader handles the connection.
    */
    auto worker = [srv_fd]() 
    {
        while (true) 
        {
            // Leader election: wait until there is no current leader (or shutdown requested).
            std::unique_lock<std::mutex> lk(g_mu);
            g_cv.wait(lk, []
            {
                return !g_hasLeader || g_shutdown; // wake when leadership is free or shutdown
            });

            // Graceful exit path (not currently toggled elsewhere).
            if (g_shutdown)
            {
               return; 
            } 

            // Become the leader: from now until we finish accept(), we are the only thread doing it.
            g_hasLeader = true; // I'm the leader now
            lk.unlock();        // Don't hold the mutex while blocking in accept().

            // Accept a client connection (this may block until a client arrives).
            sockaddr_in cli{};
            socklen_t clilen = sizeof(cli);
            int fd = accept(srv_fd, (sockaddr*)&cli, &clilen);

            // Immediately promote a follower so another thread can accept the next client.
            lk.lock();
            g_hasLeader = false;   // leadership is free
            lk.unlock();
            g_cv.notify_one();     // wake one waiting follower to become the next leader

            // Handle accept errors.
            if (fd < 0) 
            {
                if (errno == EINTR)
                {
                    // Interrupted by a signal; loop and try again.
                    continue;
                }
                if(g_shutdown){
                    return; // Graceful exit on shutdown request.
                }
                // Hard error: log and end this worker thread (others keep running).
                std::perror("accept");
                return;
            }

            // Log client connect information.
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &cli.sin_addr, ipstr, sizeof(ipstr));
            std::cout << "[LF] Client connected from " << ipstr << ":" << ntohs(cli.sin_port) << "\n";

            // Process the connection on this (former leader) thread.
            handle_client(fd);

            // After the client is handled, loop back and compete for leadership again.
            std::cout << "[LF] Client disconnected" << "\n";
        }
    };

    // Start the fixed-size thread pool.
    std::vector<std::thread> pool;
    pool.reserve(workers);
    for (int i=0;i<workers;i++)
    {
        pool.emplace_back(worker);  
    } 

    // Kick off the first leader by ensuring no leader is marked and notifying one waiter.
    {
        std::lock_guard<std::mutex> lk(g_mu);
        g_hasLeader = false;
    }
    g_cv.notify_one();

    // Join threads (blocks forever in normal operation, unless g_shutdown causes exits or
    // a worker returns due to a persistent accept() error).
    for (auto &t: pool)
    {
        t.join();
    }
}

// Helper function for trimming carriage return characters from a string
static string trim_cr(string s)
{
    if(!s.empty() && s.back()=='\r')
    {
      s.pop_back();
    }
    return s;
}

// Helper function for running an algorithm and handling errors
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

// Helper function for serializing graph edges
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
    // Persistent per-connection loop: handle multiple requests on the same TCP connection
    // until the client sends EXIT or the socket closes.
    while (true)
    {
        // 1) Read a whole request into 'req' (terminated by an END line or EXIT)
        std::string req;
        if (!recv_all_lines(fd, req)) // Check for errors or connection closure
        {
            // Peer closed or error while reading: close and end this connection handler.
            close(fd);
            return; 
        }

        //2)General logout of the server:
        if (req.find("SHUTDOWN") != std::string::npos) {
            send_response(fd, "BYE", true);
            close(fd);

            {   //Mark that the lock is in shutdown mode:
                std::lock_guard<std::mutex> lk(g_mu);
                g_shutdown = true;
            }
    
            if (g_listen_fd >= 0) {
                shutdown(g_listen_fd, SHUT_RDWR); 
            }

            g_cv.notify_all(); 
            return;            
        }


        // 3) Immediate shutdown command from client
        if (req == "EXIT\n" || req.find("\nEXIT\n") != string::npos) 
        {
            send_response(fd, "BYE", true);
            close(fd);
            return; 
        }

        // 3) Parse the line-based protocol into local variables
        std::istringstream iss(req);
        string line;
        string alg;                 // which action to perform (e.g., PREVIEW, ALL, MAX_FLOW, ...)
        int V=-1;                   // number of vertices (required)
        int E=0;                    // number of edges for random graph generation
        int directed=0;             // 0=undirected, 1=directed
        int randomFlag=0;           // 0=use provided EDGE lines, 1=generate random graph
        int seed=42;                // seed for deterministic random graph
        int src=-1,sink=-1,k=-1;    // optional algorithm parameters
        int wmin=1,wmax=1;          // weight range for random graph
        struct Edge
        {
            int u,v,w;             // explicit edges when RANDOM=0
        };
        vector<Edge> edges;
        bool parse_error=false;
        string perr;

        // Parse each line into the appropriate variable/collection
        while (std::getline(iss,line)) 
        {
            line = trim_cr(line);   // tolerate Windows CRLF by trimming trailing '\r'
            if (line=="END")
            {
               break;               // end of this request
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
                // EDGE u v [w] — optional weight (default 1)
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
                // PARAM SRC|SINK|K value
                std::istringstream ls(line);
                string t,kstr; int val; ls>>t>>kstr>>val;
                if(kstr=="SRC")src=val;
                else if(kstr=="SINK")sink=val;
                else if(kstr=="K")k=val;
            }
            else if (line.empty()) 
            {
                continue;           // ignore blank lines
            }
            else 
            {
                // Unknown directive — remember error and stop parsing this request
                parse_error=true;
                perr = string("Unknown directive: ")+line;
                break;
            }
        }

        // 4) Basic validation of parsed values
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

        // 5–7) Build + params + dispatch, with validation and exception safety
        try
        {
            // 5) Build the Graph according to the request (explicit edges or generated random)
            Graph g(V, directed!=0);

            if (!randomFlag)
            {
                // Validate all explicit edges BEFORE addEdge to avoid abort/throw inside Graph
                bool bad = false;
                for (const auto &e : edges)
                {
                    if (e.u < 0 || e.v < 0 || e.u >= V || e.v >= V)
                    {
                        send_response(fd, "Invalid EDGE vertex (out of range)", false);
                        bad = true;
                        break;
                    }
                    if (e.w <= 0)
                    {
                        send_response(fd, "Invalid EDGE weight (must be >=1)", false);
                        bad = true;
                        break;
                    }
                }
                if (bad)
                {
                    // Skip this request; go wait for the next one on the same connection
                    continue;
                }

                // Now it's safe to add
                for (const auto &e : edges)
                {
                    g.addEdge(e.u, e.v, e.w);
                }
            }
            else
            {
                // RANDOM=1: clamp E to a feasible range, normalize weight range, then generate.
                int maxE = directed ? V*(V-1) : V*(V-1)/2; // maximum simple edges possible
                if (E > maxE) { E = maxE; }
                if (E < 0)     { E = 0;    }
                if (wmax < wmin) { std::swap(wmax, wmin); }
                g = generate_random_graph(V, E, seed, directed!=0, wmin, wmax);
            }

            // 6) Prepare algorithm parameters map (only include provided keys)
            unordered_map<string,int> params;
            if (src  >= 0) { params["SRC"]  = src;  }
            if (sink >= 0) { params["SINK"] = sink; }
            if (k    >= 0) { params["K"]    = k;    }

            // 7) Dispatch by ALG and send a response
            if (alg == "PREVIEW")
            {
                // Return the graph as text edge list so the client can preview it.
                auto body = serialize_graph_edges(g, directed!=0);
                send_response(fd, body, true);
            }
            else if (alg == "ALL")
            {
                // Compute all algorithms sequentially and send one consolidated OK block.
                std::ostringstream body;
                string r1 = run_alg_or_error("MAX_FLOW",  g, params, directed!=0);
                string r2 = run_alg_or_error("SCC",       g, params, directed!=0);
                string r3 = run_alg_or_error("MST",       g, params, directed!=0);
                string r4 = run_alg_or_error("CLIQUES",   g, params, directed!=0);
                body << "RESULT MAX_FLOW="   << r1 << "\n";
                body << "RESULT SCC_COUNT="  << r2 << "\n";
                body << "RESULT MST_WEIGHT=" << r3 << "\n";
                body << "RESULT CLIQUES="    << r4 << "\n";
                send_response(fd, body.str(), true);
            }
            else
            {
                // Single-algorithm request
                auto out = run_alg_or_error(alg, g, params, directed!=0);
                send_response(fd, out, true);
            }
        }
        catch (const std::exception& ex)
        {
            // Never crash the server thread because of bad input/logic
            std::ostringstream er; er << "Exception: " << ex.what();
            send_response(fd, er.str(), false);
        }
        catch (...)
        {
            send_response(fd, "Exception: unknown", false);
        }
    }
}

int run_server(int argc, char *argv[])
{
    // 1) Determine listening port: default PORT, allow override via argv[1]
    int port = PORT;
    if (argc>=2) 
    {
        int p=std::atoi(argv[1]);
        if(p>0)
        {
            port=p;
        }
    }

    // 2) Create a TCP socket (IPv4, stream)
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv<0)
    {
        std::perror("socket");
        return 1;
    }

    // 3) Allow quick rebinding after restarts (SO_REUSEADDR)
    int opt=1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 4) Bind to 0.0.0.0:port (all local interfaces)
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

    // 5) Start listening with a backlog of 64 pending connections
    if (listen(srv, 64)<0)
    {
        std::perror("listen");
        close(srv);
        return 1;
    }
    g_listen_fd = srv;
    std::cout<<"[LF] Server listening on port "<<port<<" with Leader–Follower pool...\n";

    // 6) Start the Leader–Follower loop with N worker threads
    //    Use hardware_concurrency() when available, but at least 4 threads.
    int workers = std::max(4u, std::thread::hardware_concurrency());
    lf_server_loop(srv, workers);

    // 7) Cleanup socket after the LF loop exits (on shutdown or fatal error)
    close(srv);
    return 0;
}

int main(int argc, char* argv[])
{
    return run_server(argc, argv);
}


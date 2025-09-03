#include "server.hpp"


// Forward declarations for helpers used in pipeline stages
static std::string run_alg_or_error(const std::string& alg, const Graph& g,
                                    const std::unordered_map<std::string,int>& params,
                                    bool requestedDirected);
static std::string serialize_graph_edges(const Graph& g, bool directed);

using std::string;
using std::vector;
using std::unordered_map;

// -------------------- Pipeline implementation --------------------
namespace {
    // Queues between stages
    BlockingQueue<Job> q_max_flow;
    BlockingQueue<Job> q_scc;
    BlockingQueue<Job> q_mst;
    BlockingQueue<Job> q_cliques;
    BlockingQueue<Job> q_agg;

    // Stage loops
    void stage_max_flow_loop()
    {
        Job job;
        while (q_max_flow.pop(job))
        {
            job.res_max_flow = run_alg_or_error("MAX_FLOW", job.graph, job.params, job.directed);
            // Route next
            if (job.kind == AlgKind::SINGLE_MAX_FLOW) q_agg.push(std::move(job));
            else q_scc.push(std::move(job));
        }
    }

    void stage_scc_loop()
    {
        Job job;
        while (q_scc.pop(job))
        {
            job.res_scc = run_alg_or_error("SCC", job.graph, job.params, job.directed);
            if (job.kind == AlgKind::SINGLE_SCC) q_agg.push(std::move(job));
            else q_mst.push(std::move(job));
        }
    }

    void stage_mst_loop()
    {
        Job job;
        while (q_mst.pop(job))
        {
            job.res_mst = run_alg_or_error("MST", job.graph, job.params, job.directed);
            if (job.kind == AlgKind::SINGLE_MST) q_agg.push(std::move(job));
            else q_cliques.push(std::move(job));
        }
    }

    void stage_cliques_loop()
    {
        Job job;
        while (q_cliques.pop(job))
        {
            job.res_cliques = run_alg_or_error("CLIQUES", job.graph, job.params, job.directed);
            // Last stage for ALL and SINGLE_CLIQUES
            q_agg.push(std::move(job));
        }
    }

    void stage_aggregator_loop()
    {
        Job job;
        while (q_agg.pop(job))
        {
            if (job.kind == AlgKind::PREVIEW)
            {
                auto body = serialize_graph_edges(job.graph, job.directed);
                send_response(job.fd, body, true);
                continue;
            }

            if (job.kind == AlgKind::SINGLE_MAX_FLOW)
            {
                send_response(job.fd, job.res_max_flow, true);
                continue;
            }
            if (job.kind == AlgKind::SINGLE_SCC)
            {
                send_response(job.fd, job.res_scc, true);
                continue;
            }
            if (job.kind == AlgKind::SINGLE_MST)
            {
                send_response(job.fd, job.res_mst, true);
                continue;
            }
            if (job.kind == AlgKind::SINGLE_CLIQUES)
            {
                send_response(job.fd, job.res_cliques, true);
                continue;
            }

            // ALL
            std::ostringstream body;
            body << "RESULT MAX_FLOW=" << job.res_max_flow << "\n";
            body << "RESULT SCC_COUNT=" << job.res_scc << "\n";
            body << "RESULT MST_WEIGHT=" << job.res_mst << "\n";
            body << "RESULT CLIQUES=" << job.res_cliques << "\n";
            send_response(job.fd, body.str(), true);
        }
    }

    // Threads
    std::atomic<bool> pipeline_started{false};
}

void start_pipeline()
{
    bool expected = false;
    if (!pipeline_started.compare_exchange_strong(expected, true))
        return; // already started

    std::thread(stage_max_flow_loop).detach();
    std::thread(stage_scc_loop).detach();
    std::thread(stage_mst_loop).detach();
    std::thread(stage_cliques_loop).detach();
    std::thread(stage_aggregator_loop).detach();
}

void stop_pipeline()
{
    // Gracefully close queues to let threads exit if no more producers.
    q_max_flow.close();
    q_scc.close();
    q_mst.close();
    q_cliques.close();
    q_agg.close();
}

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

        // 2) Immediate shutdown command from client
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

        // 5) Build the Graph according to the request (explicit edges or generated random)
        Graph g(V, directed!=0);
        if (!randomFlag) 
        {
            // From explicit EDGE lines
            for (auto &e: edges)
            {
                g.addEdge(e.u,e.v,e.w);
            }
        }
        else 
        {
            // RANDOM=1: clamp E to a feasible range, normalize weight range, then generate.
            int maxE = directed? V*(V-1) : V*(V-1)/2; // maximum simple edges possible
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

        // 6) Prepare algorithm parameters map (only include provided keys)
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

        // 7) Map ALG to pipeline kind and enqueue job
        Job job;
        job.fd = fd;
        job.graph = std::move(g);
        job.params = std::move(params);
        job.directed = (directed!=0);

        if (alg == "PREVIEW") job.kind = AlgKind::PREVIEW;
        else if (alg == "ALL") job.kind = AlgKind::ALL;
        else if (alg == "MAX_FLOW") job.kind = AlgKind::SINGLE_MAX_FLOW;
        else if (alg == "SCC") job.kind = AlgKind::SINGLE_SCC;
        else if (alg == "MST") job.kind = AlgKind::SINGLE_MST;
        else if (alg == "CLIQUES") job.kind = AlgKind::SINGLE_CLIQUES;
        else {
            send_response(fd, "Unsupported algorithm", false);
            continue;
        }

        // Enqueue to appropriate entry queue
        if (job.kind == AlgKind::PREVIEW) {
            q_agg.push(std::move(job)); // aggregator will serialize and send
        } else if (job.kind == AlgKind::SINGLE_MAX_FLOW || job.kind == AlgKind::ALL) {
            q_max_flow.push(std::move(job));
        } else if (job.kind == AlgKind::SINGLE_SCC) {
            q_scc.push(std::move(job));
        } else if (job.kind == AlgKind::SINGLE_MST) {
            q_mst.push(std::move(job));
        } else if (job.kind == AlgKind::SINGLE_CLIQUES) {
            q_cliques.push(std::move(job));
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
    std::cout<<"[LF] Server listening on port "<<port<<" with Leader–Follower pool...\n";

    // Start pipeline threads once
    start_pipeline();

    // 6) Start the Leader–Follower loop with N worker threads
    //    Use hardware_concurrency() when available, but at least 4 threads.
    int workers = std::max(4u, std::thread::hardware_concurrency());
    lf_server_loop(srv, workers);

    // 7) Cleanup socket after the LF loop exits (on shutdown or fatal error)
    stop_pipeline();
    close(srv);
    return 0;
}

int main(int argc, char* argv[])
{
    return run_server(argc, argv);
}

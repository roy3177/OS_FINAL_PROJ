#include "client.hpp"

// Helper function for printing a separator line
void println_rule() { std::cout << "----------------------------------------\n"; }

// Helper function for prompting an integer input:
int prompt_int(const std::string& msg, int minVal, int maxVal) 
{
    while (true) {
        std::cout << msg;
        std::string s; std::getline(std::cin, s);
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        if (s.empty()) continue;
        bool ok = std::all_of(s.begin(), s.end(), ::isdigit);
        if (!ok) { std::cout << "Invalid number\n"; continue; }
        int v = std::stoi(s);
        if (v < minVal || v > maxVal) { std::cout << "Out of range\n"; continue; }
        return v;
    }
}


int run_client(int argc, char* argv[])
{
    int port = PORT; // Reads port from argv (default 9090).
    /*
    * argc >= 2 means a user passed at least one argument

    * std::atoi(argv[1]) parses that argument as an int and assigns it to port,
      overriding the initial PORT macro value.
    */
    if (argc >= 2)
    {
        port = std::atoi(argv[1]);
    } 
    std::cout << "Welcome to Graph Algorithms Client\n";
    // Open one connection and reuse it until the user exits
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return 1; 
    }
    sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr) <= 0)
    {
        perror("inet_pton"); close(sock); return 1; 
    }
    if (connect(sock, (sockaddr*)&sa, sizeof(sa)) < 0)
    {
        perror("connect"); close(sock); return 1;
    }

    while (true) 
    {
        println_rule();

        // Numeric menu for algorithm selection
        int sel = prompt_int(
            "Choose algorithm:\n"
            "  1) MAX_FLOW\n"
            "  2) SCC\n"
            "  3) CLIQUES\n"
            "  4) MST\n"
            "  0) Exit\n> ", 0, 4);
    if (sel == 0) 
    {
        const char* bye = "EXIT\n";
        send(sock, bye, std::strlen(bye), 0);
        close(sock);
        break; 
    }
        std::string alg;
        switch (sel) 
        {
            case 1: alg = "MAX_FLOW"; break;
            case 2: alg = "SCC"; break;
            case 3: alg = "CLIQUES"; break;
            case 4: alg = "MST"; break;
            default: std::cout << "Unknown selection.\n"; continue;
        }

        bool directed = (alg == "MAX_FLOW" || alg == "SCC");

        int V = prompt_int("Enter number of vertices (>=2): ", 2, 1000000);
        int max_edges = directed ? V * (V - 1) : V * (V - 1) / 2;
        int E = prompt_int("Enter number of edges (0.." + std::to_string(max_edges) + "): ", 0, max_edges);

        std::set<std::pair<int,int>> undup;
        std::vector<std::tuple<int,int,int>> edges;
        std::cout << "Enter edges as: u v w. For unweighted, omit w.\n";
        for (int i = 0; i < E; ++i) 
        {
            while (true) 
            {
                std::cout << "Edge " << (i+1) << ": ";
                std::string line; std::getline(std::cin, line);
                if (line == "exit") 
                {
                    const char* bye = "EXIT\n";
                    send(sock, bye, std::strlen(bye), 0);
                    close(sock); return 0; 
                }
                std::istringstream ls(line);
                int u,v,w=1;
                if (!(ls>>u>>v)) 
                {
                    std::cout<<"Bad format\n";
                    continue;
                }
                if (u<0||v<0||u>=V||v>=V||u==v) 
                {
                    std::cout<<"Invalid vertices\n";
                    continue;
                }
                if (ls>>w) 
                {
                    if (w<=0) 
                    {
                        std::cout<<"Weight must be >0\n";
                        continue;
                    }
                }
                if (!directed) 
                {
                    auto a = std::minmax(u,v);
                    if (undup.count({a.first,a.second})) 
                    {
                        std::cout<<"Duplicate edge\n";
                        continue; 
                    }
                    undup.insert({a.first,a.second});
                }
                edges.emplace_back(u,v,w);
                break;
            }
        }

        int SRC=-1,SINK=-1,K=-1;
        if (alg == "MAX_FLOW") 
        {
            SRC = prompt_int("SRC: ", 0, V-1);
            do 
            {
                SINK = prompt_int("SINK (must be different from SRC): ", 0, V-1);
                if (SINK == SRC)
                {
                    std::cout << "SINK must be different from SRC\n";
                } 
            }
            while (SINK == SRC);
        } 
        else if (alg == "CLIQUES") 
        {
            K = prompt_int("K (>=2): ", 2, V);
        }

        std::ostringstream req;
        req << "ALG " << alg << " DIRECTED " << (directed?1:0) << "\n";
        req << "V " << V << "\n";
        req << "E " << E << "\n";
        for (auto &t : edges) 
        {
            int u,v,w;
            std::tie(u,v,w)=t; // Creates a tuple of references to those variables.
            req << "EDGE " << u << " " << v << " " << w << "\n";
        }
        if (SRC>=0) req << "PARAM SRC " << SRC << "\n";
        if (SINK>=0) req << "PARAM SINK " << SINK << "\n";
        if (K>=0) req << "PARAM K " << K << "\n";
        req << "END\n";

        std::string s = req.str();
        send(sock, s.c_str(), s.size(), 0);

        char buf[4096]; int n = recv(sock, buf, sizeof(buf), 0);
        if (n>0) 
        {
            println_rule();
            std::cout << std::string(buf, n) << std::endl;
        }

    // Another round?
    std::cout << "Type 'exit' to quit or press Enter to run another.\n";
    std::string again; std::getline(std::cin, again);
    if (again == "exit") 
    {
        const char* bye = "EXIT\n";
        send(sock, bye, std::strlen(bye), 0);
        close(sock); break; 
    }
}
    std::cout << "Bye.\n";
    return 0;
}

int main(int argc, char* argv[])
{
    return run_client(argc, argv);
}

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <limits>
#include <algorithm>
#include <set>

#define PORT 8080

int main() {
    
    struct sockaddr_in serv_addr;
    char buffer[4096] = {0};

    while (true) {
        int V;
        while (true) {
            std::cout << "Enter number of vertices (positive integer): ";
            std::string v_input;
            std::getline(std::cin, v_input);
            v_input.erase(0, v_input.find_first_not_of(" \t\n\r"));
            v_input.erase(v_input.find_last_not_of(" \t\n\r") + 1);
            bool valid_int = !v_input.empty() && std::all_of(v_input.begin(), v_input.end(), ::isdigit);
            if (!valid_int) {
                std::cout << "Error: Please enter a single positive integer.\n";
                continue;
            }
            V = std::stoi(v_input);
            if (V <= 1) {
                std::cout << "Error: Number of vertices must be at least 2.\n";
            } else {
                break;
            }
        }

        int max_edges = V * (V - 1) / 2;
        int E;
        while (true) {
            std::cout << "Enter number of edges (non-negative integer, max " << max_edges << "): ";
            std::string e_input;
            std::getline(std::cin, e_input);
            e_input.erase(0, e_input.find_first_not_of(" \t\n\r"));
            e_input.erase(e_input.find_last_not_of(" \t\n\r") + 1);
            bool valid_int = !e_input.empty() && std::all_of(e_input.begin(), e_input.end(), ::isdigit);
            if (!valid_int) {
                std::cout << "Error: Please enter a single non-negative integer.\n";
                continue;
            }
            E = std::stoi(e_input);
            if (E < 0 || E > max_edges) {
                std::cout << "Error: Number of edges must be between 0 and " << max_edges << ".\n";
            } else {
                break;
            }
        }

        std::string graph_data = std::to_string(V) + " " + std::to_string(E) + "\n";
        std::cout << "Enter each edge as: u v (or type 'exit' to quit)\n";
        std::set<std::pair<int, int>> edge_set;
        for (int i = 0; i < E; ++i) {
            int u, v;
            while (true) {
                std::cout << "Edge " << i+1 << ": ";
                std::string edge_line;
                std::getline(std::cin, edge_line);
                edge_line.erase(0, edge_line.find_first_not_of(" \t\n\r"));
                edge_line.erase(edge_line.find_last_not_of(" \t\n\r") + 1);
                if (edge_line == "exit") return 0;
                size_t space_pos = edge_line.find(' ');
                if (space_pos == std::string::npos) {
                    std::cout << "Error: Please enter two integers separated by a space.\n";
                    continue;
                }
                std::string u_input = edge_line.substr(0, space_pos);
                std::string v_input = edge_line.substr(space_pos + 1);
                v_input.erase(0, v_input.find_first_not_of(" \t\n\r"));
                v_input.erase(v_input.find_last_not_of(" \t\n\r") + 1);
                if (u_input.empty() || v_input.empty() ||
                    !std::all_of(u_input.begin(), u_input.end(), ::isdigit) ||
                    !std::all_of(v_input.begin(), v_input.end(), ::isdigit)) {
                    std::cout << "Error: Vertices must be integers with no extra characters.\n";
                    continue;
                }
                try {
                    u = std::stoi(u_input);
                    v = std::stoi(v_input);
                } catch (...) {
                    std::cout << "Error: Vertices must be integers.\n";
                    continue;
                }
                if (u == v) {
                    std::cout << "Error: Self-loops are not allowed (u must not equal v).\n";
                } else if (u < 0 || v < 0 || u >= V || v >= V) {
                    std::cout << "Error: Vertices must be in range 0 to " << V-1 << ".\n";
                } else {
                    int a = std::min(u, v);
                    int b = std::max(u, v);
                    if (edge_set.count({a, b})) {
                        std::cout << "Error: Edge already exists. Please enter a new edge.\n";
                        continue;
                    }
                    edge_set.insert({a, b});
                    break;
                }
            }
            graph_data += std::to_string(u) + " " + std::to_string(v) + "\n";
        }

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Socket creation error\n";
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported\n";
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection Failed\n";
            return -1;
        }

        send(sock, graph_data.c_str(), graph_data.size(), 0);

        int valread = read(sock, buffer, 4096);
        std::cout << "Server response:\n" << std::string(buffer, valread) << std::endl;

        close(sock);

        std::string again;
        while (true) {
            std::cout << "Do you want to enter another graph? (y/n): ";
            std::cin >> again;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (again == "y" || again == "Y") {
                break;
            } else if (again == "n" || again == "N") {
                int sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock >= 0) {
                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_port = htons(PORT);
                    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) > 0) {
                        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0) {
                            std::string exit_msg = "EXIT_CLIENT";
                            send(sock, exit_msg.c_str(), exit_msg.size(), 0);
                            close(sock);
                        }
                    }
                }
                std::cout << "Goodbye!\n";
                return 0;
            } else {
                std::cout << "Invalid input. Please enter 'y' or 'n'.\n";
            }
        }
    }
}

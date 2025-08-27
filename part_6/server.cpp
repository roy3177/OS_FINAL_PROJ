#include "server.hpp"

void run_server() 
{
    /*
    Declare and initialize the main variables needed for socket setup,
    client communication, and data handling in the server program.
    */
	int  new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[4096] = {0};

	// Create socket file descriptor:
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if ((server_fd ) == 0) 
    {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
    // Initialize address structure:
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

    // Bind the socket to the specified port and address:
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    // Start listening for incoming connections:
	if (listen(server_fd, 3) < 0) 
    {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << PORT << std::endl;

    // Main server loop to accept and handle client connections:
	while (true) 
    {
        // We create a new socket for each client connection:
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		
        std::cout << "Client connected." << std::endl;

        // Check for errors in accepting the connection:
        if (new_socket < 0) 
        {
			perror("accept");
			exit(EXIT_FAILURE);
		}
        // Read data from the client:
		int valread = read(new_socket, buffer, 4096);
        // Convert the read buffer into a string
		std::string input(buffer, valread);

        // Handle client exit
        if (input == "EXIT_CLIENT") 
        {
            std::cout << "Client exited." << std::endl;
            close(new_socket);
            continue;
        }
		// Parse input: first line = vertices, second = edges, next lines = edge list
        std::istringstream iss(input);
        int V, E;
        iss >> V >> E;

        // Validate vertices and edges
        std::ostringstream msg;
        if (V <= 0) 
        {
            msg << "Error: Number of vertices must be positive.\n";
        } 
        else if (E < 0) 
        {
            msg << "Error: Number of edges cannot be negative.\n";
        } 
        else 
        {
            Graph g(V,false); // false for undirected
            bool valid = true;
            // Check validity of edges and add them if they are valid:
            for (int i = 0; i < E; ++i) 
            {
                int u, v;
                iss >> u >> v;
                if (u < 0 || v < 0 || u >= V || v >= V) 
                {
                    msg << "Error: Invalid edge (" << u << ", " << v << "). Vertices must be in range 0 to " << V-1 << ".\n";
                    valid = false;
                    break;
                }
                g.addEdge(u, v);
            }
            if (valid) 
            {
                // Run Eulerian circuit algorithm and capture output:
                std::ostringstream oss;
                std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());
                EulerCircle ec(g);
                ec.findEulerianCircuit();
                std::cout.rdbuf(old_cout);

                msg << "Welcome to the Euler Graph Server!\n";
                msg << "Vertices: " << V << "\n";
                msg << "Edges: " << E << "\n";
                msg << oss.str();
            }
        }
        std::string result = msg.str(); // Capture the response message

         // Print received input and response
        std::cout << "Received from client:\n" << input << std::endl;
        std::cout << "Response sent:\n" << result << std::endl;


        // Send the response back to the client:
		send(new_socket, result.c_str(), result.size(), 0);
		close(new_socket); // close the client's socket
	}
	return;
}

int main()
{
    run_server();
    return 0;
}






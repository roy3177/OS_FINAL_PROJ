# **Operation Systems - Final Project**

## **Authors**
- **Roy Meoded**
- **Yarin Keshet**

---

## **Overview**
This project is a comprehensive implementation of various algorithms and systems programming concepts, divided into multiple parts. Each part builds upon the previous one, culminating in a robust server-client architecture with algorithmic capabilities.

---

## **Project Structure**
The project is divided into the following parts:

### **Part 1: Graph Implementation**
- **Description**: Implements basic graph functionality, including graph creation, manipulation, and traversal.
- **Key Files**:
  - `graph_impl.cpp`: Contains the implementation of graph-related operations.
  - `graph_impl.hpp`: Header file for graph operations.
  - `main_case1.cpp`, `main_case2.cpp`: Test cases for graph functionality.
- **Purpose**: Provides the foundation for graph-based algorithms used in later parts.

---

### **Part 2: Euler Circle**
- **Description**: Focuses on finding Euler circles in graphs.
- **Key Files**:
  - `euler_circle.cpp`: Implements the algorithm for finding Euler circles.
  - `euler_circle.hpp`: Header file for Euler circle operations.
  - `main.cpp`: Entry point for testing Euler circle functionality.
- **Purpose**: Demonstrates the application of graph traversal algorithms.

---

### **Part 3: Random Graph Generation**
- **Description**: Implements random graph generation with configurable parameters such as number of vertices, edges, and weights.
- **Key Files**:
  - `random_graph.cpp`: Contains the logic for generating random graphs.
  - `random_graph.hpp`: Header file for random graph generation.
  - `main.cpp`: Entry point for testing random graph generation.
- **Purpose**: Provides a way to generate test data for graph algorithms.

---

### **Part 4: Basic Algorithms**
- **Description**: Introduces basic algorithms for graph processing, such as traversal and simple computations.
- **Key Files**:
  - `main.cpp`: Contains the implementation and testing of basic graph algorithms.
- **Purpose**: Serves as a stepping stone for more advanced algorithms in later parts.

---

### **Part 6: Server-Client Architecture**
- **Description**: Implements a basic server-client architecture for communication and data exchange.
- **Key Files**:
  - `server.cpp`: Contains the server-side logic for handling client requests.
  - `server.hpp`: Header file for server operations.
  - `client.cpp`: Contains the client-side logic for sending requests to the server.
  - `client.hpp`: Header file for client operations.
- **Purpose**: Establishes the foundation for distributed systems and network communication.

---

### **Part 7: Advanced Algorithms**
- **Description**: Adds advanced graph algorithms, including:
  - Finding SCC (Strongly Connected Components).
  - Finding Maximum Flow.
  - Finding Number of Cliques.
  - MST (Minimum Spanning Tree).
- **Key Files**:
  - `algorithms/`: Contains implementations of advanced graph algorithms.
  - `strategy_factory/`: Implements a factory pattern for algorithm selection.
  - `apps/`: Contains client-server applications for testing algorithms + PDF file with visual explanation.
- **Purpose**: Demonstrates the application of complex graph algorithms in a distributed environment.

---

### **Part 8: Random Graph Integration**
- **Description**: Integrates random graph generation with the server-client architecture, allowing clients to request random graphs and perform operations on them.
- **Key Files**:
  - `apps/`: Contains server and client logic.
  - `include/`: Contains random graph generation logic.
- **Purpose**: Combines random graph generation with distributed systems.

---

### **Part 9: Pipeline and Blocking Queue**
- **Description**: Implements pipeline processing and blocking queue mechanisms for efficient data handling and processing.
- **Key Files**:
  - `apps/`: Contains server and client logic.
  - `include/`: Contains pipeline and blocking queue implementations.
- **Purpose**: Demonstrates advanced systems programming concepts such as concurrency and data pipelines.

---

## **How to Build**

### **Option 1: Compile a Specific Part**
To compile a specific part, navigate to the part's directory and run `make`. For example:
1. Navigate to the desired part:
   ```bash
   cd part_8/build
   ```
2. Compile the part:
   ```bash
   make
   ```

### **Option 2: Compile All Parts**
To compile all parts without navigating to each directory:
1. Run the following command from the root directory:
   ```bash
   make
   ```
   This will recursively compile all parts using the general `Makefile`.

---

## **Valgrind and Gcov Usage**
Throughout the project, we used `valgrind` and `gcov` to ensure the correctness and efficiency of our code. The following tools were utilized:
- **Memcheck**: To detect memory leaks and invalid memory usage.
  ```bash
  valgrind --leak-check=full ./<executable>
  ```
- **Helgrind**: To detect data races in multithreaded programs.
  ```bash
  valgrind --tool=helgrind ./<executable>
  ```
- **Callgrind**: To analyze function call performance and identify bottlenecks.
  ```bash
  valgrind --tool=callgrind ./<executable>
  ```
- **Gcov**: To measure code coverage and ensure all parts of the code are tested. Code coverage is automatically handled by running the `run_tests.sh` script in each part:
  ```bash
  ./run_tests.sh
  ```
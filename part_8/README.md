Part 8 - Leader–Follower multi-threaded server

Overview
- Independent build that reuses part_1 graph and part_7 algorithms.
- Adds Leader–Follower server, ALG=ALL, and random graph generation (directed/undirected per user).
- Algorithms that don't apply to chosen orientation will return an error line as requested.

Run
- Build: make -C part_8/build
- Start server: part_8/build/server [port]
- Start client: part_8/build/client [port]

Protocol
- ALG=ALL
- RANDOM=0|1
- DIRECTED=0|1
- V=<n>
- E=<m>
- EDGE u v [w]
- PARAM SRC <s>
- PARAM SINK <t>
- PARAM K <k>
- END

Response (streamed):
OK
RESULT MAX_FLOW=<val or Error: ...>
RESULT SCC_COUNT=<val or Error: ...>
RESULT MST_WEIGHT=<val or Error: ...>
RESULT CLIQUES=<val or Error: ...>
END

#!/bin/bash
set -euo pipefail
mkdir -p build

echo "[1] Running server..."
./server > build/server.out 2> build/server.err &
SERVER_PID=$!
sleep 0.5

echo "[2] Complex invalid + valid input"
cat <<EOF | ./client > build/client_complex.out 2> build/client_complex.err
-2
a
!
3
4
a
3
1 1
1 -2
1 5
1 2
1 0
2 0
n
EOF

echo "[3] One edge (not Eulerian)"
cat <<EOF | ./client > build/client_one_edge.out 2> build/client_one_edge.err
3
1
1 2
n
EOF

echo "[4] Valid Eulerian circuit"
cat <<EOF | ./client > build/client_valid_euler.out 2> build/client_valid_euler.err
3
2
0 1
1 2
n
EOF

echo "[5] Exit in the middle of edge input"
cat <<EOF | ./client > build/client_exit_mid_edges.out 2> build/client_exit_mid_edges.err
3
2
0 1
exit
EOF

echo "[6] Invalid answer in again loop"
cat <<EOF | ./client > build/client_invalid_again.out 2> build/client_invalid_again.err
3
0
maybe
n
EOF

echo "[7] Edges larger than max"
cat <<EOF | ./client > build/client_edges_gt_max.out 2> build/client_edges_gt_max.err
3
5
2
0 1
1 2
n
EOF

echo "[8] Empty graph (E=0)"
cat <<EOF | ./client > build/client_empty_graph.out 2> build/client_empty_graph.err
3
0
n
EOF

echo "[9] V too small (trigger V<=1)"
cat <<EOF | ./client > build/client_v_too_small.out 2> build/client_v_too_small.err
1
3
0
n
EOF

echo "[10] Edge with no space"
cat <<EOF | ./client > build/client_edge_no_space.out 2> build/client_edge_no_space.err
3
1
01
0 1
n
EOF

echo "[11] Non-numeric edge (stoi exception)"
cat <<EOF | ./client > build/client_edge_non_numeric.out 2> build/client_edge_non_numeric.err
3
1
x y
0 1
n
EOF

echo "[12] Duplicate edge"
cat <<EOF | ./client > build/client_edge_duplicate.out 2> build/client_edge_duplicate.err
3
2
0 1
1 0
2 0
n
EOF

echo "[13] Again loop – user chooses 'y'"
cat <<EOF | ./client > build/client_again_y.out 2> build/client_again_y.err
3
0
y
3
0
n
EOF

echo "[14] Again loop – invalid then 'n'"
cat <<EOF | ./client > build/client_again_invalid.out 2> build/client_again_invalid.err
3
0
bla
n
EOF


echo "[15] Server-side: V <= 0 (invalid vertices)"
cat <<EOF | ./client > build/server_V_le_0.out 2> build/server_V_le_0.err
0
3
0
n
EOF

echo "[16] Connection error (server down)"
kill $SERVER_PID || true
timeout 2s ./client > build/client_conn_fail.out 2> build/client_conn_fail.err || true


echo "[17] Again loop – invalid then n"
cat <<EOF | ./client > build/client_again_invalid_prompt.out 2> build/client_again_invalid_prompt.err
3
0
koko
n
EOF


echo "[18] Sending EXIT_CLIENT to terminate server..."
cat <<EOF | ./client > /dev/null 2>&1
2
0
exit
EOF

sleep 0.5

echo "[19] Connection error (server already down)"
kill $SERVER_PID || true
cat <<EOF | ./client > build/client_conn_fail.out 2> build/client_conn_fail.err || true
3
0
n
EOF

echo "[20] Again loop – invalid then 'n' (extra)"
cat <<EOF | ./client > build/client_again_invalid_prompt.out 2> build/client_again_invalid_prompt.err
3
0
koko
n
EOF

echo "[21] Server timeout (no clients for 30s)"
./server > build/server_timeout.out 2> build/server_timeout.err &
SERVER_PID=$!
sleep 35



echo "[22] Done."

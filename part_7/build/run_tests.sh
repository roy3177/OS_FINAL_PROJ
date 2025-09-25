#!/bin/bash
set -euo pipefail
mkdir -p build

echo "[1] Starting server..."
./server > build/server.out 2> build/server.err &
SERVER_PID=$!
sleep 0.5

cleanup() {
  if kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "[X] Stopping server..."
    # שליחת בקשת SHUTDOWN עם END כדי שהשרת יסיים לולאת recv
    timeout 2 bash -c "echo -e 'SHUTDOWN\nEND\n' > /dev/tcp/127.0.0.1/9090" || true
    wait "$SERVER_PID" 2>/dev/null || true
  fi
}
trap cleanup EXIT

# ---------- BASIC QUICK EXIT ----------
echo "[2] Client quick exit"
cat > build/input_quick_exit.txt <<'EOF'
0
EOF
./client < build/input_quick_exit.txt > build/client_quick_exit.out 2> build/client_quick_exit.err

# ---------- INVALID MENU / INVALID NUMBERS ----------
echo "[3] Client invalid menu and numbers"
cat > build/input_invalid_numbers.txt <<'EOF'
a       # invalid menu
2       # choose SCC
a       # invalid vertices
3       # valid vertices
4       # too many edges
1       # valid edges
0  1 \\  # invalid edge (bad format)
0 1     # valid edge
exit
EOF
./client < build/input_invalid_numbers.txt > build/client_invalid_numbers.out 2> build/client_invalid_numbers.err

# ---------- ZERO EDGES (SCC) ----------
echo "[4] SCC with 0 edges"
cat > build/input_scc_zero.txt <<'EOF'
2
2
0
exit
EOF
./client < build/input_scc_zero.txt > build/client_scc_zero.out 2> build/client_scc_zero.err

# ---------- MAX_FLOW with 0 edges (skipping SRC/SINK) ----------
echo "[5] MAX_FLOW with 0 edges"
cat > build/input_maxflow_zero.txt <<'EOF'
1
2
0
exit
EOF
./client < build/input_maxflow_zero.txt > build/client_maxflow_zero.out 2> build/client_maxflow_zero.err

# ---------- MAX_FLOW with invalid SRC/SINK ----------
echo "[6] MAX_FLOW invalid SRC/SINK"
cat > build/input_maxflow_invalid_srcsink.txt <<'EOF'
1
2
1
0 1 1
0
0      # same as SRC
2      # out of range
1      # valid SINK
exit
EOF
./client < build/input_maxflow_invalid_srcsink.txt > build/client_maxflow_invalid_srcsink.out 2> build/client_maxflow_invalid_srcsink.err

# ---------- CLIQUES with invalid K ----------
echo "[7] CLIQUES invalid K"
cat > build/input_cliques_invalid_k.txt <<'EOF'
3
2
1
0 1 1
1      # K too small
2      # valid K
exit
EOF
./client < build/input_cliques_invalid_k.txt > build/client_cliques_invalid_k.out 2> build/client_cliques_invalid_k.err

# ---------- MST minimal ----------
echo "[8] MST minimal graph"
cat > build/input_mst_minimal.txt <<'EOF'
4
2
1
0 1 1
exit
EOF
./client < build/input_mst_minimal.txt > build/client_mst_minimal.out 2> build/client_mst_minimal.err

# ---------- EXTRA COVERAGE CASES ----------
echo "[9] Unknown directive"
cat > build/input_parse_error.txt <<'EOF'
HELLO
END
EOF
./client < build/input_parse_error.txt > build/client_parse_error.out 2> build/client_parse_error.err

echo "[10] Missing V"
cat > build/input_missing_v.txt <<'EOF'
ALG MAX_FLOW
END
EOF
./client < build/input_missing_v.txt > build/client_missing_v.out 2> build/client_missing_v.err

echo "[11] Unsupported ALG"
cat > build/input_unsupported_alg.txt <<'EOF'
ALG BLAH
V 3
E 0
END
EOF
./client < build/input_unsupported_alg.txt > build/client_unsupported_alg.out 2> build/client_unsupported_alg.err

echo "[12] SRC == SINK with PARAM"
cat > build/input_src_equals_sink.txt <<'EOF'
ALG MAX_FLOW
V 3
E 0
PARAM SRC 1
PARAM SINK 1
END
EOF
./client < build/input_src_equals_sink.txt > build/client_src_equals_sink.out 2> build/client_src_equals_sink.err

echo "[13] Invalid edge (out of range)"
cat > build/input_invalid_edge.txt <<'EOF'
ALG MST
V 2
E 1
EDGE 0 5 1
END
EOF
./client < build/input_invalid_edge.txt > build/client_invalid_edge.out 2> build/client_invalid_edge.err

echo "[14] Client abrupt disconnect"
timeout 1 bash -c '{ printf "ALG MST\n"; sleep 0.1; } | nc 127.0.0.1 9090' || true

# ---------- EDGE OUT OF RANGE ----------
echo "[15] Edge out of range"
printf "ALG MST\nV 2\nE 1\nEDGE 5 5 1\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- TOO MANY EDGES ----------
echo "[16] Too many edges"
printf "ALG MST\nV 2\nE 1\nEDGE 0 1 1\nEDGE 1 0 1\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- V negative ----------
echo "[17] Negative V"
printf "ALG SCC\nV -3\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- Missing END abrupt ----------
echo "[18] Missing END abrupt"
timeout 1 bash -c '{ printf "ALG MST\nV 2\nE 1\nEDGE 0 1"; sleep 0.1; } | nc 127.0.0.1 9090' || true

# ---------- CRLF (Windows line endings) ----------
echo "[19] CRLF endings"

printf "ALG MST\r\nV 2\r\nE 1\r\nEDGE 0 1 1\r\nEND\r\n" | nc -N 127.0.0.1 9090 || true

# ---------- PARAM lines ----------
echo "[20] PARAM lines"
printf "ALG MAX_FLOW\nV 3\nE 1\nEDGE 0 1 1\nPARAM SRC 0\nPARAM SINK 2\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- Bare edge  ----------
echo "[21] Bare edge line"
printf "ALG MST\nV 2\nE 1\n0 1 5\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- Simple header  ----------
echo "[22] Simple header V E"
printf "V 2 E 1\nEDGE 0 1 1\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- SRC == SINK  ----------
echo "[23] SRC == SINK explicit"
printf "ALG MAX_FLOW\nV 2\nE 1\nEDGE 0 1 1\nPARAM SRC 0\nPARAM SINK 0\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- Unsupported ALG  ----------
echo "[24] Unsupported ALG again"
printf "ALG UNKNOWNALG\nV 2\nE 0\nEND\n" | nc -N 127.0.0.1 9090 || true

# ---------- Bind failure (perror(bind)) ----------
echo "[25] Bind failure"
nc -l 9091 >/dev/null 2>&1 &
NC_PID=$!
sleep 0.2
./server 9091 || true
kill $NC_PID || true

# ---------- Signal handling (handle_signal) ----------
echo "[26] SIGINT to server"
./server 9092 > build/server_signal.out 2> build/server_signal.err &
TEMP_PID=$!
sleep 0.5
kill -INT "$TEMP_PID"
wait "$TEMP_PID" || true

echo "[27] Unknown selection"
printf "9\n0\n" | ./client > build/client_unknown_selection.out 2> build/client_unknown_selection.err || true

echo "[28] Bad edge format"
printf "4\n2\n1\nabc\n0 1 1\n0\n" | ./client > build/client_bad_edge.out 2> build/client_bad_edge.err || true

echo "[29] Invalid vertices"
printf "4\n2\n1\n0 5 1\n0 1 1\n0\n" | ./client > build/client_invalid_vertices.out 2> build/client_invalid_vertices.err || true

echo "[30] Timeout at startup"
timeout 12 ./client > build/client_timeout_start.out 2> build/client_timeout_start.err || true

echo "[31] Connect failure (no server)"
./client 9999 > build/client_connect_fail.out 2> build/client_connect_fail.err || true

echo "[32] Unknown menu option"
printf "9\n0\n" | ./client > build/client_unknown_menu.out 2> build/client_unknown_menu.err || true

echo "[33] Timeout during edge input"
{ printf "4\n2\n1\n"; sleep 12; } | ./client > build/client_timeout_edge.out 2> build/client_timeout_edge.err || true

echo "[34] Exit command during edge input"
printf "4\n2\n1\nexit\n" | ./client > build/client_exit_edge.out 2> build/client_exit_edge.err || true

echo "[35] Invalid weight (<=0)"
printf "4\n2\n1\n0 1 0\n0 1 1\n0\n" | ./client > build/client_bad_weight.out 2> build/client_bad_weight.err || true

echo "[36] Duplicate edge (undirected)"
printf "4\n3\n2\n0 1 1\n1 0 1\n0 2 1\n0\n" | ./client > build/client_dup_edge.out 2> build/client_dup_edge.err || true

echo "[37] SINK == SRC in MAX_FLOW"
printf "1\n2\n1\n0 1 1\n0\n0\n1\n0\n" | ./client > build/client_sink_eq_src.out 2> build/client_sink_eq_src.err || true

echo "[38] Timeout at 'again' prompt"
{ printf "2\n2\n0\n"; sleep ; } | ./client > build/client_timeout_again.out 2> build/client_timeout_again.err || true

echo " All test runs completed."

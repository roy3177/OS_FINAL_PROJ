#!/bin/bash
set -euo pipefail

# ─────────────────────  Output dir (avoid build/build when run from build/) ─────────────────────
LOG_DIR="build"
if [[ "$(basename "$PWD")" == "build" ]]; then
  LOG_DIR="testlogs"   # When running from build/, keep logs under build/testlogs
fi
mkdir -p "$LOG_DIR"

PORT="${PORT:-9090}"

# ─────────────────────  Helper: run client with a timeout  ─────────────────────
run_with_input() {
  local in_file="$1"
  local out_file="$2"
  local err_file="$3"
  # 25s is a safe margin (tune if needed)
  timeout 25s ./client < "$in_file" > "$out_file" 2> "$err_file" || true
}

# ─────────────────────  Start server  ─────────────────────
echo "[1] Starting server..."

# If the port is already used, try to kill the process gently
if command -v fuser >/dev/null 2>&1; then
  fuser -k "${PORT}/tcp" 2>/dev/null || true
fi

./server > "$LOG_DIR/server.out" 2> "$LOG_DIR/server.err" &
SERVER_PID=$!
sleep 0.5

cleanup() {
  if kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "[X] Stopping server..."
    # Ask the server to shutdown gracefully via its TCP command
    timeout 2 bash -c "echo -e 'SHUTDOWN\nEND\n' > /dev/tcp/127.0.0.1/${PORT}" || true
    wait "$SERVER_PID" 2>/dev/null || true
  fi
}
trap cleanup EXIT

# Verify the server actually started
if ! kill -0 "$SERVER_PID" 2>/dev/null; then
  echo "[!] Server failed to start. Dumping $LOG_DIR/server.err:"
  echo "----------------------------------------"
  ( [[ -s "$LOG_DIR/server.err" ]] && cat "$LOG_DIR/server.err" ) || echo "(empty)"
  echo "----------------------------------------"
  exit 1
fi

# ─────────────────────  [2] Client quick exit  ─────────────────────
echo "[2] Client quick exit"
cat > "$LOG_DIR/input_quick_exit.txt" <<'EOF'
0
EOF
run_with_input "$LOG_DIR/input_quick_exit.txt" "$LOG_DIR/client_quick_exit.out" "$LOG_DIR/client_quick_exit.err"

# ─────────────────────  [3] Undirected PREVIEW only (do not run ALL)  ─────────────────────
# 1 (ALL menu) → directed=0 → V,E,WMIN,WMAX,SRC=-1,K → Run ALL? 0 → main menu → 0 (Exit)
echo "[3] Undirected PREVIEW only"
cat > "$LOG_DIR/input_undirected_preview_only.txt" <<'EOF'
1
0
6
7
1
5
-1
3
0
0
EOF
run_with_input "$LOG_DIR/input_undirected_preview_only.txt" "$LOG_DIR/client_undirected_preview_only.out" "$LOG_DIR/client_undirected_preview_only.err"

# ─────────────────────  [4] Directed PREVIEW -> ALL  ─────────────────────
# 1 (ALL menu) → 1 → 3,5,2,2, SRC=-1, K=2 → Run ALL? 1 → ENTER to continue → 0
echo "[4] Directed PREVIEW -> ALL"
cat > "$LOG_DIR/input_directed_preview_all.txt" <<'EOF'
1
1
3
5
2
2
-1
2
1

0
EOF
run_with_input "$LOG_DIR/input_directed_preview_all.txt" "$LOG_DIR/client_directed_preview_all.out" "$LOG_DIR/client_directed_preview_all.err"

# ─────────────────────  [5] Client invalid numbers (with timeout)  ─────────────────────
echo "[5] Client invalid numbers"
cat > "$LOG_DIR/input_invalid_numbers.txt" <<'EOF'
1
0
a
5
6
-2
2
-1
3
0
0
EOF
run_with_input "$LOG_DIR/input_invalid_numbers.txt" "$LOG_DIR/client_invalid_numbers.out" "$LOG_DIR/client_invalid_numbers.err"

# ─────────────────────  [6] Directed ALL with MST/CLIQUES not allowed  ─────────────────────
echo "[6] Directed ALL with MST/CLIQUES not allowed]"
cat > "$LOG_DIR/input_directed_disallow_set.txt" <<'EOF'
1
1
4
6
1
3
-1
3
1

0
EOF
run_with_input "$LOG_DIR/input_directed_disallow_set.txt" "$LOG_DIR/client_directed_disallow_set.out" "$LOG_DIR/client_directed_disallow_set.err"

# ─────────────────────  Raw TCP tests (netcat)  ─────────────────────

# [7] Unknown directive
echo "[7] Unknown directive (raw to server)"
printf "HELLO\nEND\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_unknown_directive.out" 2> "$LOG_DIR/raw_unknown_directive.err" || true

# [8] Missing V
echo "[8] Missing V (raw to server)"
printf "ALG MAX_FLOW\nEND\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_missing_v.out" 2> "$LOG_DIR/raw_missing_v.err" || true

# [9] Unsupported ALG
echo "[9] Unsupported ALG (raw to server)"
printf "ALG NOPE\nV 3\nE 0\nEND\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_unsupported_alg.out" 2> "$LOG_DIR/raw_unsupported_alg.err" || true

# [10] Invalid edge (out of range)
echo "[10] Invalid edge (raw to server)"
printf "ALG MST\nV 2\nE 1\nEDGE 0 5 1\nEND\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_invalid_edge.out" 2> "$LOG_DIR/raw_invalid_edge.err" || true

# [11] CRLF endings
echo "[11] CRLF endings (raw to server)"
printf "ALG MST\r\nV 2\r\nE 1\r\nEDGE 0 1 1\r\nEND\r\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_crlf.out" 2> "$LOG_DIR/raw_crlf.err" || true

# [12] PARAM lines (MAX_FLOW with SRC/SINK)
echo "[12] PARAM lines (raw to server)"
printf "ALG MAX_FLOW\nV 3\nE 1\nEDGE 0 1 1\nPARAM SRC 0\nPARAM SINK 2\nEND\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_params.out" 2> "$LOG_DIR/raw_params.err" || true

# [13] Directed/Undirected mismatch (CLIQUES/MST on directed)
echo "[13] Directed mismatch checks (raw to server)"
printf "ALG CLIQUES\nDIRECTED 1\nV 3\nE 0\nEND\n" | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_cliques_directed.out" 2> "$LOG_DIR/raw_cliques_directed.err" || true
printf "ALG MST\nDIRECTED 1\nV 3\nE 0\nEND\n"      | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_mst_directed.out" 2> "$LOG_DIR/raw_mst_directed.err" || true

# ─────────────────────  [14] Preview then exit  ─────────────────────
echo "[14] Client preview then exit"
cat > "$LOG_DIR/input_preview_then_exit.txt" <<'EOF'
1
0
5
5
1
2
-1
2
0
0
EOF
run_with_input "$LOG_DIR/input_preview_then_exit.txt" "$LOG_DIR/client_preview_then_exit.out" "$LOG_DIR/client_preview_then_exit.err"

# ─────────────────────  Extra cases to improve coverage  ─────────────────────

# [15] Client: validate SRC/SINK (ask again when SINK==SRC) + exit with 'exit'
echo "[15] Client SRC/SINK validation + exit"
cat > "$LOG_DIR/input_src_sink_validation.txt" <<'EOF'
1
1
3
3
1
2
0
0
1
2
1
exit
EOF
run_with_input "$LOG_DIR/input_src_sink_validation.txt" \
  "$LOG_DIR/client_src_sink_validation.out" "$LOG_DIR/client_src_sink_validation.err"

# [16] Server: RANDOM=0 with valid explicit undirected graph (EDGE path)
echo "[16] Raw explicit undirected MST (RANDOM=0, valid edges)"
printf "ALG MST\nDIRECTED 0\nV 4\nE 3\nEDGE 0 1 1\nEDGE 1 2 2\nEDGE 2 3 3\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_explicit_mst_ok.out" 2> "$LOG_DIR/raw_explicit_mst_ok.err" || true

# [17] Server: RANDOM=0 with invalid weight (0)
echo "[17] Raw explicit MST with invalid weight"
printf "ALG MST\nDIRECTED 0\nV 2\nE 1\nEDGE 0 1 0\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_edge_weight_zero.out" 2> "$LOG_DIR/raw_edge_weight_zero.err" || true

# [18] Server: RANDOM=1 but negative E
echo "[18] Random graph with negative E]"
printf "ALG PREVIEW\nDIRECTED 0\nRANDOM 1\nV 5\nE -1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_random_E_negative.out" 2> "$LOG_DIR/raw_random_E_negative.err" || true

# [19] Server: Directed MAX_FLOW on explicit graph (uses PARAM SRC/SINK)
echo "[19] Explicit directed MAX_FLOW with SRC/SINK"
printf "ALG MAX_FLOW\nDIRECTED 1\nV 4\nE 4\nEDGE 0 1 3\nEDGE 1 3 2\nEDGE 0 2 2\nEDGE 2 3 4\nPARAM SRC 0\nPARAM SINK 3\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_explicit_maxflow_ok.out" 2> "$LOG_DIR/raw_explicit_maxflow_ok.err" || true

# [20] Server: mismatch — MAX_FLOW on undirected graph -> error path
echo "[20] Directed mismatch for MAX_FLOW on undirected"
printf "ALG MAX_FLOW\nDIRECTED 0\nV 3\nE 0\nPARAM SRC 0\nPARAM SINK 2\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_maxflow_undirected_mismatch.out" 2> "$LOG_DIR/raw_maxflow_undirected_mismatch.err" || true

# [21] Server: RANDOM with WMAX < WMIN -> range swap
echo "[21] Random with WMAX < WMIN (swap)"
printf "ALG PREVIEW\nDIRECTED 1\nRANDOM 1\nV 5\nE 4\nWMIN 10\nWMAX 3\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_random_wswap.out" 2> "$LOG_DIR/raw_random_wswap.err" || true
# ─────────────────────  EXTRA TESTS to push coverage further  ─────────────────────

# [22] Client: hammer prompt_int (overflow, invalid, out-of-range) then exit
echo "[22] Client prompt_int overflow/invalid/out-of-range"
cat > "$LOG_DIR/input_prompt_int_hammer.txt" <<'EOF'
1        # ALL
0        # undirected
-3       # V: invalid (negative not allowed) -> reprompt
1        # V: out-of-range (<2)           -> reprompt
3        # V: ok
9999999999999999999999  # E: overflow -> catch(...) -> reprompt
100                     # E: out-of-range for V=3    -> reprompt
2                       # E: ok
9999999999999999999999  # WMIN: overflow -> reprompt
1                       # WMIN: ok
0                       # WMAX: out-of-range (<WMIN) -> reprompt
2                       # WMAX: ok
-2       # SRC: out-of-range (<-1)        -> reprompt
-1       # SRC: skip
2        # K: ok (>=2)
0        # Run ALL? no (preview only)
0        # back to menu: exit
EOF
run_with_input "$LOG_DIR/input_prompt_int_hammer.txt" \
  "$LOG_DIR/client_prompt_int_hammer.out" "$LOG_DIR/client_prompt_int_hammer.err"

# [23] Server: explicit undirected CLIQUES with K=3 (valid)
echo "[23] Explicit undirected CLIQUES (K=3)"
printf "ALG CLIQUES\nDIRECTED 0\nV 4\nE 3\nEDGE 0 1 1\nEDGE 1 2 2\nEDGE 2 3 3\nPARAM K 3\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_cliques_ok.out" 2> "$LOG_DIR/raw_cliques_ok.err" || true

# [24] Server: directed SCC on explicit graph (valid)
echo "[24] Explicit directed SCC"
printf "ALG SCC\nDIRECTED 1\nV 5\nE 6\nEDGE 0 1 1\nEDGE 1 2 1\nEDGE 2 0 1\nEDGE 1 3 1\nEDGE 3 4 1\nEDGE 4 3 1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_scc_ok.out" 2> "$LOG_DIR/raw_scc_ok.err" || true

# [25] Server: CLIQUES with invalid K (<2) -> error path
echo "[25] CLIQUES invalid K"
printf "ALG CLIQUES\nDIRECTED 0\nV 4\nE 0\nPARAM K 1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_cliques_bad_k.out" 2> "$LOG_DIR/raw_cliques_bad_k.err" || true

# [26] Server: unknown PARAM name
echo "[26] Unknown PARAM name"
printf "ALG MST\nDIRECTED 0\nV 3\nE 2\nEDGE 0 1 1\nEDGE 1 2 1\nPARAM FOO 7\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_param_unknown.out" 2> "$LOG_DIR/raw_param_unknown.err" || true

# [27] Server: invalid DIRECTED value (not 0/1)
echo "[27] Invalid DIRECTED value"
printf "ALG PREVIEW\nDIRECTED 2\nV 3\nE 1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_directed_invalid.out" 2> "$LOG_DIR/raw_directed_invalid.err" || true

# [28] Server: missing ALG line
echo "[28] Missing ALG line"
printf "DIRECTED 0\nV 3\nE 0\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_missing_alg.out" 2> "$LOG_DIR/raw_missing_alg.err" || true

# [29] Server: bad tokens (non-numeric V/E/SEED)
echo "[29] Bad numeric tokens"
printf "ALG PREVIEW\nDIRECTED 0\nV a\nE b\nSEED x\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_bad_tokens.out" 2> "$LOG_DIR/raw_bad_tokens.err" || true

# [30] Server: EDGE count mismatch and negative vertex
echo "[30] EDGE mismatch + negative vertex"
printf "ALG MST\nDIRECTED 0\nV 3\nE 2\nEDGE -1 2 1\nEDGE 0 1 1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_edge_badverts.out" 2> "$LOG_DIR/raw_edge_badverts.err" || true

# [31] Server: RANDOM=1 with E exceeding possible edges (should error)
echo "[31] Random E too large for V"
printf "ALG PREVIEW\nDIRECTED 0\nRANDOM 1\nV 4\nE 999\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_random_E_too_large.out" 2> "$LOG_DIR/raw_random_E_too_large.err" || true
# ─────────────────────  MORE TESTS to push coverage  ─────────────────────

# [32] Client: connect() failure path (use a non-listening port)
echo "[32] Client connect() failure"
: > "$LOG_DIR/empty_stdin.txt"
timeout 5s ./client 1 < "$LOG_DIR/empty_stdin.txt" \
  > "$LOG_DIR/client_connect_fail.out" 2> "$LOG_DIR/client_connect_fail.err" || true

# [33] Client: argv port parsing + quick exit (use actual server port)
echo "[33] Client argv port success"
cat > "$LOG_DIR/input_client_argv_quit.txt" <<'EOF'
0
EOF
timeout 10s ./client "$PORT" < "$LOG_DIR/input_client_argv_quit.txt" \
  > "$LOG_DIR/client_argv_quit.out" 2> "$LOG_DIR/client_argv_quit.err" || true

# [34] Client: prompt_int empty-line loop (blank line -> reprompt)
echo "[34] Client prompt_int empty-line loop"
cat > "$LOG_DIR/input_client_empty_lines.txt" <<'EOF'
1

0
2
1
1
-1
2
0
0
EOF
run_with_input "$LOG_DIR/input_client_empty_lines.txt" \
  "$LOG_DIR/client_empty_lines.out" "$LOG_DIR/client_empty_lines.err"

# [35] Server: fewer EDGE lines than declared E
echo "[35] Fewer EDGE lines than E"
printf "ALG MST\nDIRECTED 0\nV 4\nE 3\nEDGE 0 1 1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_edge_fewer_than_E.out" 2> "$LOG_DIR/raw_edge_fewer_than_E.err" || true

# [36] Server: more EDGE lines than declared E
echo "[36] More EDGE lines than E"
printf "ALG MST\nDIRECTED 0\nV 4\nE 1\nEDGE 0 1 1\nEDGE 2 3 1\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_edge_more_than_E.out" 2> "$LOG_DIR/raw_edge_more_than_E.err" || true

# [37] Server: SCC on undirected (mismatch path in run_alg_or_error)
echo "[37] SCC on undirected"
printf "ALG SCC\nDIRECTED 0\nV 3\nE 0\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_scc_undirected_mismatch.out" 2> "$LOG_DIR/raw_scc_undirected_mismatch.err" || true

# [38] Server: WMIN < 1 (invalid)
echo "[38] Random with WMIN < 1"
printf "ALG PREVIEW\nDIRECTED 1\nRANDOM 1\nV 3\nE 2\nWMIN 0\nWMAX 5\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_wmin_lt1.out" 2> "$LOG_DIR/raw_wmin_lt1.err" || true

# [39] Server: MAX_FLOW missing SINK / missing SRC
echo "[39] MAX_FLOW missing SINK / SRC"
printf "ALG MAX_FLOW\nDIRECTED 1\nV 3\nE 0\nPARAM SRC 0\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_maxflow_missing_sink.out" 2> "$LOG_DIR/raw_maxflow_missing_sink.err" || true
printf "ALG MAX_FLOW\nDIRECTED 1\nV 3\nE 0\nPARAM SINK 2\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_maxflow_missing_src.out" 2> "$LOG_DIR/raw_maxflow_missing_src.err" || true

# [40] Server: EDGE with non-numeric weight
echo "[40] EDGE with non-numeric weight"
printf "ALG MST\nDIRECTED 0\nV 3\nE 1\nEDGE 0 1 X\nEND\n" \
  | nc -N 127.0.0.1 "$PORT" > "$LOG_DIR/raw_edge_weight_nonnumeric.out" 2> "$LOG_DIR/raw_edge_weight_nonnumeric.err" || true

echo " All test runs completed."

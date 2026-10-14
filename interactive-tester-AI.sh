#!/usr/bin/env bash

# Usage:
#   bash interactive.sh          # Untitled-6.cpp を1回
#   bash interactive.sh 100      # Untitled-6.cpp を100回
#   bash interactive.sh main 100 # main.cpp を100回

set -u

# --- 設定 ---
DEFAULT_FILE="Untitled-6"
JUDGE="judge"
GEN="randomtest_generate.py"
LIB="/home/kakaokakao/lib/ac-library-master"
INC="/home/kakaokakao/projects/kyopro"
TIME_LIMIT=5

FILE="$DEFAULT_FILE"
TESTS=1

if [[ $# -ge 1 ]]; then
    if [[ "$1" =~ ^[0-9]+$ ]]; then
        TESTS="$1"
    else
        FILE="$1"
    fi
fi

if [[ $# -ge 2 ]]; then
    if [[ "$1" =~ ^[0-9]+$ ]]; then
        echo "Usage: bash interactive.sh [test_count] OR [solution_file_without_extension] [test_count]" >&2
        exit 2
    fi
    TESTS="$2"
fi

if [[ $# -gt 2 || ! "$TESTS" =~ ^[0-9]+$ ]] || (( TESTS < 1 )); then
    echo "Usage: bash interactive.sh [test_count] OR [solution_file_without_extension] [test_count]" >&2
    echo "Examples: bash interactive.sh 100 | bash interactive.sh main 100" >&2
    exit 2
fi

OPTS=(
    -DLOCAL
    -fdiagnostics-color=always
    -std=c++23
    -O0
    -g0
    -I .
    -I "$INC"
    -I "$LIB"
)
WARNS=(
    -Wshadow
    -Winvalid-pch
    -Wunused
    -Wmisleading-indentation
    -Wreorder
    -Wreturn-type
    -Wuninitialized
    -Wparentheses
    -Wfloat-equal
)

# --- 必要ファイルの確認 ---
for src in "${FILE}.cpp" "${JUDGE}.cpp" "$GEN"; do
    if [[ ! -f "$src" ]]; then
        echo "File not found: $src" >&2
        exit 1
    fi
done

# --- コンパイル（各1回だけ） ---
echo "Compiling..."
if ! g++ "${OPTS[@]}" "${WARNS[@]}" "${FILE}.cpp" -o "$FILE"; then
    exit 1
fi
if ! g++ "${OPTS[@]}" "${WARNS[@]}" "${JUDGE}.cpp" -o "$JUDGE"; then
    exit 1
fi

# --- テストループ ---
for ((i = 1; i <= TESTS; i++)); do
    printf '\rTrial: %d' "$i"

    if ! python3 "$GEN" > input.txt; then
        printf '\n[!] Generator failed on trial %d\n' "$i"
        echo "--- Input generated so far ---"
        cat input.txt
        exit 1
    fi

    # Python exit status: 0=AC, 1=WA, 2=RE, 3=TLE, 4=protocol/runtime error
    python3 - "$FILE" "$TIME_LIMIT" <<'PY'
import subprocess
import threading
import sys
import time

solution_name = sys.argv[1]
time_limit = float(sys.argv[2])

with open("input.txt", "r", encoding="utf-8") as f:
    initial_input = f.read()

solution = None
judge = None
threads = []
logs = []
logs_lock = threading.Lock()
result = ("ERROR", "test did not finish")
start = time.monotonic()

def add_log(prefix, text):
    with logs_lock:
        logs.append(f"[{prefix}] {text}")

def close_quietly(stream):
    try:
        if stream is not None:
            stream.close()
    except (OSError, ValueError):
        pass

def drain_stderr(proc, prefix):
    try:
        for line in proc.stderr:
            add_log(prefix + " stderr", line)
    except (OSError, ValueError):
        pass

try:
    solution = subprocess.Popen(
        ["./" + solution_name],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1,
    )
    judge = subprocess.Popen(
        ["./judge"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1,
    )

    # 初期入力を Judge に渡す
    judge.stdin.write(initial_input)
    judge.stdin.flush()

    def judge_to_solution():
        try:
            for line in judge.stdout:
                add_log("J", line)
                solution.stdin.write(line)
                solution.stdin.flush()
        except (BrokenPipeError, OSError, ValueError) as exc:
            add_log("bridge", f"judge -> solution: {exc}\n")
        finally:
            close_quietly(solution.stdin)

    def solution_to_judge():
        try:
            for line in solution.stdout:
                add_log("S", line)
                judge.stdin.write(line)
                judge.stdin.flush()
        except (BrokenPipeError, OSError, ValueError) as exc:
            add_log("bridge", f"solution -> judge: {exc}\n")
        finally:
            close_quietly(judge.stdin)

    threads = [
        threading.Thread(target=judge_to_solution, daemon=True),
        threading.Thread(target=solution_to_judge, daemon=True),
        threading.Thread(target=drain_stderr, args=(solution, "S"), daemon=True),
        threading.Thread(target=drain_stderr, args=(judge, "J"), daemon=True),
    ]
    for thread in threads:
        thread.start()

    while True:
        solution_rc = solution.poll()
        judge_rc = judge.poll()

        # 非正常終了した解答は RE
        if solution_rc is not None and solution_rc != 0:
            result = ("RE", f"solution exited with {solution_rc}")
            break

        # Judge終了時に解答の終了も少し待ってから判定
        if judge_rc is not None:
            try:
                solution_rc = solution.wait(timeout=0.2)
            except subprocess.TimeoutExpired:
                result = ("ERROR", "judge exited before solution finished")
                break

            if solution_rc != 0:
                result = ("RE", f"solution exited with {solution_rc}")
            elif judge_rc == 0:
                result = ("AC", "")
            else:
                result = ("WA", f"judge exited with {judge_rc}")
            break

        if time.monotonic() - start > time_limit:
            result = ("TLE", f"time limit = {time_limit}s")
            break

        time.sleep(0.01)

except Exception as exc:
    result = ("ERROR", repr(exc))

finally:
    # 子プロセスを停止して回収
    for proc in (solution, judge):
        if proc is not None and proc.poll() is None:
            proc.kill()

    for proc in (solution, judge):
        if proc is not None:
            try:
                proc.wait(timeout=1)
            except subprocess.TimeoutExpired:
                proc.kill()
                proc.wait()
            close_quietly(proc.stdin)
            close_quietly(proc.stdout)
            close_quietly(proc.stderr)

    for thread in threads:
        thread.join(timeout=0.3)

# AC時は静かに終了。非AC時だけログを表示する。
if result[0] != "AC":
    print(f"\n[!] {result[0]}: {result[1]}".rstrip(), flush=True)
    print("--- Interaction log ---", flush=True)
    with logs_lock:
        print("".join(logs), end="", flush=True)
    print("--- End of log ---", flush=True)

sys.exit({"AC": 0, "WA": 1, "RE": 2, "TLE": 3}.get(result[0], 4))
PY

    status=$?

    if (( status != 0 )); then
        printf '\n[!] Non-AC found on trial %d (exit code: %d)\n' "$i" "$status"
        echo "--- Input ---"
        cat input.txt
        echo
        echo "----------------"
        cp input.txt "failed_input_${i}.txt"
        echo "Failed input saved: failed_input_${i}.txt"
        exit "$status"
    fi
done

printf '\nAll %d tests passed!\n' "$TESTS"

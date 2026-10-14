#!/bin/bash

# --- 設定：ファイル名やパスをここで管理 ---
FILE="Untitled-6"
NAIVE="randomtest_naieve"
GEN="randomtest_generate.py"
LIB="/home/kakaokakao/lib/ac-library-master"
INC="/home/kakaokakao/projects/kyopro"

# コンパイルオプション（見やすく整理）
OPTS="-DLOCAL -fdiagnostics-color=always -std=c++23 -O0 -g0 -I . -I $INC -I $LIB"
WARNS="-Wshadow -Winvalid-pch -Wunused -Wmisleading-indentation -Wreorder -Wreturn-type -Wuninitialized -Wparentheses -Wfloat-equal"

# 1. コンパイル
echo "Compiling..."
if ! g++ $OPTS $WARNS "${FILE}.cpp" -o "$FILE"; then exit 1; fi
if ! g++ $OPTS $WARNS "${NAIVE}.cpp" -o "naive"; then exit 1; fi

# 2. テストループ
i=0
while true; do
    i=$((i+1))
    echo -ne "\rTrial: $i"

    # ケース生成
    python3 "$GEN" > input.txt

    # 実行
    ./"$FILE" < input.txt > out.txt
    ./"naive" < input.txt > ans.txt

    # 3. 比較
    if ! diff -w out.txt ans.txt > /dev/null; then
        echo -e "\n\e[31m[!] Wrong Answer found!\e[0m"
        echo "--- Input ---"
        cat input.txt
        echo -e "\n--- Expected ---"
        cat ans.txt
        echo -e "\n--- Found ---"
        cat out.txt
        break
    fi
done

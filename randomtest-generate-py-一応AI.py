"""



import networkx as nx
import random

n = 10
print(n)
for u, v in nx.random_tree(n).edges():
    print(u + 1, v + 1)
import networkx as nx

N = 10
M = 15

# --- 木を生成 ---
tree = nx.random_tree(N)
# エッジ（辺）のリストが [(0, 1), (1, 2), ...] の形で取れる
for u, v in tree.edges():
    print(u + 1, v + 1)

    


# --- 連結なグラフを生成 (G(n, m) モデル) ---
# 連結になるまで再試行してくれる便利な関数はないので、
# 木を作ってから M-(N-1) 本の辺を足すのが確実
G = nx.gnm_random_graph(N, M)
# ※これだと連結にならない可能性があるので注意



# import numpy as np

# --- 1からNのランダムな順列 ---
N = 10
P = np.random.permutation(np.arange(1, N + 1))
print(*P)




# --- 重み付きランダム (特定の数値が出やすいケース) ---
# 例: 1は10%, 2は20%, 3は70% の確率で選ぶ
values = [1, 2, 3]
weights = [0.1, 0.2, 0.7]
choice = np.random.choice(values, p=weights)
import random




def generate_graph(n, m):
    edges = set()
    # 1. 連結にしたいなら、まず適当な木を作る
    for i in range(2, n + 1):
        parent = random.randint(1, i - 1)
        edges.add(tuple(sorted((parent, i))))
    
    # 2. 残りの辺をランダムに追加
    while len(edges) < m:
        u = random.randint(1, n)
        v = random.randint(1, n)
        if u == v: continue
        edges.add(tuple(sorted((u, v))))
    
    return edges

# 出力
n, m = 5, 7
print(n, m)
for u, v in generate_graph(n, m):
    print(u, v)

    
    
import networkx as nx
import random

n1, n2 = 5, 5 # 左右の頂点数
G = nx.complete_bipartite_graph(n1, n2)

# 辺をランダムに半分消して「疎な二部グラフ」にする例
edges = list(G.edges())
random.shuffle(edges)
G.remove_edges_from(edges[:len(edges)//2])

for u, v in G.edges():
    print(u + 1, v + 1)


    
# ノード数N, 辺の確率p のランダムなDAG
N = 10
G = nx.fast_gnp_random_graph(N, 0.3, directed=True)
DAG = nx.DiGraph([(u, v) for (u, v) in G.edges() if u < v])
# 番号が u < v の方向しかないので、絶対にサイクルができない
# 
# d = 3 # 各頂点の次数
n = 10 # 全体の頂点数 (d * n が偶数である必要あり)
G = nx.random_regular_graph(d, n)
# 
# 
# 
# import networkx as nx
import matplotlib.pyplot as plt # 描画にはこれが必要

T = nx.random_tree(10)
nx.draw(T, with_labels=True) # node番号付きで描画
plt.show() # ウィンドウが開いてグラフが表示される


###
# 
# def gen_unique_array(n, v_min, v_max):
    相異なる要素を持つ数列を生成 (setで重複排除)
    res = set()
    while len(res) < n:
        res.add(random.randint(v_min, v_max))
    arr = list(res)
    random.shuffle(arr)
    return arr

#def gen_connected_graph(n, m):
  連結で自己ループ・多重辺なしの無向グラフ
    uf = UnionFind(n)
    edges = set()
    # 1. まず全域木を作って連結を保証
    while len(edges) < n - 1:
        u, v = random.randint(1, n), random.randint(1, n)
        if u != v and uf.union(u, v):
            edges.add(tuple(sorted((u, v))))
    
    # 2. 残りの辺を多重辺なしで追加
    while len(edges) < m:
        u, v = random.randint(1, n), random.randint(1, n)
        if u != v:
            edges.add(tuple(sorted((u, v))))
    return list(edges)
def main():
    # 例：N頂点M辺の連結グラフを出力する場合
    n = random.randint(N_MIN, N_MAX)
    m = random.randint(n - 1, min(M_MAX, n*(n-1)//2))
    
    # 配列の例
    # a = gen_unique_array(n, V_MIN, V_MAX)
    
    # グラフの生成
    edges = gen_connected_graph(n, m)
    
    # 標準出力 (問題の形式に合わせて調整)
    print(f"{n} {m}")
    for u, v in edges:
        # 重み付きなら random.randint を足す
        print(f"{u} {v}")

        
有向グラフで「$S \to T$ に到達可能」を DFS で確認する場合は、以下のような関数を追加して while ループで回すと良いです。
def can_reach(n, edges, start, target):
    adj = [[] for _ in range(n + 1)]
    for u, v in edges: adj[u].append(v)
    stack = [start]
    visited = {start}
    while stack:
        curr = stack.pop()
        if curr == target: return True
        for nxt in adj[curr]:
            if nxt not in visited:
                visited.add(nxt)
                stack.append(nxt)
    return False

"""


import random

n=random.randint(1,10000)
print(n)

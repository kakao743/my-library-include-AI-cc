// verified:

// DP
// https://atcoder.jp/contests/abc460/submissions/76770165
// https://atcoder.jp/contests/joisp2024/submissions/76770176
// https://atcoder.jp/contests/abc351/submissions/76770172

// Rerooting
// https://atcoder.jp/contests/abc460/submissions/76770157

// DPwithEdge
// https://judge.yosupo.jp/submission/380341

// RerootingWithEdge
// https://judge.yosupo.jp/submission/380340

// 応用
// https://atcoder.jp/contests/ndpc/submissions/76780586

// template:

// 辺情報なし
// struct Cluster {
//     struct Path {};
//     struct Point {};
//     struct Info {};
//     static Path compress(const Path& p, const Path& c) {}
//     static Point rake(const Point& a, const Point& b) {}
//     static Path add_vertex(const Point& x, const Info& i) {}
//     static Point add_edge(const Path& x) {}
//     static Path vertex(const Info& i) {}
// };

// 辺情報あり
// struct Cluster {
//     struct Path {};
//     struct Point {};
//     struct Info {};
//     struct Edge {};
//     static Path compress(const Path& p, const Path& c, const Edge& e) {}
//     static Point rake(const Point& a, const Point& b) {}
//     static Path add_vertex(const Point& x, const Info& i) {}
//     static Point add_edge(const Path& x, const Edge& e) {}
//     static Path vertex(const Info& i) {}
// };
//(1) 二つの部分木にHeavy Edgeを追加し、1つの部分木にする（Compress)
//(2) 部分木の根のvirtualな頂点を、通常の頂点に置き換える（Add Vertex)
//(3) virtualな頂点を根とする二つの部分木を、virtualな頂点を合体させることで一つの部分木にする（Rake[1]）
//(4) 実際の頂点を根とする部分木に、Light Edgeとvirtualな頂点を追加し、そのvirtualな頂点を根とする部分木にする (Add Edge)
//(5) 何もない状態から、実際の頂点を追加する (Vertex)
//Path: 縦方向にマージ可能な、あるパスとその周囲の情報を表す構造体。
//Point: ある頂点から生えているLight Edgeたちの情報を横方向にまとめた構造体
//Info: 各頂点が元々持っている固有の情報（重みなど)
//Edge: 各辺が元々持っている固有の情報（コストなど)
namespace STT {

struct StaticTopTree {
    enum OpType { Compress, Rake, AddVertex, AddEdge, Vertex };

    vector<vector<int>> G;
    int n = G.size(), root, stt_root, siz = n;
    vector<int> P, L, R, E, e_to_v;
    vector<OpType> T;

    StaticTopTree() = default;
    explicit StaticTopTree(const vector<vector<int>> &g, int r = 0) :
        G(g), root(r), P(4 * n), L(4 * n), R(4 * n), E(4 * n), e_to_v(n), T(4 * n)
    {
        dfs_hld(root);
        stt_root = compress(root).second;
    }

    int dfs_hld(int u) {
        int s = 1, best = 0;
        for (auto &v : G[u]) {
            erase(G[v], u);
            int c = dfs_hld(v);
            s += c;
            if (best < c) best = c, swap(v, G[u][0]);
        }
        return s;
    }

    int add(int k, int l, int r, OpType t, int e = -1) {
        if (k == -1) k = siz++;
        P[k] = -1; L[k] = l; R[k] = r; T[k] = t;
        if (l != -1) P[l] = k;
        if (r != -1) P[r] = k;
        if (e != -1) {
            E[k] = e;
            e_to_v[e] = k;
        }
        return k;
    }

    pair<int, int> compress(int u) {
        // https://atcoder.jp/contests/abc351/editorial/9910
        vector<pair<pair<int, int>, int>> stk;

        auto merge = [&] () {
            auto [y, er] = stk.back(); stk.pop_back();
            auto [x, el] = stk.back(); stk.pop_back();
            stk.emplace_back(make_pair(max(x.first, y.first) + 1, add(-1, x.second, y.second, Compress, er)), el);
        };

        while (true) {
            stk.emplace_back(add_vertex(u), u);
            while (true) {
                int l = stk.size();
                if (l >= 3 && (stk[l - 3].first.first == stk[l - 2].first.first || stk[l - 3].first.first <= stk[l - 1].first.first)) {
                    auto back = stk.back(); stk.pop_back();
                    merge();
                    stk.emplace_back(back);
                } else if (l >= 2 && stk[l - 2].first.first <= stk[l - 1].first.first) {
                    merge();
                } else break;
            }
            if (G[u].size() == 0) break;
            u = G[u][0];
        }

        while (stk.size() > 1) merge();
        return stk[0].first;
    }

    pair<int, int> rake(int u) {
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        for (auto v : G[u] | views::drop(1)) pq.emplace(add_edge(v));
        while (pq.size() > 1) {
            auto [h1, l] = pq.top(); pq.pop();
            auto [h2, r] = pq.top(); pq.pop();
            pq.emplace(max(h1, h2) + 1, add(-1, l, r, Rake));
        }
        return pq.top();
    }

    pair<int, int> add_edge(int u) {
        auto [h, v] = compress(u);
        return {h + 1, add(-1, v, -1, AddEdge, u)};
    }

    pair<int, int> add_vertex(int u) {
        if (G[u].size() > 1) {
            auto [h, v] = rake(u);
            return {h + 1, add(u, v, -1, AddVertex)};
        } else {
            return {1, add(u, -1, -1, Vertex)};
        }
    }
};

template <class Cluster, bool Rerootable = false>
struct StaticTopTreeDP {
    using STT = StaticTopTree;
    using Path = Cluster::Path;
    using Point = Cluster::Point;
    using Info = Cluster::Info;
    static constexpr auto compress = Cluster::compress;
    static constexpr auto rake = Cluster::rake;
    static constexpr auto add_vertex = Cluster::add_vertex;
    static constexpr auto add_edge = Cluster::add_edge;
    static constexpr auto vertex = Cluster::vertex;

    STT stt;
    vector<Path> path;
    conditional_t<Rerootable, vector<Path>, monostate> rpath;
    vector<Point> point;
    vector<Info> info;

    StaticTopTreeDP() = default;
    explicit StaticTopTreeDP(const vector<vector<int>> &g, const vector<Info> &i, int r = 0) :
        stt(STT(g, r)), path(stt.siz), point(stt.siz), info(i)
    {
        if constexpr (Rerootable) rpath.resize(stt.siz);
        init(stt.stt_root);
    }

    void set(int u, const Info &i) {
        info[u] = i;
        while (u != -1) {
            update(u);
            u = stt.P[u];
        }
    }

    Path prod() { return path[stt.stt_root]; }

    Path prod(int u) requires Rerootable {
        if (u == stt.root) return path[stt.stt_root];

        vector<bool> dirs;
        for (int k = u; k != stt.stt_root; k = stt.P[k]) {
            int p = stt.P[k];
            dirs.push_back(stt.L[p] != k);
        }
        ranges::reverse(dirs);

        optional<Path> pl, pr;
        optional<Point> po;
        int v;

        int k = stt.stt_root;
        for (auto dir : dirs) {
            switch (stt.T[k]) {
                case STT::Compress: {
                    if (dir) pl = pl ? compress(rpath[stt.L[k]], *pl) : rpath[stt.L[k]];
                    else pr = pr ? compress(path[stt.R[k]], *pr) : path[stt.R[k]];
                    break;
                }
                case STT::AddVertex: {
                    if (pl || pr) {
                        po = pl && pr ? rake(add_edge(*pl), add_edge(*pr)) : 
                            pl ? add_edge(*pl) : add_edge(*pr);
                        pl = pr = nullopt;
                    }
                    v = k;
                    break;
                }
                case STT::Rake: {
                    po = rake(*po, point[(dir ? stt.L : stt.R)[k]]);
                    break;
                }
                case STT::AddEdge: {
                    pl = add_vertex(*po, info[v]);
                    po = nullopt;
                    break;
                }
            }
            k = (dir ? stt.R : stt.L)[k];
        }

        po = pl && pr ? rake(add_edge(*pl), add_edge(*pr)) : 
            pl ? add_edge(*pl) : add_edge(*pr);
        pl = pr = nullopt;
        if (stt.T[k] == STT::AddVertex) po = rake(*po, point[stt.L[k]]);
        return add_vertex(*po, info[k]);
    }

    void init(int u) {
        if (u == -1) return;
        init(stt.L[u]); init(stt.R[u]);
        update(u);
    }

    void update(int k) {
        switch (stt.T[k]) {
            case STT::Compress: {
                path[k] = compress(path[stt.L[k]], path[stt.R[k]]);
                if constexpr (Rerootable) rpath[k] = compress(rpath[stt.R[k]], rpath[stt.L[k]]);
                break;
            }
            case STT::Rake: point[k] = rake(point[stt.L[k]], point[stt.R[k]]); break;
            case STT::AddVertex: {
                path[k] = add_vertex(point[stt.L[k]], info[k]);
                if constexpr (Rerootable) rpath[k] = path[k];
                break;
            }
            case STT::AddEdge: point[k] = add_edge(path[stt.L[k]]); break;
            case STT::Vertex: {
                path[k] = vertex(info[k]);
                if constexpr (Rerootable) rpath[k] = path[k];
                break;
            }
        }
    }
};

template <class Cluster, bool Rerootable = false>
struct StaticTopTreeDPwithEdge {
    using STT = StaticTopTree;
    using Path = Cluster::Path;
    using Point = Cluster::Point;
    using Info = Cluster::Info;
    using Edge = Cluster::Edge;
    static constexpr auto compress = Cluster::compress;
    static constexpr auto rake = Cluster::rake;
    static constexpr auto add_vertex = Cluster::add_vertex;
    static constexpr auto add_edge = Cluster::add_edge;
    static constexpr auto vertex = Cluster::vertex;

    STT stt;
    vector<int> eidx;
    vector<Path> path;
    conditional_t<Rerootable, vector<Path>, monostate> rpath;
    vector<Point> point;
    vector<Info> info;
    vector<Edge> edge;

    StaticTopTreeDPwithEdge() = default;
    explicit StaticTopTreeDPwithEdge(const vector<vector<pair<int, int>>> &g, const vector<Info> &i, const vector<Edge> &e, int r = 0) :
        eidx(g.size() - 1), info(i)
    {
        vector<vector<int>> G(g.size());
        auto dfs = [&] (auto self, int u, int p) -> void {
            for (auto [v, i] : g[u]) {
                if (v == p) continue;
                eidx[i] = v;
                G[u].emplace_back(v);
                self(self, v, u);
            }
        };
        dfs(dfs, r, -1);
        stt = STT(G, r);
        path.resize(stt.siz); point.resize(stt.siz);
        if constexpr (Rerootable) rpath.resize(stt.siz);
        edge.resize(stt.n);
        for (int i = 0; i < stt.n - 1; i++) edge[eidx[i]] = e[i];
        init(stt.stt_root);
    }

    void set(int u, const Info &i) {
        info[u] = i;
        while (u != -1) {
            update(u);
            u = stt.P[u];
        }
    }

    void set_edge(int u, const Edge &e) {
        u = eidx[u];
        edge[u] = e;
        u = stt.e_to_v[u];
        while (u != -1) {
            update(u);
            u = stt.P[u];
        }
    }

    Path prod() { return path[stt.stt_root]; }

    Path prod(int u = -1) requires Rerootable {
        if (u == -1) u = stt.root;
        if (u == stt.root) return path[stt.stt_root];

        vector<bool> dirs;
        for (int k = u; k != stt.stt_root; k = stt.P[k]) {
            int p = stt.P[k];
            dirs.push_back(stt.L[p] != k);
        }
        ranges::reverse(dirs);

        optional<Path> pl, pr;
        optional<Point> po;
        int v, el, er;

        int k = stt.stt_root;
        for (auto dir : dirs) {
            switch (stt.T[k]) {
                case STT::Compress: {
                    if (dir) {
                        pl = pl ? compress(rpath[stt.L[k]], *pl, edge[el]) : rpath[stt.L[k]];
                        el = stt.E[k];
                    } else {
                        pr = pr ? compress(path[stt.R[k]], *pr, edge[er]) : path[stt.R[k]];
                        er = stt.E[k];
                    }
                    break;
                }
                case STT::AddVertex: {
                    if (pl || pr) {
                        po = pl && pr ? rake(add_edge(*pl, edge[el]), add_edge(*pr, edge[er])) : 
                            pl ? add_edge(*pl, edge[el]) : add_edge(*pr, edge[er]);
                        pl = pr = nullopt;
                    }
                    v = k;
                    break;
                }
                case STT::Rake: {
                    po = rake(*po, point[(dir ? stt.L : stt.R)[k]]);
                    break;
                }
                case STT::AddEdge: {
                    pl = add_vertex(*po, info[v]);
                    el = stt.E[k];
                    po = nullopt;
                    break;
                }
            }
            k = (dir ? stt.R : stt.L)[k];
        }

        po = pl && pr ? rake(add_edge(*pl, edge[el]), add_edge(*pr, edge[er])) : 
            pl ? add_edge(*pl, edge[el]) : add_edge(*pr, edge[er]);
        pl = pr = nullopt;
        if (stt.T[k] == STT::AddVertex) po = rake(*po, point[stt.L[k]]);
        return add_vertex(*po, info[k]);
    }

    void init(int u) {
        if (u == -1) return;
        init(stt.L[u]); init(stt.R[u]);
        update(u);
    }

    void update(int k) {
        switch (stt.T[k]) {
            case STT::Compress: {
                path[k] = compress(path[stt.L[k]], path[stt.R[k]], edge[stt.E[k]]);
                if constexpr (Rerootable) rpath[k] = compress(rpath[stt.R[k]], rpath[stt.L[k]], edge[stt.E[k]]);
                break;
            }
            case STT::Rake: point[k] = rake(point[stt.L[k]], point[stt.R[k]]); break;
            case STT::AddVertex: {
                path[k] = add_vertex(point[stt.L[k]], info[k]);
                if constexpr (Rerootable) rpath[k] = path[k];
                break;
            }
            case STT::AddEdge: point[k] = add_edge(path[stt.L[k]], edge[stt.E[k]]); break;
            case STT::Vertex: {
                path[k] = vertex(info[k]);
                if constexpr (Rerootable) rpath[k] = path[k];
                break;
            }
        }
    }
};

} // namespace STT

using namespace STT;

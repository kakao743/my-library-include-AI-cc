template<class TreeDPInfo>
struct Rerooting{
  using Path=TreeDPInfo::Path;
  using Point=TreeDPInfo::Point;
  using Info=TreeDPInfo::Info;
  using EdgeInfo=TreeDPInfo::EdgeInfo;
  int n;
  vector<vector<pair<int,EdgeInfo>>> G;
  vector<Info> info;
  vector<Path> dp_down;//親pからみて、子cから流れてくる情報(+-o)
  vector<Path> ans;//頂点uを根としたときの答え
  Rerooting(const int _n,const vector<Info>&info):n(_n),G(n),info(info),dp_down(n),ans(n){}
  void add_edge(int u,int v,EdgeInfo w){G[u].emplace_back(pair(v,w));G[v].emplace_back(pair(u,w));}
  void dfs_down(int u,int p){
    Point ch=TreeDPInfo::point_e();
    for(auto [v,w]:G[u]){
      if(v==p)continue;
      dfs_down(v,u);
      ch=TreeDPInfo::rake(ch,TreeDPInfo::add_edge(dp_down[v],w));
    }
    dp_down[u]=TreeDPInfo::add_vertex(ch,info[u]);
  }
  void dfs_up(int u,int p,Point p_val){
    int deg=G[u].size();
    vector<Point> left(deg+1,TreeDPInfo::point_e());
    vector<Point> right(deg+1,TreeDPInfo::point_e());
    for(int i=0;i<deg;i++){
      auto [v,w]=G[u][i];
      Point cur_val=(v==p ? p_val:TreeDPInfo::add_edge(dp_down[v],w));
      left[i+1]=TreeDPInfo::rake(left[i],cur_val);
    }
    for(int i=deg-1;i>=0;i--){
      auto [v,w]=G[u][i];
      Point cur_val=(v==p ? p_val:TreeDPInfo::add_edge(dp_down[v],w));
      right[i]=TreeDPInfo::rake(right[i+1],cur_val);
    }
    ans[u]=TreeDPInfo::add_vertex(left[deg],info[u]);
    for(int i=0;i<deg;i++){
      auto [v,w]=G[u][i];
      if(v==p)continue;
      Point v_except=TreeDPInfo::rake(left[i],right[i+1]);
      Point to_v=TreeDPInfo::add_edge(TreeDPInfo::add_vertex(v_except,info[u]),w);
      dfs_up(v,u,to_v);
    }
  }
  vector<Path> solve(int root=0){
    dfs_down(root,-1);
    dfs_up(root,-1,TreeDPInfo::point_e());
    return ans;
  }
  vector<Path> solve_nomal(int root=0){
    dfs_down(root,-1);
    return dp_down;
  }
};
struct TreeDPInfo {
  struct Path{
    int len, ans, up, down;
  };//+-o
  struct Point{
    int ans, dist;
  };//-o
  using Info=bool;//the given value of each vartex
  using EdgeInfo=int;
  static Path compress(const Path& p, const Path& c){
    return path_e(); //rerooting->dummy
  };
  static Point rake(const Point& a, const Point& b){
    return {max({a.ans, b.ans, a.dist + b.dist}), max(a.dist, b.dist)};
  };
  static Point add_edge(const Path& a,EdgeInfo e){
    return {a.ans, a.down + e};
  };
  static Path add_vertex(const Point& a, Info x){
    return {0, max(a.ans, (x ? a.dist : -INF)), max(a.dist, (x ? 0 : -INF)), max(a.dist, (x ? 0 : -INF))};
  };
  static Path path_e(){
    return {-1, -INF, -INF, -INF};
  }
  static Point point_e(){
    return {-INF, -INF};
  }
};

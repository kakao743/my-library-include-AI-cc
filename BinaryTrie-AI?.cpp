template<typename T>
struct BinaryTrie{
  struct Node{
    int ch[2];
    int cnt;//このNodeを通過している要素数
    Node():cnt(0){ch[0]=ch[1]=-1;}
  };
  vector<Node> nodes;
  int max_bit;
  T lazy_xor;//集合全体にかかっているxor
  BinaryTrie(int max_bit=30):max_bit(max_bit),lazy_xor(0),nodes(1){}
  int size() const{return nodes[0].cnt;}
  bool empty(){return size()==0;}
  void xor_all(T x){
    lazy_xor^=x;
  }
  void insert(T x){
    x^=lazy_xor;
    int v=0;
    nodes[v].cnt++;
    for(int i=max_bit;i>=0;i--){
      int bit=(x>>i)&1;
      if(nodes[v].ch[bit]==-1){
        nodes[v].ch[bit]=nodes.size();
        nodes.emplace_back();
      }
      v=nodes[v].ch[bit];
      nodes[v].cnt++;
    }
  }
  void erase(T x,bool all=false){
    int num=count(x);
    if(num<=0)return;
    int erase_cnt=(all? num:1);
    x^=lazy_xor;
    int v=0;
    nodes[v].cnt-=erase_cnt;
    for(int i=max_bit;i>=0;i--){
      int bit=(x>>i) & 1;
      v=nodes[v].ch[bit];
      nodes[v].cnt-=erase_cnt;
    }
  }
  int count(T x){
    T target=x^lazy_xor;
    int v=0;
    for(int i=max_bit;i>=0;i--){
      int bit=(target>>i)&1;
      if(nodes[v].ch[bit]==-1 or nodes[nodes[v].ch[bit]].cnt==0)return 0;
      v=nodes[v].ch[bit];
    }
    return nodes[v].cnt;
  }
  bool find(T x){return count(x);}
  bool contains(T x){return count(x);}
  T get_kth_with_xor(T x,int k){
    //xとxorした中でのk番目に小さい値の生を返す
    //実際にこの値とxをxorした値が知りたければふつうに外部でxorしろ
    //0-indexed
    if(k<0 or k>=size())return -1;
    int v=0;
    T res=0;
    x^=lazy_xor;
    for(int i=max_bit;i>=0;i--){
      int bit=(x>>i) & 1;
      int min_child=nodes[v].ch[bit];
      int min_child_cnt=(min_child==-1 ? 0:nodes[min_child].cnt);
      if(k<min_child_cnt){
        v=min_child;
      }else {
        k-=min_child_cnt;
        v=nodes[v].ch[!bit];
        res|=T(1)<<i;
      }
      if(v==-1)return -1;
    }
    return res^x;
  }
  T get_kth(int k){return get_kth_with_xor(T(0),k);}
  T min_element(){return get_kth(0);}
  T max_element(){return get_kth(size()-1);}
  T min_element_with_xor(T x){return get_kth_with_xor(x,0);}
  T max_element_with_xor(T x){return get_kth_with_xor(x,size()-1);}
  int count_less_with_xor(T x,T k){
    //xとxorさせた状態でk未満の個数を返す
    int v=0;
    int res_cnt=0;
    for(int i=max_bit;i>=0;i--){
      if(v==-1)break;
      int bit=(x>>i)&1;
      int bit2=(k>>i)&1;
      if(bit2==1){
        //kのi桁目が1なら、xとのxorのi桁目が0のやつは全部k未満
        int min_child=nodes[v].ch[bit];
        res_cnt+=(min_child==-1 ? 0:nodes[min_child].cnt);
        v=nodes[v].ch[!bit];
      }else {
        v=nodes[v].ch[bit];
      }
    }
    return res_cnt;
  }
  int count_less(T x){return count_less_with_xor(T(0),x);}
  int lower_bound_with_xor(T x,T k){return count_less_with_xor(x,k);}
  int lower_bound(T x){return count_less_with_xor(T(0),x);}
  int upper_bound_with_xor(T x,T k){return count_less_with_xor(x,k+1);}
  int upper_bound(T x){return count_less_with_xor(T(0),x);}
  T operator[](int k){return get_kth(k);}
};
/*
roots(N+1)でroots[i+1]=trie.insert(roots[i],x)の様に更新
node_L=roots[L],node_R=roots[R+1](0-indexed)
*/
template<typename T>
struct ParsistentBinaryTrie{
  struct Node{
    int ch[2];
    int cnt;
    Node():cnt(0){ch[0]=ch[1]=-1;}
  };
  vector<Node> nodes;
  int max_bit;
  ParsistentBinaryTrie(int n,int max_bit=30):max_bit(max_bit){
    //n=挿入予定の数
    nodes.reserve(n*(max_bit+2));
    nodes.emplace_back();
  }
  //要素を追加して新しいルート番号を返す
  int insert(int old_root,T x){
    return insert(old_root,x,max_bit);
  }
  //区間[node_l,node_r]で(A[i]^x)<kとなるものの個数
  int count_less_with_xor(int node_L,int node_R,T x,T k){
    int vL=node_L,vR=node_R;
    int res_cnt=0;
    for(int i=max_bit;i>=0;i--){
      if(vR==-1)break;
      int bit_x=(x>>i)&1;
      int bit_k=(k>>i)&1;
      if(bit_k==1){
        int min_child_L=(vL==-1 ? -1:nodes[vL].ch[bit_x]);
        int min_child_R=nodes[vR].ch[bit_x];
        res_cnt+=get_cnt(min_child_R)-get_cnt(min_child_L);
        vL=(vL==-1 ? -1:nodes[vL].ch[!bit_x]);
        vR=nodes[vR].ch[!bit_x];
      }else {
        vL=(vL==-1 ? -1:nodes[vL].ch[bit_x]);
        vR=nodes[vR].ch[bit_x];
      }
    }
    return res_cnt;
  }
  T get_kth_with_xor(int node_L,int node_R,T x,int k){
    if(k<0 or k>=(nodes[node_R].cnt-get_cnt(node_L)))return -1;
    int vL=node_L,vR=node_R;
    T res=0;
    for(int i=max_bit;i>=0;i--){
      int bit=(x>>i)&1;
      int m_L=(vL==-1 ? -1:nodes[vL].ch[bit]);
      int m_R=(vR==-1 ? -1:nodes[vR].ch[bit]);
      int m_cnt=get_cnt(m_R)-get_cnt(m_L);
      if(k<m_cnt){
        vL=m_L,vR=m_R;
      }else {
        k-=m_cnt;
        vL=(vL==-1 ? -1:nodes[vL].ch[!bit]);
        vR=(vR==-1 ? -1:nodes[vR].ch[!bit]);
        res|=T(1)<<i;
      }
    }
    return res^x;
  }
  T min_element_with_xor(int nL, int nR, T x) {
    return get_kth_with_xor(nL, nR, 0, x);
  }
  inline int get_cnt(int v) const {return (v==-1 ? 0:nodes[v].cnt);}
  int insert(int old_v,T x,int bit){
    int newv=nodes.size();
    nodes.push_back(old_v==-1 ? Node():nodes[old_v]);
    nodes[newv].cnt++;
    if(bit<0)return newv;
    int b=(x>>bit)&1;
    int next_new_v=insert((old_v==-1 ? -1:nodes[old_v].ch[b]),x,bit-1);
    nodes[newv].ch[b]=next_new_v;
    return newv;
  }
};

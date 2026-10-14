template <typename S, auto op, auto e>
struct Segtree{
  //indexで(1)-(2,3)-(4,5,6,7)-(8,9,10,11,12,13,14,15)・・・
  //参考:https://qiita.com/AkariLuminous/items/32cbf5bc3ffb2f84a898
  private:
    int n,sz;
    vector<S> seg;
    void update(int k){seg[k]=op(seg[k<<1],seg[k<<1|1]);}
  public:
    explicit Segtree(int n):Segtree(vector<S>(n,e())){}
    explicit Segtree(const vector<S> &v):n(v.size()),sz(std::bit_ceil(unsigned(n))),seg(sz<<1,e()){
      for(int i=0;i<n;i++)seg[sz+i]=v[i];
      for(int i=sz-1;i>=1;i--)update(i);
    }
    void set(int k,S x){
      assert(0<=k and k<n);
      int ima=sz+k;
      seg[ima]=x;
      while(ima>1)update(ima>>=1);
    }
    S get(int k) const{
      assert(0<=k and k<n);
      return seg[sz+k];
    }
    S prod(int l,int r) const{
      assert(0<=l and l<=r && r<=n);
      l+=sz,r+=sz;
      S vl=e(),vr=e();
      while(l<r){
        if(l&1)vl=op(vl,seg[l++]);
        if(r&1)vr=op(seg[--r],vr);
        l>>=1,r>>=1;
      }
      return op(vl,vr);
    }
    S all_prod() const{return seg[1];}
    template <bool (*f)(S)> int max_right(int l) const {
        return max_right(l, [](S x) { return f(x); });
    }
    template <class F> int max_right(int l, F f) const {
        assert(0 <= l && l <=n);
        assert(f(e()));
        if (l == n) return n;
        l += sz;
        S sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(op(sm, seg[l]))) {
                while (l < sz) {
                    l = (2 * l);
                    if (f(op(sm, seg[l]))) {
                        sm = op(sm, seg[l]);
                        l++;
                    }
                }
                return l - sz;
            }
            sm = op(sm, seg[l]);
            l++;
        } while ((l & -l) != l);
        return n;
    }

    template <bool (*f)(S)> int min_left(int r) const {
        return min_left(r, [](S x) { return f(x); });
    }
    template <class F> int min_left(int r, F f) const {
        assert(0 <= r && r <= n);
        assert(f(e()));
        if (r == 0) return 0;
        r += sz;
        S sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(op(seg[r], sm))) {
                while (r < sz) {
                    r = (2 * r + 1);
                    if (f(op(seg[r], sm))) {
                        sm = op(seg[r], sm);
                        r--;
                    }
                }
                return r + 1 - sz;
            }
            sm = op(seg[r], sm);
        } while ((r & -r) != r);
        return 0;
    }
};

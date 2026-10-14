template<typename mint,int CIRC_N>
struct CircularFPS{
  static_assert(CIRC_N>0,"CIRC_N must be positive");
  vector<mint> a;
  CircularFPS():a(CIRC_N,mint(0)){}
  CircularFPS(initializer_list<mint> v):a(CIRC_N,mint(0)){int i=0;for(auto x:v){a[i%CIRC_N]+=x;++i;}}
  CircularFPS(const vector<mint>& v):a(CIRC_N,mint(0)){for(int i=0;i<(int)v.size();++i)a[i%CIRC_N]+=v[i];}
  CircularFPS(const FPS<mint>& f):a(CIRC_N,mint(0)){for(int i=0;i<(int)f.size();++i)a[i%CIRC_N]+=f[i];}
  mint& operator[](int i){return a[i];}
  const mint& operator[](int i)const{return a[i];}
  int size()const{return CIRC_N;}
  FPS<mint> to_fps()const{return FPS<mint>(a);}
  CircularFPS operator-()const{CircularFPS r;for(int i=0;i<CIRC_N;++i)r.a[i]=-a[i];return r;}
  CircularFPS operator+(const CircularFPS& r)const{return CircularFPS(*this)+=r;}
  CircularFPS operator-(const CircularFPS& r)const{return CircularFPS(*this)-=r;}
  CircularFPS operator*(const mint& v)const{return CircularFPS(*this)*=v;}
  CircularFPS operator/(const mint& v)const{return CircularFPS(*this)/=v;}
  CircularFPS& operator+=(const CircularFPS& r){for(int i=0;i<CIRC_N;++i)a[i]+=r.a[i];return*this;}
  CircularFPS& operator-=(const CircularFPS& r){for(int i=0;i<CIRC_N;++i)a[i]-=r.a[i];return*this;}
  CircularFPS& operator*=(const mint& v){for(int i=0;i<CIRC_N;++i)a[i]*=v;return*this;}
  CircularFPS& operator/=(const mint& v){assert(v!=mint(0));mint iv=v.inv();for(int i=0;i<CIRC_N;++i)a[i]*=iv;return*this;}
  CircularFPS operator*(const CircularFPS& r)const{return CircularFPS(*this)*=r;}
  CircularFPS& operator*=(const CircularFPS& r){ // 巡回畳み込み mod x^N-1
    FPS<mint> fc=FPS<mint>(a)*FPS<mint>(r.a);
    fill(a.begin(),a.end(),mint(0));
    for(int i=0;i<(int)fc.size();++i)a[i%CIRC_N]+=fc[i];
    return*this;
  }
  bool operator==(const CircularFPS& r)const{return a==r.a;}
  bool operator!=(const CircularFPS& r)const{return a!=r.a;}
  CircularFPS operator+(const mint& v)const{CircularFPS r(*this);r.a[0]+=v;return r;}
  CircularFPS operator-(const mint& v)const{CircularFPS r(*this);r.a[0]-=v;return r;}
  CircularFPS& operator+=(const mint& v){a[0]+=v;return*this;}
  CircularFPS& operator-=(const mint& v){a[0]-=v;return*this;}
  CircularFPS shift(int k)const{ // 左巡回シフト k 個
    k=((k%CIRC_N)+CIRC_N)%CIRC_N;
    CircularFPS r;for(int i=0;i<CIRC_N;++i)r.a[(i+k)%CIRC_N]=a[i];return r;
  }
  static tuple<FPS<mint>,FPS<mint>,FPS<mint>> _poly_extgcd(FPS<mint> f,FPS<mint> g){ // 多項式拡張ユークリッド互除法 O(N^2)
    auto trim=[](FPS<mint>& p){while(!p.empty()&&p.back()==mint(0))p.pop_back();};
    trim(f);trim(g);
    FPS<mint> u0={mint(1)},v0={},u1={},v1={mint(1)},f_cur=f,g_cur=g;
    while(!g_cur.empty()){
      auto[qq,rr]=div_fps(f_cur,g_cur);
      FPS<mint> q=qq,r=rr;trim(r);
      FPS<mint> nu=u0-q*u1,nv=v0-q*v1;trim(nu);trim(nv);
      f_cur=g_cur;u0=u1;v0=v1;g_cur=r;u1=nu;v1=nv;
    }
    trim(f_cur);
    if(!f_cur.empty()&&f_cur.back()!=mint(1)){
      mint li=f_cur.back().inv();
      for(auto& c:f_cur)c*=li;for(auto& c:u0)c*=li;for(auto& c:v0)c*=li;
    }
    return{f_cur,u0,v0};
  }
  CircularFPS inv()const{ // mod x^N-1 での逆元 (extgcd)
    FPS<mint> mod(CIRC_N+1,mint(0));mod[CIRC_N]=mint(1);mod[0]=mint(-1);
    FPS<mint> fa(a);while(!fa.empty()&&fa.back()==mint(0))fa.pop_back();
    assert(!fa.empty());
    auto[d,u,v]=_poly_extgcd(fa,mod);
    assert(d.size()==1&&d[0]==mint(1));
    CircularFPS r;for(int i=0;i<(int)u.size();++i)r.a[i%CIRC_N]+=u[i];return r;
  }
  CircularFPS operator/(const CircularFPS& r)const{return CircularFPS(*this)/=r;}
  CircularFPS& operator/=(const CircularFPS& r){return*this*=r.inv();}
  CircularFPS diff()const{ // 形式的微分
    CircularFPS r;for(int i=1;i<CIRC_N;++i)r.a[i-1]=a[i]*mint(i);return r;
  }
  CircularFPS integral()const{ // 形式的積分 (積分定数=0, a[N-1]/N はres[0]に加算しない)
    CircularFPS r;for(int i=0;i<CIRC_N-1;++i)r.a[i+1]=a[i]/mint(i+1);return r;
  }
  CircularFPS pow(long long e)const{ // 繰り返し二乗法
    CircularFPS r,b(*this);r.a[0]=mint(1);
    while(e>0){if(e&1)r*=b;b*=b;e>>=1;}return r;
  }
  CircularFPS log()const{ // log(f)=∫(f'/f) 条件: f[0]==1
    assert(a[0]==mint(1));return(diff()*inv()).integral();
  }
  CircularFPS exp()const{ // Newton反復 g<-g*(1+f-log(g)) 条件: f[0]==0
    assert(a[0]==mint(0));
    CircularFPS g;g.a[0]=mint(1);
    for(int len=1;len<CIRC_N;len<<=1){
      CircularFPS lg=g.log(),d=*this;d.a[0]+=mint(1);
      for(int i=0;i<CIRC_N;++i)d.a[i]-=lg.a[i];g*=d;
    }
    return g;
  }
  CircularFPS sqrt()const{ // Newton反復 g<-(g+f*g^{-1})/2 条件: f[0]==1
    assert(a[0]==mint(1));
    CircularFPS g;g.a[0]=mint(1);mint inv2=mint(2).inv();
    for(int len=1;len<CIRC_N;len<<=1)g=(g+(*this)*g.inv())*inv2;return g;
  }
  mint eval(const mint& v)const{ // Horner法による点評価
    mint r=mint(0);for(int i=CIRC_N-1;i>=0;--i)r=r*v+a[i];return r;
  }
  CircularFPS dft()const{vector<mint> v=a;_dft_impl(v,false);return CircularFPS(v);}
  CircularFPS idft()const{vector<mint> v=a;_dft_impl(v,true);return CircularFPS(v);}
  CircularFPS pointwise_mul(const CircularFPS& r)const{ // DFT領域での点ごとの積
    CircularFPS res;for(int i=0;i<CIRC_N;++i)res.a[i]=a[i]*r.a[i];return res;
  }
  friend istream& operator>>(istream& is,CircularFPS& f){for(int i=0;i<CIRC_N;++i)is>>f.a[i];return is;}
  friend ostream& operator<<(ostream& os,const CircularFPS& f){for(int i=0;i<CIRC_N;++i){if(i)os<<' ';os<<f.a[i];}return os;}
private:
  static void _dft_impl(vector<mint>& v,bool inv_flag){ // NTT butterfly (2^k) or 素朴DFT O(N^2)
    int n=(int)v.size();assert(n==CIRC_N);
    if(n>0&&(n&(n-1))==0){
      if(!inv_flag){NTT::butterfly(v);}
      else{NTT::butterfly_inv(v);mint in=mint(n).inv();for(auto& x:v)x*=in;}
    }else{
      int mod=mint::get_mod();
      long long gr=NTT::modpow(NTT::calc_primitive_root(mod),(long long)(mod-1)/n,mod);
      if(inv_flag)gr=NTT::modinv(gr,mod);
      vector<mint> res(n,mint(0));mint wj=mint(1);
      for(int j=0;j<n;++j){mint wjk=mint(1);for(int k=0;k<n;++k){res[j]+=v[k]*wjk;wjk*=wj;}wj*=mint(gr);}
      if(inv_flag){mint in=mint(n).inv();for(auto& x:res)x*=in;}
      v=move(res);
    }
  }
};

template<typename mint>
struct CircularFPSDyn{
  int N;vector<mint> a;
  CircularFPSDyn():N(0){}
  explicit CircularFPSDyn(int n):N(n),a(n,mint(0)){}
  CircularFPSDyn(int n,const vector<mint>& v):N(n),a(n,mint(0)){for(int i=0;i<(int)v.size();++i)a[i%n]+=v[i];}
  CircularFPSDyn(int n,const FPS<mint>& f):N(n),a(n,mint(0)){for(int i=0;i<(int)f.size();++i)a[i%n]+=f[i];}
  mint& operator[](int i){return a[i];}
  const mint& operator[](int i)const{return a[i];}
  int size()const{return N;}
  FPS<mint> to_fps()const{return FPS<mint>(a);}
  CircularFPSDyn operator-()const{CircularFPSDyn r(N);for(int i=0;i<N;++i)r.a[i]=-a[i];return r;}
  CircularFPSDyn& operator+=(const CircularFPSDyn& r){assert(N==r.N);for(int i=0;i<N;++i)a[i]+=r.a[i];return*this;}
  CircularFPSDyn& operator-=(const CircularFPSDyn& r){assert(N==r.N);for(int i=0;i<N;++i)a[i]-=r.a[i];return*this;}
  CircularFPSDyn& operator*=(const mint& v){for(int i=0;i<N;++i)a[i]*=v;return*this;}
  CircularFPSDyn& operator/=(const mint& v){mint iv=v.inv();for(int i=0;i<N;++i)a[i]*=iv;return*this;}
  CircularFPSDyn operator+(const CircularFPSDyn& r)const{return CircularFPSDyn(*this)+=r;}
  CircularFPSDyn operator-(const CircularFPSDyn& r)const{return CircularFPSDyn(*this)-=r;}
  CircularFPSDyn operator*(const mint& v)const{return CircularFPSDyn(*this)*=v;}
  CircularFPSDyn operator/(const mint& v)const{return CircularFPSDyn(*this)/=v;}
  CircularFPSDyn& operator*=(const CircularFPSDyn& r){ // 巡回畳み込み mod x^N-1
    assert(N==r.N);FPS<mint> fc=FPS<mint>(a)*FPS<mint>(r.a);
    fill(a.begin(),a.end(),mint(0));for(int i=0;i<(int)fc.size();++i)a[i%N]+=fc[i];return*this;
  }
  CircularFPSDyn operator*(const CircularFPSDyn& r)const{return CircularFPSDyn(*this)*=r;}
  CircularFPSDyn shift(int k)const{
    k=((k%N)+N)%N;CircularFPSDyn r(N);for(int i=0;i<N;++i)r.a[(i+k)%N]=a[i];return r;
  }
  static tuple<FPS<mint>,FPS<mint>,FPS<mint>> _poly_extgcd(FPS<mint> f,FPS<mint> g){ // 多項式拡張GCD O(N^2)
    auto trim=[](FPS<mint>& p){while(!p.empty()&&p.back()==mint(0))p.pop_back();};
    trim(f);trim(g);
    FPS<mint> u0={mint(1)},v0={},u1={},v1={mint(1)},f_cur=f,g_cur=g;
    while(!g_cur.empty()){
      auto[qq,rr]=div_fps(f_cur,g_cur);FPS<mint> q=qq,r=rr;trim(r);
      FPS<mint> nu=u0-q*u1,nv=v0-q*v1;trim(nu);trim(nv);
      f_cur=g_cur;u0=u1;v0=v1;g_cur=r;u1=nu;v1=nv;
    }
    trim(f_cur);
    if(!f_cur.empty()&&f_cur.back()!=mint(1)){
      mint li=f_cur.back().inv();for(auto& c:f_cur)c*=li;for(auto& c:u0)c*=li;for(auto& c:v0)c*=li;
    }
    return{f_cur,u0,v0};
  }
  CircularFPSDyn inv()const{ // mod x^N-1 での逆元 (extgcd)
    FPS<mint> mod(N+1,mint(0));mod[N]=mint(1);mod[0]=mint(-1);
    FPS<mint> fa(a);while(!fa.empty()&&fa.back()==mint(0))fa.pop_back();
    assert(!fa.empty());auto[d,u,v]=_poly_extgcd(fa,mod);
    assert(d.size()==1&&d[0]==mint(1));
    CircularFPSDyn r(N);for(int i=0;i<(int)u.size();++i)r.a[i%N]+=u[i];return r;
  }
  CircularFPSDyn operator/(const CircularFPSDyn& r)const{return CircularFPSDyn(*this)/=r;}
  CircularFPSDyn& operator/=(const CircularFPSDyn& r){return*this*=r.inv();}
  CircularFPSDyn diff()const{CircularFPSDyn r(N);for(int i=1;i<N;++i)r.a[i-1]=a[i]*mint(i);return r;}
  CircularFPSDyn integral()const{CircularFPSDyn r(N);for(int i=0;i<N-1;++i)r.a[i+1]=a[i]/mint(i+1);return r;}
  CircularFPSDyn pow(long long e)const{
    CircularFPSDyn r(N),b(*this);r.a[0]=mint(1);
    while(e>0){if(e&1)r*=b;b*=b;e>>=1;}return r;
  }
  CircularFPSDyn log()const{assert(a[0]==mint(1));return(diff()*inv()).integral();}
  CircularFPSDyn exp()const{ // Newton反復 g<-g*(1+f-log(g))
    assert(a[0]==mint(0));CircularFPSDyn g(N);g.a[0]=mint(1);
    for(int len=1;len<N;len<<=1){
      CircularFPSDyn lg=g.log(),d(N);
      for(int i=0;i<N;++i)d.a[i]=a[i];d.a[0]+=mint(1);
      for(int i=0;i<N;++i)d.a[i]-=lg.a[i];g*=d;
    }
    return g;
  }
  CircularFPSDyn sqrt()const{ // Newton反復 g<-(g+f*g^{-1})/2
    assert(a[0]==mint(1));CircularFPSDyn g(N);g.a[0]=mint(1);mint inv2=mint(2).inv();
    for(int len=1;len<N;len<<=1)g=(g+(*this)*g.inv())*inv2;return g;
  }
  mint eval(const mint& v)const{mint r=mint(0);for(int i=N-1;i>=0;--i)r=r*v+a[i];return r;}
  bool operator==(const CircularFPSDyn& r)const{return N==r.N&&a==r.a;}
  bool operator!=(const CircularFPSDyn& r)const{return!(*this==r);}
  friend istream& operator>>(istream& is,CircularFPSDyn& f){for(int i=0;i<f.N;++i)is>>f.a[i];return is;}
  friend ostream& operator<<(ostream& os,const CircularFPSDyn& f){for(int i=0;i<f.N;++i){if(i)os<<' ';os<<f.a[i];}return os;}
};

// 巡回畳み込み (任意N, mod x^N-1)
template<typename mint>
vector<mint> circular_conv(const vector<mint>& a,const vector<mint>& b,int N=-1){
  if(N<0)N=max((int)a.size(),(int)b.size());
  return(CircularFPSDyn<mint>(N,a)*CircularFPSDyn<mint>(N,b)).a;
}

// DFT領域での3点積 (NTT-friendly modのみ)
template<typename mint,int CIRC_N>
CircularFPS<mint,CIRC_N> dft3_product(const CircularFPS<mint,CIRC_N>& a,const CircularFPS<mint,CIRC_N>& b,const CircularFPS<mint,CIRC_N>& c){
  return a.dft().pointwise_mul(b.dft()).pointwise_mul(c.dft()).idft();
}

// Burnsideの補題による巡回等価類カウント
template<typename mint>
mint burnside_cyclic(int N,const vector<mint>& fixed_by_shift){
  assert((int)fixed_by_shift.size()==N);
  mint s=mint(0);for(int k=0;k<N;++k)s+=fixed_by_shift[k];return s/mint(N);
}

template<class S,auto op,auto e,auto inv,class T>
struct kDRuiseki{
  int D;//次元数
  vector<long long>sz;
  vector<long long>stride;//vec(2,3,4)なら12,4,1
  vector<S> dat;
  constexpr explicit kDRuiseki(const vector<long long>&dims):sz(dims),D(dims.size()),stride(D){
    long long prod=1;
    for(int i=D-1;i>=0;i--){stride[i]=prod;prod*=sz[i];}dat.resize(prod,e());
  }
  long long encode(const vector<long long>&a)const{
    long long res=0;
    for(int i=0;i<D;i++)res+=stride[i]*a[i];return res;
  }
  void set(const vector<long long>&a,S x){dat[encode(a)]=x;}
  S& get(const vector<long long>&a)const{return dat[encode(a)];}
  void apply(const vector<long long>&l,const vector<long long>&r,S val){
    for(int i=0;i<D;i++)if(l[i]>=r[i])return;
    for(int mask=0;mask<(1<<D);mask++){
      bool skip=false;
      int popbit=0;
      long long id=0;
      for(int i=0;i<D;i++){
        popbit^=(mask>>i&1);
        long long x;
        if(mask>>i&1){
          x=l[i];
          if(l[i]>=sz[i]){skip=true;break;}
        }else{
          x=r[i];
          if(r[i]>=sz[i]){skip=true;break;}
        }
        id+=stride[i]*x;
      }
      if(skip)continue;
      if(popbit)dat[id]=op(dat[id],inv(val));
      else dat[id]=op(dat[id],val);
    }
  }
  void build(){
    for(int k=0;k<D;k++)for(long long id=0;id<dat.size();id++){
      if(id/stride[k]%sz[k]!=0)dat[id]=op(dat[id],dat[id-stride[k]]);
    }
  }
  //[l,r)
  S prod(const vector<long long>&l,const vector<long long>&r)const{
    for(int i=0;i<D;i++)if(l[i]>=r[i])return e();
    S res=e();
    for(int mask=0;mask<(1<<D);mask++){
      bool skip=false;
      int popbit=0;
      long long id=0;
      for(int i=0;i<D;i++){
        popbit^=(mask>>i&1);
        long long x;
        if(mask>>i&1){
          x=l[i]-1;
          if(l[i]==0){skip=true;break;}
        }else{
          x=r[i]-1;
          if(r[i]==0){skip=true;break;}
        }
        id+=stride[i]*x;
      }
      if(skip)continue;
      if(popbit)res=op(res,inv(dat[id]));
      else res=op(res,dat[id]);
    }
    return res;
  }
};

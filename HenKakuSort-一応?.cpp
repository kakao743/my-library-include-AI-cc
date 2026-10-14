//偏角ソート、共線なら中心距離順
namespace HenKakuSort{
  weak_ordering comp1(const ll2&u,const ll2&v){
    auto islower_xbar=[](const ll2&p)->bool{
      return pair(p.b,p.a)<pair(0LL,0LL);
    };
    auto res=islower_xbar(u)<=>islower_xbar(v);
    if(res!=0)return res;
    auto cross_product=(u.b*v.a)<=>(u.a*v.b);
    return cross_product;
    //return (u.a*u.a+u.b*u.b)<=>(v.a*v.a+v.b*v.b);
  }
  weak_ordering comp2(const ll2&u,const ll2&v){
    if(comp1(u,v)==0)return (u.a*u.a+u.b*u.b)<=>(v.a*v.a+v.b*v.b);
    return comp1(u,v);
  }
  bool comp(const ll2&u,const ll2&v){
    return (comp2(u,v))<0;
  }
}

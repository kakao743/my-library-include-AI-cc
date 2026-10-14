template<typename T=__int128_t>
struct Decimal{
  T a,b;//a/b,bは正
  constexpr Decimal():a(T(0)),b(T(1)){}
  constexpr explicit Decimal(T a,T b):a(a),b(b){normalize();}
  constexpr explicit Decimal(double x){
    bool neg=false;
    if(x<0){neg=true;x=-x;}
    double r=x;
    T p0=0,q0=1,p1=1,q1=0,max_denominator=1000000000;//1e9以下に抑えて連分数展開
    while(r>1e-14){
      double a_i=std::floor(r);
      T a_t=static_cast<T>(a_i),p2=a_t*p1+p0,q2=a_t*q1+q0;
      if(q2>max_denominator)break;
      p0=p1;p1=p2;q0=q1;q1=q2;
      if(std::abs(r-a_i)<1e-15)break;
      r=1.0/(r-a_i);
    }
    this->a=p1;this->b=q1;
    if(neg)this->a=-this->a;
    normalize();
  }
  constexpr Decimal& normalize(){
    if(b<T(0)){b=-b,a=-a;}
    if(b==T(0)){a=(T(0)<a)-(a<T(0));return *this;}
    auto get_gcd = [](auto self,T x,T y) -> T {
      return y==0?x:self(self,y,x%y);
    };
    T d=get_gcd(get_gcd,(a<T(0)?-a:a),b);
    this->a/=d,this->b/=d;
    return *this;
  }
  constexpr bool operator==(const Decimal&oth)const{
    return this->a*oth.b==this->b*oth.a;
  }
  constexpr std::strong_ordering operator<=>(const Decimal&oth)const{
    return this->a*oth.b<=>this->b*oth.a;
  }
  constexpr Decimal operator+(const Decimal&oth)const{
    return Decimal(this->a*oth.b+oth.a*this->b,this->b*oth.b);
  }
  constexpr Decimal operator*(const Decimal&oth)const{
    return Decimal(this->a*oth.a,this->b*oth.b);
  }
  constexpr Decimal operator-(const Decimal&oth)const{
    return Decimal(this->a*oth.b-oth.a*this->b,this->b*oth.b);
  }
  constexpr Decimal operator/(const Decimal&oth)const{
    return Decimal(this->a*oth.b,this->b*oth.a);
  }
  constexpr double to_double()const{
    return double(a)/double(b);
  }
  friend std::ostream&operator<<(std::ostream&os,const Decimal&d){
    os<<d.a<<"/"<<d.b;return os;
  }
};

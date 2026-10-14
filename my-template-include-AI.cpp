#ifdef LOCAL
#include "my_header.h"
#else
#define PRAGMA_OPTIMIZE(s) _Pragma(#s)
PRAGMA_OPTIMIZE(GCC optimize("Ofast"))
PRAGMA_OPTIMIZE(GCC optimize("unroll-loops")) // ループ
#pragma GCC optimize("fast-math", "no-stack-protector")
// #pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")//浮動小数点 fmaやったらなんかerror?
#pragma GCC target("avx,avx2,avx512f,sse4.2,bmi,bmi2,popcnt,tune=native,mmx,abm")//四則演算
  #include <bits/stdc++.h>
  using namespace std;
  #if __has_include(<absl/container/btree_map.h>)
  #define HAS_ABSL 1
  #include <absl/container/btree_map.h>
  #include <absl/container/btree_set.h>
#else
  #define HAS_ABSL 0
#endif
#if __has_include(<boost/unordered/unordered_flat_map.hpp>)
  #define HAS_BOOST 1
  #include <boost/container/flat_map.hpp>
  #include <boost/container/flat_set.hpp>
  #include <boost/unordered/unordered_flat_map.hpp>
  #include <boost/unordered/unordered_flat_set.hpp>
#else
  #define HAS_BOOST 0
#endif
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#if __has_include(<atcoder/all>)
#include <atcoder/all>
using namespace atcoder;
using minta = modint998244353;
using mintb = modint1000000007;
std::ostream&operator<<(std::ostream& os,const minta& v){os << v.val();return os;}
std::istream&operator>>(std::istream&is,minta &v){long long t;is >> t;v=t;return is;}
std::ostream&operator<<(std::ostream& os,const mintb& v){os << v.val();return os;}
std::istream&operator>>(std::istream&is,mintb &v){long long t;is >> t;v=t;return is;}
istream &operator>>(istream &is, __int128_t &x) {
  string S;
  is >> S;
  x = 0;
  int flag = 0;
  for (auto &c : S) {
    if (c == '-') {
      flag = true;
      continue;
    }
    x *= 10;
    x += c - '0';
  }
  if (flag) x = -x;
  return is;
}
ostream &operator<<(ostream &os, __int128_t x) {
  if (x == 0) return os << 0;
  if (x < 0) os << '-', x = -x;
  string S;
  while (x) S.push_back('0' + x % 10), x /= 10;
  reverse(begin(S), end(S));
  return os << S;
}
template<typename T1,typename T2>ostream&operator<<(ostream&os,const pair<T1,T2>& v){os << '(' << v.first << ',' << v.second << ')';return os;}
template<typename T1,typename T2>istream&operator>>(istream&is,pair<T1,T2> &v){is >> v.first >> v.second;return is;}
#define HAS_ACL 1
#else
#define HAS_ACL 0
#endif
using ll=long long;
template <typename T1, typename T2>
struct ob2 {
    T1 a; T2 b;
    constexpr ob2() : a(), b() {}
    constexpr ob2(T1 a, T2 b) : a(a), b(b) {}
    friend auto operator<=>(const ob2&, const ob2&)=default;
    constexpr ob2& operator+=(const ob2& o) noexcept {
      a+=o.a;b+=o.b;return *this;
    }
    constexpr ob2& operator-=(const ob2& o) noexcept {
      a-=o.a;b-=o.b;return *this;
    }
    template<class I> constexpr ob2& operator*=(I k) noexcept {
      a*=k;b*=k;return *this;
    }
    template<class I> constexpr ob2& operator/=(I k) noexcept {
      a/=k;b/=k;return *this;
    }
    friend constexpr ob2 operator+(ob2 x, const ob2& y) noexcept {
      return x+=y;
    }
    friend constexpr ob2 operator-(ob2 x, const ob2& y) noexcept {
      return x-=y;
    }
    template<class I> friend constexpr ob2 operator*(ob2 x, I k) noexcept {
      return x*=k;
    }
    template<class I> friend constexpr ob2 operator*(I k, ob2 x) noexcept {
      return x*=k;
    }
    template<class I> friend constexpr ob2 operator/(ob2 x, I k) noexcept {
      return x/=k;
    }
    template<class I> constexpr auto& operator[](I i) noexcept{
      assert(0<=i && i<2);
      return (i==0?a:b);
    }
    template<class I> constexpr const auto& operator[](I i) const noexcept {
      assert(0<=i && i<2);
      return (i==0?a:b);  
    }
    friend std::ostream& operator<<(std::ostream& os, const ob2& p) { 
        return os << "(" << p.a << ", " << p.b << ")"; 
    }
    friend std::istream& operator>>(std::istream& is, ob2& p) { 
        return is >> p.a >> p.b; 
    }
};

template <typename T1, typename T2, typename T3>
struct ob3 {
    T1 a; T2 b; T3 c;
    ob3() : a(), b(), c() {}
    ob3(T1 a, T2 b, T3 c) : a(a), b(b), c(c) {}
    friend auto operator<=>(const ob3&, const ob3&) = default;
    ob3 operator+(const ob3& o) const { return {a + o.a, b + o.b, c + o.c}; }
    ob3 operator-(const ob3& o) const { return {a - o.a, b - o.b, c - o.c}; }
    template<class I> constexpr auto& operator[](I i) noexcept{
      assert(0<=i && i<3);
      if(i==0)return a;
      if(i==1)return b;
      return c;
    }
    template<class I> constexpr const auto& operator[](I i) const noexcept {
      assert(0<=i && i<3);
      if(i==0)return a;
      if(i==1)return b;
      return c;  
    }
    friend std::ostream& operator<<(std::ostream& os, const ob3& p) { 
        return os << "(" << p.a << ", " << p.b << ", " << p.c << ")"; 
    }
    friend std::istream& operator>>(std::istream& is, ob3& p) { 
        return is >> p.a >> p.b >> p.c; 
    }
};

template <typename T1, typename T2, typename T3, typename T4>
struct ob4 {
    T1 a; T2 b; T3 c; T4 d;
    ob4() : a(), b(), c(), d() {}
    ob4(T1 a, T2 b, T3 c, T4 d) : a(a), b(b), c(c), d(d) {}
    friend auto operator<=>(const ob4&, const ob4&) = default;
    ob4 operator+(const ob4& o) const { return {a + o.a, b + o.b, c + o.c, d + o.d}; }
    ob4 operator-(const ob4& o) const { return {a - o.a, b - o.b, c - o.c, d - o.d}; }
    template<class I> constexpr auto& operator[](I i) noexcept{
      assert(0<=i && i<4);
      if(i==0)return a;
      if(i==1)return b;
      if(i==2)return c;
      return d;
    }
    template<class I> constexpr const auto& operator[](I i) const noexcept {
      assert(0<=i && i<4);
      if(i==0)return a;
      if(i==1)return b;
      if(i==2)return c; 
      return d;
    }
    friend std::ostream& operator<<(std::ostream& os, const ob4& p) { 
        return os << "(" << p.a << ", " << p.b << ", " << p.c << ", " << p.d << ")"; 
    }
    friend std::istream& operator>>(std::istream& is, ob4& p) { 
        return is >> p.a >> p.b >> p.c >> p.d; 
    }
};

template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct ob5 {
    T1 a; T2 b; T3 c; T4 d; T5 e;
    ob5() : a(), b(), c(), d(), e() {}
    ob5(T1 a, T2 b, T3 c, T4 d, T5 e) : a(a), b(b), c(c), d(d), e(e) {}
    friend auto operator<=>(const ob5&, const ob5&) = default;
    ob5 operator+(const ob5& o) const { return {a + o.a, b + o.b, c + o.c, d + o.d, e + o.e}; }
    ob5 operator-(const ob5& o) const { return {a - o.a, b - o.b, c - o.c, d - o.d, e - o.e}; }
    template<class I> constexpr auto& operator[](I i) noexcept{
      assert(0<=i && i<5);
      if(i==0)return a;
      if(i==1)return b;
      if(i==2)return c;
      if(i==3)return d;
      return e;
    }
    template<class I> constexpr const auto& operator[](I i) const noexcept {
      assert(0<=i && i<5);
      if(i==0)return a;
      if(i==1)return b;
      if(i==2)return c; 
      if(i==3)return d;
      return e;
    }
    friend std::ostream& operator<<(std::ostream& os, const ob5& p) { 
        return os << "(" << p.a << ", " << p.b << ", " << p.c << ", " << p.d << ", " << p.e << ")"; 
    }
    friend std::istream& operator>>(std::istream& is, ob5& p) { 
        return is >> p.a >> p.b >> p.c >> p.d >> p.e; 
    }
};

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
struct ob6 {
    T1 a; T2 b; T3 c; T4 d; T5 e; T6 f;
    ob6() : a(), b(), c(), d(), e(), f() {}
    ob6(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f) : a(a), b(b), c(c), d(d), e(e), f(f) {}
    friend auto operator<=>(const ob6&, const ob6&) = default;
    ob6 operator+(const ob6& o) const { return {a + o.a, b + o.b, c + o.c, d + o.d, e + o.e, f + o.f}; }
    ob6 operator-(const ob6& o) const { return {a - o.a, b - o.b, c - o.c, d - o.d, e - o.e, f - o.f}; }
    template<class I> constexpr auto& operator[](I i) noexcept{
      assert(0<=i && i<6);
      if(i==0)return a;
      if(i==1)return b;
      if(i==2)return c;
      if(i==3)return d;
      if(i==4)return e;
      return f;
    }
    template<class I> constexpr const auto& operator[](I i) const noexcept {
      assert(0<=i && i<6);
      if(i==0)return a;
      if(i==1)return b;
      if(i==2)return c; 
      if(i==3)return d;
      if(i==4)return e;
      return f;
    }
    friend std::ostream& operator<<(std::ostream& os, const ob6& p) { 
        return os << "(" << p.a << ", " << p.b << ", " << p.c << ", " << p.d << ", " << p.e << ", " << p.f << ")"; 
    }
    friend std::istream& operator>>(std::istream& is, ob6& p) { 
        return is >> p.a >> p.b >> p.c >> p.d >> p.e >> p.f; 
    }
};
using ll2 = ob2<ll, ll>;
using ll3 = ob3<ll, ll, ll>;
using ll4 = ob4<ll, ll, ll, ll>;
using ll5 = ob5<ll, ll, ll, ll, ll>;
using ll6 = ob6<ll, ll, ll, ll, ll, ll>;
template<typename T> struct is_ob : std::false_type {};
template<typename... Ts> struct is_ob<ob2<Ts...>> : std::true_type {};
template<typename... Ts> struct is_ob<ob3<Ts...>> : std::true_type {};
template<typename... Ts> struct is_ob<ob4<Ts...>> : std::true_type {};
template<typename... Ts> struct is_ob<ob5<Ts...>> : std::true_type {};
template<typename... Ts> struct is_ob<ob6<Ts...>> : std::true_type {};
namespace for_debugging{
    struct subscript_and_location{
        int sub;
        std::source_location loc;
        template<class T> subscript_and_location(T sub_,std::source_location loc_=std::source_location::current()){
            if(!std::is_integral<T>::value){
                std::clog << loc_.file_name() << ":(" << loc_.line() << ":" << loc_.column() << "):" << loc_.function_name() << std::endl;
                std::clog << "subscript is not integer: subscript = " << sub_ << std::endl;
                exit(EXIT_FAILURE);
            }
            sub=sub_;
            loc=loc_;
        }
        void check_out_of_range(size_t sz){
            if(sub<0||(int)sz<=sub){
                std::clog << loc.file_name() << ":(" << loc.line() << ":" << loc.column() << "):" << loc.function_name() << std::endl;
                std::clog << "out of range: subscript = " << sub << ", vector_size = " << sz << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    };
}
namespace std{
    template<class T,class Allocator=std::allocator<T>> class vector_for_debugging:public std::vector<T,Allocator>{
        using std::vector<T,Allocator>::vector;
        public:
            [[nodiscard]] constexpr std::vector<T,Allocator>::reference operator[](for_debugging::subscript_and_location n) noexcept(!std::is_same<T,bool>::value){
                n.check_out_of_range(this->size());
                return std::vector<T,Allocator>::operator[](n.sub);
            }
            [[nodiscard]] constexpr std::vector<T,Allocator>::const_reference operator[](for_debugging::subscript_and_location n) const noexcept(!std::is_same<T,bool>::value){
                n.check_out_of_range(this->size());
                return std::vector<T,Allocator>::operator[](n.sub);
            }
    };
    namespace pmr{
        template<class T> using vector_for_debugging=std::vector_for_debugging<T,std::pmr::polymorphic_allocator<T>>;
    }
}
#define vfd vector_for_debugging
/*//多倍長整数
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
namespace mp = boost::multiprecision;
// 任意長整数型
using Bint = mp::cpp_int;
 //仮数部が10進数で1024桁の浮動小数点数型(TLEしたら小さくする)
using Real = mp::number<mp::cpp_dec_float<1024>>;
*/
#define rep(i,n) for(long long i=0;i<(long long)n;i++)
#define reps(i,n) for(long long i=1;i<=(long long)n;i++)
#define repi(i,n) for(int i=0;i<(int)n;i++)
#define loop(i,l,r) for(long long i=(long long)l;i<=(long long)r;i++)
#define loopi(i,l,r) for(int i=(int)l;i<=(int)r;i++)
#define drep(i,n) for(long long i=(long long)n-1;i>=0;i--)
#define drepi(i,n) for(int i=(int)n-1;i>=0;i--)
#define dreps(i,n) for(int i=(int)n;i>=1;i--)
#define dloop(i,l,r) for(long long i=(long long)l;i>=(long long)r;i--)
#define dloopi(i,l,r) for(int i=(int)l;i>=(int)r;i--)
#define all(v) v.begin(), v.end()
#define rall(v) v.rbegin(), v.rend()
#define yna(x) cout << (x? "Yes":"No") << endl;
#define yn(x) out(bool(x));
#define cou(x) cout << x << endl;
#define emp emplace_back
const long long moda=998244353LL;
const long long modb=1000000007LL;
const int kaz=1000000005;
long long yab=2500000000000000000LL;
const long long aho =-yab;
const long double eps=1.0e-14L;
const long double pi=acosl(-1.0L);
using st=string;
using tup=tuple<ll,ll,ll>;
using vi=vector<ll>;
using vin=vector<int>;
using vc=vector<char>;
using vb=vector<bool>;
using vd=vector<double>;
using vs=vector<string>;
using si=set<ll>;
using vvi=vector<vector<ll>>;
using vvin=vector<vin>;
using vvc=vector<vc>;
using vvb=vector<vb>;
using vvvi=vector<vvi>;
using vvvin=vector<vvin>;
const int dx[4]={0,1,0,-1};
const int dy[4]={1,0,-1,0};
const vector<int> ex = {-1, -1, -1, 0, 0, 1, 1, 1};
const vector<int> ey = {-1, 0, 1, -1, 1, -1, 0, 1};
template<typename T>istream&operator>>(istream&is,vector<T>&v){for(T&in:v)is>>in;return is;}
template<typename T>ostream&operator<<(ostream&os,const vector<T>&v){for(int i=0;i<int(v.size());i++)os<<v[i]<<(i+1!=v.size()?" ":"\n");return os;}
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

template<typename T> struct is_str_t : std::false_type {};
template<> struct is_str_t<std::string> : std::true_type {};
template<typename T> struct is_char_vec_t : std::false_type {};
template<> struct is_char_vec_t<std::vector<char>> : std::true_type {};
template<typename T, typename = void> struct is_container_t : std::false_type {};
template<typename T> struct is_container_t<T, std::void_t<decltype(std::begin(std::declval<T&>())), decltype(std::end(std::declval<T&>()))>> : std::bool_constant<!is_str_t<T>::value && !is_char_vec_t<T>::value && !std::is_array_v<T>> {};
template<typename T> constexpr bool is_container_v2 = is_container_t<T>::value;
template<typename T> struct is_pair_t2 : std::false_type {};
template<typename A, typename B> struct is_pair_t2<std::pair<A,B>> : std::true_type {};
template<typename T, typename = void> struct has_fp_io_tag2 : std::false_type {};
template<typename T> struct has_fp_io_tag2<T, std::void_t<typename T::fp_io_tag>> : std::true_type {};
template<typename T, bool = is_container_v2<T>> struct container_depth_impl { static constexpr int value = 0; };
template<typename T> struct container_depth_impl<T, true> { static constexpr int value = 1 + container_depth_impl<typename T::value_type>::value; };
template<typename T> constexpr int container_depth_v = container_depth_impl<T>::value;

struct FastIO {
  bool use_mmap = false;
  char* mp = nullptr;
  char* mp_end = nullptr;
  static constexpr int IN_BUFSIZE = 1 << 20;
  char inbuf[IN_BUFSIZE];
  char* in_ptr = inbuf;
  char* in_end = inbuf;
  static constexpr int OUT_BUFSIZE = 1 << 20;
  char out_buf[OUT_BUFSIZE];
  int out_pos = 0;
  FastIO() {
    struct stat st;
    if (!isatty(0) && fstat(0, &st) == 0 && S_ISREG(st.st_mode) && st.st_size > 0) {
      void* res = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, 0, 0);
      if (res != MAP_FAILED) { mp = (char*)res; mp_end = mp + st.st_size; use_mmap = true; }
    }
  }
  ~FastIO() { flush(); }
  inline void refill() {
    int len = (int)read(0, inbuf, IN_BUFSIZE);
    in_ptr = inbuf;
    in_end = inbuf + (len > 0 ? len : 0);
  }
  inline int getc() {
    if (use_mmap) return (mp == mp_end) ? -1 : (unsigned char)(*mp++);
    if (in_ptr == in_end) refill();
    if (in_ptr == in_end) return -1;
    return (unsigned char)(*in_ptr++);
  }
  inline void putc(char c) {
    if (out_pos == OUT_BUFSIZE) flush();
    out_buf[out_pos++] = c;
  }
  void flush() { if (out_pos > 0) { write(1, out_buf, out_pos); out_pos = 0; } }
  inline bool is_space(int c) { return c==' '||c=='\n'||c=='\r'||c=='\t'; }
  template<typename T>
  void read_recursive(T& x) {
    if constexpr (std::is_same_v<T, bool>) {
      long long t; read_recursive(t); x = (t != 0);
    } else if constexpr (std::is_same_v<T, char>) {
      int c; do { c = getc(); } while (c != -1 && is_space(c));
      x = (char)c;
    } else if constexpr (
#if HAS_ACL
      std::is_same_v<T, minta> || std::is_same_v<T, mintb>
#else
      false
#endif
    ) {
      long long t; read_recursive(t); x = t;
    } else if constexpr (has_fp_io_tag2<T>::value) {
      long long t; read_recursive(t); x = t;
    } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
      for (size_t i = 0; i < x.size(); i++) { long long t; read_recursive(t); x[i] = (t != 0); }
    } else if constexpr (std::is_same_v<T, std::string>) {
      int c; do { c = getc(); } while (c != -1 && is_space(c));
      x.clear();
      while (c != -1 && !is_space(c)) { x += (char)c; c = getc(); }
    } else if constexpr (std::is_same_v<T, std::vector<char>>) {
      int c; do { c = getc(); } while (c != -1 && is_space(c));
      x.clear();
      while (c != -1 && !is_space(c)) { x.push_back((char)c); c = getc(); }
    } else if constexpr (std::is_integral_v<T> || std::is_same_v<T, __int128_t>) {
      int c; do { c = getc(); } while (c != -1 && c != '-' && (c < '0' || c > '9'));
      bool neg = false;
      if (c == '-') { neg = true; c = getc(); }
      T v = 0;
      while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = getc(); }
      x = neg ? -v : v;
    } else if constexpr (std::is_floating_point_v<T>) {
      std::string s; read_recursive(s);
      x = (T)strtold(s.c_str(), nullptr);
    } else if constexpr (is_pair_t2<T>::value) {
      read_recursive(x.first); read_recursive(x.second);
    } else if constexpr (is_ob<T>::value) {
      if constexpr (requires { x.a; }) read_recursive(x.a);
      if constexpr (requires { x.b; }) read_recursive(x.b);
      if constexpr (requires { x.c; }) read_recursive(x.c);
      if constexpr (requires { x.d; }) read_recursive(x.d);
      if constexpr (requires { x.e; }) read_recursive(x.e);
      if constexpr (requires { x.f; }) read_recursive(x.f);
    } else if constexpr (is_container_v2<T>) {
      for (auto& e : x) read_recursive(e);
    }
  }
  void write_int(long long v){
    uint64_t x;
    if(v<0){
        putc('-');
        x = -(uint64_t)v;
    }else{
        x = v;
    }
    char buf[21];
    int p=0;
    do{
        buf[p++]='0'+x%10;
        x/=10;
    }while(x);
    while(p--) putc(buf[p]);
  }
  void write_uint64(uint64_t x){
    if(x==0){
        putc('0');
        return;
    }
    char buf[21];
    int p=0;
    while(x){
        buf[p++]='0'+x%10;
        x/=10;
    }
    while(p--) putc(buf[p]);
  }
  void write_str(const std::string& s) { for (char c : s) putc(c); }
  void write_double(double x) {
    if (std::isnan(x)) { putc('0'); return; }
    if (std::isinf(x)) { if (x < 0) putc('-'); putc('i'); putc('n'); putc('f'); return; }
    if (x < 0) { putc('-'); x = -x; }
    if (std::abs(x) >= 9e18) {
      char buf[64];
      auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf),x, std::chars_format::general, 17);
      for (char* p = buf; p != ptr; ++p) putc(*p);
      return;
    }
    double offset = 0.5;
    for (int i = 0; i < 15; i++) offset /= 10.0;
    x += offset;
    long long int_part = (long long)x;
    write_int(int_part); putc('.');
    double fraction = x - (double)int_part;
    for (int i = 0; i < 15; i++) {
      fraction *= 10; int d = (int)fraction;
      putc('0' + d); fraction -= d;
    }
  }
  void write_u128(__uint128_t x){
    if(!x){putc('0');return;}
    char buf[40];
    int p=0;
    while(x){
      buf[p++]='0'+x%10;
      x/=10;
    }
    while(p--)putc(buf[p]);
  }
  void write_i128(__int128_t x){
    __uint128_t y;
    if(x<0){
      putc('-');
      y=__uint128_t(-(x+1))+1;
    }else y=x;
    write_u128(y);
  }
  template<typename T>
  void write_recursive(const T& x, const std::string& sep = " ", const std::string& rowsep = "\n") {
    if constexpr (std::is_same_v<T, bool>) {
      write_str(x ? "Yes" : "No");
    } else if constexpr (std::is_same_v<T, char>) {
      putc(x);
    } else if constexpr (
#if HAS_ACL
      std::is_same_v<T, minta> || std::is_same_v<T, mintb>
#else
      false
#endif
    ) {
      write_int((long long)x.val());
    } else if constexpr (has_fp_io_tag2<T>::value) {
      write_uint64((uint64_t)x.val());
    } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
      bool first = true;
      for (bool b : x) { if (!first) write_str(sep); first = false; write_str(b ? "1" : "0"); }
    } else if constexpr (std::is_same_v<T, std::string>) {
      write_str(x);
    } else if constexpr (std::is_same_v<T, std::vector<char>>) {
      for (char c : x) putc(c);
    }else if constexpr (is_floating_point_v<T>) {
      write_double((double)x);
    }else if constexpr(std::is_same_v<T,__int128_t>) {
      write_i128(x);
    }else if constexpr(std::is_same_v<T,__uint128_t>) {
      write_u128(x);
    }else if constexpr (std::is_integral_v<T>) {
      if constexpr(std::is_unsigned_v<T>)
        write_uint64(x);
      else if constexpr(std::is_signed_v<T>)
        write_int(x);
    } else if constexpr (is_pair_t2<T>::value) {
      putc('(');
      write_recursive(x.first, sep, rowsep);
      write_str(", ");
      write_recursive(x.second, sep, rowsep);
      putc(')');
    } else if constexpr (is_ob<T>::value) {
      putc('(');
      bool first = true;
      auto put_field = [&](const auto& f) {
        if (!first) write_str(", ");
        first = false;
        write_recursive(f, sep, rowsep);
      };
      if constexpr (requires { x.a; }) put_field(x.a);
      if constexpr (requires { x.b; }) put_field(x.b);
      if constexpr (requires { x.c; }) put_field(x.c);
      if constexpr (requires { x.d; }) put_field(x.d);
      if constexpr (requires { x.e; }) put_field(x.e);
      if constexpr (requires { x.f; }) put_field(x.f);
      putc(')');
    } else if constexpr ((std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
                   || std::is_same_v<std::decay_t<T>, char*>
                   || std::is_same_v<std::decay_t<T>, const char*>) {write_str(x);
    } else if constexpr (is_container_v2<T>) {
      using ValueT = typename T::value_type;
      constexpr int d = container_depth_v<ValueT>;
      std::string joiner = (d == 0) ? sep : (d == 1 ? rowsep : "\n\n");
      bool first = true;
      for (const auto& e : x) {
        if (!first) write_str(joiner);
        first = false;
        write_recursive(e, sep, rowsep);
      }
    }
  }
} io;
template <typename... Args> void in(Args&... args) { (io.read_recursive(args), ...); }
template <typename... Args>
struct out_t {
  std::tuple<const Args&...> args;
  std::string sep_str = " ";
  std::string rowsep_str = "\n";
  std::string end_str = "\n";
  bool do_flush = false;
  out_t(const Args&... a) : args(a...) {}
  out_t& sep(std::string s) { sep_str = std::move(s); return *this; }
  out_t& rowsep(std::string s) { rowsep_str = std::move(s); return *this; }
  out_t& end(std::string s) { end_str = std::move(s); return *this; }
  out_t& flush() { do_flush = true; return *this; }
  ~out_t() {
    std::apply([this](const auto&... ts) {
      int n = (int)sizeof...(Args), i = 0;
      ((io.write_recursive(ts, sep_str, rowsep_str), io.write_str(++i == n ? std::string() : sep_str)), ...);
    }, args);
    io.write_str(end_str);
    if (do_flush) io.flush();
  }
};
template<typename... Args>
out_t<Args...> out(const Args&... args) { return out_t<Args...>(args...); }
template<typename T1,typename T2>
void co(bool x,T1 y,T2 z){
  if(x)cout << y << endl;
  else cout << z << endl;
}
long long isqrt(long long n){
  long long ok=0,ng=1000000000;//1e9
  while(ng-ok>1){
    long long mid=(ng+ok)/2;
    if(mid*mid<=n)ok=mid;
    else ng=mid;
  }
  return ok;
}
template<typename T>
bool chmax(T &a, T b){
	if(a<b){
		a=b;
		return true;
	}
	return false;
}
template <typename T, typename U>
T ceil(T x, U y) {
  return (x > 0 ? (x + y - 1) / y : x / y);
}

template <typename T, typename U>
T floor(T x, U y) {
  return (x > 0 ? x / y : (x - y + 1) / y);
}
template <typename T> auto vec(size_t n, T val) {return vector<T>(n, val); } 
template <typename... Sizes> auto vec(size_t n, Sizes... sizes) { 
  return std::vector<decltype(vec(sizes...))>(n, vec(sizes...)); }
template<typename T,typename U>
pair<pair<T,T>,T> unit(T x, T y,U k){
	T s=floor(x-1,k);
	T t=floor(y,k);
  if(s==t)return make_pair(make_pair(0,0),-1);
	return make_pair(make_pair(s+1,t),1);
}
pair<pair<ll,ll>,int> jufuku(ll a,ll b,ll c,ll d){
	//a<=b,c<=dが保証されているとする
	if(a>c){
		swap(a,c);
		swap(b,d);
	}
	if(c>b)return make_pair(make_pair(0,0),-1);
	return make_pair(make_pair(c,min(b,d)),1);
}
template<typename T>
bool chmin(T &a, T b){
	if(a>b){
		a=b;
		return true;
	}
	return false;
}
template<typename T>
void her(vector<T> &a){
  for(auto &g:a)g--;
}
template <typename T>
void dec(vector<T> &t,T k=1){
  for(auto &i:t)i-=k;
}
template <typename T>
void inc(vector<T> &t,T k=1){
  for(auto &i:t)i+=k;
}
template<class T0, class T1, class... Ts>
constexpr auto min(T0 a, T1 b, Ts... xs) {
    return min(initializer_list<common_type_t<T0,T1,Ts...>>{a,b,xs...});
}
template<class T0, class T1, class... Ts>
constexpr auto max(T0 a, T1 b, Ts... xs) {
    return max(initializer_list<common_type_t<T0,T1,Ts...>>{a,b,xs...});
}
template<class T>
constexpr T min(const vector<T>&a){return (a.empty()?numeric_limits<T>::max():ranges::min(a));}
template<class T>
constexpr T max(const vector<T>&a){return (a.empty()?numeric_limits<T>::lowest():ranges::max(a));}
ll sum(const vector<ll>&v){return reduce(v.begin(),v.end(),0LL);}
#define LL(...) ll __VA_ARGS__; in(__VA_ARGS__);
#define PR(l,r,op) LL(l,r);out(op);
void Yes(){out("Yes");return;}
void No(){out("No");return;}
ll mypow(ll x,ll y,ll MOD){
	if(MOD==-1){
		MOD=9223372036854775807LL;
	}
  x%=MOD;
	ll ret=1;
	while(y>0){
		if(y&1)ret=ret*x%MOD;
		x=x*x%MOD;
		y>>=1;
	}
	return ret;
}
template<class T, class Comp = std::less<T>>
struct Top2 {
  T a, b;
  bool has_a = false;
  bool has_b = false;
  Comp comp;
  Top2(Comp comp=Comp()):comp(comp) {}
  Top2(T v,Comp comp=Comp()) : a(v), has_a(true),comp(comp) {}
  Top2(T a, T b,Comp comp=Comp()) : comp(comp) {add(a),add(b);}
  static Top2 e() { return Top2(); }
  void add(const T &v) {
    if (!has_a) {
      a = v;
      has_a = true;
    } else if (comp(v, a)) {
      b = a;
      has_b = has_a;
      a = v;
    } else if (!has_b or comp(v, b)) {
      b = v;
      has_b = true;
    }
  }
  friend Top2 operator*(const Top2 &lhs, const Top2 &rhs) {
    Top2 res(lhs.comp); 
    res.a = lhs.a; res.b = lhs.b;
    res.has_a = lhs.has_a; res.has_b = lhs.has_b;
    if(rhs.has_a)res.add(rhs.a);
    if(rhs.has_b)res.add(rhs.b);
    return res;
  }
  Top2 &operator*=(const Top2 &rhs) {
    *this = *this * rhs;
    return *this;
  }
};
struct UnionFind {
    vector<int> par,siz,mi,ma;
    UnionFind(int n) : par(n,-1), siz(n,1) {mi.resize(n);ma.resize(n);iota(mi.begin(),mi.end(),0);iota(ma.begin(),ma.end(),0); }
    int root(int x) {
      if(par[x]==-1)return x; 
      else return par[x]=root(par[x]); 
    }
    bool same(int x, int y) {
      return root(x)==root(y);
    }
    bool marge(int x, int y) {
      int rx = root(x), ry = root(y); 
      if (rx==ry) return false; 
			if(siz[rx]<siz[ry])swap(rx,ry);
      par[ry] = rx;
      siz[rx] += siz[ry];
			mi[rx]=min(mi[rx],mi[ry]);
			ma[rx]=max(ma[rx],ma[ry]);
      return true;
    }
    int size(int x) {
      return siz[root(x)];
    }
		int mini(int x){
			return mi[root(x)];
		}
		int maxi(int x){
			return ma[root(x)];
		}
};
ll nckmod;
vector<long long> fac,finv,invs;
// テーブルを作る前処理
void COMinit(int MAX,ll MOD) {
  nckmod=MOD;
	fac.resize(MAX);
	finv.resize(MAX);
	invs.resize(MAX);
    fac[0] = fac[1] = 1;
    finv[0] = finv[1] = 1;
    invs[1] = 1;
    for (int i = 2; i < MAX; i++){
        fac[i] = fac[i - 1] * i % MOD;
        invs[i] = MOD - invs[MOD%i] * (MOD / i) % MOD;
        finv[i] = finv[i - 1] * invs[i] % MOD;
    }
}
// 二項係数計算
long long binom(int n, int k){
    if (n < k) return 0;
    if (n < 0 || k < 0) return 0;
    return fac[n] * (finv[k] * finv[n - k] % nckmod) % nckmod;
}
template<typename Container>
auto assyuku(const Container&v){
  using T=typename Container::value_type;
	vector<ob3<int,int,T>> ans;
  if(v.empty())return ans;
	int sum=0;
	T pos=v[0];
	int n=v.size();
	for(int i=0;i<n;i++){
		if(v[i]==pos)sum++;
		else {
			ans.emplace_back(ob3(i-sum,i-1,pos));
			sum=1;
			pos=v[i];
		}
	}
	ans.emplace_back(ob3(n-sum,n-1,pos));
	return ans;
};
template<typename Container,typename F>
vector<ob3<int,int,bool>> assyuku2(const Container &v,F f){
	vector<ob3<int,int,bool>> ans;
  if(v.empty())return ans;
	int sum=0;
	bool pos=f(v[0]);
	int n=v.size();
	for(int i=0;i<n;i++){
		if(f(v[i])==pos)sum++;
		else {
		    ans.emplace_back(ob3(i-sum,i-1,pos));
			sum=1;
			pos=f(v[i]);
		}
	}
	ans.emplace_back(ob3(n-sum,n-1,pos));
	return ans;
};
struct safe_custom_hash {
  static uint64_t get_seed() {
    static const uint64_t FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
    return FIXED_RANDOM;
  }
  static uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
  }
  template <class T> uint64_t combine(uint64_t seed, const T& val) const {
    return seed ^ ((*this)(val) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
  }
  template <class T, std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, nullptr_t> = nullptr>
  size_t operator()(T x) const {
    return splitmix64(static_cast<uint64_t>(x) + get_seed());
  }
  size_t operator()(unsigned __int128 x) const {
    uint64_t high = static_cast<uint64_t>(x >> 64);
    uint64_t low = static_cast<uint64_t>(x);
    uint64_t h = combine(get_seed(), high);
    return splitmix64(combine(h, low));
  }
  size_t operator()(__int128_t x) const {
    return (*this)(static_cast<unsigned __int128>(x));
  }
  size_t operator()(double x) const {
    if (x == 0.0) x = 0.0; // -0.0 正規化
    uint64_t val = 0;
    std::memcpy(&val, &x, sizeof(double));
    return splitmix64(val + get_seed());
  }
  size_t operator()(float x) const {
    if (x == 0.0f) x = 0.0f; // -0.0 正規化
    uint32_t val = 0;
    std::memcpy(&val, &x, sizeof(float));
    return splitmix64(val + get_seed());
  }
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2>& p) const {
    uint64_t h = combine(get_seed(), p.first);
    return splitmix64(combine(h, p.second));
  }
  template <class T1, class T2, template <class, class> class ob2>
  size_t operator()(const ob2<T1, T2>& p) const {
    uint64_t h = combine(get_seed(), p.a);
    return splitmix64(combine(h, p.b));
  }
  template <class T1, class T2, class T3, template <class, class, class> class ob3>
  size_t operator()(const ob3<T1, T2, T3>& p) const {
    uint64_t h = combine(get_seed(), p.a);
    h = combine(h, p.b);
    return splitmix64(combine(h, p.c));
  }
  size_t operator()(const std::string& s) const {
    uint64_t h = get_seed() ^ 0x9e3779b97f4a7c15ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
    size_t n = s.size();
    while (n >= 8) {
      uint64_t v;
      std::memcpy(&v, p, 8);
      h = splitmix64(h ^ v);
      p += 8; n -= 8;
    }
    uint64_t tail = 0;
    for (size_t i = 0; i < n; ++i) tail |= uint64_t(p[i]) << (8 * i);
    return splitmix64(h ^ tail ^ splitmix64(s.size()));
  }
  template <class T> size_t operator()(const std::vector<T>& v) const {
    uint64_t h = get_seed();
    for (const auto& elem : v) {
      h = combine(h, elem);
    }
    return splitmix64(h ^ splitmix64(v.size()));
  }
};
template<class T,class Container>
struct to_multiset{
  Container c;
  int id=0;
  void insert(const T& val){c.insert({val,id++});}
  bool erase(const T& val){
    auto it=c.lower_bound({val,0});
    if(it!=c.end()&&it->first==val){c.erase(it);return true;}return false;
  }
  bool eraseonly(const T& val){
    auto it=c.lower_bound({val,0});
    if(it==c.end())return false;
    c.erase(it);return true;
  }
  auto lower_bound(const T& val)const{return c.lower_bound({val,0});}
  auto upper_bound(const T& val)const{return c.lower_bound({val,2000000000});}
  bool contains(const T& val)const{
    auto it=c.lower_bound({val,0});return (it!=c.end()&&it->first==val);
  }
  size_t count(const T& val)const{
    auto it1=c.lower_bound({val,0});if(it1==c.end() or it1->first!=val)return 0;
    auto it2=c.lower_bound({val,2000000000});
    return std::distance(it1,it2);
  }
  size_t size()const{return c.size();}
  void clear(){c.clear();id=0;}
};
template<class K,bool U=true,class C=std::less<>>
struct PBDSImpl{
  using I=std::numeric_limits<long long>;
  using E=std::conditional_t<U,K,std::pair<K,long long>>;
  struct P{
    C c;
    bool operator()(const E&a,const E&b)const{
      if constexpr(U)return c(a,b);
      else return c(a.first,b.first)?true:c(b.first,a.first)?false:a.second<b.second;
    }
  };
  using T=__gnu_pbds::tree<E,__gnu_pbds::null_type,P,__gnu_pbds::rb_tree_tag,__gnu_pbds::tree_order_statistics_node_update>;
  T t;long long id=0;
  const K& g(typename T::const_iterator it)const{if constexpr(U)return *it;else return it->first;}
  int size()const{return t.size();}
  bool empty()const{return t.empty();}
  void clear(){t.clear();id=0;}
  auto begin()const{return t.begin();}
  auto end()const{return t.end();}
  const K& operator[](int i)const{return g(t.find_by_order(i));}
  auto lower_bound(const K& k)const{if constexpr(U)return t.lower_bound(k);else return t.lower_bound({k,0LL});}
  auto upper_bound(const K& k)const{if constexpr(U)return t.upper_bound(k);else return t.lower_bound({k,I::max()});}
  auto find(const K& k)const{auto it=lower_bound(k);return(it!=t.end()&&g(it)==k)?it:t.end();}
  auto insert(const K& x){if constexpr(U)return t.insert(x);else return t.insert({x,id++});}
  auto emplace(const K& x){return insert(x);}
  int order_of_key(const K& k)const{if constexpr(U)return t.order_of_key(k);else return t.order_of_key({k,I::max()});}
  int count(const K& k)const{if constexpr(U)return t.count(k);else return t.order_of_key({k,I::max()})-t.order_of_key({k,0LL});}
  bool contains(const K& k)const{return count(k);}
  auto erase(typename T::const_iterator it){return t.erase(it);}
  int erase(const K& k){
    if constexpr(U)return t.erase(k);
    int c=0;for(auto it=lower_bound(k);it!=t.end()&&g(it)==k;c++)it=t.erase(it);
    return c;
  }
};
template<class K, class V, class C =std::less<>>using pbmap = __gnu_pbds::tree<K, V, C, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;
template<class K,class C=std::less<>>using pbset=PBDSImpl<K,true,C>;
template<class K,class C=std::less<>>using pbmset=PBDSImpl<K,false,C>;
#if HAS_ABSL
template<class K,class V,class C=std::less<>>using bmap=absl::btree_map<K,V,C>;
template<class K,class C=std::less<>>using bset=absl::btree_set<K,C>;
template<class K,class C=std::less<>>using bmset=to_multiset<K,bset<std::pair<K,int>,C>>;
#endif
#if HAS_BOOST
template<class K,class V,class C=std::less<>>using fmap=boost::container::flat_map<K,V,C>;
template<class K,class C=std::less<>>using fset=boost::container::flat_set<K,C>;
template<class K,class C=std::less<>>using fmset=to_multiset<K,fset<std::pair<K,int>,C>>;
template<class K,class V>using umap=boost::unordered_flat_map<K,V,safe_custom_hash>;
template<class K>using uset=boost::unordered_flat_set<K,safe_custom_hash>;
#endif
//参照多数fset,動的少数bset,動的k番pbset動的多数->vector or std::map
#endif
int main(){
  cin.tie(nullptr);
  
}

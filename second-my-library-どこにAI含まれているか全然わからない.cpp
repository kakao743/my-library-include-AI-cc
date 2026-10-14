//second my-library

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include<ext/pb_ds/hash_policy.hpp>
#include<ext/pb_ds/tag_and_trait.hpp>
#include <ext/pb_ds/priority_queue.hpp>
using namespace __gnu_pbds;
using namespace __gnu_cxx;
template<typename T>
using Tree = tree<T,null_type,less<T>,rb_tree_tag,tree_order_statistics_node_update>;
//query 1 update object[pos]*=x;
//query 2 count a[l],a[l+1],,,,,a[r]の中のxの個数 1-indexed
#pragma region FCT
template<class T>
class FCT{
    public:
    using OP=function<T(T,T)>;
    vector<T> vec;
    map<T,ordered_set<int>> data;
    OP op;
    FCT(int n,T init):vec(n+1,init){
        for(int i=1; i<=n; i++)data[init].insert(i);
        op=[](T a,T b){return b;};
    }
    FCT(int n,T init,OP op_):vec(n+1,init),op(op_){
        for(int i=1; i<=n; i++)data[init].insert(i);
    }
    FCT(vector<T> v):vec(v){
        for(int i=1; i<v.size(); i++)data[v[i]].insert(i);
        op=[](T a,T b){return b;};
    }
    FCT(vector<T> v,OP op_):vec(v),op(op_){
        for(int i=1; i<v.size(); i++)data[v[i]].insert(i);
    }
    void update(int pos,T x){
        data[vec[pos]].erase(pos);
        vec[pos]=op(vec[pos],x);
        data[vec[pos]].insert(pos);
    }
    int count(int l,int r,T x){
        if(l>r)return 0;
        if(!data.count(x))return 0;
        int lef=data[x].order_of_key(l);
        int rig=data[x].order_of_key(r+1);
        return rig-lef;
		}
};
//3 agment(https://onlinejudge.u-aizu.ac.jp/status/users/kakao745/submissions/1/3326/judge/9609495/C++17)
#pragma endregion

/*
#include<boost/multiprecision/cpp_int.hpp>//やばい整数
#include <boost/multiprecision/cpp_dec_float.hpp>//やばい少数派(重い)
#include <boost/math/constants/constants.hpp>//円周率他
#include <boost/dynamic_bitset.hpp> //dynamicbitset
namespace mp=boost::multiprecision;
using bint=mp::cpp_int;
using ultra=mp::number<mp::cpp_dec_float<1024>>;
*/


template <typename T> bool next_combination(const T first, const T last, int k) {
    const T subset = first + k;
    // empty container | k = 0 | k == n 
    if (first == last || first == subset || last == subset) {
        return false;
    }
    T src = subset;
    while (first != src) {
        src--;
        if (*src < *(last - 1)) {
            T dest = subset;
            while (*src >= *dest) {
                dest++;
            }
            iter_swap(src, dest);
            rotate(src + 1, dest + 1, last);
            rotate(subset, subset + (last - dest) - 1, last);
            return true;
        }
    }
    // restore
    rotate(first, subset, last);
    return false;
}

ll jo(ll x,ll n){
  ll ok=-1,ng=1000000001;
  while(ng-ok>1){
    ll mid=(ng+ok)/2,kak=1;bool kaz=true;
    reps(j,x){
      if((long double)kak*mid>(long double)n){
        kaz=false;
        break;
      }
      kak*=mid;
    }
    if(kaz&&kak<=n)ok=mid;
    else ng=mid;
  }
  return ok;
}
//ざあつ
#pragma region zaatsu
class CoordinateCompression {
public:
    int no = 0;
    std::unordered_map<long long, int> zip;    // 元の値 -> 圧縮した値
    std::unordered_map<int, long long> unzip;  // 圧縮した値 -> 元の値

    CoordinateCompression(const std::vector<long long> &v, int base = 0) {
        this->no = base;
        std::set<long long> s(v.begin(), v.end());

        for (long long x : s) {
            this->zip[x] = this->no;
            this->unzip[this->no] = x;
            this->no++;
        }
    }
};
#pragma endregion
#pragma region n_ary
/// @brief N進数の文字から10進数の数値に変換します
/// @param c N進数の文字
/// @return 変換した10進数の数値
inline int ntodec(const char c)
{
	switch (c)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'B':
		return 11;
	case 'C':
		return 12;
	case 'D':
		return 13;
	case 'E':
		return 14;
	case 'F':
		return 15;
	case 'G':
		return 16;
	case 'H':
		return 17;
	case 'I':
		return 18;
	case 'J':
		return 19;
	case 'K':
		return 20;
	case 'L':
		return 21;
	case 'M':
		return 22;
	case 'N':
		return 23;
	case 'O':
		return 24;
	case 'P':
		return 25;
	case 'Q':
		return 26;
	case 'R':
		return 27;
	case 'S':
		return 28;
	case 'T':
		return 29;
	case 'U':
		return 30;
	case 'V':
		return 31;
	case 'W':
		return 32;
	case 'X':
		return 33;
	case 'Y':
		return 34;
	case 'Z':
		return 35;
	default:
		return -1;
	}
}

/// @brief 10進数の数値をN進数の文字に変換します
/// @param n 10進数の数値
/// @return N進数の文字
inline char decton(const int n)
{
	switch (n)
	{
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	case 10:
		return 'A';
	case 11:
		return 'B';
	case 12:
		return 'C';
	case 13:
		return 'D';
	case 14:
		return 'E';
	case 15:
		return 'F';
	case 16:
		return 'G';
	case 17:
		return 'H';
	case 18:
		return 'I';
	case 19:
		return 'J';
	case 20:
		return 'K';
	case 21:
		return 'L';
	case 22:
		return 'M';
	case 23:
		return 'N';
	case 24:
		return 'O';
	case 25:
		return 'P';
	case 26:
		return 'Q';
	case 27:
		return 'R';
	case 28:
		return 'S';
	case 29:
		return 'T';
	case 30:
		return 'U';
	case 31:
		return 'V';
	case 32:
		return 'W';
	case 33:
		return 'X';
	case 34:
		return 'W';
	case 35:
		return 'Z';
	default:
		return '\0';
	}
}

/// @brief N進数の文字列をM進数に直して出力します。
/// @param str N進数の文字
/// @param n 文字の進数
/// @param m 出力の進数
/// @return M進数の文字
inline string n_ary(const string &str, const int n, const int m)
{
	unsigned long tmp = 0;
	string ret;

	for (unsigned long long i = 0; i < str.length(); i++)
	{
		tmp += (unsigned long)ntodec(str[str.length() - 1 - i]) * mypow(n, i,-1LL);
	}

	if (tmp == 0)
		return "0";
	while (tmp != 0)
	{
		ret = decton(tmp % m) + ret;
		tmp /= m;
	}
	return ret;
}
#pragma endregion
/// @brief 二次元配列を90度時計回りに回転する
/// @tparam T 配列の型(vector<vector<T>>)
/// @param arr 二次元配列
/// @return 返り値
#pragma region rot90
template <typename T>
inline vector<vector<T>> rot90(const vector<vector<T>> &A)
{
	ll N = A.size(), M = A[0].size();
	vector<vector<T>> ret(M, vector<T>(N));

	ll _i = 0, _j = 0;

	rep(j, M)
	{
		for (ll i = N - 1; i >= 0; i--)
		{
			ret[_i][_j] = A[i][j];
			_j++;
		}
		_j = 0;
		_i++;
	}

	return ret;
}
#pragma endregion
///@brief 高速素数判定
#pragma region pollard-rho
namespace math{
using u32=uint32_t;
using u64=uint64_t;
using u128=__uint128_t;
constexpr u64 get_nr(u64 M){
    u64 IV=2-M;
    for(int i=0;i<5;++i){
        IV*=2-M*IV;
    }
    return IV;
}
constexpr u64 mul(u64 x,u64 y,u64 IV,u64 M){
    auto t=u128(x)*y;
    u64 lo=t,hi=t>>64,res=(hi+M)-u64((u128(lo*IV)*M)>>64);
    return res;
}
constexpr bool isprime(u64 x){
    if(x<64){return (u64(1)<<x)&0x28208a20a08a28ac;}
    if(x%2==0){return false;}
    u64 d=x-1,IV=get_nr(x),R=(-x)%x,R2=(-u128(x))%x,nR=x-R;
    int k=__builtin_ctzll(d);
    d>>=k;
    auto mr3=[&](u64 t1,u64 t2,u64 t3){
        u64 r1=R,r2=R,r3=R;
        t1=mul(t1,R2,IV,x),t2=mul(t2,R2,IV,x),t3=mul(t3,R2,IV,x);
        for(u64 b=d;b;b>>=1){
            if(b&1){
                r1=mul(r1,t1,IV,x),r2=mul(r2,t2,IV,x),r3=mul(r3,t3,IV,x);
            }
            t1=mul(t1,t1,IV,x),t2=mul(t2,t2,IV,x),t3=mul(t3,t3,IV,x);
        }
        r1=std::min(r1,r1-x),r2=std::min(r2,r2-x),r3=std::min(r3,r3-x);
        int res1=(r1==R)|(r1==nR),res2=(r2==R)|(r2==nR),res3=(r3==R)|(r3==nR);
        for(int j=0;j<k-1;++j){
            r1=mul(r1,r1,IV,x),r2=mul(r2,r2,IV,x),r3=mul(r3,r3,IV,x);
            res1|=(std::min(r1,r1-x)==nR),res2|=(std::min(r2,r2-x)==nR),res3|=(std::min(r3,r3-x)==nR);
        }
        return res1&res2&res3;
    };
    auto mr4=[&](u64 t1,u64 t2,u64 t3,u64 t4){
        u64 r1=R,r2=R,r3=R,r4=R;
        t1=mul(t1,R2,IV,x),t2=mul(t2,R2,IV,x),t3=mul(t3,R2,IV,x),t4=mul(t4,R2,IV,x);
        for(u64 b=d;b;b>>=1){
            if(b&1){
                r1=mul(r1,t1,IV,x),r2=mul(r2,t2,IV,x),r3=mul(r3,t3,IV,x),r4=mul(r4,t4,IV,x);
            }
            t1=mul(t1,t1,IV,x),t2=mul(t2,t2,IV,x),t3=mul(t3,t3,IV,x),t4=mul(t4,t4,IV,x);
        }
        r1=std::min(r1,r1-x),r2=std::min(r2,r2-x),r3=std::min(r3,r3-x),r4=std::min(r4,r4-x);
        int res1=(r1==R)|(r1==nR),res2=(r2==R)|(r2==nR),res3=(r3==R)|(r3==nR),res4=(r4==R)|(r4==nR);
        for(int j=0;j<k-1;++j){
            r1=mul(r1,r1,IV,x),r2=mul(r2,r2,IV,x),r3=mul(r3,r3,IV,x),r4=mul(r4,r4,IV,x);
            res1|=(std::min(r1,r1-x)==nR),res2|=(std::min(r2,r2-x)==nR),res3|=(std::min(r3,r3-x)==nR),res4|=(std::min(r4,r4-x)==nR);
        }
        return res1&res2&res3&res4;
    };
    if(x<(u64(1)<<32)){
        return mr3(2,7,61);
    }
    return mr3(2,325,9375)&&mr4(28178,450775,9780504,1795265022);
}
}
#pragma endregion
uint64_t kth_root(uint64_t N, uint64_t K) {
    assert(K >= 1);
    if (N <= 1 || K == 1) return N;
    if (K >= 64) return 1;
    if (N == uint64_t(-1)) --N;
    
    auto mul = [&](uint64_t x, uint64_t y) -> uint64_t {
        if (x < UINT_MAX && y < UINT_MAX) return x * y;
        if (x == uint64_t(-1) || y == uint64_t(-1)) return uint64_t(-1);
        return (x <= uint64_t(-1) / y ? x * y : uint64_t(-1));
    };
    auto power = [&](uint64_t x, uint64_t k) -> uint64_t {
        if (k == 0) return 1ULL;
        uint64_t res = 1ULL;
        while (k) {
            if (k & 1) res = mul(res, x);
            x = mul(x, x);
            k >>= 1;
        }
        return res;
    };
    
    uint64_t res;
    if (K == 2) res = sqrtl(N) - 1;
    else if (K == 3) res = cbrt(N) - 1;
    else res = pow(N, nextafter(1 / double(K), 0));
    while (power(res + 1, K) <= N) ++res;
    return res;
}

#include <ext/rope>
using namespace __gnu_cxx;
//https://xuzijian629.hatenablog.com/entry/2019/09/09/012207
//推奨（経路圧縮なしだがバグはない）
#pragma region WeightedUF
template <typename T, typename F, typename G>
struct WeightedUF {
  int n;
  vector<int> par, siz;
  vector<T> diff_weight;
  T identity;
  F op;  // 二項演算: op(a, b) -> a + b
  G inv; // 逆元を返す単項演算: inv(a) -> -a

  WeightedUF(int n_, T id, F f, G g)
    : n(n_), par(n_), siz(n_, 1), diff_weight(n_, id), identity(id), op(f), inv(g) {
    iota(par.begin(), par.end(), 0);
  }

  int find(int x) {
    while (par[x] != x) x = par[x];
    return x;
  }

  T weight(int x) {
    T res = identity;
    while (par[x] != x) {
      res = op(diff_weight[x], res);
      x = par[x];
    }
    return res;
  }

  // weight(y) = op(weight(x), w) となるように結合
  bool unite(int x, int y, T w) {
    int rx = find(x), ry = find(y);
    T wx = weight(x), wy = weight(y);
    
    // wx + w = wy + root_w  =>  root_w = (wx + w) - wy
    // 逆演算を op(A, inv(B)) で表現
    if (rx == ry) return wy == op(wx, w);

    T root_w = op(op(wx, w), inv(wy)); 

    if (siz[rx] < siz[ry]) {
      par[rx] = ry;
      diff_weight[rx] = inv(root_w); // -root_w
      siz[ry] += siz[rx];
    } else {
      par[ry] = rx;
      diff_weight[ry] = root_w;
      siz[rx] += siz[ry];
    }
    return true;
  }

  // diff(x, y) = w s.t. op(weight(x), w) = weight(y)
  // つまり w = weight(y) - weight(x)
  T diff(int x, int y) { 
    return op(weight(y), inv(weight(x))); 
  }
};
#pragma endregion
#pragma region normal-weighted-unionfind-hisuisyo
class WeightedUF{
	int n;
	vector<int>par,rank;
	vector<ll>diff_weight;
public:
	WeightedUF(int n_)
		:n(n_),par(n_),rank(n_),diff_weight(n_){
		iota(par.begin(),par.end(),0);
	}
	//xの根
	int find(int x){
		if(par[x]==x)
			return x;
		int r=find(par[x]);
		diff_weight[x]+=diff_weight[par[x]];
		return par[x]=r;
	}
	//weight[x]
	ll weight(int x){
		find(x);
		return diff_weight[x];
	}
	//weight[y]-weight[x]
	//same(x,y)はfalse
	ll diff(int x,int y){
		return weight(y)-weight(x);
	}
	//weight[y]=weight[x]+w;
	//same(x,y)の時はなにもしない
	bool unite(int x,int y,ll w){
		w+=weight(x);
		w-=weight(y);
		x=find(x);
		y=find(y);
		if(x==y)
			return false;
		if(rank[x]<rank[y]){
			par[x]=y;
			diff_weight[x]=-w;
		}else{
			par[y]=x;
			diff_weight[y]=w;
			if(rank[x]==rank[y])rank[x]++;
		}
		return true;
	}
	//xとyが同じところにあるか
	bool same(int x,int y){
		return find(x)==find(y);
	}
};
#pragma endregion
//0<=i<=nでjbit目が立っているものの個数
long long mbit(long long j,long long n){
  long long res=((n>>(j+1))<<j);//(n/(2^(j+1)))*(2^j)
  if(n&(1ll<<j)){
    res+=((n&((1ll<<j)-1))+1);//if n>=(2^j) res+=(n%(2^(j+1))-(2^j)+1
  }
  return res;
}

ll keta(vvvi &dp,vi &n,ll a,ll k){
  dp[0][0][0]=1;
  rep(i,0,a-1){
    rep(smaller,0,1){
      rep(j,0,a-1){
        rep(x,0,(smaller ? 9 : n[i])){
          dp[i+1][smaller||x<n[i]][j+(bo)(x)]+=dp[i][smaller][j];
        }
      }
    }
  }
  return dp[a][0][k]+dp[a][1][k];
}
template <typename T> struct merge_sort_tree {
    int N;
    std::vector<std::vector<T>> x;
    merge_sort_tree() = default;
    merge_sort_tree(const std::vector<T> &vec) : N(vec.size()) {
        x.resize(N * 2);
        for (int i = 0; i < N; i++) x[N + i] = {vec[i]};
        for (int i = N - 1; i; i--) {
            std::merge(x[i * 2].begin(), x[i * 2].end(), x[i * 2 + 1].begin(), x[i * 2 + 1].end(),
                       std::back_inserter(x[i]));
        }
    }
    int cntLess(int l, int r, T query) const {
        l += N, r += N;
        int ret = 0;
        while (l < r) {
            if (l & 1)
                ret += std::lower_bound(x[l].begin(), x[l].end(), query) - x[l].begin(), l++;
            if (r & 1)
                r--, ret += std::lower_bound(x[r].begin(), x[r].end(), query) - x[r].begin();
            l >>= 1, r >>= 1;
        }
        return ret;
    }
    int cntLesseq(int l, int r, T query) const {
        l += N, r += N;
        int ret = 0;
        while (l < r) {
            if (l & 1)
                ret += std::upper_bound(x[l].begin(), x[l].end(), query) - x[l].begin(), l++;
            if (r & 1)
                r--, ret += std::upper_bound(x[r].begin(), x[r].end(), query) - x[r].begin();
            l >>= 1, r >>= 1;
        }
        return ret;
    }
    int cntMore(int begin, int end, T query) const {
        int tot = std::max(0, std::min(end, N) - std::max(begin, 0));
        return tot - cntLesseq(begin, end, query);
    }
    int cntMoreeq(int begin, int end, T query) const {
        int tot = std::max(0, std::min(end, N) - std::max(begin, 0));
        return tot - cntLess(begin, end, query);
    }

    template <class OStream> friend OStream &operator<<(OStream &os, const merge_sort_tree &clt) {
        os << '[';
        for (int i = 0; i < clt.N; i++) os << clt.x[clt.N + i][0] << ',';
        return os << ']';
    }
};
//i,j,k(0<i<j<k<n)それぞれについてa[i]*a[j]*a[k]の和
ll sansum(vi &a){
  ll ans=0,sum=0;
  for(ll i:a)sum+=i;
  ans=sum*sum*sum;
  for(ll i:a)ans-=i*i*i;
  for(ll i:a)ans-=(sum-i)*i*i*3;
  return ans/6;
}
//f^x(s)=g をもとめる n=limit m=sqrt(n)（だいたい）fm=f^m,mk_key=値を変換する,数値なら基本は
//mk_key=[&](ll x){return x;}みたいな感じでいい
template<typename T, typename U>
ll bsgs(T s, T g, ll n, ll m, function<T(T)> f, function<T(T)> fm, function<U(T)> mk_key) {
		if(s==g)return 0LL;
    map<U,int> mp;
	//Baby
    for (int i = 0; i < m; i++) {
				U key = mk_key(g);
				mp[key] = -i;
				g = f(g);
    }
	//Giant
    for (int i = 1; ; i++) {
        s = fm(s);
        U key = mk_key(s);
        if (mp.count(key)) return m*i+mp[key];
        if (n<=m*i) break;
    }
    return -1;
}
//fast_fact k! mod pをO(sqrt P log P)でもとめる　BabyStepGiantStep活用
ll fast_fact(ll k,ll p){
	return k;
}
//反転とかのみtreap 軽い
template<class S, S(*op)(S, S), S(*e)()>
class Treap {
	class xorshift {
		unsigned long long x;
	public:
		xorshift() {
			mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());
			x = rnd();
			for (int i = 0; i < 100; i++) {
				random();
			}
		}

		unsigned long long random() {
			x = x ^ (x << 7);
			return x = x ^ (x >> 9);
		}
	} rnd;
	struct Node {
		S val;
		Node* lch, *rch;
		int pri;
		int siz;
		S sum;
		bool rev;
		Node(S v, int p) :val(v), pri(p), siz(1), sum(v), rev(false) {
			lch = nullptr;
			rch = nullptr;
		}
	};
	Node* root;

	int size(Node* t) {
		if (t == nullptr)return 0;
		return t->siz;
	}
	S prod(Node* t) {
		if (t == nullptr)return e();
		return t->sum;
	}

	void push(Node* t) {
		if (t == nullptr)return;
		if (t->rev) {
			swap(t->lch, t->rch);
			if (t->lch)t->lch->rev ^= true;
			if (t->rch)t->rch->rev ^= true;
			t->rev = false;
		}
	}

	Node* update(Node* t) {
		t->siz = size(t->lch) + size(t->rch) + 1;
		t->sum = op(op(prod(t->lch), prod(t->rch)), t->val);
		return t;
	}

	Node* merge(Node* l, Node* r) {
		push(l);
		push(r);
		if (l == nullptr)return r;
		if (r == nullptr)return l;

		if (l->pri > r->pri) {
			l->rch = merge(move(l->rch), move(r));
			return update(move(l));
		}
		else {
			r->lch = merge(move(l), move(r->lch));
			return update(move(r));
		}
	}

	pair<Node*, Node*> split(Node* t, int k) {
		if (t == nullptr)return { nullptr,nullptr };

		push(t);
		if (k <= size(t->lch)) {
			pair<Node*, Node*> s = split(move(t->lch), k);
			t->lch = move(s.second);
			return { move(s.first),update(move(t)) };
		}
		else {
			pair<Node*, Node*> s = split(move(t->rch), k - size(t->lch) - 1);
			t->rch = move(s.first);
			return { update(move(t)),move(s.second) };
		}
	}

	Node* insert(Node* t, int k, S v) {
		auto [l, r] = split(move(t), k);
		Node* ret = merge(move(l), new Node(v, rnd.random()));
		ret = merge(move(ret), move(r));
		return ret;
	}

	Node* erase(Node* t, int k) {
		auto [l, r_aux] = split(move(t), k);
		auto [m, r] = split(move(r_aux), 1);
		delete m;
		return merge(move(l), move(r));
	}

	void reverse(Node* t, int l, int r) {
		auto [b_aux, c] = split(move(t), r);
		auto [a, b] = split(move(b_aux), l);
		if (b != nullptr)b->rev ^= true;
		auto t_aux = merge(move(a), move(b));
		t = merge(move(t_aux), move(c));
	}

	void rotate(Node* t, int l, int m, int r) {
		reverse(t, l, r);
		reverse(t, l, l + r - m);
		reverse(t, l + r - m, r);
	}

public:

	int size() {
		return size(root);
	}

	void insert(int k, S v) {
		assert(0 <= k and k <= size(root));
		root = insert(move(root), k, v);
	}

	void erase(int k) {
		assert(0 <= k and k < size(root));
		root = erase(move(root), k);
	}

	S prod(int l, int r) {
		assert(0 <= l and l <= r and r <= size(root));
		auto [a, b] = split(move(root), r);
		auto [c, d] = split(move(a), l);

		S ret = prod(d);

		root = merge(merge(move(c), move(d)), move(b));

		return ret;
	}

	S operator[](int k) {
		assert(0 <= k and k < size(root));
		return prod(k, k + 1);
	}

	void reverse(int l, int r) {
		assert(0 <= l and l <= r and r <= size(root));
		reverse(root, l, r);
	}

	void rotate(int l, int m, int r) {
		assert(0 <= l and l <= m and m <= r and r <= size(root));
		rotate(root, l, m, r);
	}

	void debug() {
		int n = size(root);
		if (n > 0)push(root);
		for (int i = 0; i < n; ++i) {
			cerr << (*this)[i] << " ";
		}
		cerr << endl;
	}
};
https://github.com/aoblue2547/competitive_programming_library/blob/main/Treap.md
namespace asalib::_internal::math {
  template<integral T>
  T modpow(T a, T n, T mod) {
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "T must be 32-bit or 64-bit integer type");
    constexpr bool is_64bit = sizeof(T) == 8;

    T res = 1;
    while (n) {
      if constexpr (is_64bit) {
        if (n & 1) res = static_cast<__uint128_t>(res) * a % mod;
        a = static_cast<__uint128_t>(a) * a % mod;
      }
      else {
        if (n & 1) res = static_cast<unsigned long long>(res) * a % mod;
        a = static_cast<unsigned long long>(a) * a % mod;
      }
      n >>= 1;
    }
    return res;
  }
}  // namespace asalib::_internal::math
namespace asalib::math {
  template<integral T>
  constexpr bool miller_rabin_test(T n) {
    if (n < 2) [[unlikely]]
      return false;
    if (n % 2 == 0) return n == 2;
    T d = n - 1, s = 0;
    while (d % 2 == 0) d >>= 1, ++s;
    assert(d % 2 == 1);

    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "T must be 32-bit or 64-bit integer type");
    constexpr bool is_64bit = sizeof(T) == 8;

    function<bool(T)> test = [&](T a) {
      T x = _internal::math::modpow(a, d, n);
      if (x == 0) [[unlikely]]
        return false;
      if (x == 1) [[unlikely]]
        return true;
      for (T _ = 0; _ < s; ++_) {
        T y;
        if constexpr (is_64bit)
          y = static_cast<__uint128_t>(x) * x % n;
        else
          y = static_cast<unsigned long long>(x) * x % n;
        if (y == 1 && x != 1 && x != n - 1) return false;
        x = y;
      }
      return x == 1;
    };

    function<bool(vector<T>)> tests = [&](vector<T> basis) {
      for (T a : basis)
        if (!test(a)) return false;
      return true;
    };

    if (n < 2'047) return test(2);
    if (n < 1'373'653) return tests({ 2, 3 });
    if (n < 9'080'191) return tests({ 31, 73 });
    if (n < 25'326'001) return tests({ 2, 3, 5 });
    if (n < 3'215'031'751) return tests({ 2, 3, 5, 7 });
    if (n < 4'759'123'141) return tests({ 2, 7, 61 });
    if (n < 1'122'004'669'633) return tests({ 2, 13, 23, 1662803 });
    if (n < 2'152'302'898'747) return tests({ 2, 3, 5, 7, 11 });
    if (n < 3'474'749'660'383) return tests({ 2, 3, 5, 7, 11, 13 });
    if (n < 341'550'071'728'321) return tests({ 2, 3, 5, 7, 11, 13, 17 });
    if (n < 3'825'123'056'546'413'051) return tests({ 2, 3, 5, 7, 11, 13, 17, 19, 23 });
    if (static_cast<unsigned long long>(n) <= 18'446'744'073'709'551'615ULL) return tests({ 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 });
    // 18'446'744'073'709'551'616 は 2^64 であり、これ以上の数は 64 ビット整数型では表現できない
    // TODO: 多倍長整数を使う場合は if を追加する
    // TODO: C++23 以降なら __builtin_unreachable() -> std::unreachable() に変更可能
    __builtin_unreachable();
  }
}  // namespace asalib::math
namespace asalib::math {
  template<integral T>
  constexpr vector<pair<T, T>> pollards_rho(T n) {
    if (n < 2) [[unlikely]]
      return {};

    vector<pair<T, T>> res(0);
    // ある程度小さい素因数については前もって素因数分解しておく
    for (T i = 2; i * i <= min(static_cast<T>(10000), n); ++i) {
      if (n % i == 0) {
        T e = 0;
        while (n % i == 0) n /= i, ++e;
        res.emplace_back(i, e);
      }
    }
    if (n == 1) [[unlikely]]
      return res;

    // ここからメイン
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "T must be 32-bit or 64-bit integer type");
    constexpr bool is_64bit = sizeof(T) == 8;
    auto f = [&](T x, T s) -> T {
      if constexpr (is_64bit)
        return (static_cast<__uint128_t>(x) * x + s) % n;
      else
        return (static_cast<unsigned long long>(x) * x + s) % n;
    };
    function<void(T&, T)> rho = [&](T& num, T cnt) {
      if (num == 1) [[unlikely]]
        return;
      if (miller_rabin_test(num)) {
        res.emplace_back(num, cnt);
        num = 1;
        return;
      }
      T s = 0;
      while (true) {
        T x = ++s, y = f(x, s);
        while (true) {
          T p = gcd(y - x + num, num);
          if (p == 0 || p == num) break;  // 失敗、 seed 変えてやり直し
          if (p != 1) {
            assert(num % p == 0);
            T e = 0;
            while (num % p == 0) num /= p, ++e;
            rho(p, cnt * e);
            if (num > 1) rho(num, cnt);
            return;
          }
          x = f(x, s), y = f(f(y, s), s);
        }
      }
    };
    while (n > 1) rho(n, 1);

    ranges::sort(res);
    return res;
  }
}  // namespace asalib::math
//素因数分解

#if __has_include(<atcoder/all>)
#include <atcoder/all>
using namespace atcoder;
using minta = modint998244353;
using mintb = modint1000000007;
std::ostream&operator<<(std::ostream& os,minta v){os << v.val();return os;}
std::istream&operator>>(std::istream&is,minta &v){long long t;is >> t;v=t;return is;}
#define HAS_ACL 1
#else
#define HAS_ACL 0
#endif
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

struct FastIO {
    char *p;
    char out_buf[1 << 21];
    int out_pos = 0;

    FastIO() {
        struct stat st;
        if (isatty(0) == 0 && fstat(0, &st) == 0 && st.st_size > 0) {
            p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, 0, 0);
        } else p = nullptr;
    }
    ~FastIO() { flush(); }

    void flush() {
        if (out_pos > 0) {
            write(1, out_buf, out_pos);
            out_pos = 0;
        }
    }

    void write_char(char c) {
        out_buf[out_pos++] = c;
        if (out_pos > (1 << 20)) flush();
    }

    // --- 入力ロジック ---
    template <typename T> void read_recursive(T &x) {
        if(!p){
            cin >> x;
            return;
        }
        #if HAS_ACL
        if constexpr (is_same_v<T, minta> || is_same_v<T, mintb>) {
            long long t; read_recursive(t); x = t; return;
        }
        #endif
        if constexpr (is_integral_v<T> || is_same_v<T, __int128_t>) {
            x = 0; bool neg = false;
            while (p && *p && (*p < '0' || *p > '9') && *p != '-') p++;
            if (!p || !*p) return;
            if (*p == '-') { neg = true; p++; }
            while (*p >= '0' && *p <= '9') x = x * 10LL + (*p++ - '0');
            if (neg) x = -x;
        } else if constexpr (is_floating_point_v<T>) {
            double res = 0, f = 1; bool neg = false;
            while (p && *p && (*p < '0' || *p > '9') && *p != '-') p++;
            if (!p || !*p) return;
            if (*p == '-') { neg = true; p++; }
            while (*p >= '0' && *p <= '9') res = res * 10 + (*p++ - '0');
            if (*p == '.') {
                p++;
                while (*p >= '0' && *p <= '9') res += (f /= 10) * (*p++ - '0');
            }
            x = (neg ? -res : res);
        } else if constexpr (is_same_v<T, string>) {
            x.clear();
            while (p && *p && *p <= ' ') p++;
            if (!p || !*p) return;
            while (*p > ' ') x += *p++;
        } else if constexpr (is_pair<T>::value) {
            read_recursive(x.first); read_recursive(x.second);
        } else {
            for (auto &it : x) read_recursive(it);
        }
    }

    // --- 出力ロジック ---
    void write_int64(long long n) {
        if (n == 0) { write_char('0'); return; }
        if (n < 0) { write_char('-'); n = -n; }
        char tmp[20]; int t = 0;
        while (n > 0) { tmp[t++] = (char)((n % 10) + '0'); n /= 10; }
        while (t--) write_char(tmp[t]);
    }

    void write_int128(__int128_t n) {
        if (n == 0) { write_char('0'); return; }
        if (n < 0) { write_char('-'); n = -n; }
        char tmp[40]; int t = 0;
        while (n > 0) { tmp[t++] = (char)((n % 10) + '0'); n /= 10; }
        while (t--) write_char(tmp[t]);
    }

    void write_double(double x) {
        if (x < 0) { write_char('-'); x = -x; }
        write_int64((long long)x);
        write_char('.');
        x -= (long long)x;
        for (int i = 0; i < 15; i++) {
            x *= 10; int d = (int)x;
            write_char(d + '0'); x -= d;
        }
    }

    template <typename T> void write_recursive(const T &x) {
        #if HAS_ACL
        if constexpr (is_same_v<T, minta> || is_same_v<T, mintb>) {
            write_int64((long long)x.val()); return;
        }
        #endif
        if constexpr (is_floating_point_v<T>) {
            write_double((double)x);
        } else if constexpr (is_same_v<T, __int128_t>) {
            write_int128(x);
        } else if constexpr (is_integral_v<T>) {
            write_int64((long long)x);
        } else if constexpr (is_same_v<T, string>) {
            for (char c : x) write_char(c);
        } else if constexpr (is_same_v<T, char>) {
            write_char(x);
        } else if constexpr (is_pair<T>::value) {
            write_char('('); write_recursive(x.first);
            write_char(','); write_recursive(x.second);
            write_char(')');
        } else {
            for (int i = 0; i < (int)x.size(); i++) {
                write_recursive(x[i]);
                if (i + 1 != (int)x.size()) write_char(' ');
            }
        }
    }

    template<typename T> struct is_pair : std::false_type {};
    template<typename T, typename U> struct is_pair<std::pair<T, U>> : std::true_type {};

} io;

template <typename... Args> void in(Args &...args) { (io.read_recursive(args), ...); }
template <typename... Args> void out(const Args &...args) { 
    int n = sizeof...(Args), i = 0;
    ( (io.write_recursive(args), io.write_char(++i == n ? '\n' : ' ')), ... );
}
//disjoint sparse table
template <class S,
          auto op,
          auto e>
struct disjoint_sparse_table {
    static_assert(std::is_convertible_v<decltype(op), std::function<S(S, S)>>,
                  "op must work as S(S, S)");
    static_assert(std::is_convertible_v<decltype(e), std::function<S()>>,
                  "e must work as S()");

  public:
    explicit disjoint_sparse_table(const std::vector<S> &v) : _n(v.size()) {
        int n = _n + 2;
        int h = 32 - std::countl_zero((unsigned)n - 1);
        sum.assign(h, std::vector<S>(n, e()));

        for (int k = 1; k < h; k++) {
            std::vector<S> &s = sum[k];
            int w = 1 << k;
            for (int i = w; i < n; i += w + w) {
                for (int j = i - 1; j > i - w; j--) {
                    s[j - 1] = op(v[j - 1], s[j]);
                }
                for (int j = i; j < std::min(i + w, n) - 1; j++) {
                    s[j + 1] = op(s[j], v[j - 1]);
                }
            }
        }
    }

    S get(int i) const {
        assert(0 <= i && i < _n);
        return prod(i, i + 1);
    }

    S prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        r++;
        const std::vector<S> &s = sum[31 - std::countl_zero((unsigned)l ^ r)];
        return op(s[l], s[r]);
    }

  private:
    int _n;
    std::vector<std::vector<S>> sum;
};


template<typename T,typename Compare=std::less<T>>
class SlidingWindow{
private:
  struct Node{
    T value;
    long long id;
  };
  std::deque<Node> deq;
  Compare comp;
public:
  SlidingWindow()=default;
  explicit SlidingWindow(Compare c):comp(c){}
  void push(const T& val,long long id){
    while(!deq.empty()&&!comp(deq.back().value,val)) deq.pop_back();
    deq.push_back({val,id});
  }
  void pop(long long id){
    if(!deq.empty()&&deq.front().id<=id) deq.pop_front();
  }
  const T& get()const{return deq.front().value;}
  const long long get_id()const{return deq.front().id;}
  bool empty()const{return deq.empty();}
};

// --- セグメント木の構造定義（面積・重なり数用） ---
struct S {
    int min_val; // 現在いくつの長方形に覆われているか(min)<-こうすることで面積が幅の和-min_val==0のときの幅の和で求まる
    ll cnt;      // その重なり数を持つ区間の長さ(y座標の差)の合計
};
using F = int; // 区間加算（重なりの増減）

S op(S a, S b) {
    if (a.min_val == b.min_val) return {a.min_val, a.cnt + b.cnt};
    return (a.min_val < b.min_val ? a : b);
}
S e() { return {1000000000, 0}; }
S mapping(F f, S x) { return {x.min_val + f, x.cnt}; }//区間に長方形を追加！
F composition(F f, F g) { return f + g; }
F id() { return 0; }

// --- 抽象化走査線クラス ---
struct Union_of_Rectangles {
    struct Event {
      ll x;
      ll y1, y2;
      int type; // 0: Rectangle Add, 1: Point Query, 2: Rectangle Remove
      int id;   // 点クエリ用
      bool operator<(const Event& o) const {
              if (x != o.x) return x < o.x;
              return type < o.type; // 同じXなら 追加 -> クエリ -> 削除 の順
      }
    };
    vector<Event> events;
    vector<ll> y_coords;//圧倒的座標圧縮
    void add_rectangle(ll x1, ll y1, ll x2, ll y2) {
      events.push_back({x1, y1, y2, 0, -1});
      events.push_back({x2, y1, y2, 2, -1});
      y_coords.push_back(y1);
      y_coords.push_back(y2);
    }
    void add_point_query(ll x, ll y, int id) {
      events.push_back({x, y, y, 1, id});
      y_coords.push_back(y);
    }
    // 実行関数
    struct Result {
      ll area;
      vector<bool> point_contained;
    };
    Result solve() {
      if (events.empty()) return {0, {}};
      sort(events.begin(), events.end());
      sort(y_coords.begin(), y_coords.end());
      y_coords.erase(unique(y_coords.begin(), y_coords.end()), y_coords.end());

      int m = y_coords.size();
      vector<S> initial_s(m - 1);
      for (int i = 0; i < m - 1; ++i) {
        initial_s[i] = {0, y_coords[i + 1] - y_coords[i]};
      }

      atcoder::lazy_segtree<S, op, e, F, mapping, composition, id> seg(initial_s);
      ll total_width = y_coords.back() - y_coords.front();

      ll ans_area = 0;
      vector<bool> point_res;
      ll prev_x = events[0].x;

      for (auto& ev : events) {
          // Xが動いた分の面積を加算
          S res = seg.all_prod();
          ll covered_len = total_width - (res.min_val == 0 ? res.cnt : 0);
          ans_area += covered_len * (ev.x - prev_x);
          prev_x = ev.x;
          if (ev.type == 0) { // Add
              int l = lower_bound(y_coords.begin(), y_coords.end(), ev.y1) - y_coords.begin();
              int r = lower_bound(y_coords.begin(), y_coords.end(), ev.y2) - y_coords.begin();
              seg.apply(l, r, 1);
          } else if (ev.type == 2) { // Remove
              int l = lower_bound(y_coords.begin(), y_coords.end(), ev.y1) - y_coords.begin();
              int r = lower_bound(y_coords.begin(), y_coords.end(), ev.y2) - y_coords.begin();
              seg.apply(l, r, -1);
          } else if (ev.type == 1) { // Point Query
              int idx = lower_bound(y_coords.begin(), y_coords.end(), ev.y1) - y_coords.begin();
              // 座標が区間の境界上の場合の処理
              int val = seg.get(idx).min_val;
              // 点の包含結果を格納（実際にはIDで紐付ける）
          }
      }
      return {ans_area, point_res};
    }
};

/*@brief monotone minima ２変数関数f(i,j)についてn*mの範囲で各行ごとに最小値を求めた配列を返す
  @brief なお、その配列の答えが広義単調増加になっていないといけない
  @brief また、この実装では同じ行に同じ値が複数あったら一番左のposを返す
  @brief monge性（四辺形不等式が成り立つ）->tortally monotone->monotone
  @brief 計算量はO(p log p) <-pってなんだよ（困惑）
*/
namespace MonotoneMinima {
  template <typename T, typename Compare, typename F>
  void solve(int u, int d, int l, int r, std::vector<int>& idx, const F& f, const Compare& comp,int diff=0) {
    if (u > d) return;
    int mid_row = std::midpoint(u, d);
    int best_col = l;
    T best_val = f(mid_row, l);
    for(int j = l + 1; j <= r; ++j) {
      T cur_val = f(mid_row, j);
      if (comp(cur_val, best_val)) {
        best_val = cur_val;
        best_col = j;
      }
    }
    idx[mid_row-diff] = best_col;
    solve<T>(u, mid_row - 1, l, best_col, idx, f, comp, diff);
    solve<T>(mid_row + 1, d, best_col, r, idx, f, comp, diff);
  }
  template <typename T = long long, typename Compare = std::less<T>, typename F>
  std::vector<int> get(int n, int m, const F& f, Compare comp = Compare()) {
    vector<int> idx(n);
    if(n>0)solve<T>(0, n - 1, 0, m - 1, idx, f, comp);
    return idx;
  }
  /*@brief dp[i]=min((j=0..i-1)dp[j]+w(j,i))みたいなものを解く
    @brief ちゃんとmonotone満たしてるよな?
    @brief 
  */
  template <typename T, typename F, typename Compare = std::less<T>>
    void solve_1d1d(int n, std::vector<T>& dp, const F& w ,T bad,Compare comp = Compare()) {
    auto rec = [&](auto self, int L, int R) -> void {
      if (L == R) return;
      int mid = std::midpoint(L, R);
        
      self(self, L, mid);//左半分を固定
      auto cost=[&](int i,int j)->T{
        if(j>mid or j<L or j>=i or dp[j]==bad)return bad;
        return dp[j]+w(j,i);
      };
      // 2. 左から右への遷移を一括計算
      // i: 遷移先(行), j: 遷移元(列)
      std::vector<int> best_j(R - mid);
      solve<T>(mid + 1, R, L, mid, best_j, cost, comp, mid+1);
        for (int i = mid + 1; i <= R; ++i) {
          T val = cost(i ,best_j[i - (mid + 1)]);
          if (val != bad && (dp[i] == bad || comp(val, dp[i]))) {
            dp[i] = val;
          }
        }
      self(self, mid + 1, R);
    };
    rec(rec, 0, n);
  }
}

/**
 * @brief Aliens DP (WQS Binary Search) の実装例
 * @param n 要素数
 * @param k 選択したい個数
 * @param solve_dp 罰金 p を与えた時に {最大利益, 選んだ個数} を返す関数
 * @param 最大化したいので罰金pをしたときの値が上に凸である必要がある
 * @param 最小化したいなら下に凸
 */
long long solve_aliens(int n, int k, auto solve_dp) {
    // 罰金（コスト）の範囲を二分探索
    // 最小化問題なら罰金を大きくすると個数が減り、最大化なら罰金を大きくすると個数が減る
    long long low = -1e12, high = 1e12; 
    long long ans = 0;

    for (int i = 0; i < 100; ++i) { // 精度が必要な場合は回数を増やすか while (low <= high)
        long long mid = (low + high) / 2;
        auto [val, count] = solve_dp(mid);

        if (count >= k) {
            // 個数が多すぎる（または丁度）なら、もっと罰金を重くして個数を減らす方向へ
            ans = val - (long long)k * mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans;
}
  /*/*@brief 木上のAliens DPのイメージ ちょうど長さがkのpathの重みの最大値を求めたい
  struct Result {
    long long score;
    int count;

    // スコア優先、同じならカウント（辺の数）が多い方を優先
    bool operator<(const Result& other) const {
        if (score != other.score) return score < other.score;
        return count < other.count;
    }
    Result operator+(const Result& other) const {
        return {score + other.score, count + other.count};
    }
  };
  // 罰金 P のもとでの Tree DP
auto solve_dp = [&](long long P) -> pair<long long, int> {
    int N = G.size();
    vector<Result> dp0(N, {0, 0}), dp1(N, {-1e18, 0});

    auto dfs = [&](auto self, int u, int p) -> void {
        dp1[u] = {0, 0}; // 頂点 u だけ（辺 0 本）のパスで初期化

        for (auto& edge : G[u]) {
            if (edge.to == p) continue;
            self(self, edge.to, u);

            // 1. u でパスを完結させるパターン (dp1[u] + dp1[v] + w - P)
            Result candidate = dp1[u] + dp1[edge.to] + Result{edge.cost - P, 1};
            dp0[u] = max({dp0[u], dp0[edge.to], candidate});

            // 2. u を端点として上に伸ばすパターン (dp1[v] + w - P)
            dp1[u] = max(dp1[u], dp1[edge.to] + Result{edge.cost - P, 1});
        }
    };

    dfs(dfs, 0, -1);
    Result final_res = max(dp0[0], dp1[0]);
    return {final_res.score, final_res.count};
  };
  long long low = -1e12, high = 1e12; // 辺の重みの範囲に応じて調整
  long long ans = 0;

  for (int i = 0; i < 100; ++i) {
    long long mid = (low + high) / 2;
    auto [val, count] = solve_dp(mid);

    if (count >= K) {
        // 辺が K 個以上選べるなら、罰金を重くして個数を減らしにいく
        // 同時に「今の罰金 mid での K 個相当のスコア」を暫定解とする
        ans = val + (long long)K * mid; 
        low = mid + 1;
    } else {
        high = mid - 1;
    }
  }
  */

  #include <vector>
#include <functional>
#include <algorithm>

using namespace std;

/**
 * @brief 永続セグメント木 (Fully Persistent Segment Tree)
 * * 通常のセグメント木の完全永続版。
 * ノードの再利用（ポインタ貼り替え）により、過去のバージョンの保持や
 * 部分木のコピーを $O(\log N)$ で行う。
 * * @tparam T データ型
 * @tparam F マージ関数オブジェクト型 (T, T) -> T
 * @tparam NODES ノードプールの最大数（メモリ制限に応じて調整）
 */
template <typename T, typename F, int NODES = 20000000>
struct PersistentSegmentTree {
  using ll = long long;

  /** @struct Node @brief セグメント木の各ノード */
  struct Node {
    T data;
    Node *l, *r;
    Node() {}
    Node(const T &_data) : data(_data), l(nullptr), r(nullptr) {}
  };

  Node *pool;
  int pid;
  ll N;
  const F f;
  const T ID;
  Node *nil;
  vector<Node *> roots;

  /**
   * @brief 初期配列から構築するコンストラクタ
   * @param v 初期データ
   * @param f_ マージ関数
   * @param ID_ 単位元
   */
  PersistentSegmentTree(const vector<T> &v, const F &f_, const T &ID_)
      : pid(0), f(f_), ID(ID_), nil(nullptr) {
    pool = new Node[NODES];
    nil = my_new(ID);
    nil->l = nil->r = nil;
    roots.reserve(262144);
    roots.push_back(build(v));
  }

  /**
   * @brief サイズのみ指定して構築するコンストラクタ
   * @param N_ 配列サイズ
   * @param f_ マージ関数
   * @param ID_ 単位元
   */
  PersistentSegmentTree(const ll &N_, const F &f_, const T &ID_)
      : pid(0), N(N_), f(f_), ID(ID_), nil(nullptr) {
    pool = new Node[NODES];
    nil = my_new(ID);
    nil->l = nil->r = nil;
    roots.reserve(262144);
    roots.push_back(nil);
  }

  /** @brief 新しいノードをプールから割り当てる */
  Node *my_new(const T &data) {
    pool[pid].data = data;
    pool[pid].l = pool[pid].r = nil;
    return &(pool[pid++]);
  }

  /** @brief 二つの子をマージして新しい親を作る */
  Node *merge(Node *l, Node *r) {
    pool[pid].data = f(l->data, r->data);
    pool[pid].l = l;
    pool[pid].r = r;
    return &(pool[pid++]);
  }

  /** @brief ベクトルから全ノードを構築 */
  Node *build(const vector<T> &v) {
    N = (ll)v.size();
    return build(0, N, v);
  }

  /** @brief 再帰的に構築を行う内部関数 */
  Node *build(ll l, ll r, const vector<T> &v) {
    if (l + 1 == r) return my_new(v[l]);
    ll m = (l + r) >> 1;
    return merge(build(l, m, v), build(m, r, v));
  }

 private:
  Node *update_(ll a, const T &x, Node *n, ll l, ll r) {
    if (l + 1 == r) return my_new(x);
    ll m = (l + r) >> 1;
    if (a < m) return merge(update_(a, x, n->l, l, m), n->r);
    return merge(n->l, update_(a, x, n->r, m, r));
  }

  Node *add_(ll a, const T &x, Node *n, ll l, ll r) {
    if (l + 1 == r) return my_new(f(n->data, x));
    ll m = (l + r) >> 1;
    if (a < m) return merge(add_(a, x, n->l, l, m), n->r);
    return merge(n->l, add_(a, x, n->r, m, r));
  }

  T query_(ll a, ll b, Node *n, ll l, ll r) {
    if (n == nil || r <= a || b <= l) return ID;
    if (a <= l && r <= b) return n->data;
    ll m = (l + r) >> 1;
    return f(query_(a, b, n->l, l, m), query_(a, b, n->r, m, r));
  }

  Node *copy_range_(Node *n_dst, ll u, ll v, Node *n_src, ll l, ll r, ll cur_l, ll cur_r) {
    if (u <= cur_l && cur_r <= v) return n_src;
    ll m = (cur_l + cur_r) >> 1;
    Node *nl = n_dst->l, *nr = n_dst->r;
    if (u < m) nl = copy_range_(n_dst->l, u, v, n_src->l, l, r, cur_l, m);
    if (v > m) nr = copy_range_(n_dst->r, u, v, n_src->r, l, r, m, cur_r);
    return merge(nl, nr);
  }

  template <typename C>
  ll max_right_(Node *n, ll l, ll r, ll a, ll b, T &cur, C &check) {
    if (n == nil || r <= a || b <= l) return b;
    if (a <= l && r <= b) {
      T next_prod = f(cur, n->data);
      if (check(next_prod)) {
        cur = next_prod;
        return b;
      }
      if (l + 1 == r) return l;
    }
    ll m = (l + r) >> 1;
    ll res = max_right_(n->l, l, m, a, b, cur, check);
    if (res != b) return res;
    return max_right_(n->r, m, r, a, b, cur, check);
  }

  template <typename C>
  ll min_left_(Node *n, ll l, ll r, ll a, ll b, T &cur, C &check) {
    if (n == nil || r <= a || b <= l) return a;
    if (a <= l && r <= b) {
      T next_prod = f(n->data, cur);
      if (check(next_prod)) {
        cur = next_prod;
        return a;
      }
      if (l + 1 == r) return r;
    }
    ll m = (l + r) >> 1;
    ll res = min_left_(n->r, m, r, a, b, cur, check);
    if (res != a) return res;
    return min_left_(n->l, l, m, a, b, cur, check);
  }

 public:
  // --- スロット管理 (Slot Management) ---

  /** @brief 指定した数のスロットを初期状態で確保する */
  void init_slots(int num, const vector<T> &v = {}) {
    roots.clear();
    Node *base = v.empty() ? nil : build(v);
    roots.assign(num, base);
  }

  /** @brief 新しいスロットを追加してインデックスを返す */
  int create_slot(Node *n = nullptr) {
    roots.push_back(n ? n : nil);
    return (int)roots.size() - 1;
  }

  /** @brief スロット t を非破壊更新し、結果でスロット t を上書きする */
  void update_at(int t, ll k, const T &x) { roots[t] = update_(k, x, roots[t], 0, N); }
  
  /** @brief スロット t に作用させ、結果でスロット t を上書きする */
  void add_at(int t, ll k, const T &x) { roots[t] = add_(k, x, roots[t], 0, N); }
  
  /** @brief スロット t の区間 [u,v) を スロット s の区間 [l,r) で上書きする */
  void copy_range_at(int t, ll u, ll v, int s, ll l, ll r) {
    roots[t] = copy_range_(roots[t], u, v, roots[s], l, r, 0, N);
  }

  /** @brief スロット t にスロット s のポインタを代入する */
  void assign_at(int t, int s) { roots[t] = roots[s]; }

  // --- 履歴追加モード (History Mode) ---

  /** @brief 最新の状態を更新し、新しいスロットとして末尾に追加する */
  int update(ll k, const T &x) { return create_slot(update_(k, x, roots.back(), 0, N)); }
  
  /** @brief バージョン t を元に更新し、新しいスロットとして末尾に追加する */
  int update_from(int t, ll k, const T &x) { return create_slot(update_(k, x, roots[t], 0, N)); }
  
  /** @brief 最新の状態に作用させ、新しいスロットとして末尾に追加する */
  int add(ll k, const T &x) { return create_slot(add_(k, x, roots.back(), 0, N)); }
  
  /** @brief 置換を行い、結果を新しいスロットとして末尾に追加する */
  int copy_range(int t_dst, ll u, ll v, int t_src, ll l, ll r) {
    return create_slot(copy_range_(roots[t_dst], u, v, roots[t_src], l, r, 0, N));
  }

  // --- クエリ・二分探索 ---

  /** @brief スロット t の区間 [a, b) の積を取得する */
  T query_at(int t, ll a, ll b) { return query_(a, b, roots[t], 0, N); }

  /**
   * @brief スロット t において check(f(query(l, r))) が true となる最大の r を探索
   * @tparam C check関数の型 bool(T)
   */
  template <typename C>
  ll max_right(int t, ll l, C check) {
    T cur = ID;
    return max_right_(roots[t], 0, N, l, N, cur, check);
  }

  /**
   * @brief スロット t において check(f(query(l, r))) が true となる最小の l を探索
   * @tparam C check関数の型 bool(T)
   */
  template <typename C>
  ll min_left(int t, ll r, C check) {
    T cur = ID;
    return min_left_(roots[t], 0, N, 0, r, cur, check);
  }

  // --- ユーティリティ ---
  /** @brief スロット t の根のポインタを取得 */
  Node *get_root(int t) { return roots[t]; }
  /** @brief 空の木のポインタを取得 */
  Node *new_tree() { return nil; }
  /** @brief 現在保持しているスロット（バージョン）数を取得 */
  int current_size() { return (int)roots.size(); }
};

// montgomery modint (MOD < 2^62, MOD is odd)
struct MontgomeryModInt64 {
    using mint = MontgomeryModInt64;
    using u64 = uint64_t;
    using u128 = __uint128_t;
    
    // static menber
    static u64 MOD;
    static u64 INV_MOD;  // INV_MOD * MOD ≡ 1 (mod 2^64)
    static u64 T128;  // 2^128 (mod MOD)
    
    // inner value
    u64 val;
    
    // constructor
    MontgomeryModInt64() : val(0) { }
    MontgomeryModInt64(long long v) : val(reduce((u128(v) + MOD) * T128)) { }
    u64 get() const {
        u64 res = reduce(val);
        return res >= MOD ? res - MOD : res;
    }
    
    // mod getter and setter
    static u64 get_mod() { return MOD; }
    static void set_mod(u64 mod) {
        assert(mod < (1LL << 62));
        assert((mod & 1));
        MOD = mod;
        T128 = -u128(mod) % mod;
        INV_MOD = get_inv_mod();
    }
    static u64 get_inv_mod() {
        u64 res = MOD;
        for (int i = 0; i < 5; ++i) res *= 2 - MOD * res;
        return res;
    }
    static u64 reduce(const u128 &v) {
        return (v + u128(u64(v) * u64(-INV_MOD)) * MOD) >> 64;
    }
    
    // arithmetic operators
    mint operator - () const { return mint() - mint(*this); }
    mint operator + (const mint &r) const { return mint(*this) += r; }
    mint operator - (const mint &r) const { return mint(*this) -= r; }
    mint operator * (const mint &r) const { return mint(*this) *= r; }
    mint operator / (const mint &r) const { return mint(*this) /= r; }
    mint& operator += (const mint &r) {
        if ((val += r.val) >= 2 * MOD) val -= 2 * MOD;
        return *this;
    }
    mint& operator -= (const mint &r) {
        if ((val += 2 * MOD - r.val) >= 2 * MOD) val -= 2 * MOD;
        return *this;
    }
    mint& operator *= (const mint &r) {
        val = reduce(u128(val) * r.val);
        return *this;
    }
    mint& operator /= (const mint &r) {
        *this *= r.inv();
        return *this;
    }
    mint inv() const { return pow(MOD - 2); }
    mint pow(u128 n) const {
        mint res(1), mul(*this);
        while (n > 0) {
            if (n & 1) res *= mul;
            mul *= mul;
            n >>= 1;
        }
        return res;
    }

    // other operators
    bool operator == (const mint &r) const {
        return (val >= MOD ? val - MOD : val) == (r.val >= MOD ? r.val - MOD : r.val);
    }
    bool operator != (const mint &r) const {
        return (val >= MOD ? val - MOD : val) != (r.val >= MOD ? r.val - MOD : r.val);
    }
    friend istream& operator >> (istream &is, mint &x) {
        long long t;
        is >> t;
        x = mint(t);
        return is;
    }
    friend ostream& operator << (ostream &os, const mint &x) {
        return os << x.get();
    }
    friend mint modpow(const mint &r, long long n) {
        return r.pow(n);
    }
    friend mint modinv(const mint &r) {
        return r.inv();
    }
};

typename MontgomeryModInt64::u64
MontgomeryModInt64::MOD, MontgomeryModInt64::INV_MOD, MontgomeryModInt64::T128;
// Miller-Rabin
bool MillerRabin(long long N, const vector<long long> &A) {
    assert(N % 2 == 1);
    assert(N < (1LL<<62));
    using mint = MontgomeryModInt64;
    mint::set_mod(N);
    
    long long s = 0, d = N - 1;
    while (d % 2 == 0) {
        ++s;
        d >>= 1;
    }
    for (auto a : A) {
        if (N <= a) continue;
        mint x = mint(a).pow(d);
        if (x != 1) {
            long long t;
            for (t = 0; t < s; ++t) {
                if (x == N - 1) break;
                x *= x;
            }
            if (t == s) return false;
        }
    }
    return true;
}

bool is_prime(long long N) {
    if (N <= 1) return false;
    else if (N == 2) return true;
    else if (N % 2 == 0) return false;
    else if (N < 4759123141LL)
        return MillerRabin(N, {2, 7, 61});
    else
        return MillerRabin(N, {2, 325, 9375, 28178, 450775, 9780504, 1795265022});
}

// Pollard's Rho
unsigned int xor_shift_rng() {
    static unsigned int tx = 123456789, ty=362436069, tz=521288629, tw=88675123;
    unsigned int tt = (tx^(tx<<11));
    tx = ty, ty = tz, tz = tw;
    return ( tw=(tw^(tw>>19))^(tt^(tt>>8)) );
}

long long pollard(long long N) {
    if (N % 2 == 0) return 2;
    if (is_prime(N)) return N;
    
    assert(N < (1LL<<62));
    using mint = MontgomeryModInt64;
    mint::set_mod(N);
    
    long long step = 0;
    while (true) {
        mint r = xor_shift_rng();  // random r
        auto f = [&](mint x) -> mint { return x * x + r; };
        mint x = ++step, y = f(x);
        while (true) {
            long long p = gcd((y - x).get(), N);
            if (p == 0 || p == N) break;
            if (p != 1) return p;
            x = f(x);
            y = f(f(y));
        }
    }
}

vector<long long> pollard_prime_factorize(long long N) {
    if (N == 1) return {};
    long long p = pollard(N);
    if (p == N) return {p};
    vector<long long> left = pollard_prime_factorize(p);
    vector<long long> right = pollard_prime_factorize(N / p);
    if (left.size() > right.size()) swap(left, right);
    left.insert(left.end(), right.begin(), right.end());
    sort(left.begin(), left.end());
    return left;
}

vector<pair<long long, long long>> prime_factorize(long long N) {
    vector<pair<long long, long long>> res;
    const auto &prs = pollard_prime_factorize(N);
    long long prev = -1, num = 0;
    for (const auto &pr : prs) {
        if (pr == prev) ++num;
        else {
            if (prev != -1) res.emplace_back(prev, num);
            prev = pr, num = 1;
        }
    }
    if (prev != -1) res.emplace_back(prev, num);
    return res;
}

// calc primitive root
constexpr int calc_primitive_root(long long m) {
    if (m == 1) return -1;
    if (m == 2) return 1;
    if (m == 998244353) return 3;
    if (m == 167772161) return 3;
    if (m == 469762049) return 3;
    if (m == 754974721) return 11;
    if (m == 645922817) return 3;
    if (m == 897581057) return 3;
    
    long long divs[20] = {};
    divs[0] = 2;
    long long cnt = 1;
    long long x = (m - 1) / 2;
    while (x % 2 == 0) x /= 2;
    for (long long i = 3; i * i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) x /= i;
        }
    }
    if (x > 1) divs[cnt++] = x;
    for (long long g = 2; ; g++) {
        bool ok = true;
        for (int i = 0; i < cnt; i++) {
            if (mod_pow(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}

// various methods mod prime P
struct PrimeProcessor {
    using mint = MontgomeryModInt64;
    
    // input prime
    long long prime;
    vector<pair<long long, long long>> pf;  // prime factorization of p-1
    
    // constructors
    PrimeProcessor() {}
    PrimeProcessor(long long p) : prime(p) {
        init(p);
    }
    
    // initializer
    void init(long long p) {
        assert(is_prime(p));
        prime = p;
        if (p % 2 == 1) {
            assert(p < (1LL<<62));
            prime = p;
            pf = prime_factorize(prime - 1);
            mint::set_mod(prime);
        }
    }
    
    // min: x s.t. a^x \equiv 1 (mod prime)
    long long calc_order(long long a) {
        assert(a != 0);
        if (prime == 2) return 1;
        long long res = prime - 1;
        for (const auto &[p, num] : pf) {
            while (res % p == 0 && mint(a).pow(res / p) == 1) res /= p;
        }
        return res;
    }
};

//超速いhash table

#include <boost/unordered/unordered_flat_map.hpp>
double get_time_sec() {
  using namespace std::chrono;
  return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}
struct Random {
  static random_device the_random_device;
  mt19937_64 rng;
  Random() : rng(the_random_device()) {}
  Random(unsigned seed) : rng(seed) {}
  ll range(ll i, ll j) {
    if (i >= j) {
      throw runtime_error("Random.range: invalid range");
    }
    uniform_int_distribution<ll> dist(i, j - 1);
    return dist(rng);
  }
  template<typename RandomIt>
  void shuffle(RandomIt first, RandomIt last) { std::shuffle(first, last, rng); }

};
random_device Random::the_random_device;

#if !defined(__TEMPLATE_SAFE_CUSTOM_HASH__)
#define __TEMPLATE_SAFE_CUSTOM_HASH__
template <typename T, typename Enable = void>
struct safe_custom_hash;
#endif
// For integer types (int, ll, u64, unsigned, ....)
template <typename T>
struct safe_custom_hash<T, typename enable_if<is_integral<T>::value>::type> {
  static uint64_t splitmix64(uint64_t x) {
    // http://xorshift.di.unimi.it/splitmix64.c
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
  }

  size_t operator()(uint64_t x) const {
    static const uint64_t FIXED_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
    return splitmix64(x + FIXED_RANDOM);
  }
};

// For string
template <>
struct safe_custom_hash<string, void> {
  static uint64_t mix(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
  }

  size_t operator()(const string& s) const {
    static const uint64_t seed = chrono::steady_clock::now().time_since_epoch().count();
    uint64_t h = seed ^ 0x9e3779b97f4a7c15ULL;
    const unsigned char* p = (const unsigned char*)s.data();
    size_t n = s.size();
    while (n >= 8) {
      uint64_t v;
      memcpy(&v, p, 8);
      h = mix(h ^ v);
      p += 8; n -= 8;
    }
    uint64_t tail = 0;
    for (size_t i = 0; i < n; ++i) tail |= uint64_t(p[i]) << (8*i);
    h = mix(h ^ tail);
    return (size_t)h;
  }
};

// For pair
template <typename T1, typename T2>
struct safe_custom_hash<pair<T1, T2>, void> {
  size_t operator()(const pair<T1, T2>& x) const {
    static const uint64_t frand = chrono::steady_clock::now().time_since_epoch().count();
    static const uint64_t a = (frand ^ 0x9e3779b97f4a7c15) | 1;
    static const uint64_t b = (frand ^ 0xbf58476d1ce4e5b9) | 1;
    return a * safe_custom_hash<T1>{}(x.first) + b * safe_custom_hash<T2>{}(x.second);
  }
};
//For ll2
template<typename T1,typename T2>
struct safe_custom_hash<ob2<T1,T2>,void>{
  size_t operator()(const ob2<T1,T2> &x) const {
    static const uint64_t frand=chrono::steady_clock::now().time_since_epoch().count();
    static const uint64_t a=(frand^ 0x9e3779b97f4a7c15) | 1;
    static const uint64_t b=(frand ^ 0xbf58476d1ce4e5b9) | 1;
    return a*safe_custom_hash<T1>{}(x.a) + b*safe_custom_hash<T2>{}(x.b);
  }
};

boost::unordered_flat_map<ll, ll, safe_custom_hash<ll>> mp5;

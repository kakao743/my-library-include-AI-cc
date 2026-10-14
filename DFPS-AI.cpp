// 複素数 FFT (Cooley-Tukey)
// T = double or long double
namespace FFT {
  using cd = complex<double>;
  const double PI = acos(-1.0);

  void fft(vector<cd>& a, bool inv) {
    int n = a.size();
    for (int i = 1, j = 0; i < n; ++i) {
      int bit = n >> 1;
      for (; j & bit; bit >>= 1) j ^= bit;
      j ^= bit;
      if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
      double ang = 2 * PI / len * (inv ? -1 : 1);
      cd wlen(cos(ang), sin(ang));
      for (int i = 0; i < n; i += len) {
        cd w(1);
        for (int j = 0; j < len/2; ++j) {
          cd u = a[i+j], v = a[i+j+len/2] * w;
          a[i+j] = u + v;
          a[i+j+len/2] = u - v;
          w *= wlen;
        }
      }
    }
    if (inv) for (auto& x : a) x /= n;
  }

  vector<double> mul(const vector<double>& a, const vector<double>& b) {
    int n = a.size(), m = b.size();
    if (!n || !m) return {};
    if (min(n, m) <= 60) {
      vector<double> res(n + m - 1);
      for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) res[i+j] += a[i] * b[j];
      return res;
    }
    int sz = 1;
    while (sz < n + m - 1) sz <<= 1;
    vector<cd> fa(sz), fb(sz);
    for (int i = 0; i < n; ++i) fa[i] = a[i];
    for (int i = 0; i < m; ++i) fb[i] = b[i];
    fft(fa, false); fft(fb, false);
    for (int i = 0; i < sz; ++i) fa[i] *= fb[i];
    fft(fa, true);
    vector<double> res(n + m - 1);
    for (int i = 0; i < n + m - 1; ++i) res[i] = fa[i].real();
    return res;
  }
}
template<typename T>  // T = double or long double
struct DFPS : vector<T> {
  using vector<T>::vector;
  DFPS(const vector<T>& r) : vector<T>(r) {}

  DFPS pre(int n) const {
    return DFPS(begin(*this), begin(*this) + min((int)this->size(), n));
  }
  DFPS rev() const {
    DFPS r = *this; reverse(r.begin(), r.end()); return r;
  }
  DFPS& normalize() {
    while (!this->empty() && abs(this->back()) < 1e-9) this->pop_back();
    return *this;
  }

  DFPS operator-() const {
    DFPS r = *this; for (auto& x : r) x = -x; return r;
  }
  DFPS operator+(const DFPS& r) const { return DFPS(*this) += r; }
  DFPS operator-(const DFPS& r) const { return DFPS(*this) -= r; }
  DFPS operator*(const T& v)    const { return DFPS(*this) *= v; }
  DFPS operator*(const DFPS& r) const { return DFPS(*this) *= r; }

  DFPS& operator+=(const DFPS& r) {
    if (r.size() > this->size()) this->resize(r.size(), T(0));
    for (int i = 0; i < (int)r.size(); ++i) (*this)[i] += r[i];
    return this->normalize();
  }
  DFPS& operator-=(const DFPS& r) {
    if (r.size() > this->size()) this->resize(r.size(), T(0));
    for (int i = 0; i < (int)r.size(); ++i) (*this)[i] -= r[i];
    return this->normalize();
  }
  DFPS& operator*=(const T& v) {
    for (auto& x : *this) x *= v; return *this;
  }
  // ここだけ FFT::mul に差し替え
  DFPS& operator*=(const DFPS& r) {
    return *this = FFT::mul(*this, r);
  }

  // inv, log, exp, sqrt は FPS と同じ Newton 反復で書ける
  // (mod x^k の冪零構造を使うので、浮動小数点でも成立する)

  DFPS inv(int deg) const {
    assert(!this->empty() && abs((*this)[0]) > 1e-9);
    DFPS res = {T(1) / (*this)[0]};
    for (int i = 1; i < deg; i <<= 1)
      res = (res + res - res * res * pre(i << 1)).pre(i << 1);
    res.resize(deg);
    return res;
  }
  DFPS inv() const { return inv(this->size()); }

  DFPS diff() const {
    int n = this->size();
    DFPS res(n - 1);
    for (int i = 1; i < n; ++i) res[i-1] = (*this)[i] * T(i);
    return res;
  }
  DFPS integral() const {
    int n = this->size();
    DFPS res(n + 1, T(0));
    for (int i = 0; i < n; ++i) res[i+1] = (*this)[i] / T(i+1);
    return res;
  }

  DFPS log(int deg) const {
    assert(abs((*this)[0] - T(1)) < 1e-9);
    return (diff() * inv(deg)).integral().pre(deg);
  }
  DFPS log() const { return log(this->size()); }

  DFPS exp(int deg) const {
    assert(abs((*this)[0]) < 1e-9);
    DFPS res = {T(1)};
    for (int i = 1; i < deg; i <<= 1)
      res = res * (pre(i<<1) - res.log(i<<1) + T(1)).pre(i<<1);
    res.resize(deg);
    return res;
  }
  DFPS exp() const { return exp(this->size()); }

  DFPS pow(long long e, int deg) const {
    if (e == 0) { DFPS r(deg, T(0)); r[0] = T(1); return r; }
    int i = 0;
    while (i < (int)this->size() && abs((*this)[i]) < 1e-9) ++i;
    if (i == (int)this->size() || (long long)i > (deg-1)/e)
      return DFPS(deg, T(0));
    T k = (*this)[i];
    DFPS res = (((*this >> i) * (T(1)/k)).log(deg) * T(e)).exp(deg)
               * pow(k, (double)e);  // std::pow
    res = (res << (e * i)).pre(deg);
    res.resize(deg);
    return res;
  }
  DFPS pow(long long e) const { return pow(e, this->size()); }
};
template<typename T>
struct CircularDFPS {
  int N;
  vector<T> a;

  CircularDFPS() : N(0) {}
  explicit CircularDFPS(int n) : N(n), a(n, T(0)) {}
  CircularDFPS(int n, const vector<T>& v) : N(n), a(n, T(0)) {
    for (int i = 0; i < (int)v.size(); ++i) a[i % n] += v[i];
  }

  T& operator[](int i) { return a[i]; }
  const T& operator[](int i) const { return a[i]; }
  int size() const { return N; }

  CircularDFPS operator+(const CircularDFPS& r) const { return CircularDFPS(*this) += r; }
  CircularDFPS operator-(const CircularDFPS& r) const { return CircularDFPS(*this) -= r; }
  CircularDFPS operator*(const T& v) const { return CircularDFPS(*this) *= v; }
  CircularDFPS operator*(const CircularDFPS& r) const { return CircularDFPS(*this) *= r; }

  CircularDFPS& operator+=(const CircularDFPS& r) {
    assert(N == r.N);
    for (int i = 0; i < N; ++i) a[i] += r.a[i];
    return *this;
  }
  CircularDFPS& operator-=(const CircularDFPS& r) {
    assert(N == r.N);
    for (int i = 0; i < N; ++i) a[i] -= r.a[i];
    return *this;
  }
  CircularDFPS& operator*=(const T& v) {
    for (auto& x : a) x *= v; return *this;
  }
  // ここだけ FFT::mul + 巡回折り畳み
  CircularDFPS& operator*=(const CircularDFPS& r) {
    assert(N == r.N);
    auto fc = FFT::mul(a, r.a);
    fill(a.begin(), a.end(), T(0));
    for (int i = 0; i < (int)fc.size(); ++i) a[i % N] += fc[i];
    return *this;
  }

  CircularDFPS shift(int k) const {
    k = ((k % N) + N) % N;
    CircularDFPS res(N);
    for (int i = 0; i < N; ++i) res.a[(i + k) % N] = a[i];
    return res;
  }

  CircularDFPS pow(long long e) const {
    CircularDFPS res(N), base(*this);
    res.a[0] = T(1);
    while (e > 0) {
      if (e & 1) res *= base;
      base *= base;
      e >>= 1;
    }
    return res;
  }

  T eval(T v) const {
    T res = T(0);
    for (int i = N - 1; i >= 0; --i) res = res * v + a[i];
    return res;
  }

  friend ostream& operator<<(ostream& os, const CircularDFPS& f) {
    for (int i = 0; i < f.N; ++i) { if (i) os << ' '; os << f.a[i]; }
    return os;
  }
};
//convolution_all 重複宣言に気を付けて
template <typename Poly>
Poly convolution_all(vector<Poly>& polys) {
  using elem = typename Poly::value_type;
  if (polys.size() == 0) return {elem(1)};
  while (1) {
    int n = polys.size();
    if (n == 1) break;
    int m = (n + 1) / 2;
    rep(i, m) {
      if (2 * i + 1 == n) {
        polys[i] = polys[2 * i];
      } else {
        polys[i] = polys[2 * i] * polys[2 * i + 1];
      }
    }
    polys.resize(m);
  }
  return polys[0];
}

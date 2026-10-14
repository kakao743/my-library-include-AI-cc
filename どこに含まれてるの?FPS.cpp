//けんちょんさんの（一部Nyaanさんからも）
//CC0とかマジ？
// modint
//NTTのmulのあれは絶対にやろう！
// min non-negative i such that (x & (1 << i)) != 0
int bsf(int x) { return __builtin_ctz(x); }
int bsf(unsigned int x) { return __builtin_ctz(x); }
int bsf(long long x) { return __builtin_ctzll(x); }
int bsf(unsigned long long x) { return __builtin_ctzll(x); }

// max non-negative i such that (x & (1 << i)) != 0
int bsr(int x) { return 8 * (int)sizeof(int) - 1 - __builtin_clz(x); }
int bsr(unsigned int x) { return 8 * (int)sizeof(unsigned int) - 1 - __builtin_clz(x); }
int bsr(long long x) { return 8 * (int)sizeof(long long) - 1 - __builtin_clzll(x); }
int bsr(unsigned long long x) { return 8 * (int)sizeof(unsigned long long) - 1 - __builtin_clzll(x); }

// ============================================================
// ★追加: mod 2^64 演算用 FpU64 (FastIO より前に定義が必要)
//   Fp<-1> として使う。+,-,* はすべて mod 2^64 (自然なwrap around)
// ============================================================
struct FpU64 {
    using fp_io_tag = void;
    uint64_t _v;
    constexpr FpU64() : _v(0) {}
    constexpr FpU64(long long v) : _v((uint64_t)v) {}
    constexpr uint64_t val() const { return _v; }
    constexpr uint64_t get() const { return _v; }
    static constexpr int get_mod() { return -1; }
    constexpr FpU64 operator+() const { return *this; }
    constexpr FpU64 operator-() const { return FpU64((long long)(-_v)); }
    constexpr FpU64 operator+(const FpU64 &r) const { return FpU64((long long)(_v + r._v)); }
    constexpr FpU64 operator-(const FpU64 &r) const { return FpU64((long long)(_v - r._v)); }
    constexpr FpU64 operator*(const FpU64 &r) const { return FpU64((long long)(_v * r._v)); }
    constexpr FpU64& operator+=(const FpU64 &r) { _v += r._v; return *this; }
    constexpr FpU64& operator-=(const FpU64 &r) { _v -= r._v; return *this; }
    constexpr FpU64& operator*=(const FpU64 &r) { _v *= r._v; return *this; }
    constexpr bool operator==(const FpU64 &r) const { return _v == r._v; }
    constexpr bool operator!=(const FpU64 &r) const { return _v != r._v; }
    // fft_info<FpU64> のインスタンス化を通すためのダミー (if constexpr で実際には呼ばれない)
    constexpr FpU64 pow(long long) const { return *this; }
    constexpr FpU64 inv() const { return *this; }
    friend istream& operator>>(istream &is, FpU64 &x) { uint64_t v; is >> v; x._v = v; return is; }
    friend ostream& operator<<(ostream &os, const FpU64 &x) { return os << x._v; }
};

template<int MOD>
struct FpEven {
    using fp_io_tag = void;
    long long _v;
 
    constexpr FpEven() : _v(0) {}
    constexpr FpEven(long long v) : _v(v % MOD) {
        if (_v < 0) _v += MOD;
    }
 
    // val() : 真の値を返す (偶数MODでは内部値がそのまま真の値)
    constexpr long long val() const { return _v; }
    constexpr long long get() const { return val(); }
    static constexpr int get_mod() { return MOD; }
 
    constexpr FpEven operator+() const { return FpEven(*this); }
    constexpr FpEven operator-() const { return FpEven(0) - FpEven(*this); }
    constexpr FpEven operator+(const FpEven &r) const { return FpEven(*this) += r; }
    constexpr FpEven operator-(const FpEven &r) const { return FpEven(*this) -= r; }
    constexpr FpEven operator*(const FpEven &r) const { return FpEven(*this) *= r; }
    constexpr FpEven operator/(const FpEven &r) const { return FpEven(*this) /= r; }
 
    constexpr FpEven& operator+=(const FpEven &r) {
        _v += r._v;
        if (_v >= MOD) _v -= MOD;
        return *this;
    }
    constexpr FpEven& operator-=(const FpEven &r) {
        _v -= r._v;
        if (_v < 0) _v += MOD;
        return *this;
    }
    constexpr FpEven& operator*=(const FpEven &r) {
        _v = _v * r._v % MOD;
        return *this;
    }
    constexpr FpEven& operator/=(const FpEven &r) {
        long long a = r._v, b = MOD, u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; swap(a, b);
            u -= t * v; swap(u, v);
        }
        _v = _v * u % MOD;
        if (_v < 0) _v += MOD;
        return *this;
    }
    constexpr FpEven pow(long long n) const {
        FpEven res(1), mul(*this);
        while (n > 0) {
            if (n & 1) res *= mul;
            mul *= mul;
            n >>= 1;
        }
        return res;
    }
    constexpr FpEven inv() const {
        return FpEven(1) / (*this);
    }
    constexpr FpEven sqrt() {
        if (val() < 2) return *this;
        if (pow((MOD - 1) >> 1).val() != 1) return FpEven(0);
        FpEven b = 1, one = 1;
        while (b.pow((MOD - 1) >> 1) == FpEven(1)) b += one;
        long long m = MOD - 1, e = 0;
        while (m % 2 == 0) m >>= 1, e += 1;
        FpEven x = pow((m - 1) >> 1);
        FpEven y = (*this) * x * x;
        x *= (*this);
        FpEven z = b.pow(m);
        while (y.val() != 1) {
            int j = 0;
            FpEven t = y;
            while (t != one) j += 1, t *= t;
            z = z.pow(1LL << (e - j - 1));
            x *= z; z *= z; y *= z; e = j;
        }
        return x;
    }
 
    constexpr bool operator==(const FpEven &r) const { return _v == r._v; }
    constexpr bool operator!=(const FpEven &r) const { return _v != r._v; }
 
    constexpr FpEven& operator++() {
        ++_v;
        if (_v >= MOD) _v -= MOD;
        return *this;
    }
    constexpr FpEven& operator--() {
        if (_v == 0) _v += MOD;
        --_v;
        return *this;
    }
    constexpr FpEven operator++(int) {
        FpEven res = *this; ++(*this); return res;
    }
    constexpr FpEven operator--(int) {
        FpEven res = *this; --(*this); return res;
    }
 
    friend istream& operator>>(istream &is, FpEven<MOD> &x) {
        long long v; is >> v;
        x = FpEven(v);
        return is;
    }
    friend ostream& operator<<(ostream &os, const FpEven<MOD> &x) {
        return os << x.val();
    }
    friend FpEven<MOD> pow(const FpEven<MOD> &r, long long n) { return r.pow(n); }
    friend FpEven<MOD> inv(const FpEven<MOD> &r) { return r.inv(); }
};
 
// ============================================================
//  奇数MOD用 FpOdd (Montgomery乗算)
// ============================================================
template<int MOD>
struct FpOdd {
    static_assert(MOD % 2 == 1, "MOD must be odd for Montgomery");
    using fp_io_tag = void;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using i64 = int64_t;
 
    // -MOD^{-1} mod 2^32  (Newton法: 4回で32bit収束)
    static constexpr u32 calc_mod_inv() {
        u32 x = (u32)get_mod();
        for (int i = 0; i < 4; i++) x *= 2 - (u32)get_mod() * x;
        return (u32)(-(u64)x);
    }
    // R^2 mod MOD  (R = 2^32)
    static constexpr u32 calc_R2() {
        u64 r = ((u64)1 << 32) % (u32)get_mod();
        return (u32)(r * r % (u32)get_mod());
    }
 
    static constexpr int  get_mod()  { return MOD; }
    static constexpr u32  MOD_INV = calc_mod_inv();
    static constexpr u32  R2      = calc_R2();
 
    u32 _v; // Montgomery表現: a * R mod MOD
 
    // Montgomery reduction:  T * R^{-1} mod MOD
    static constexpr u32 reduce(u64 T) {
        u32 m = (u32)T * MOD_INV;
        u32 t = (u32)((T + (u64)m * (u32)get_mod()) >> 32);
        return t >= (u32)get_mod() ? t - (u32)get_mod() : t;
    }
 
    // --- constructors ---
    constexpr FpOdd() : _v(0) {}
    constexpr FpOdd(long long v) {
        v %= get_mod();
        if (v < 0) v += get_mod();
        _v = reduce((u64)(u32)v * R2); // aR mod MOD
    }
    static constexpr FpOdd raw(u32 v) { FpOdd x; x._v = v; return x; }
 
    constexpr long long val() const { return reduce(_v); }
    constexpr long long get() const { return val(); }
 
    constexpr FpOdd operator+() const { return *this; }
    constexpr FpOdd operator-() const { return FpOdd(0) - (*this); }
    constexpr FpOdd operator+(const FpOdd &r) const { return FpOdd(*this) += r; }
    constexpr FpOdd operator-(const FpOdd &r) const { return FpOdd(*this) -= r; }
    constexpr FpOdd operator*(const FpOdd &r) const { return FpOdd(*this) *= r; }
    constexpr FpOdd operator/(const FpOdd &r) const { return FpOdd(*this) /= r; }
 
    constexpr FpOdd& operator+=(const FpOdd &r) {
        _v += r._v;
        if (_v >= (u32)get_mod()) _v -= (u32)get_mod();
        return *this;
    }
    constexpr FpOdd& operator-=(const FpOdd &r) {
        if (_v < r._v) _v += (u32)get_mod();
        _v -= r._v;
        return *this;
    }
    constexpr FpOdd& operator*=(const FpOdd &r) {
        _v = reduce((u64)_v * r._v);
        return *this;
    }
    constexpr FpOdd& operator/=(const FpOdd &r) {
        return *this *= r.inv();
    }
 
    constexpr FpOdd pow(long long n) const {
        FpOdd res(1), mul(*this);
        while (n > 0) {
            if (n & 1) res *= mul;
            mul *= mul;
            n >>= 1;
        }
        return res;
    }
    constexpr FpOdd inv() const {
        long long a = val(), b = get_mod(), u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; swap(a, b);
            u -= t * v; swap(u, v);
        }
        return FpOdd(u);
    }
    constexpr FpOdd sqrt() {
        if (val() < 2) return *this;
        if (pow((get_mod() - 1) >> 1).val() != 1) return FpOdd(0);
        FpOdd b(1), one(1);
        while (b.pow((get_mod() - 1) >> 1) == FpOdd(1)) b += one;
        long long m = get_mod() - 1, e = 0;
        while (m % 2 == 0) m >>= 1, e += 1;
        FpOdd x = pow((m - 1) >> 1);
        FpOdd y = (*this) * x * x;
        x *= (*this);
        FpOdd z = b.pow(m);
        while (y.val() != 1) {
            int j = 0;
            FpOdd t = y;
            while (t != one) j += 1, t *= t;
            z = z.pow(1LL << (e - j - 1));
            x *= z; z *= z; y *= z; e = j;
        }
        return x;
    }
 
    constexpr bool operator==(const FpOdd &r) const { return _v == r._v; }
    constexpr bool operator!=(const FpOdd &r) const { return _v != r._v; }
 
    constexpr FpOdd& operator++() { return *this += FpOdd(1); }
    constexpr FpOdd& operator--() { return *this -= FpOdd(1); }
    constexpr FpOdd operator++(int) { FpOdd res = *this; ++(*this); return res; }
    constexpr FpOdd operator--(int) { FpOdd res = *this; --(*this); return res; }
 
    friend istream& operator>>(istream &is, FpOdd<MOD> &x) {
        long long v; is >> v;
        x = FpOdd(v);
        return is;
    }
    friend ostream& operator<<(ostream &os, const FpOdd<MOD> &x) {
        return os << x.val();
    }
    friend FpOdd<MOD> pow(const FpOdd<MOD> &r, long long n) { return r.pow(n); }
    friend FpOdd<MOD> inv(const FpOdd<MOD> &r) { return r.inv(); }
};

// ============================================================
//  Fp<MOD> : MODの偶奇で自動切り替え
//  ★追加: Fp<-1> → FpU64 (mod 2^64)
template<int MOD, bool is_odd = (MOD % 2 == 1)>
struct FpSelector { using type = FpOdd<MOD>; };
 
template<int MOD>
struct FpSelector<MOD, false> { using type = FpEven<MOD>; };

// ★追加: Fp<-1> → FpU64
template<>
struct FpSelector<-1, false> { using type = FpU64; };
 
template<int MOD>
using Fp = typename FpSelector<MOD>::type;

namespace NTT {
    // --- 基本ユーティリティ ---

    long long modpow(long long a, long long n, int mod) {
        long long res = 1;
        a %= mod;
        while (n > 0) {
            if (n & 1) res = res * a % mod;
            a = a * a % mod;
            n >>= 1;
        }
        return res;
    }

    long long modinv(long long a, int mod) {
        long long b = mod, u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; std::swap(a, b);
            u -= t * v; std::swap(u, v);
        }
        u %= mod;
        if (u < 0) u += mod;
        return u;
    }

    // 2のべき乗への切り上げ (旧 get_fft_size の代わり)
    int bit_ceil(int n) {
        int x = 1;
        while (x < n) x <<= 1;
        return x;
    }

    constexpr int calc_primitive_root(long long mod) {
      if (mod == 1) return -1;
      if (mod == 2) return 1;
      if (mod == 998244353) return 3;
      if (mod == 167772161) return 3;
      if (mod == 469762049) return 3;
      if (mod == 754974721) return 11;
      if (mod == 645922817) return 3;
      if (mod == 897581057) return 3;
      if (mod == 2013265921) return 31;
        long long divs[20] = {};
        divs[0] = 2;
        long long cnt = 1;
        long long x = (mod - 1) / 2;
        while (x % 2 == 0) x /= 2;
        for (long long i = 3; i * i <= x; i += 2) {
            if (x % i == 0) {
                divs[cnt++] = (int)i;
                while (x % i == 0) x /= i;
            }
        }
        if (x > 1) divs[cnt++] = (int)x;
        for (int g = 2;; g++) {
            bool ok = true;
            for (int i = 0; i < cnt; i++) {
                if (modpow(g, (mod - 1) / divs[i], mod) == 1) {
                    ok = false; break;
                }
            }
            if (ok) return g;
        }
    }
    // --- NTT 内部構造 ---

    template <class mint> struct fft_info {
        static constexpr int rank2 = __builtin_ctz(mint::get_mod() - 1);
        mint root[rank2 + 1], iroot[rank2 + 1];
        mint rate2[std::max(0, rank2 - 2 + 1)], irate2[std::max(0, rank2 - 2 + 1)];
        mint rate3[std::max(0, rank2 - 3 + 1)], irate3[std::max(0, rank2 - 3 + 1)];

        fft_info() {
            int g = calc_primitive_root(mint::get_mod());
            root[rank2] = mint(g).pow((mint::get_mod() - 1) >> rank2);
            iroot[rank2] = root[rank2].inv();
            for (int i = rank2 - 1; i >= 0; i--) {
                root[i] = root[i + 1] * root[i + 1];
                iroot[i] = iroot[i + 1] * iroot[i + 1];
            }
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2[i] = root[i + 2] * prod;
                irate2[i] = iroot[i + 2] * iprod;
                prod *= iroot[i + 2];
                iprod *= root[i + 2];
            }
            prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 3; i++) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
    };

    template <class mint> void butterfly(std::vector<mint>& a) {
        int n = int(a.size());
        int h = __builtin_ctz((unsigned int)n);
        static const fft_info<mint> info;
        int len = 0;
        while (len < h) {
            if (h - len == 1) {
                int p = 1 << (h - len - 1);
                mint rot = 1;
                for (int s = 0; s < (1 << len); s++) {
                    int offset = s << (h - len);
                    for (int i = 0; i < p; i++) {
                        auto l = a[i + offset], r = a[i + offset + p] * rot;
                        a[i + offset] = l + r; a[i + offset + p] = l - r;
                    }
                    if (s + 1 != (1 << len)) rot *= info.rate2[__builtin_ctz(~(unsigned int)(s))];
                }
                len++;
            } else {
                int p = 1 << (h - len - 2);
                mint rot = 1, imag = info.root[2];
                for (int s = 0; s < (1 << len); s++) {
                    mint rot2 = rot * rot, rot3 = rot2 * rot;
                    int offset = s << (h - len);
                    for (int i = 0; i < p; i++) {
                        auto a0 = a[i + offset], a1 = a[i + offset + p] * rot;
                        auto a2 = a[i + offset + 2 * p] * rot2, a3 = a[i + offset + 3 * p] * rot3;
                        auto a1na3imag = (a1 - a3) * imag;
                        a[i + offset] = a0 + a2 + a1 + a3;
                        a[i + offset + p] = a0 + a2 - (a1 + a3);
                        a[i + offset + 2 * p] = a0 - a2 + a1na3imag;
                        a[i + offset + 3 * p] = a0 - a2 - a1na3imag;
                    }
                    if (s + 1 != (1 << len)) rot *= info.rate3[__builtin_ctz(~(unsigned int)(s))];
                }
                len += 2;
            }
        }
    }

    template <class mint> void butterfly_inv(std::vector<mint>& a) {
        int n = int(a.size());
        int h = __builtin_ctz((unsigned int)n);
        static const fft_info<mint> info;
        int len = h;
        while (len) {
            if (len == 1) {
                int p = 1 << (h - len);
                mint irot = 1;
                for (int s = 0; s < (1 << (len - 1)); s++) {
                    int offset = s << (h - len + 1);
                    for (int i = 0; i < p; i++) {
                        auto l = a[i + offset], r = a[i + offset + p];
                        a[i + offset] = l + r; a[i + offset + p] = (l - r) * irot;
                    }
                    if (s + 1 != (1 << (len - 1))) irot *= info.irate2[__builtin_ctz(~(unsigned int)(s))];
                }
                len--;
            } else {
                int p = 1 << (h - len);
                mint irot = 1, iimag = info.iroot[2];
                for (int s = 0; s < (1 << (len - 2)); s++) {
                    mint irot2 = irot * irot, irot3 = irot2 * irot;
                    int offset = s << (h - len + 2);
                    for (int i = 0; i < p; i++) {
                        auto a0 = a[i + offset], a1 = a[i + offset + p];
                        auto a2 = a[i + offset + 2 * p], a3 = a[i + offset + 3 * p];
                        auto a2na3iimag = (a2 - a3) * iimag;
                        a[i + offset] = a0 + a1 + a2 + a3;
                        a[i + offset + p] = (a0 - a1 + a2na3iimag) * irot;
                        a[i + offset + 2 * p] = (a0 + a1 - a2 - a3) * irot2;
                        a[i + offset + 3 * p] = (a0 - a1 - a2na3iimag) * irot3;
                    }
                    if (s + 1 != (1 << (len - 2))) irot *= info.irate3[__builtin_ctz(~(unsigned int)(s))];
                }
                len -= 2;
            }
        }
    }
    // power_projection 専用: 正規化込みの変換
    template<class mint>
    void trans(std::vector<mint>& v) {
        butterfly(v);
    }

    template<class mint>
    void trans_inv(std::vector<mint>& v) {
        butterfly_inv(v);
        int n = (int)v.size();
        mint iv = mint(n).inv();
        for (auto& x : v) x *= iv;
    }
    // --- 畳み込みの実装 ---

    template<class T> std::vector<T> naive_mul(const std::vector<T> &A, const std::vector<T> &B) {
        if (A.empty() || B.empty()) return {};
        int N = (int)A.size(), M = (int)B.size();
        std::vector<T> res(N + M - 1);
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < M; ++j) res[i + j] += A[i] * B[j];
        return res;
    }

    // ★追加: mod 2^64 畳み込み (32bit split + 3-mod NTT)
    // a[i], b[i] を上位32bit/下位32bitに分割し、各サブ畳み込みをGarnerで復元
    // 各サブ係数の最大値 <= N * (2^32)^2 = N*2^64 で、N<=2^19 なら < p0*p1*p2 ≈ 2^89
    // ※ mul より前に定義するため、butterfly/butterfly_inv を直接呼ぶ
    std::vector<FpU64> mul_u64(std::vector<FpU64> a, std::vector<FpU64> b) {
        int n = (int)a.size(), m = (int)b.size();
        if (!n || !m) return {};
        int sz = n + m - 1;

        static constexpr int M0 = 167772161;  // 2^25
        static constexpr int M1 = 469762049;  // 2^26  
        static constexpr int M2 = 2013265921; // g=31, 2^27
        using mint0 = Fp<M0>; using mint1 = Fp<M1>; using mint2 = Fp<M2>;

        // NTT-friendly素数での単一畳み込みヘルパ (butterfly を直接利用)
        auto ntt_conv = [&]<class mint>(std::vector<mint> x, std::vector<mint> y) {
            int nx = (int)x.size(), ny = (int)y.size();
            int z = bit_ceil(nx + ny - 1);
            x.resize(z); butterfly(x);
            y.resize(z); butterfly(y);
            for (int i = 0; i < z; i++) x[i] *= y[i];
            butterfly_inv(x);
            x.resize(nx + ny - 1);
            mint iz = mint(z).inv();
            for (auto& v : x) v *= iz;
            return x;
        };

        // a, b を lo(下位32bit) / hi(上位32bit) に分割
        std::vector<mint0> alo0(n),ahi0(n),blo0(m),bhi0(m);
        std::vector<mint1> alo1(n),ahi1(n),blo1(m),bhi1(m);
        std::vector<mint2> alo2(n),ahi2(n),blo2(m),bhi2(m);
        for (int i = 0; i < n; ++i) {
            uint32_t lo = (uint32_t)(a[i]._v), hi = (uint32_t)(a[i]._v >> 32);
            alo0[i]=lo; ahi0[i]=hi; alo1[i]=lo; ahi1[i]=hi; alo2[i]=lo; ahi2[i]=hi;
        }
        for (int i = 0; i < m; ++i) {
            uint32_t lo = (uint32_t)(b[i]._v), hi = (uint32_t)(b[i]._v >> 32);
            blo0[i]=lo; bhi0[i]=hi; blo1[i]=lo; bhi1[i]=hi; blo2[i]=lo; bhi2[i]=hi;
        }

        // 3つの素数でサブ畳み込み (ll=lo*lo, lh=lo*hi, hl=hi*lo)
        // hh=hi*hi は mod 2^64 で消える (係数に 2^64 がかかる)
        auto cll0=ntt_conv.template operator()<mint0>(alo0,blo0);
        auto clh0=ntt_conv.template operator()<mint0>(alo0,bhi0);
        auto chl0=ntt_conv.template operator()<mint0>(ahi0,blo0);
        auto cll1=ntt_conv.template operator()<mint1>(alo1,blo1);
        auto clh1=ntt_conv.template operator()<mint1>(alo1,bhi1);
        auto chl1=ntt_conv.template operator()<mint1>(ahi1,blo1);
        auto cll2=ntt_conv.template operator()<mint2>(alo2,blo2);
        auto clh2=ntt_conv.template operator()<mint2>(alo2,bhi2);
        auto chl2=ntt_conv.template operator()<mint2>(ahi2,blo2);

        // Garnerで各サブ係数の真値を復元してから mod 2^64 で合成
        // 各サブ係数 <= N*(2^32-1)^2 < 2^83 < M0*M1*M2 ≈ 2^89 → Garner正確
        static const mint1 iM0_1  = mint1(M0).inv();
        static const mint2 iM0_2  = mint2(M0).inv();
        static const mint2 iM1_2  = mint2(M1).inv();
        // Garner係数 (mod 2^64 での復元用)
        static const uint64_t m0   = (uint64_t)M0;
        static const uint64_t m01  = (uint64_t)M0 * (uint64_t)M1; // mod 2^64 自動

        auto garner3_u64 = [&](mint0 v0, mint1 v1, mint2 v2) -> uint64_t {
            // x ≡ v0 (mod M0), x ≡ v1 (mod M1), x ≡ v2 (mod M2)
            // CRT → x = y0 + y1*M0 + y2*M0*M1  (mod M0*M1*M2)
            uint64_t y0 = (uint64_t)(int)v0.val();
            uint64_t y1 = (uint64_t)(int)((v1 - mint1((long long)y0)) * iM0_1).val();
            uint64_t y2 = (uint64_t)(int)((v2 - mint2((long long)y0) - mint2((long long)y1) * mint2(M0)) * iM0_2 * iM1_2).val();
            return y0 + y1 * m0 + y2 * m01; // mod 2^64 自動
        };

        std::vector<FpU64> res(sz);
        for (int i = 0; i < sz; ++i) {
            uint64_t vll = garner3_u64(cll0[i], cll1[i], cll2[i]);
            uint64_t vlh = garner3_u64(clh0[i], clh1[i], clh2[i]);
            uint64_t vhl = garner3_u64(chl0[i], chl1[i], chl2[i]);
            // C[k] = vll + (vlh + vhl) * 2^32  (hh項は 2^64 係数なので消える)
            res[i]._v = vll + ((vlh + vhl) << 32);
        }
        return res;
    }

    template<class mint> std::vector<mint> mul(std::vector<mint> a, std::vector<mint> b) {
        int n = int(a.size()), m = int(b.size());
        if (!n || !m) return {};
        if (std::min(n, m) <= 60) return naive_mul(a, b);

        int MOD = mint::get_mod();

        // ★追加: MOD==-1 は FpU64 → mod 2^64 畳み込み
        // if constexpr で butterfly<FpU64> のインスタンス化を防ぐ
        if constexpr (mint::get_mod() == -1) {
            // mint == FpU64 が保証されているのでreinterpret_castは安全
            auto *pa = reinterpret_cast<std::vector<FpU64>*>(&a);
            auto *pb = reinterpret_cast<std::vector<FpU64>*>(&b);
            auto rc = mul_u64(std::move(*pa), std::move(*pb));
            return *reinterpret_cast<std::vector<mint>*>(&rc);
        }
        int z = bit_ceil(n + m - 1);
        if ((MOD - 1) % z == 0) {//NTT-Friendlyの場合
            a.resize(z); butterfly(a);
            b.resize(z); butterfly(b);
            for (int i = 0; i < z; i++) a[i] *= b[i];
            butterfly_inv(a);
            a.resize(n+m-1);
            mint iz = mint(z).inv();
            for (auto& x : a) x *= iz;
            return a;
        }

        static constexpr int MOD0 = 167772161;  // 2^25
        static constexpr int MOD1 = 469762049;  // 2^26  
        static constexpr int MOD2 = 2013265921; // g=31, 2^27
        using mint0 = Fp<MOD0>; using mint1 = Fp<MOD1>; using mint2 = Fp<MOD2>;

        std::vector<mint0> a0(n), b0(m); std::vector<mint1> a1(n), b1(m); std::vector<mint2> a2(n), b2(m);
        for(int i=0; i<n; ++i) { a0[i] = a[i].val(); a1[i] = a[i].val(); a2[i] = a[i].val(); }
        for(int i=0; i<m; ++i) { b0[i] = b[i].val(); b1[i] = b[i].val(); b2[i] = b[i].val(); }

        auto c0 = mul(std::move(a0), std::move(b0));
        auto c1 = mul(std::move(a1), std::move(b1));
        auto c2 = mul(std::move(a2), std::move(b2));

        static const mint1 imod0 = mint1(MOD0).inv();
        static const mint2 imod1 = mint2(MOD1).inv(), imod01 = imod1 * mint2(MOD0).inv();

        int res_size = n + m - 1;
        std::vector<mint> res(res_size);
        long long m0 = MOD0 % MOD, m01 = (long long)MOD0 * MOD1 % MOD;
        for (int i = 0; i < res_size; ++i) {
            int y0 = c0[i].val();
            int y1 = ((c1[i] - y0) * imod0).val();
            int y2 = ((c2[i] - y0) * imod01 - mint2(y1) * imod1).val();
            res[i] = (long long)y0 + (long long)y1 * m0 + (long long)y2 * m01;
        }
        return res;
    }
    template<class mint> void ntt(vector<mint>&a) {
        butterfly(a);
    }
    template<class mint> void intt(vector<mint>&a) {
        butterfly_inv(a);
        mint iz = mint(a.size()).inv();
        for (auto& x : a) x *= iz;
    }

    template<class mint> void ntt_doubling(vector<mint> &a) {
    int M = (int)a.size();
    auto b = a;
    intt(b);
    mint r = 1, zeta = mint(calc_primitive_root(mint::get_mod())).pow((mint::get_mod() - 1) / (M << 1));
    for (int i = 0; i < M; i++) b[i] *= r, r *= zeta;
    ntt(b);
    copy(begin(b), end(b), back_inserter(a));
    }
};
template<class T> struct BiCoef {
    vector<T> fact_, inv_, finv_;
    constexpr BiCoef() {}
    constexpr BiCoef(int n) noexcept : fact_(n, 1), inv_(n, 1), finv_(n, 1) {
        init(n);
    }
    constexpr void init(int n) noexcept {
        fact_.assign(n, 1), inv_.assign(n, 1), finv_.assign(n, 1);
        int MOD = fact_[0].get_mod();
        for(int i = 2; i < n; i++){
            fact_[i] = fact_[i-1] * i;
            inv_[i] = -inv_[MOD%i] * (MOD/i);
            finv_[i] = finv_[i-1] * inv_[i];
        }
    }
    constexpr T com(int n, int k) const noexcept {
        if (n < k || n < 0 || k < 0) return 0;
        return fact_[n] * finv_[k] * finv_[n-k];
    }
    constexpr T fact(int n) const noexcept {
        if (n < 0) return 0;
        return fact_[n];
    }
    constexpr T inv(int n) const noexcept {
        if (n < 0) return 0;
        return inv_[n];
    }
    constexpr T finv(int n) const noexcept {
        if (n < 0) return 0;
        return finv_[n];
    }
};
// Formal Power Series
template<typename mint> struct FPS : vector<mint> {
    using vector<mint>::vector;
 
    // constructor
    constexpr FPS(const vector<mint> &r) : vector<mint>(r) {}
 
    // core operator
    constexpr FPS pre(int siz) const {
        return FPS(begin(*this), begin(*this) + min((int)this->size(), siz));
    }
    constexpr FPS rev() const {
        FPS res = *this;
        reverse(begin(res), end(res));
        return res;
    }
    constexpr FPS& normalize() {
        while (!this->empty() && this->back() == 0) this->pop_back();
        return *this;
    }
 
    // basic operator
    constexpr FPS operator - () const noexcept {
        FPS res = (*this);
        for (int i = 0; i < (int)res.size(); ++i) res[i] = -res[i];
        return res;
    }
    constexpr FPS operator + (const mint &v) const { return FPS(*this) += v; }
    constexpr FPS operator + (const FPS &r) const { return FPS(*this) += r; }
    constexpr FPS operator - (const mint &v) const { return FPS(*this) -= v; }
    constexpr FPS operator - (const FPS &r) const { return FPS(*this) -= r; }
    constexpr FPS operator * (const mint &v) const { return FPS(*this) *= v; }
    constexpr FPS operator * (const FPS &r) const { return FPS(*this) *= r; }
    constexpr FPS operator / (const mint &v) const { return FPS(*this) /= v; }
    constexpr FPS operator / (const FPS &r) const { return FPS(*this) /= r; }
    constexpr FPS operator % (const FPS &r) const { return FPS(*this) %= r; }
    constexpr FPS operator << (int x) const { return FPS(*this) <<= x; }
    constexpr FPS operator >> (int x) const { return FPS(*this) >>= x; }
    constexpr FPS& operator += (const mint &v) {
        if (this->empty()) this->resize(1);
        (*this)[0] += v;
        return this->normalize();
    }
    constexpr FPS& operator += (const FPS &r) {
        if (r.size() > this->size()) this->resize(r.size());
        for (int i = 0; i < (int)r.size(); ++i) (*this)[i] += r[i];
        return *this;
    }
    constexpr FPS& operator -= (const mint &v) {
        if (this->empty()) this->resize(1);
        (*this)[0] -= v;
        return *this;
    }
    constexpr FPS& operator -= (const FPS &r) {
        if (r.size() > this->size()) this->resize(r.size());
        for (int i = 0; i < (int)r.size(); ++i) (*this)[i] -= r[i];
        return this ->normalize();
    }
    constexpr FPS& operator *= (const mint &v) {
        for (int i = 0; i < (int)this->size(); ++i) (*this)[i] *= v;
        return *this;
    }
    constexpr FPS& operator *= (const FPS &r) {
        return *this = NTT::mul((*this), r);
    }
	constexpr FPS& operator*=(vector<pair<int, mint>> g) {
	    using T=mint;
  	  int n = (*this).size();
  	  auto [d, c] = g.front();
  	  if (d == 0) g.erase(g.begin());
  	  else c = 0;
  	  drep(i, n) {
  	    (*this)[i] *= c;
  	    for (auto &[j, b] : g) {
  	      if (j > i) break;
  	      (*this)[i] += (*this)[i-j] * b;
  	    }
  	  }
  	  return *this;
  	}
  	constexpr FPS& operator/=(vector<pair<int, mint>> g) {
			using T=mint;
  	  int n = (*this).size();
  	  auto [d, c] = g.front();
  	  assert(d == 0 && c != T(0));
  	  T ic = c.inv();
  	  g.erase(g.begin());
  	  rep(i, n) {
  	    for (auto &[j, b] : g) {
  	      if (j > i) break;
  	      (*this)[i] -= (*this)[i-j] * b;
  	    }
  	    (*this)[i] *= ic;
  	  }
  	  return *this;
  	}
    constexpr FPS& operator /= (const mint &v) {
        assert(v != 0);
        mint iv = v.inv();
        for (int i = 0; i < (int)this->size(); ++i) (*this)[i] *= iv;
        return *this;
    }
    
    // division, r must be normalized (r.back() must not be 0)
    constexpr FPS& operator /= (const FPS &r) {
        assert(!r.empty());
        assert(r.back() != 0);
        this->normalize();
        if (this->size() < r.size()) {
            this->clear();
            return *this;
        }
        int need = (int)this->size() - (int)r.size() + 1;
        *this = (rev().pre(need) * r.rev().inv(need)).pre(need).rev();
        return *this;
    }
    constexpr FPS& operator %= (const FPS &r) {
        assert(!r.empty());
        assert(r.back() != 0);
        this->normalize();
        FPS q = (*this) / r;
        return *this -= q * r;
    }
    constexpr FPS& operator <<= (int x) {
        FPS res(x, 0);
        res.insert(res.end(), begin(*this), end(*this));
        return *this = res;
    }
    constexpr FPS& operator >>= (int x) {
        FPS res;
        res.insert(res.end(), begin(*this) + x, end(*this));
        return *this = res;
    }
    constexpr mint eval(const mint &v) const {
        mint res = 0;
        for (int i = (int)this->size()-1; i >= 0; --i) {
            res *= v;
            res += (*this)[i];
        }
        return res;
    }

    // advanced operation
    // df/dx
    constexpr FPS diff() const {
        int n = (int)this->size();
        FPS res(n-1);
        for (int i = 1; i < n; ++i) res[i-1] = (*this)[i] * i;
        return res;
    }
    
    // \int f dx
    constexpr FPS integral() const {
        int n = (int)this->size();
        FPS res(n+1, 0);
        for (int i = 0; i < n; ++i) res[i+1] = (*this)[i] / (i+1);
        return res;
    }
    
    // inv(f), f[0] must not be 0
    constexpr FPS inv(int deg) const {
        assert((*this)[0] != 0);
        if (deg < 0) deg = (int)this->size();
        FPS res({mint(1) / (*this)[0]});
        for (int i = 1; i < deg; i <<= 1) {
            res = (res + res - res * res * pre(i << 1)).pre(i << 1);
        }
        res.resize(deg);
        return res;
    }
    constexpr FPS inv() const {
        return inv((int)this->size());
    }
    constexpr FPS inv_sparse(vector<pair<int, mint>> f){
		mint f0 = f[0].second ;
		mint g0 = mint(1)/f0 ;
		f.erase(f.begin()) ;
		const int N = (*this).size() ;
		FPS g(N) ;
		g[0] = g0 ;
		for(int n=1; n < N ;n++){
			mint rhs = 0 ;
			for(auto &&[i, fi]:f){
				if(i <= n) rhs -= fi * g[n-i] ;
			}
			g[n] = rhs * g0 ;
		}
		return *this = g ;
	}
    // log(f) = \int f'/f dx, f[0] must be 1
    constexpr FPS log(int deg) const {
        assert((*this)[0]==1);
        FPS res = (diff() * inv(deg)).integral();
        res.resize(deg);
        return res;
    }
    constexpr FPS log() const {
        return log((int)this->size());
    }
    constexpr FPS log_sparse(){
			FPS f=(*this);
    	assert(f[0]==1);
    	const int n=f.size();
    	const int mod=f[0].get_mod();
    	vector<pair<int,mint>>f_sub;
    	for(int i=1;i<n;i++)if(f[i]!=0)f_sub.emplace_back(i,f[i]);
    	FPS g(n),inv(n);
    	for(auto[a,c]:f_sub)if(a)g[a]=c;
    	if(n>1)inv[1]=1;
    	for(int i=1;i<n;i++){
        mint sum=0;
        for(auto[a,c]:f_sub)if(a&&a<=i)sum+=c*(i-a)*g[i-a];
        if(i>=2){
            inv[i]=-inv[mod%i]*(mod/i);
            sum*=inv[i];
        }
        g[i]-=sum;
  		}
    	return g;
		}

    // exp(f), f[0] must be 0
    constexpr FPS exp(int deg) const {
        assert((*this)[0] == 0);
        FPS res(1, 1);
        for (int i = 1; i < deg; i <<= 1) {
            res = res * (pre(i<<1) - res.log(i<<1) + 1).pre(i<<1);
        }
        res.resize(deg);
        return res;
    }
    constexpr FPS exp() const {
        return exp((int)this->size());
    }
		static constexpr FPS exp_sparse(const vector<pair<int,mint>>& f_sub, int n){
			int mod=f_sub[0].second.get_mod();
    	FPS g(n),inv(n);
    	g[0]=1;
    	if(n>1)inv[1]=1;
    	for(int i=1;i<n;i++){
        for(auto[a,c]:f_sub)if(a<=i)g[i]+=c*g[i-a]*a;
        if(i>=2){
            inv[i]=-inv[mod%i]*(mod/i);
            g[i]*=inv[i];
        }
    	}
    	return g;
		}
    //pow(f) = exp(e * log f)
    constexpr FPS pow(long long e, int deg) const {
        if (e == 0) {
            FPS res(deg, 0);
            res[0] = 1;
            return res;
        }
        long long i = 0;
        while (i < (int)this->size() && (*this)[i] == 0) ++i;
        if (i == (int)this->size() || i > (deg - 1) / e) return FPS(deg, 0);
        mint k = (*this)[i];
        FPS res = ((((*this) >> i) / k).log(deg) * e).exp(deg) * mint(k).pow(e) << (e * i);
        res.resize(deg);
        return res;
    }
    constexpr FPS pow(long long e) const {
        return pow(e, (int)this->size());
    }
    constexpr FPS pow_sparse(const vector<pair<int,mint>>&f_sub, int n, long long k){
			
    	FPS g(n);
    	if(f_sub.empty()){
        if(k==0)g[0]=1;
        return g;
    	}
    	auto[a0,c0]=f_sub[0];
    	if(a0>0){
        if(k>(n-1)/a0)return FPS(n);
        vector<pair<int,mint>>g_sub=f_sub;
        for(auto&[a,c]:g_sub)a-=a0;
        auto h=pow_sparse(g_sub,n-k*a0,k);
        for(int i=0;i<n-k*a0;i++)g[i+k*a0]=h[i];
        return g;
    	}
    	const long long mod=(*this)[0].get_mod();
    	FPS inv(n);
    	inv[1]=1;
    	for(int i=2;i<n;i++)inv[i]=-inv[mod%i]*(mod/i);
    	g[0]=c0.pow(k);
    	mint cef=c0.inv();
    	for(int i=1;i<n;i++){
        for(auto[a,c]:f_sub)if(a&&i-a>=0)g[i]+=c*g[i-a]*(mint(k+1)*a-i);
        g[i]*=inv[i]*cef;
    	}
    	return g;
		}
    // sqrt(f), f[0] must be 1
    constexpr FPS sqrt_base(int deg) const {
        assert((*this)[0] == 1);
        mint inv2 = mint(1) / 2;
        FPS res(1, 1);
        for (int i = 1; i < deg; i <<= 1) {
            res = (res + pre(i << 1) * res.inv(i << 1)).pre(i << 1);
            for (mint &x : res) x *= inv2;
        }
        res.resize(deg);
        return res;
    }
    constexpr FPS sqrt_sparse() {
		FPS f = *this ;
		int n = f.size();
		int di = n;
		for (int i = n - 1; i >= 0; --i)if (f[i] != 0) di = i;
		if (di == n) return f;
		if (di & 1) return {};
		mint y = f[di];
		mint x = y.sqrt();
		if (x * x != y) return {};
		mint c = mint(1) / y;
		FPS g(n - di);
		for (int i = 0; i < n - di; ++i) g[i] = f[di + i] * c;
		g = g.sqrt_base();
		for (int i = 0; i < int(g.size()); ++i) g[i] *= x; g.resize(n);
		for (int i = n - 1; i >= 0; --i) {
			if (i >= di / 2) g[i] = g[i - di / 2];
			else g[i] = 0;
		}
        return  *this = g;
    }

    constexpr FPS sqrt_base() const {
        return sqrt_base((int)this->size());
    }
     //prod (1+keisux^a[i])
	static constexpr FPS subset_sum(const vector<int>& s,int t,long long keisu=1) {
        vector<mint> a(t+1);
        for(auto g:s)if(g<=t)a[g]+=1;
		vector<mint> invmemo(t+1);
        reps(i,t){
            mint k=i;
            invmemo[i]=k.inv();
        }
        FPS g(t+1);
        reps(i,t){
        for(int j=1;j*i<=t;j++){
            g[j*i]+=invmemo[j]*a[i]*(-mint(-keisu).pow(j));
        }
    }
    g=g.exp();
    return g;
    }	
    static constexpr FPS subset_sum2(const vector<int>& s,int t) {
        vector<mint> a(t+1);
        for(auto g:s)if(g<=t)a[g]+=1;
		vector<mint> invmemo(t+1);
        reps(i,t){
            mint k=i;
            invmemo[i]=k.inv();
        }
        FPS g(t+1);
        reps(i,t){
        for(int j=1;j*i<=t;j++){
            g[j*i]+=invmemo[j]*a[i];
        }
    }
    g=g.exp();
    return g;
    }	
    // ntt 系ラッパー (multipoint_eval の高速版などで使う)
    constexpr void ntt() {
        NTT::butterfly(*this);
    }
    constexpr void intt() {
        NTT::butterfly_inv(*this);
        mint iz = mint(this->size()).inv();
        for (auto& x : *this) x *= iz;
    }
    constexpr void ntt_doubling() {
        NTT::ntt_doubling(*this);
    }
    static constexpr mint ntt_pr() {
    return mint(NTT::calc_primitive_root(mint::get_mod()));
    }
    // friend operators
    friend constexpr FPS diff(const FPS &f) { return f.diff(); }
    friend constexpr FPS integral(const FPS &f) { return f.integral(); }
    friend constexpr FPS inv(const FPS &f, int deg) { return f.inv(deg); }
    friend constexpr FPS inv(const FPS &f) { return f.inv((int)f.size()); }
    friend constexpr FPS log(const FPS &f, int deg) { return f.log(deg); }
    friend constexpr FPS log(const FPS &f) { return f.log((int)f.size()); }
    friend constexpr FPS exp(const FPS &f, int deg) { return f.exp(deg); }
    friend constexpr FPS exp(const FPS &f) { return f.exp((int)f.size()); }
    friend constexpr FPS pow(const FPS &f, long long e, int deg) { return f.pow(e, deg); }
    friend constexpr FPS pow(const FPS &f, long long e) { return f.pow(e, (int)f.size()); }
    friend constexpr FPS sqrt_base(const FPS &f, int deg) { return f.sqrt_base(deg); }
    friend constexpr FPS sqrt_base(const FPS &f) { return f.sqrt_base((int)f.size()); }
};


//------------------------------//
// Polynomial, FPS algorithms
//------------------------------//
// ============================================================
// Multipoint Evaluation
// f(p[0]), f(p[1]), ..., f(p[m-1]) を O((N+M) log^2(N+M)) で求める
// ============================================================

// 部分積木を構築: g[v] = Π (x - p[i]) for i in subtree of v
// leaves: g[leaf] = (x - p[i])
// これをセグ木的に bottom-up でかけ合わせる
template<typename mint>
vector<FPS<mint>> build_subproduct_tree(const vector<mint>& p) {
    int m = p.size();
    // 完全二分木のサイズ (葉をm個以上持つ最小の2のべき)
    int sz = 1;
    while (sz < m) sz <<= 1;

    // tree[sz + i] = (x - p[i]),  i < m
    // tree[sz + i] = 1,           i >= m (パディング)
    vector<FPS<mint>> tree(2 * sz, FPS<mint>{mint(1)});
    for (int i = 0; i < m; i++) {
        tree[sz + i] = FPS<mint>{-p[i], mint(1)}; // (x - p[i])
    }
    // bottom-up でかけ合わせ
    for (int v = sz - 1; v >= 1; v--) {
        tree[v] = tree[2*v] * tree[2*v+1];
    }
    return tree;
}

// 再帰で f mod g[v] を下に流していく
// node v の担当区間は [l, r)
template<typename mint>
void eval_rec(
    const vector<FPS<mint>>& tree,
    int v, int l, int r,
    FPS<mint> f_mod, // f mod tree[v] が渡ってくる
    int m,//original size
    vector<mint>& ans
) {
    if(l>=m)return;
    if (r - l == 1) {
        // 葉: f mod (x - p[l]) = f(p[l]) (定数)
        if (!f_mod.empty()) ans[l] = f_mod[0];
        else ans[l] = mint(0);
        return;
    }
    int mid = (l + r) / 2;
    // f mod g[left_child], f mod g[right_child] を求める
    eval_rec(tree, 2*v,   l,   mid, f_mod % tree[2*v],  m, ans);
    eval_rec(tree, 2*v+1, mid, r,   f_mod % tree[2*v+1], m,ans);
}
template <typename mint>
vector<mint> multipoint_eval(const FPS<mint> &f,
                           const vector<mint> &xs) {
  using fps = FPS<mint>;
  if (xs.size() == 1) return vector<mint>{f.eval(xs[0])};
  int s = xs.size();
  int N = 1 << (32 - __builtin_clz((int)xs.size() - 1));
  if(f.empty() || xs.empty()) return vector<mint>(s, mint(0));
  vector<FPS<mint>> buf(2 * N);
  for (int i = 0; i < N; i++) {
    mint n = mint{i < s ? -xs[i] : mint(0)};
    buf[i + N] = fps{n + 1, n - 1};
  }
  for (int i = N - 1; i > 0; i--) {
    fps &g(buf[(i << 1) | 0]), &h(buf[(i << 1) | 1]);
    int n = g.size();
    int m = n << 1;
    buf[i].reserve(m);
    buf[i].resize(n);
    for (int j = 0; j < n; j++) buf[i][j] = g[j] * h[j] - mint(1);
    if (i != 1) {
      buf[i].ntt_doubling();
      for (int j = 0; j < m; j++) buf[i][j] += j < n ? mint(1) : -mint(1);
    }
  }

  int fs = f.size();
  fps root = buf[1];
  root.intt();
  root.push_back(1);
  reverse(begin(root), end(root));
  root = root.inv(fs).rev() * f;
  root.erase(begin(root), begin(root) + fs - 1);
  root.resize(N, mint(0));

  vector<mint> ans(s);

  auto calc = [&](auto rec, int i, int l, int r, fps g) -> void {
    if (i >= N) {
      ans[i - N] = g[0];
      return;
    }
    int len = g.size(), m = (l + r) >> 1;
    g.ntt();
    fps tmp = buf[i * 2 + 1];
    for (int j = 0; j < len; j++) tmp[j] *= g[j];
    tmp.intt();
    rec(rec, i * 2 + 0, l, m, fps{begin(tmp) + (len >> 1), end(tmp)});
    if (m >= s) return;
    tmp = buf[i * 2 + 0];
    for (int j = 0; j < len; j++) tmp[j] *= g[j];
    tmp.intt();
    rec(rec, i * 2 + 1, m, r, fps{begin(tmp) + (len >> 1), end(tmp)});
  };
  calc(calc, 1, 0, N, root);
  return ans;
}
// ============================================================
// Lagrange Interpolation (任意点) O(N log^2 N)
// 点 (xs[0],ys[0]), ..., (xs[n-1],ys[n-1]) から
// deg < n の多項式 f を復元する
// ============================================================
template <typename mint>
struct SubproductTree {
  using fps = FPS<mint>;
  int logN, n, nn;
  vector<mint> xs;
  vector<mint> buf;
  vector<mint *> gss;
  fps all, roots;

  void ntt(mint *a, int s) const {
    static fps buf2;
    buf2.resize(s);
    copy(a, a + s, buf2.data());
    buf2.ntt();
    copy(buf2.data(), buf2.data() + s, a);
  }
  void intt(mint *a, int s) const {
    static fps buf2;
    buf2.resize(s);
    copy(a, a + s, buf2.data());
    buf2.intt();
    copy(buf2.data(), buf2.data() + s, a);
  }

  SubproductTree(const vector<mint> &xs_) {
    n = xs_.size();
    for (logN = 0, nn = 1; nn < n; ++logN, nn <<= 1) {
    }
    roots.resize(logN + 1);
    for (int i = 0; i <= logN; i++) {
      int mod = mint::get_mod();
      roots[i] = mint(fps::ntt_pr()).pow((mod - 1) >> i);
    }
    xs.assign(nn, 0U);
    memcpy(xs.data(), xs_.data(), n * sizeof(mint));
    buf.assign((logN + 1) * (nn << 1), 0U);
    gss.assign(nn << 1, nullptr);
    for (int h = 0; h <= logN; ++h)
      for (int u = 1 << h; u < 1 << (h + 1); ++u) {
        gss[u] =
            buf.data() + (h * (nn << 1) + ((u - (1 << h)) << (logN - h + 1)));
      }
    for (int i = 0; i < nn; ++i) {
      gss[nn + i][0] = -xs[i] + 1;
      gss[nn + i][1] = -xs[i] - 1;
    }
    if (nn == 1) gss[1][1] += 2;
    for (int h = logN; --h >= 0;) {
      const int m = 1 << (logN - h);
      for (int u = 1 << (h + 1); --u >= 1 << h;) {
        for (int i = 0; i < m; ++i)
          gss[u][i] = gss[u << 1][i] * gss[u << 1 | 1][i];
        memcpy(gss[u] + m, gss[u], m * sizeof(mint));
        intt(gss[u] + m, m);
        if (h > 0) {
          gss[u][m] -= 2;
          const mint a = roots[logN - h + 1];
          mint aa = 1;
          for (int i = m; i < m << 1; ++i) {
            gss[u][i] *= aa;
            aa *= a;
          };
          ntt(gss[u] + m, m);
        }
      }
    }
    all.resize(nn + 1);
    all[0] = 1;
    for (int i = 1; i < nn; ++i) all[i] = gss[1][nn + nn - i];
    all[nn] = gss[1][nn] - 1;
  }
  vector<mint> multiEval(const fps &fs) const {
    vector<mint> work0(nn), work1(nn), work2(nn);
    {
      const int m = max<int>(fs.size(), 1);
      auto invAll = all.inv(m);
      std::reverse(invAll.begin(), invAll.end());
      int mm;
      for (mm = 1; mm < m - 1 + nn; mm <<= 1) {
      }
      invAll.resize(mm, 0U);
      ntt(invAll.data(), invAll.size());
      vector<mint> ffs(mm, 0U);
      memcpy(ffs.data(), fs.data(), fs.size() * sizeof(mint));
      ntt(ffs.data(), ffs.size());
      for (int i = 0; i < mm; ++i) ffs[i] *= invAll[i];
      intt(ffs.data(), ffs.size());
      memcpy(((logN & 1) ? work1 : work0).data(), ffs.data() + m - 1,
             nn * sizeof(mint));
    }
    for (int h = 0; h < logN; ++h) {
      const int m = 1 << (logN - h);
      for (int u = 1 << h; u < 1 << (h + 1); ++u) {
        mint *hs = (((logN - h) & 1) ? work1 : work0).data() +
                   ((u - (1 << h)) << (logN - h));
        mint *hs0 = (((logN - h) & 1) ? work0 : work1).data() +
                    ((u - (1 << h)) << (logN - h));
        mint *hs1 = hs0 + (m >> 1);
        ntt(hs, m);
        for (int i = 0; i < m; ++i) work2[i] = gss[u << 1 | 1][i] * hs[i];
        intt(work2.data(), m);
        memcpy(hs0, work2.data() + (m >> 1), (m >> 1) * sizeof(mint));
        for (int i = 0; i < m; ++i) work2[i] = gss[u << 1][i] * hs[i];
        intt(work2.data(), m);
        memcpy(hs1, work2.data() + (m >> 1), (m >> 1) * sizeof(mint));
      }
    }
    work0.resize(n);
    return work0;
  }

  fps interpolate(const vector<mint> &ys) const {
    assert(static_cast<int>(ys.size()) == n);
    fps gs(n);
    for (int i = 0; i < n; ++i) gs[i] = all[n - (i + 1)] * (i + 1);
    const vector<mint> denoms = multiEval(gs);
    vector<mint> work(nn << 1, 0U);
    for (int i = 0; i < n; ++i) {
      assert(denoms[i] != 0);
      work[i << 1] = work[i << 1 | 1] = ys[i] / denoms[i];
    }
    for (int h = logN; --h >= 0;) {
      const int m = 1 << (logN - h);
      for (int u = 1 << (h + 1); --u >= 1 << h;) {
        mint *hs = work.data() + ((u - (1 << h)) << (logN - h + 1));
        for (int i = 0; i < m; ++i)
          hs[i] = gss[u << 1 | 1][i] * hs[i] + gss[u << 1][i] * hs[m + i];
        if (h > 0) {
          memcpy(hs + m, hs, m * sizeof(mint));
          intt(hs + m, m);
          const mint a = roots[logN - h + 1];
          mint aa = 1;
          for (int i = m; i < m << 1; ++i) {
            hs[i] *= aa;
            aa *= a;
          };
          ntt(hs + m, m);
        }
      }
    }
    intt(work.data(), nn);
    return {work.data() + nn - n, work.data() + nn};
  }
};
template<class mint> vector<mint> interpolation(vector<mint>&xs,vector<mint>&ys){
  SubproductTree sp(xs);
  return sp.interpolate(ys);
}
// ============================================================
// Lagrange Interpolation (連続整数点) O(N)
// f(0), f(1), ..., f(n-1) が与えられたとき f(t) を求める
//
// ラグランジュ公式:
//   f(t) = Σ_i y[i] * Π_{j≠i}(t-j) / Π_{j≠i}(i-j)
//
// Π_{j≠i}(i-j) = i! * (n-1-i)! * (-1)^(n-1-i) (符号付き階乗)
// Π_{j≠i}(t-j) は prefix/suffix 積で O(N) で全部求まる
// ============================================================
// 上の最適化版 (除算を逆数テーブルに変える)
template<typename mint>
mint lagrange_interpolation_sequential_fast(const vector<mint>& y, mint t) {
    int n = y.size();
    assert(n > 0);

    for (int i = 0; i < n; i++) {
        if (t == mint(i)) return y[i];
    }

    vector<mint> prefix(n+1), suffix(n+1);
    prefix[0] = mint(1);
    for (int i = 0; i < n; i++) prefix[i+1] = prefix[i] * (t - mint(i));
    suffix[n] = mint(1);
    for (int i = n-1; i >= 0; i--) suffix[i] = suffix[i+1] * (t - mint(i));

    vector<mint> fact(n), inv_fact(n);
    fact[0] = mint(1);
    for (int i = 1; i < n; i++) fact[i] = fact[i-1] * mint(i);
    inv_fact[n-1] = fact[n-1].inv();
    for (int i = n-2; i >= 0; i--) inv_fact[i] = inv_fact[i+1] * mint(i+1);

    mint ans = mint(0);
    for (int i = 0; i < n; i++) {
        mint contrib = y[i] * prefix[i] * suffix[i+1] * inv_fact[i] * inv_fact[n-1-i];
        if ((n-1-i) % 2 == 1) contrib = -contrib;
        ans += contrib;
    }
    return ans;
}
//高速ゼータ変換
template<typename T>
vector<T> zeta(vector<T> x,int type){
	int n=x.size();
	for(int b=1;b<n;b<<=1){
		for(int j=0;j<n;j++){
			switch(type){
				case 1:
					if(!(b&j))x[j]+=x[b|j];
					break;
				case 2:
					if(!(b&j))x[b|j]+=x[j];
					break;
				case 3:
					if(!(b&j))x[j]-=x[b|j];
					break;
				case 4:
					if(!(b&j))x[b|j]-=x[j];
					break;
			}
		}
	}
    return x;
}
template<typename mint>
FPS<mint> convolution_or(FPS<mint> &a,FPS<mint> &b){
    FPS<mint> A=zeta(a,2),B=zeta(b,2);
    A*=B;
    return zeta(A,4);
};
template<typename mint>
FPS<mint> subset_convolution(FPS<mint> &a,FPS<mint> &b){
    vector<int> pc(max(a.size(),b.size()));
    for(int i=1;i<pc.size();i++)pc[i]=pc[i-(i&(-i))]+1;
    auto lift=[&](FPS<mint> &a){
        vector<FPS<mint>> A(a.size(),FPS<mint>(__builtin_ctz(a.size())+1,0));
        for(int i=0;i<a.size();i++){
            A[i][pc[i]]=a[i];
        }
        return A;
    };
    auto unlift=[&](vector<FPS<mint>> &A){
        FPS<mint> a(A.size());
        for(int i=0;i<a.size();i++){
            a[i]=A[i][pc[i]];
        }
        return a;
    };
    vector<FPS<mint>> A=lift(a),B=lift(b);
    A=zeta(A,2),B=zeta(B,2);
    auto prod=[&](vector<FPS<mint>>& A,vector<FPS<mint>>& B) {
        int n = A.size(), d = __builtin_ctz(n);
        for(int i=0;i<n;i++)B[i].resize(d+1);
        for(int i=0;i<n;i++)A[i].resize(d+1);
        for (int i = 0; i < n; i++) {
        FPS<mint> c(d+3);
        for (int j = 0; j <= d; j++) {
            for (int k = 0; k <= d - j; k++) {
            c[j + k] +=
            A[i][j] * 
            B[i]
            [k];
            }
        }
        A[i].swap(c);
        }
        return;
    };
    prod(A,B);
    A=zeta(A,4);
    return unlift(A);
  }
//convolution all
template <typename mint>
FPS<mint> convolution_all(vector<FPS<mint>>& polys) {
  if (polys.size() == 0) return {mint(1)};
  while (1) {
    int n = polys.size();
    if (n == 1) break;
    int m = ( n + 1 )/2;
    rep(i, m) {
      if (2 * i + 1 == n) {
        polys[i] = polys[2 * i];
      } else {
        polys[i] = polys[2 * i]*polys[2 * i + 1];
      }
    }
    polys.resize(m);
  }
  return polys[0];
}
//普通の除算としてのFPS
template<typename mint>
pair<FPS<mint>,FPS<mint>> div_fps(FPS<mint> &f,FPS<mint> g){
  while(!g.empty()&&g.back().val()==0){
    g.pop_back();
  }
  if(g.empty()){
    cerr<<"ERROR : g is 0"<<endl;
    exit(9);
  }
  if(f.size()<g.size()){
    return {{},f};
  }
  reverse(all(g));

  int n=f.size(),m=g.size();
  FPS<mint> invg=g.inv(n-m+1);

  reverse(all(f));

  FPS<mint> t=f*invg;
  t.resize(n-m+1);

  FPS<mint> q(n-m+1);
  rep(i,n-m+1){
    q[n-m-i]=t[i];
  }

  reverse(all(f));
  reverse(all(g));

  t=g*q;

  FPS<mint> r(m-1);
  rep(i,m-1){
    r[i]=f[i]-t[i];
  }

  while(!r.empty()&&r.back().val()==0){
    r.pop_back();
  }

  return {q,r};
}
// input : y(0), y(1), ..., y(n - 1)
// output : y(t), y(t + 1), ..., y(t + m - 1)
// (if m is default, m = n)
template <typename mint>
FPS<mint> SamplePointShift(FPS<mint>& y, mint t,
                                         int m = -1) {
  if (m == -1) m = y.size();
  long long T = t.get();
  int k = (int)y.size() - 1;
  T %= mint::get_mod();
  if (T <= k) {
    FPS<mint> ret(m);
    int ptr = 0;
    for (int64_t i = T; i <= k and ptr < m; i++) {
      ret[ptr++] = y[i];
    }
    if (k + 1 < T + m) {
      auto suf = SamplePointShift<mint>(y, k + 1, m - ptr);
      for (int i = k + 1; i < T + m; i++) {
        ret[ptr++] = suf[i - (k + 1)];
      }
    }
    return ret;
  }
  if (T + m > mint::get_mod()) {
    auto pref = SamplePointShift<mint>(y, T, mint::get_mod() - T);
    auto suf = SamplePointShift<mint>(y, 0, m - pref.size());
    copy(begin(suf), end(suf), back_inserter(pref));
    return pref;
  }

  FPS<mint> finv(k + 1, 1), d(k + 1);
  for (int i = 2; i <= k; i++) finv[k] *= i;
  finv[k] = mint(1) / finv[k];
  for (int i = k; i >= 1; i--) finv[i - 1] = finv[i] * i;
  for (int i = 0; i <= k; i++) {
    d[i] = finv[i] * finv[k - i] * y[i];
    if ((k - i) & 1) d[i] = -d[i];
  }

  FPS<mint> h(m + k);
  for (int i = 0; i < m + k; i++) {
    h[i] = mint(1) / (T - k + i);
  }

  auto dh = d * h;

  FPS<mint> ret(m);
  mint cur = T;
  for (int i = 1; i <= k; i++) cur *= T - i;
  for (int i = 0; i < m; i++) {
    ret[i] = cur * dh[k + i];
    cur *= T + i + 1;
    cur *= h[i];
  }
  return ret;
}
// Bostan-Mori
// find [x^N] P(x)/Q(x), O(K log K log N)
// deg(Q(x)) = K, deg(P(x)) < K
template<typename mint> mint BostanMori(const FPS<mint> &P, const FPS<mint> &Q, long long N) {
    assert(!P.empty() && !Q.empty());
    if (N == 0 || Q.size() == 1) return P[0] / Q[0];
    
    int qdeg = (int)Q.size();
    FPS<mint> P2{P}, minusQ{Q};
    P2.resize(qdeg - 1);
    for (int i = 1; i < (int)Q.size(); i += 2) minusQ[i] = -minusQ[i];
    P2 *= minusQ;
    FPS<mint> Q2 = Q * minusQ;
    FPS<mint> S(qdeg - 1), T(qdeg);
    for (int i = 0; i < (int)S.size(); ++i) {
        S[i] = (N % 2 == 0 ? P2[i * 2] : P2[i * 2 + 1]);
    }
    for (int i = 0; i < (int)T.size(); ++i) {
        T[i] = Q2[i * 2];
    }
    return BostanMori(S, T, N >> 1);
}
//no composition
//0~n partition number
template <typename mint>
FPS<mint> partition_number(int N) {
  ll M = sqrt(N) + 10;
  FPS<mint> f(N + 1);
  for(int x=-M;x<M;x++) {
    ll d = x * (3 * x - 1) / 2;
    if (d > N) continue;
    f[d] += (x % 2 == 0 ? 1 : -1);
  }
  return f.inv();
}
//はい　数式を入れたら 漸化式が返ってくる 例 1 1 2 3 5 8->1 -1 -1(返ってくるのが1,-1,-1なのは注意)2d+1項以上いれないと作動しない（はず）
template <typename mint>
vector<mint> BerlekampMassey(const vector<mint> &s) {
  const int N = (int)s.size();
  vector<mint> b, c;
  b.reserve(N + 1);
  c.reserve(N + 1);
  b.push_back(mint(1));
  c.push_back(mint(1));
  mint y = mint(1);
  for (int ed = 1; ed <= N; ed++) {
    int l = int(c.size()), m = int(b.size());
    mint x = 0;
    for (int i = 0; i < l; i++) x += c[i] * s[ed - l + i];
    b.emplace_back(mint(0));
    m++;
    if (x == mint(0)) continue;
    mint freq = x / y;
    if (l < m) {
      auto tmp = c;
      c.insert(begin(c), m - l, mint(0));
      for (int i = 0; i < m; i++) c[m - 1 - i] -= freq * b[m - 1 - i];
      b = tmp;
      y = x;
    } else {
      for (int i = 0; i < m; i++) c[l - 1 - i] -= freq * b[m - 1 - i];
    }
  }
  reverse(begin(c), end(c));
  return c;
}
template <typename mint>
mint nth_term(long long n, const vector<mint> &s) {
  FPS<mint> bm = BerlekampMassey<mint>(s);
  FPS<mint> ss=s;
  ss*=bm;
  ss.resize(bm.size()-1);
  return BostanMori(ss,bm,n);
}

template<class mint> FPS<mint> PolynomialTaylorShift(const FPS<mint> &f, long long c) {
    int N = (int)f.size() - 1;
    BiCoef<mint> bc(N + 1);
    
    // convollution
    FPS<mint> p(N + 1), q(N + 1);
    for (int i = 0; i <= N; ++i) {
        p[i] = f[i] * bc.fact(i);
        q[N - i] = mint(c).pow(i) * bc.finv(i);
    }
    FPS<mint> pq=p*q;
    
    // find
    FPS<mint> res(N + 1);
    for (int i = 0; i <= N; ++i) res[i] = pq[i + N] * bc.finv(i);
    return res;
}
// f_d(x) = (x+1)(x+2)...(x+d) を構築する
// 計算量: O(sqrt(P) log P)
template<class mint>
FPS<mint> build_factorial_poly(long long d) {
    BiCoef<mint> bc(d+1);
    if (d == 0) return {mint(1)};
    if (d == 1) return {mint(1), mint(1)}; // (x + 1)

    if (d % 2 == 1) {
        // d が奇数の場合は f_{d-1}(x) * (x + d)
        FPS<mint> f = build_factorial_poly<mint>(d - 1);
        FPS<mint> res(d + 1, 0);
        for (int i = 0; i < (int)f.size(); ++i) {
            res[i + 1] += f[i];       // x の係数
            res[i] += f[i] * mint(d); // 定数項
        }
        return res;
    } else {
        // ダブリング: f_{2k}(x) = f_k(x) * f_k(x + k)
        long long k = d / 2;
        FPS<mint> fk = build_factorial_poly<mint>(k);
        FPS<mint> fk_shifted = PolynomialTaylorShift(fk, k); 
        return fk * fk_shifted; // NTTによる多項式乗算
    }
}
template <class mint>
vector<mint> shift_sample_points(const vector<mint>& a, mint c, BiCoef<mint>& bc) {
    int d = (int)a.size() - 1;
    FPS<mint> A(d + 1), B(2 * d + 1);
    for (int i = 0; i <= d; i++) {
        A[i] = a[i] * bc.finv(i) * bc.finv(d - i);
        if ((d - i) % 2 == 1) A[i] = -A[i];
    }
    for (int i = 0; i <= 2 * d; i++) B[i] = (c - mint(d) + mint(i)).inv();
    FPS<mint> AB = A * B;
    vector<mint> res(d + 1);
    mint prod = 1;
    for (int i = 0; i <= d; i++) prod *= (c - mint(i));
    for (int i = 0; i <= d; i++) {
        res[i] = AB[d + i] * prod;
        if (i < d) prod *= (c + mint(1)) * (c - mint(d)).inv();
    }
    return res;
}

template<class mint>
FPS<mint> samples_to_poly(const vector<mint>& y) {
    int n = y.size();
    if (n == 0) return {};
    if (n == 1) return {y[0]};

    vector<mint> xs(n);
    for (int i = 0; i < n; i++) xs[i] = mint(i);
    // lagrange_interpolation は既存関数そのまま
    return lagrange_interpolation(xs, y);
}

// ============================================================
// poly_to_samples_fast: 係数列 → 連続整数点  O((N+M) log(N+M))
// ============================================================
template<class mint>
vector<mint> poly_to_samples(const FPS<mint>& f, long long t, int m) {
    if (m == 0) return {};
    if (f.empty()) return vector<mint>(m, mint(0));

    // TaylorShift で f(x+t) を求め、0..m-1 で評価
    // = f(t+0),...,f(t+m-1)
    // f(x+t) の係数列 h を求める  O(N log N)
    FPS<mint> h = PolynomialTaylorShift(f, t);

    // h(0),...,h(m-1) を連続整数点評価  O((N+M) log(N+M))
    // h の値列 h(0),...,h(n-1) を shift の extend で求める
    // まず h(0),...,h(n-1) を EGF で求める (係数列 → 連続整数点)

    int n = (int)h.size();
    int sz = max(n, m);
    vector<mint> fact(sz+1), inv_fact(sz+1);
    fact[0] = 1;
    for (int i=1;i<=sz;i++) fact[i] = fact[i-1]*i;
    inv_fact[sz] = fact[sz].inv();
    for (int i=sz-1;i>=0;i--) inv_fact[i] = inv_fact[i+1]*(i+1);

    vector<mint> xs(m);
    for (int i=0;i<m;i++) xs[i] = mint(i); // t=0 で h を評価
    return multipoint_eval(h, xs);
}
template <class mint>
mint fast_factorial(long long n) {
    if (n == 0) return mint(1);
    long long mod = mint().get_mod();
    if (n >= mod) return mint(0);

    if (n > mod / 2) {
        mint den = 1;
        for (long long i = n + 1; i < mod; i++) den *= mint(i);
        return mint(mod - 1) / den;
    }

    long long B = 1;
    while (B * B < n) B++;
    
    FPS<mint> f_poly = build_factorial_poly<mint>(B);

    FPS<mint> g_poly = f_poly;
    mint B_val = mint(B);
    mint B_pow = 1;
    for (int i = 0; i <= (int)B; i++) {
        g_poly[i] *= B_pow;
        B_pow *= B_val;
    }

    int k = n / B;
    mint res = 1;
    for (int j = 0; j < k; j++) {
        res *= PolynomialTaylorShift<mint>(g_poly, (long long)j)[0];
    }

    for (long long i = (long long)k * B + 1; i <= n; i++) {
        res *= mint(i);
    }

    return res;
}
// composition of FPS, calc g(f(x))　f(0)=0 f[1]!=0 NTT=friendlyのみ
template<typename mint>
FPS<mint> composition(FPS<mint> g, FPS<mint> f, int deg) {
    auto rec = [&](auto &&rec, FPS<mint> Q, int n, int h, int k) -> FPS<mint> {
        if (n == 0) {
            FPS<mint> T{begin(Q), begin(Q) + k};
            T.emplace_back(mint(1));
            FPS<mint> u = g * T.rev().inv().rev();
            FPS<mint> P(h * k);
            for (int i = 0; i < (int)g.size(); i++) P[k - i - 1] = u[i + k];
            return P;
        }
        FPS<mint> nQ(h * k * 4), nR(h * k * 2);
        for (int i = 0; i < k; i++) {
            copy(begin(Q) + i * h, begin(Q) + i * h + n + 1, begin(nQ) + i * h * 2);
        }
        nQ[h * k * 2] += 1;
        NTT::trans(nQ);
        for (int i = 0; i < h * k * 4; i += 2) swap(nQ[i], nQ[i + 1]);
        for (int i = 0; i < h * k * 2; i++) nR[i] = nQ[i * 2] * nQ[i * 2 + 1];
        NTT::trans_inv(nR);
        nR[0] -= 1;
        Q.assign(h * k, 0);
        for (int i = 0; i < k * 2; i++) for (int j = 0; j <= n / 2; j++) {
            Q[i * h / 2 + j] = nR[i * h + j];
        }
        auto P = rec(rec, Q, n / 2, h / 2, k * 2);
        FPS<mint> nP(h * k * 4);
        for (int i = 0; i < k * 2; i++) for (int j = 0; j <= n / 2; j++) {
            nP[i * h * 2 + j * 2 + n % 2] = P[i * h / 2 + j];
        }
        NTT::trans(nP);
        for (int i = 1; i < h * k * 4; i <<= 1) reverse(begin(nQ) + i, begin(nQ) + i * 2);
        for (int i = 0; i < h * k * 4; i++) nP[i] *= nQ[i];
        NTT::trans_inv(nP);
        P.assign(h * k, 0);
        for (int i = 0; i < k; i++) {
            copy(begin(nP) + i * h * 2, begin(nP) + i * h * 2 + n + 1, begin(P) + i * h);
        }
        return P;
    };
    if (deg == -1) deg = max((int)f.size(), (int)g.size());
    f.resize(deg), g.resize(deg);
    int n = (int)f.size() - 1, h = 1, k = 1;
    while (h < n + 1) h *= 2;
    FPS<mint> Q(h * k);
    for (int i = 0; i <= n; i++) Q[i] = -f[i];
    FPS<mint> P = rec(rec, Q, n, h, k);
    return P.pre(n + 1).rev();
}

template<typename mint>
FPS<mint> composition(FPS<mint> g, FPS<mint> f) {
    return composition(g, f, max(f.size(), g.size()));
}
// Power Projection
// for i = 0, 1, ..., m, calc [x^(f の最高次数)] f(x)^i g(x) 
template<class mint, int MOD = mint::get_mod(), int pr = NTT::calc_primitive_root(MOD)>
FPS<mint> power_projection(FPS<mint> f, FPS<mint> g = {1}, int m = -1) {
    int n = (int)f.size() - 1, k = 1, h = 1;
    g.resize(n + 1);
    if (m < 0) m = n;
    while (h < n + 1) h <<= 1;
    FPS<mint> P((n + 1) * k), Q((n + 1) * k), nP, nQ, buf, buf2;
    for (int i = 0; i <= n; i++) P[i * k] = g[i];
    for (int i = 0; i <= n; i++) Q[i * k] = -f[i];
    Q[0]++;
    mint iv2 = mint(2).inv();
    while (n) {
        mint w = mint(pr).pow((MOD - 1) / (2 * k)), iw = w.inv();
        buf2.resize(k);
        auto ntt_doubling = [&]() {
            copy(begin(buf), end(buf), begin(buf2));
            NTT::trans_inv(buf2);
            mint c = 1;
            for (int i = 0; i < k; i++) buf2[i] *= c, c *= w;
            NTT::trans(buf2);
            copy(begin(buf2), end(buf2), back_inserter(buf));
        };
        nP.clear(), nQ.clear();
        for (int i = 0; i <= n; i++) {
            buf.resize(k);
            copy(begin(P) + i * k, begin(P) + (i + 1) * k, begin(buf));
            ntt_doubling();
            copy(begin(buf), end(buf), back_inserter(nP));
            buf.resize(k);
            copy(begin(Q) + i * k, begin(Q) + (i + 1) * k, begin(buf));
            if (i == 0) {
                for (int j = 0; j < k; j++) buf[j]--;
                ntt_doubling();
                for (int j = 0; j < k; j++) buf[j]++;
                for (int j = 0; j < k; j++) buf[k + j]--;
            } else {
                ntt_doubling();
            }
            copy(begin(buf), end(buf), back_inserter(nQ));
        }
        nP.resize(h * 2 * k * 2), nQ.resize(h * 2 * k * 2);
        FPS<mint> p(h * 2), q(h * 2);
        w = mint(pr).pow((MOD - 1) / (h * 2)), iw = w.inv();
        vector<int> btr;
        if (n % 2) {
            btr.resize(h);
            for (int i = 0, lg = bsf(h); i < h; i++) {
                btr[i] = (btr[i >> 1] >> 1) + ((i & 1) << (lg - 1));
            }
        }
        for (int j = 0; j < k * 2; j++) {
            p.assign(h * 2, 0), q.assign(h * 2, 0);
            for (int i = 0; i < h; i++) p[i] = nP[i * k * 2 + j], q[i] = nQ[i * k * 2 + j];
            NTT::trans(p), NTT::trans(q);
            for (int i = 0; i < h * 2; i += 2) swap(q[i], q[i + 1]);
            for (int i = 0; i < h * 2; i++) p[i] *= q[i];
            for (int i = 0; i < h; i++) q[i] = q[i * 2] * q[i * 2 + 1];
            if (n & 1) {
                mint c = iv2;
                buf.resize(h);
                for (int i : btr) buf[i] = (p[i * 2] - p[i * 2 + 1]) * c, c *= iw;
                swap(p, buf);
            } else {
                for (int i = 0; i < h; i++) p[i] = (p[i * 2] + p[i * 2 + 1]) * iv2;
            }
            p.resize(h), q.resize(h);
            NTT::trans_inv(p), NTT::trans_inv(q);
            for (int i = 0; i < h; i++) nP[i * k * 2 + j] = p[i];
            for (int i = 0; i < h; i++) nQ[i * k * 2 + j] = q[i];
        }
        nP.resize((n / 2 + 1) * k * 2), nQ.resize((n / 2 + 1) * k * 2);
        swap(P, nP), swap(Q, nQ);
        n /= 2, h /= 2, k *= 2;
    }
    FPS<mint> S{begin(P), begin(P) + k}, T{begin(Q), begin(Q) + k};
    NTT::trans_inv(S),NTT::trans_inv(T);
    T[0]--;
    if (T[0] == 0) return S.rev().pre(m + 1);
    else return (S.rev() * (T + (FPS<mint>{1} << k)).rev().inv(m + 1)).pre(m + 1);
}

// find g s.t. f(g(x)) ≡ x (mod x^{deg}) f(0)=0 f[1]!=0
template<class mint>
FPS<mint> compositional_inverse(FPS<mint> f, int deg = -1) {
    assert((int)f.size() >= 2 && f[1] != 0);
    if (deg == -1) deg = (int)f.size();
    if (deg < 2) return FPS<mint>{0, f[1].inv()}.pre(deg);
    int n = deg - 1;
    FPS<mint> h = power_projection(f) * n;
    for (int k = 1; k <= n; k++) h[k] /= k;
    h = h.rev(), h *= h[0].inv();
    FPS<mint> g = (h.log() * mint(-n).inv()).exp();
    g *= f[1].inv();
    return (g << 1).pre(deg);
}
// product of 1-A[i]x
template <typename mint>
FPS<mint> convolution_all_1(vector<mint> A,bool minus=true) {

  if ((mint::get_mod()-1)%(1<<23)!=0) {
    vector<FPS<mint>> polys;
    for (auto& a: A) polys.emplace_back(FPS<mint>({mint(1), minus? -a:a}));
    return convolution_all(polys);
  }
  int D = 6;
  using poly = vector<mint>;
  int n = 1;
  while (n < A.size()) n *= 2;
  int k = __builtin_ctz(n);
  vector<mint> F(n), nxt_F(n);
  repi(i, A.size()) F[i] = minus? -A[i]:A[i];
  repi(d, k) {
    int b = 1 << d;
    if (d < D) {
      fill(all(nxt_F), mint(0));
      for (int L = 0; L < n; L += 2 * b) {
        repi(i, b) repi(j, b) { nxt_F[L + i + j] += F[L + i] * F[L + b + j]; }
        repi(i, b) nxt_F[L + b + i] += F[L + i] + F[L + b + i];
      }
    }
    else if (d == D) {
      for (int L = 0; L < n; L += 2 * b) {
        poly f1 = {F.begin() + L, F.begin() + L + b};
        poly f2 = {F.begin() + L + b, F.begin() + L + 2 * b};
        f1.resize(2 * b), f2.resize(2 * b), NTT::ntt(f1), NTT::ntt(f2);
        repi(i, b) nxt_F[L + i] = f1[i] * f2[i] + f1[i] + f2[i];
        for(int i=b;i<2*b;i++) nxt_F[L + i] = f1[i] * f2[i] - f1[i] - f2[i];
      }
    }
    else {
      for (int L = 0; L < n; L += 2 * b) {
        poly f1 = {F.begin() + L, F.begin() + L + b};
        poly f2 = {F.begin() + L + b, F.begin() + L + 2 * b};
        NTT::ntt_doubling(f1), NTT::ntt_doubling(f2);
        repi(i, b) nxt_F[L + i] = f1[i] * f2[i] + f1[i] + f2[i];
        for(int i=b;i<2*b;i++) nxt_F[L + i] = f1[i] * f2[i] - f1[i] - f2[i];
      }
    }
    swap(F, nxt_F);
  }
  if (k - 1 >= D) NTT::intt(F);
  F.emplace_back(1), reverse(all(F));
  F.resize(A.size() + 1);
  return F;
}
template<class mint>
FPS<mint> convolution(vector<mint> a, vector<mint> b) {
    return NTT::mul(a,b);
}
// 入力 : f/(x-b_1)(x-b_2)...(x-b_n), f は n-1 次, b_i は distinct
// 出力 : a_1/(x-b_1) + ... + a_n/(x-b_n)
template<typename mint>
vector<mint> Bubun_Bunsu_Bunkai(FPS<mint>&f,vector<mint>&b){
  int n=b.size();
  assert(int(f.size())<=n);
  auto a1=multipoint_eval(f,b);
  auto g=convolution_all_1(b);
  reverse(all(g));
  g=g.diff();
  auto a2=multipoint_eval(g,b);
  rep(i,n)a1[i]/=a2[i];
  return a1;
}
// 有理式の和を計算する。分割統治 O(Nlog^2N)。N は次数の和。
template <typename mint>
ob2<FPS<mint>, FPS<mint>> sum_of_rationals(vector<ob2<FPS<mint>, FPS<mint>>> dat) {
  if (dat.size() == 0) {
    FPS<mint> f = {0}, g = {1};
    return {f, g};
  }
  using P = ob2<FPS<mint>, FPS<mint>>;
  auto add = [&](P& a, P& b) -> P {
    int na = a.a.size() - 1, da = a.b.size() - 1;
    int nb = b.a.size() - 1, db = b.b.size() - 1;
    int n = max(na + db, da + nb);
    FPS<mint> num(n + 1);
    {
      auto f = a.a*b.b;
      repi(i, f.size()) num[i] += f[i];
    }
    {
      auto f = a.b*b.a;
      repi(i, f.size()) num[i] += f[i];
    }
    auto den = a.b*b.b;
    return {num, den};
  };

  while (dat.size() > 1) {
    int n = dat.size();
    for(int i=1;i<n;i+=2) { dat[i - 1] = add(dat[i - 1], dat[i]); }
    repi(i, ceil(n, 2)) dat[i] = dat[2 * i];
    dat.resize(ceil(n, 2));
  }
  return dat[0];
}

// sum wt[i]/(1-A[i]x)
template <typename mint>
ob2<FPS<mint>, FPS<mint>> sum_of_rationals_1(FPS<mint> A, FPS<mint> wt) {
  using poly = FPS<mint>;
  if ((mint::get_mod()-1)%(1<<23)!=0) {
    vector<ob2<poly, poly>> rationals;
    repi(i, A.size()) rationals.emplace_back(poly({wt[i]}), poly({mint(1), -A[i]}));
    return sum_of_rationals(rationals);
  }
  int n = 1;
  while (n < A.size()) n *= 2;
  int k = __builtin_ctz(n);
  FPS<mint> F(n), G(n);
  FPS<mint> nxt_F(n), nxt_G(n);
  repi(i, A.size()) F[i] = -A[i], G[i] = wt[i];
  int D = 6;

  repi(d, k) {
    int b = 1 << d;
    if (d < D) {
      fill(all(nxt_F), mint(0)), fill(all(nxt_G), mint(0));
      for (int L = 0; L < n; L += 2 * b) {
        repi(i, b) repi(j, b) nxt_F[L + i + j] += F[L + i] * F[L + b + j];
        repi(i, b) repi(j, b) nxt_G[L + i + j] += F[L + i] * G[L + b + j];
        repi(i, b) repi(j, b) nxt_G[L + i + j] += F[L + b + i] * G[L + j];
        repi(i, b) nxt_F[L + b + i] += F[L + i] + F[L + b + i];
        repi(i, b) nxt_G[L + b + i] += G[L + i] + G[L + b + i];
      }
    }
    else if (d == D) {
      for (int L = 0; L < n; L += 2 * b) {
        poly f1 = {F.begin() + L, F.begin() + L + b};
        poly f2 = {F.begin() + L + b, F.begin() + L + 2 * b};
        poly g1 = {G.begin() + L, G.begin() + L + b};
        poly g2 = {G.begin() + L + b, G.begin() + L + 2 * b};
        f1.resize(2 * b), f2.resize(2 * b), g1.resize(2 * b), g2.resize(2 * b);
        NTT::ntt(f1), NTT::ntt(f2), NTT::ntt(g1), NTT::ntt(g2);
        repi(i, b) f1[i] += 1, f2[i] += 1;
        for(int i=b;i<2*b;i++) f1[i] -= 1, f2[i] -= 1;
        repi(i, 2 * b) nxt_F[L + i] = f1[i] * f2[i] - 1;
        repi(i, 2 * b) nxt_G[L + i] = g1[i] * f2[i] + g2[i] * f1[i];
      }
    }
    else {
      for (int L = 0; L < n; L += 2 * b) {
        poly f1 = {F.begin() + L, F.begin() + L + b};
        poly f2 = {F.begin() + L + b, F.begin() + L + 2 * b};
        poly g1 = {G.begin() + L, G.begin() + L + b};
        poly g2 = {G.begin() + L + b, G.begin() + L + 2 * b};
        NTT::ntt_doubling(f1), NTT::ntt_doubling(f2), NTT::ntt_doubling(g1), NTT::ntt_doubling(g2);
        repi(i, b) f1[i] += 1, f2[i] += 1;
        for(int i=b;i<2*b;i++) f1[i] -= 1, f2[i] -= 1;
        repi(i, 2 * b) nxt_F[L + i] = f1[i] * f2[i] - 1;
        repi(i, 2 * b) nxt_G[L + i] = g1[i] * f2[i] + g2[i] * f1[i];
      }
    }
    swap(F, nxt_F), swap(G, nxt_G);
  }
  if (k - 1 >= D) NTT::intt(F), NTT::intt(G);
  F.emplace_back(1);
  reverse(all(F)), reverse(all(G));
  F.resize(A.size() + 1);
  G.resize(A.size());
  return {G, F};
}
//no composition halfgcd (f^k mod g) (x^k mod f) (f^k mod (1-x^n))(Π(1-x^a_i)^-1)
using mint=Fp<moda>;

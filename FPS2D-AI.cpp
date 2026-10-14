/*
 * ================================================================
 *  FPS2D : 2変数形式的冪級数ライブラリ
 *  f(x, y) = Σ_{i,j} a[i][j] x^i y^j   (mod x^Dx, y^Dy)
 *
 *  依存: FpOdd/FpEven/FpU64, NTT::mul, FPS<mint> (1変数版), BiCoef<mint>
 *
 *  主要オペレーション
 *   四則演算  +, -, *, /
 *   inv       f[0][0] != 0 が必要
 *   log       f[0][0] == 1 が必要
 *   exp       f[0][0] == 0 が必要
 *   pow       exp(e * log f) ベース
 *   sqrt      f[0][0] が平方数であること
 *   diff_x / diff_y / integral_x / integral_y
 *   eval      (x0, y0) での値
 *   2変数 Bostan-Mori      [x^N y^M] P(x,y)/Q(x,y)
 *   BerlekampMassey2D      2変数線形漸化式の検出 (Sakata algorithm)
 *   nth_term2D(s,n,m)      数列 s[i][j] の [n][m] 項を BM+BostanMori で高速計算
 *
 *  sparse 版オペレーション (非ゼロ項が少ないとき高速)
 *   Sparse2D = vector<tuple<int,int,mint>>  // (i, j, c) = c*x^i*y^j
 *   *=  sparse   O(Dx Dy S)   S = |sparse|
 *   /=  sparse   O(Dx Dy S)   先頭項 (0,0) が非ゼロ必須
 *   inv_sparse   O(Dx Dy S)
 *   log_sparse   O(Dx Dy S)   f[0][0]==1 必須
 *   exp_sparse   O(Dx Dy S)   f[0][0]==0 必須
 *   pow_sparse   O(Dx Dy S)   exp(e log f) ベース (sparse f 向け)
 *
 *  計算量メモ (D = max(Dx, Dy))
 *   *, inv        O(Dx Dy log(Dx Dy))   内部で 1次元 NTT × O(Dy) 行
 *   log/exp       O(Dx Dy log(Dx Dy))
 *   BostanMori2D  O(K Dy log(Dy) log N)
 * ================================================================
 */

#pragma once
#include <bits/stdc++.h>
// 上の1変数 FPS / NTT が include 済みであること前提

// ================================================================
//  FPS2D 本体
// ================================================================
template <typename mint>
struct FPS2D {
  // coef[i][j] = [x^i y^j]、サイズ Dx 行 × Dy 列
  int Dx, Dy;
  vector<vector<mint>> coef;

  // ---- 構築 ----
  FPS2D() : Dx(0), Dy(0) {}
  FPS2D(int dx, int dy) : Dx(dx), Dy(dy), coef(dx, vector<mint>(dy, 0)) {}
  FPS2D(int dx, int dy, const vector<vector<mint>>& c)
    : Dx(dx), Dy(dy), coef(c) { _trim(); }

  static FPS2D one(int dx, int dy) {
    FPS2D f(dx, dy);
    if (dx > 0 && dy > 0) f[0][0] = 1;
    return f;
  }

  // ---- アクセス ----
  vector<mint>&       operator[](int i)       { return coef[i]; }
  const vector<mint>& operator[](int i) const { return coef[i]; }
  mint at(int i, int j) const {
    if (i < 0 || i >= Dx || j < 0 || j >= Dy) return 0;
    return coef[i][j];
  }

  // ---- 切り詰め ----
  FPS2D pre(int dx, int dy) const {
    FPS2D res(dx, dy);
    for (int i = 0; i < min(Dx, dx); i++)
      for (int j = 0; j < min(Dy, dy); j++)
        res[i][j] = coef[i][j];
    return res;
  }

  // サイズ固定ポリシー: 余分な行/列をゼロ埋めするだけ
  void _trim() {
    coef.resize(Dx);
    for (auto& row : coef) row.resize(Dy, 0);
  }

  bool operator==(const FPS2D& r) const { return coef == r.coef; }
  bool operator!=(const FPS2D& r) const { return !(*this == r); }

  // ================================================================
  //  基本四則
  // ================================================================
  FPS2D operator-() const {
    FPS2D res(Dx, Dy);
    for (int i = 0; i < Dx; i++)
      for (int j = 0; j < Dy; j++)
        res[i][j] = -coef[i][j];
    return res;
  }
  FPS2D operator+(const FPS2D& r) const { return FPS2D(*this) += r; }
  FPS2D operator-(const FPS2D& r) const { return FPS2D(*this) -= r; }
  FPS2D operator*(const FPS2D& r) const { return FPS2D(*this) *= r; }
  FPS2D operator/(const FPS2D& r) const { return FPS2D(*this) /= r; }
  FPS2D operator+(mint v) const { return FPS2D(*this) += v; }
  FPS2D operator-(mint v) const { return FPS2D(*this) -= v; }
  FPS2D operator*(mint v) const { return FPS2D(*this) *= v; }
  FPS2D operator/(mint v) const { return FPS2D(*this) /= v; }

  FPS2D& operator+=(const FPS2D& r) {
    for (int i = 0; i < min(Dx, r.Dx); i++)
      for (int j = 0; j < min(Dy, r.Dy); j++)
        coef[i][j] += r.coef[i][j];
    return *this;
  }
  FPS2D& operator-=(const FPS2D& r) {
    for (int i = 0; i < min(Dx, r.Dx); i++)
      for (int j = 0; j < min(Dy, r.Dy); j++)
        coef[i][j] -= r.coef[i][j];
    return *this;
  }
  FPS2D& operator+=(mint v) { if (Dx > 0 && Dy > 0) coef[0][0] += v; return *this; }
  FPS2D& operator-=(mint v) { if (Dx > 0 && Dy > 0) coef[0][0] -= v; return *this; }
  FPS2D& operator*=(mint v) {
    for (auto& row : coef) for (auto& c : row) c *= v;
    return *this;
  }
  FPS2D& operator/=(mint v) { return *this *= v.inv(); }

  // ================================================================
  //  掛け算: y 方向 NTT + x 方向素朴畳み込み
  //   f * g = (Σ_i f_i(y) x^i)(Σ_j g_j(y) x^j)
  //         = Σ_k (Σ_{i+j=k} f_i(y)*g_j(y)) x^k
  //   各行を y 方向 NTT → 周波数ドメインで x 畳み込み → 逆 NTT
  //   計算量: O(Dx^2 Dy log Dy)  (Dx 小さい典型ケース向け)
  // ================================================================
  FPS2D& operator*=(const FPS2D& r) {
    int RDx = Dx, RDy = Dy;
    int zy = NTT::bit_ceil(Dy + r.Dy - 1);
    auto ntt_rows = [&](const FPS2D& f, int nrows) {
      vector<vector<mint>> res(nrows, vector<mint>(zy, 0));
      for (int i = 0; i < min(f.Dx, nrows); i++) {
        for (int j = 0; j < f.Dy; j++) res[i][j] = f[i][j];
        NTT::butterfly(res[i]);
      }
      return res;
    };
    auto AF = ntt_rows(*this, Dx);
    auto BF = ntt_rows(r, r.Dx);
    // c[k][t] = Σ_{i+j=k} AF[i][t] * BF[j][t]
    vector<vector<mint>> CF(Dx + r.Dx - 1, vector<mint>(zy, 0));
    for (int i = 0; i < Dx; i++)
      for (int j = 0; j < r.Dx; j++)
        if (i + j < (int)CF.size())
          for (int t = 0; t < zy; t++)
            CF[i + j][t] += AF[i][t] * BF[j][t];
    mint izy = mint(zy).inv();
    FPS2D res(RDx, RDy);
    for (int i = 0; i < RDx; i++) {
      if (i >= (int)CF.size()) break;
      NTT::butterfly_inv(CF[i]);
      for (int j = 0; j < RDy && j < zy; j++)
        res[i][j] = CF[i][j] * izy;
    }
    return *this = res;
  }

  // ================================================================
  //  inv: x 方向 Newton 法
  //   G mod x^1  : G[0] = F(0,y)^{-1}  → 1変数 FPS の inv
  //   G mod x^{2k}: G = G * (2 - F * G)
  // ================================================================
  FPS2D inv() const {
    assert(Dx > 0 && Dy > 0);
    FPS<mint> f0(Dy);
    for (int j = 0; j < Dy; j++) f0[j] = at(0, j);
    FPS<mint> g0 = f0.inv();
    FPS2D G(1, Dy);
    for (int j = 0; j < Dy; j++) G[0][j] = g0[j];
    for (int k = 1; k < Dx; k <<= 1) {
      int nk = min(k * 2, Dx);
      FPS2D Fk = pre(nk, Dy);
      G = G.pre(nk, Dy);
      FPS2D FG = (Fk * G).pre(nk, Dy);
      FPS2D two = FPS2D::one(nk, Dy) * mint(2);
      G = (G * (two - FG)).pre(nk, Dy);
    }
    return G.pre(Dx, Dy);
  }
  FPS2D& operator/=(const FPS2D& r) { return *this = *this * r.inv(); }

  // ================================================================
  //  diff / integral
  // ================================================================
  FPS2D diff_x() const {
    if (Dx <= 1) return FPS2D(0, Dy);
    FPS2D res(Dx - 1, Dy);
    for (int i = 1; i < Dx; i++)
      for (int j = 0; j < Dy; j++)
        res[i - 1][j] = coef[i][j] * i;
    return res;
  }
  FPS2D diff_y() const {
    if (Dy <= 1) return FPS2D(Dx, 0);
    FPS2D res(Dx, Dy - 1);
    for (int i = 0; i < Dx; i++)
      for (int j = 1; j < Dy; j++)
        res[i][j - 1] = coef[i][j] * j;
    return res;
  }
  FPS2D integral_x() const {
    FPS2D res(Dx + 1, Dy);
    for (int i = 0; i < Dx; i++)
      for (int j = 0; j < Dy; j++)
        res[i + 1][j] = coef[i][j] / mint(i + 1);
    return res;
  }
  FPS2D integral_y() const {
    FPS2D res(Dx, Dy + 1);
    for (int i = 0; i < Dx; i++)
      for (int j = 0; j < Dy; j++)
        res[i][j + 1] = coef[i][j] / mint(j + 1);
    return res;
  }

  // ================================================================
  //  log: h = log F = ∫ (∂F/∂x) * F^{-1} dx   (f[0][0]==1 必須)
  // ================================================================
  FPS2D log() const {
    assert(at(0, 0) == mint(1));
    return (diff_x() * inv()).pre(Dx, Dy).integral_x().pre(Dx, Dy);
  }

  // ================================================================
  //  exp: x 方向 Newton 法
  //   G mod x^1  : G[0] = exp(F(0,y))  → 1変数 FPS の exp
  //   G mod x^{2k}: G = G * (1 + F - log G)
  // ================================================================
  FPS2D exp() const {
    assert(at(0, 0) == mint(0));
    FPS<mint> f0(Dy);
    for (int j = 0; j < Dy; j++) f0[j] = at(0, j);
    FPS<mint> g0 = f0.exp();
    FPS2D G(1, Dy);
    for (int j = 0; j < Dy; j++) G[0][j] = g0[j];
    for (int k = 1; k < Dx; k <<= 1) {
      int nk = min(k * 2, Dx);
      FPS2D Fk = pre(nk, Dy);
      G = G.pre(nk, Dy);
      G = (G * (FPS2D::one(nk, Dy) + Fk - G.log())).pre(nk, Dy);
    }
    return G.pre(Dx, Dy);
  }

  // ================================================================
  //  pow: exp(e * log f)
  //   f = x^a * y^b * c * h (h[0][0]==1) に分解して対応
  // ================================================================
  FPS2D pow(long long e) const {
    if (e == 0) return FPS2D::one(Dx, Dy);
    if (at(0, 0) == mint(1)) return (log() * mint(e)).exp();
    // x 方向先頭を探す
    int a = 0;
    while (a < Dx && [&]{ for (int j = 0; j < Dy; j++) if (coef[a][j] != mint(0)) return false; return true; }()) a++;
    if (a == Dx || (long long)a * e >= Dx) return FPS2D(Dx, Dy);
    FPS2D h(Dx - a, Dy);
    for (int i = 0; i < Dx - a; i++)
      for (int j = 0; j < Dy; j++)
        h[i][j] = at(i + a, j);
    // y 方向先頭を探す
    int b = 0;
    while (b < Dy && h[0][b] == mint(0)) b++;
    if (b == Dy || (long long)b * e >= Dy) return FPS2D(Dx, Dy);
    FPS2D k(Dx - a, Dy - b);
    for (int i = 0; i < Dx - a; i++)
      for (int j = 0; j < Dy - b; j++)
        k[i][j] = h.at(i, j + b);
    mint c = k[0][0];
    k /= c;
    int RDx = Dx - (int)(a * e), RDy = Dy - (int)(b * e);
    FPS2D res = (k.pre(RDx, RDy).log() * mint(e)).exp();
    res *= c.pow(e);
    res = res.pre(RDx, RDy);
    FPS2D ans(Dx, Dy);
    for (int i = 0; i < RDx && i + a*e < Dx; i++)
      for (int j = 0; j < RDy && j + b*e < Dy; j++)
        ans[i + (int)(a*e)][j + (int)(b*e)] = res[i][j];
    return ans;
  }

  // ================================================================
  //  sqrt: Newton 法  G = (G + F/G) / 2   (f[0][0] が平方数必須)
  // ================================================================
  FPS2D sqrt() const {
    FPS<mint> f0(Dy);
    for (int j = 0; j < Dy; j++) f0[j] = at(0, j);
    FPS<mint> g0 = f0.sqrt_sparse();
    assert(!g0.empty());
    mint inv2 = mint(2).inv();
    FPS2D G(1, Dy);
    for (int j = 0; j < Dy; j++) G[0][j] = g0.at(j);
    for (int k = 1; k < Dx; k <<= 1) {
      int nk = min(k * 2, Dx);
      FPS2D Fk = pre(nk, Dy);
      G = G.pre(nk, Dy);
      G = ((G + Fk * G.inv()) * inv2).pre(nk, Dy);
    }
    return G.pre(Dx, Dy);
  }

  // ================================================================
  //  eval / taylor_shift / shift_x / shift_y
  // ================================================================
  mint eval(mint x0, mint y0) const {
    mint res = 0, xpow = 1;
    for (int i = 0; i < Dx; i++) {
      mint yres = 0, ypow = 1;
      for (int j = 0; j < Dy; j++) { yres += coef[i][j] * ypow; ypow *= y0; }
      res += yres * xpow;
      xpow *= x0;
    }
    return res;
  }

  // f(x+a, y+b): 各軸に 1変数 Taylor shift を適用
  FPS2D taylor_shift(mint a, mint b) const {
    // x 方向シフト (列ごと)
    FPS2D tmp(Dx, Dy);
    for (int j = 0; j < Dy; j++) {
      FPS<mint> col(Dx);
      for (int i = 0; i < Dx; i++) col[i] = coef[i][j];
      FPS<mint> s = PolynomialTaylorShift(col, a.val());
      for (int i = 0; i < Dx; i++) tmp[i][j] = s[i];
    }
    // y 方向シフト (行ごと)
    FPS2D res(Dx, Dy);
    for (int i = 0; i < Dx; i++) {
      FPS<mint> row(Dy);
      for (int j = 0; j < Dy; j++) row[j] = tmp[i][j];
      FPS<mint> s = PolynomialTaylorShift(row, b.val());
      for (int j = 0; j < Dy; j++) res[i][j] = s[j];
    }
    return res;
  }

  FPS2D shift_x(int d) const {
    FPS2D res(Dx, Dy);
    for (int i = 0; i + d < Dx; i++)
      for (int j = 0; j < Dy; j++)
        res[i + d][j] = coef[i][j];
    return res;
  }
  FPS2D shift_y(int d) const {
    FPS2D res(Dx, Dy);
    for (int i = 0; i < Dx; i++)
      for (int j = 0; j + d < Dy; j++)
        res[i][j + d] = coef[i][j];
    return res;
  }

  void print() const {
    cout << "FPS2D(" << Dx << "," << Dy << "):\n";
    for (int i = 0; i < Dx; i++) {
      for (int j = 0; j < Dy; j++) cout << coef[i][j].val() << " ";
      cout << "\n";
    }
  }
};

// ================================================================
//  スカラー * FPS2D
// ================================================================
template <typename mint> FPS2D<mint> operator+(mint v, const FPS2D<mint>& f) { return f + v; }
template <typename mint> FPS2D<mint> operator*(mint v, const FPS2D<mint>& f) { return f * v; }

// ================================================================
//  Sparse2D 型と整備ユーティリティ
//  (i, j, c) = c * x^i * y^j  の項リスト
// ================================================================
template <typename mint>
using Sparse2D = vector<tuple<int, int, mint>>;

// ソート・重複まとめ・ゼロ除去
template <typename mint>
Sparse2D<mint> make_sparse2d(vector<tuple<int, int, mint>> v) {
  sort(v.begin(), v.end(), [](auto& a, auto& b) {
    return make_pair(get<0>(a), get<1>(a)) < make_pair(get<0>(b), get<1>(b));
  });
  Sparse2D<mint> res;
  for (auto& [i, j, c] : v) {
    if (!res.empty() && get<0>(res.back()) == i && get<1>(res.back()) == j)
      get<2>(res.back()) += c;
    else
      res.emplace_back(i, j, c);
  }
  res.erase(remove_if(res.begin(), res.end(),
    [](auto& t) { return get<2>(t) == mint(0); }), res.end());
  return res;
}

template <typename mint>
Sparse2D<mint> to_sparse(const FPS2D<mint>& f) {
  Sparse2D<mint> res;
  for (int i = 0; i < f.Dx; i++)
    for (int j = 0; j < f.Dy; j++)
      if (f[i][j] != mint(0)) res.emplace_back(i, j, f[i][j]);
  return res;
}
template <typename mint>
FPS2D<mint> from_sparse(const Sparse2D<mint>& s, int Dx, int Dy) {
  FPS2D<mint> f(Dx, Dy);
  for (auto& [a, b, c] : s)
    if (a < Dx && b < Dy) f[a][b] += c;
  return f;
}

// ================================================================
//  dense += / -= sparse (係数加算)
// ================================================================
template <typename mint>
FPS2D<mint>& operator+=(FPS2D<mint>& f, const Sparse2D<mint>& g) {
  for (auto& [a, b, c] : g) if (a < f.Dx && b < f.Dy) f[a][b] += c;
  return f;
}
template <typename mint>
FPS2D<mint>& operator-=(FPS2D<mint>& f, const Sparse2D<mint>& g) {
  for (auto& [a, b, c] : g) if (a < f.Dx && b < f.Dy) f[a][b] -= c;
  return f;
}
template <typename mint> FPS2D<mint> operator+(FPS2D<mint> f, const Sparse2D<mint>& g) { return f += g; }
template <typename mint> FPS2D<mint> operator-(FPS2D<mint> f, const Sparse2D<mint>& g) { return f -= g; }

// ================================================================
//  *=  sparse: res[i+a][j+b] += f[i][j] * c  を全 sparse 項で実行
//  計算量: O(Dx Dy S)
// ================================================================
template <typename mint>
FPS2D<mint>& operator*=(FPS2D<mint>& f, const Sparse2D<mint>& g) {
  FPS2D<mint> res(f.Dx, f.Dy);
  for (auto& [a, b, c] : g) {
    int ilim = f.Dx - a, jlim = f.Dy - b;
    if (ilim <= 0 || jlim <= 0) continue;
    for (int i = 0; i < ilim; i++)
      for (int j = 0; j < jlim; j++)
        res[i + a][j + b] += f[i][j] * c;
  }
  return f = res;
}
template <typename mint> FPS2D<mint> operator*(FPS2D<mint> f, const Sparse2D<mint>& g) { return f *= g; }

// ================================================================
//  /= sparse: 辞書順漸化式
//   g[0][0] = c0 として h = f/g を計算
//   h[i][j] = (f[i][j] - Σ_{(a,b)!=(0,0),(a,b)<=(i,j)} g[a][b]*h[i-a][j-b]) / c0
//  計算量: O(Dx Dy S)
// ================================================================
template <typename mint>
FPS2D<mint>& operator/=(FPS2D<mint>& f, const Sparse2D<mint>& g) {
  mint c0 = 0;
  Sparse2D<mint> grest;
  for (auto& [a, b, c] : g) {
    if (a == 0 && b == 0) c0 = c;
    else grest.emplace_back(a, b, c);
  }
  assert(c0 != mint(0));
  mint ic0 = c0.inv();
  for (int i = 0; i < f.Dx; i++) {
    for (int j = 0; j < f.Dy; j++) {
      for (auto& [a, b, c] : grest) {
        if (a > i || (a == i && b > j)) break;
        if (j - b >= 0 && j - b < f.Dy) f[i][j] -= c * f[i-a][j-b];
      }
      f[i][j] *= ic0;
    }
  }
  return f;
}
template <typename mint> FPS2D<mint> operator/(FPS2D<mint> f, const Sparse2D<mint>& g) { return f /= g; }

// ================================================================
//  inv_sparse: sparse f の逆元を dense で求める
//   漸化式: g[i][j] = ([i==0&&j==0] - Σ_{(a,b)!=(0,0)} f[a][b]*g[i-a][j-b]) / f[0][0]
//  計算量: O(Dx Dy S)
// ================================================================
template <typename mint>
FPS2D<mint> inv_sparse(const Sparse2D<mint>& f, int Dx, int Dy) {
  mint c0 = 0;
  Sparse2D<mint> frest;
  for (auto& [a, b, c] : f) {
    if (a == 0 && b == 0) c0 = c;
    else frest.emplace_back(a, b, c);
  }
  assert(c0 != mint(0));
  mint ic0 = c0.inv();
  FPS2D<mint> g(Dx, Dy);
  for (int i = 0; i < Dx; i++) {
    for (int j = 0; j < Dy; j++) {
      mint val = (i == 0 && j == 0) ? mint(1) : mint(0);
      for (auto& [a, b, c] : frest) {
        if (a > i || (a == i && b > j)) break;
        if (j - b >= 0) val -= c * g[i-a][j-b];
      }
      g[i][j] = val * ic0;
    }
  }
  return g;
}

// ================================================================
//  log_sparse: sparse f の log を dense で求める  (f[0][0]==1 必須)
//   原理: h = log f → ∂h/∂x = (∂f/∂x) / f
//   漸化式: (f * hx)[i][j] = fx[i][j]
//   hx[i][j] = fx[i][j] - Σ_{(a,b)!=(0,0),(a,b)<=(i,j)} f[a][b]*hx[i-a][j-b]
//   その後 h[i][j] = hx[i-1][j] / i
//   h[0][j] は log(f(0,y)) を 1変数漸化式で
//  計算量: O(Dx Dy S)
// ================================================================
template <typename mint>
FPS2D<mint> log_sparse(const Sparse2D<mint>& f, int Dx, int Dy) {
    mint c0 = 0;
    // (a,b) -> 係数 のマップを作る
    map<pair<int,int>, mint> fmap;
    for (auto& [a, b, c] : f) {
        if (a == 0 && b == 0) c0 = c;
        else fmap[{a, b}] += c;
    }
    assert(c0 == mint(1));

    // frest: (0,0) 以外の項を辞書順で保持 (break 条件に使う)
    Sparse2D<mint> frest;
    for (auto& [ab, c] : fmap)
        if (c != mint(0)) frest.emplace_back(ab.first, ab.second, c);
    // fmap は already sorted by (a,b)

    // hx[i][j] を計算
    // hx[i][j] = fx[i][j] - Σ_{(a,b)!=(0,0), (a,b)<=(i,j)} f[a][b]*hx[i-a][j-b]
    // fx[i][j] = f[i+1][j] * (i+1) をマップから O(1) で引く
    FPS2D<mint> hx(Dx > 0 ? Dx - 1 : 0, Dy);
    for (int i = 0; i < Dx - 1; i++) {
        for (int j = 0; j < Dy; j++) {
            // fx[i][j] = f[i+1][j] * (i+1)
            mint val = 0;
            auto it = fmap.find({i + 1, j});
            if (it != fmap.end()) val = it->second * mint(i + 1);

            // Σ f[a][b] * hx[i-a][j-b]  ((a,b) != (0,0), a<=i, b<=j)
            for (auto& [a, b, c] : frest) {
                if (a > i) break;           // 辞書順なので a>i 以降は不要
                if (a == i && b > j) break; // 同じ a で b>j も不要
                int ia = i - a, jb = j - b;
                if (jb < 0 || jb >= Dy) continue;
                val -= c * hx[ia][jb];
            }
            hx[i][j] = val;
        }
    }

    // x 積分 → h[i+1][j] = hx[i][j] / (i+1)
    FPS2D<mint> h(Dx, Dy);
    for (int i = 0; i < Dx - 1; i++)
        for (int j = 0; j < Dy; j++)
            h[i + 1][j] = hx[i][j] / mint(i + 1);

    // h[0][j]: log(f(0,y)) を 1変数漸化式で
    // f0y の sparse を fmap から O(1) で引く
    h[0][0] = 0;
    for (int j = 1; j < Dy; j++) {
        mint val = 0;
        // (f0y' * h0d)[j-1] = f0y'[j-1] = f[0][j] * j
        auto it = fmap.find({0, j});
        if (it != fmap.end()) val = it->second * mint(j);
        // - Σ_{1<=k<=j} f[0][k] * h0d[j-k]  (h0d[l] = h[0][l+1]*(l+1))
        for (int k = 1; k <= j; k++) {
            auto it2 = fmap.find({0, k});
            if (it2 == fmap.end()) continue;
            // h0d[j-k] = h[0][j-k+1] * (j-k+1) ... ではなく
            // h0d は ∂h/∂y の係数: h0d[l] = h[0][l+1] * (l+1)
            // val -= f[0][k] * (j-k+1) * h[0][j-k+1]  は間違い
            // 正しくは: 1変数 log の漸化式
            // h[0][j] = (f[0][j]*j - Σ_{k=1}^{j-1} k*f[0][k]*h[0][j-k]) / j / f[0][0]
            // f[0][0]=1 なので:
        }
        // ↑ 上のループは整理して書き直す
        val = 0;
        // 1変数 log 漸化式: h[0][j]*j = f[0][j]*j - Σ_{k=1}^{j-1} f[0][k]*k*h[0][j-k]
        auto it3 = fmap.find({0, j});
        val = (it3 != fmap.end()) ? it3->second * mint(j) : mint(0);
        // f[0][0]=1 の寄与 (k=0 の項は f[0][0]*0=0 なので不要)
        for (int k = 1; k < j; k++) {
            auto it4 = fmap.find({0, k});
            if (it4 == fmap.end()) continue;
            val -= it4->second * mint(k) * h[0][j - k];
        }
        h[0][j] = val / mint(j);
    }

    return h;
}
// ================================================================
//  exp_sparse: sparse f の exp を dense で求める  (f[0][0]==0 必須)
//   原理: g = exp(f) → gx = fx * g (微分方程式)
//   g[i+1][j] = (Σ_{(a,b) in fx} fx[a][b]*g[i-a][j-b]) / (i+1)
//   g[0][j]: exp(f(0,y)) を 1変数漸化式で
//  計算量: O(Dx Dy S)
// ================================================================
template <typename mint>
FPS2D<mint> exp_sparse(const Sparse2D<mint>& f, int Dx, int Dy) {
  for (auto& [a, b, c] : f) if (a == 0 && b == 0) assert(c == mint(0));
  // fx = ∂f/∂x の sparse
  Sparse2D<mint> fx;
  for (auto& [a, b, c] : f)
    if (a >= 1 && a < Dx) fx.emplace_back(a - 1, b, c * mint(a));
  sort(fx.begin(), fx.end(), [](auto& p, auto& q) {
    return make_pair(get<0>(p), get<1>(p)) < make_pair(get<0>(q), get<1>(q));
  });
  FPS2D<mint> g(Dx, Dy);
  // g[0][j]: exp(f(0,y)) の係数
  {
    vector<pair<int, mint>> f0y_sp;
    for (auto& [a, b, c] : f) if (a == 0 && b < Dy && b >= 1) f0y_sp.emplace_back(b, c);
    g[0][0] = 1;
    for (int j = 1; j < Dy; j++) {
      mint val = 0;
      for (auto& [k, c] : f0y_sp) {
        if (k > j) break;
        val += c * mint(k) * g[0][j - k];
      }
      g[0][j] = val / mint(j);
    }
  }
  // g[i+1][j] = (Σ fx[a][b]*g[i-a][j-b]) / (i+1)
  for (int i = 0; i < Dx - 1; i++) {
    for (int j = 0; j < Dy; j++) {
      mint val = 0;
      for (auto& [a, b, c] : fx) {
        if (a > i || (a == i && b > j)) break;
        int ia = i - a, jb = j - b;
        if (ia >= 0 && jb >= 0) val += c * g[ia][jb];
      }
      g[i + 1][j] = val / mint(i + 1);
    }
  }
  return g;
}

// ================================================================
//  pow_sparse: sparse f の k 乗を dense で求める
//   f[0][0]==1 のとき: log_sparse → *k → exp_sparse
//   それ以外: dense に変換して FPS2D::pow にフォールバック
// ================================================================
template <typename mint>
FPS2D<mint> pow_sparse(const Sparse2D<mint>& f, long long k, int Dx, int Dy) {
  if (k == 0) return FPS2D<mint>::one(Dx, Dy);
  bool is_one = true;
  for (auto& [a, b, c] : f)
    if (a == 0 && b == 0 && c != mint(1)) { is_one = false; break; }
  if (!is_one) {
    FPS2D<mint> fd(Dx, Dy);
    for (auto& [a, b, c] : f) if (a < Dx && b < Dy) fd[a][b] = c;
    return fd.pow(k);
  }
  FPS2D<mint> lf = log_sparse(f, Dx, Dy);
  Sparse2D<mint> lf_sp;
  for (int i = 0; i < Dx; i++)
    for (int j = 0; j < Dy; j++)
      if (lf[i][j] != mint(0)) lf_sp.emplace_back(i, j, lf[i][j] * mint(k));
  lf_sp = make_sparse2d(lf_sp);
  return exp_sparse(lf_sp, Dx, Dy);
}

// ================================================================
//  係数取り出しヘルパ
// ================================================================
template <typename mint>
mint coeff(const FPS2D<mint>& f, int N, int M) {
  if (N < 0 || N >= f.Dx || M < 0 || M >= f.Dy) return mint(0);
  return f[N][M];
}

// ================================================================
//  2変数 Bostan-Mori   [x^N y^M] P(x,y) / Q(x,y)
//
//  アルゴリズム (正しい 2変数版):
//   P, Q を x の多項式 Σ P_i(y) x^i として表す
//   Q の deg_x = K, Q(0,y) != 0, deg_x(P) < K
//
//  各ステップ:
//   1. Q*(x,y) = Q(-x,y)  (奇数次 x を符号反転)
//   2. U = P*Q*,  V = Q*Q*  (V は x^2 の多項式)
//   3. N 偶数: [x^{N/2} y^M] U_even / V_half
//      N 奇数: [x^{(N-1)/2} y^M] U_odd / V_half
//   4. N==0 になったら [y^M] P[0]/Q[0] を 1変数 BostanMori で
//
//  計算量: O(K Dy log(Dy) log N)
// ================================================================

// 内部: x 係数ベクトルの積 (y 方向 NTT::mul, x 方向素朴)
template <typename mint>
vector<FPS<mint>> poly_mul_xy(
  const vector<FPS<mint>>& A, const vector<FPS<mint>>& B, int maxdeg, int Dy)
{
  int na = (int)A.size(), nb = (int)B.size();
  if (!na || !nb) return {};
  int nc = min(na + nb - 1, maxdeg);
  vector<FPS<mint>> C(nc, FPS<mint>(Dy, 0));
  for (int i = 0; i < na && i < maxdeg; i++)
    for (int j = 0; j < nb && i + j < maxdeg; j++) {
      auto tmp = NTT::mul(A[i], B[j]);
      auto& cij = C[i + j];
      for (int k = 0; k < Dy && k < (int)tmp.size(); k++) cij[k] += tmp[k];
    }
  return C;
}

// 内部実装: P, Q をベクトル形式で受け取る
template <typename mint>
mint BostanMori2D_impl(
  vector<FPS<mint>> P, vector<FPS<mint>> Q, long long N, int M, int Dy)
{
  int K = (int)Q.size() - 1;
  P.resize(K); for (auto& p : P) p.resize(Dy, 0);
               for (auto& q : Q) q.resize(Dy, 0);
  while (N > 0) {
    // Q* = Q(-x, y)
    vector<FPS<mint>> Qstar(Q);
    for (int i = 1; i < (int)Qstar.size(); i += 2)
      for (auto& c : Qstar[i]) c = -c;
    auto U = poly_mul_xy(P, Qstar, 2 * K,     Dy);
    auto V = poly_mul_xy(Q, Qstar, 2 * K + 1, Dy);
    // 偶数/奇数次成分を抽出
    vector<FPS<mint>> U2(K, FPS<mint>(Dy, 0));
    vector<FPS<mint>> V2(K + 1, FPS<mint>(Dy, 0));
    for (int i = 0; i < K; i++) {
      int idx = (N % 2 == 0) ? 2 * i : 2 * i + 1;
      if (idx < (int)U.size()) U2[i] = U[idx];
    }
    for (int i = 0; i <= K; i++)
      if (2 * i < (int)V.size()) V2[i] = V[2 * i];
    P = U2; Q = V2; N >>= 1;
  }
  // N==0: [y^M] P[0]/Q[0] を 1変数 BostanMori で
  FPS<mint> p0 = P.empty() ? FPS<mint>(Dy, 0) : P[0];
  FPS<mint> q0 = Q[0];
  p0.resize(Dy, 0); q0.resize(Dy, 0);
  return BostanMori(p0, q0, (long long)M);
}

// 公開インターフェース: FPS2D を受け取り [x^N y^M] P/Q を返す
template <typename mint>
mint BostanMori2D(const FPS2D<mint>& P, const FPS2D<mint>& Q, long long N, long long M) {
  assert(!Q.coef.empty() && !Q.coef[0].empty());
  // Q の x 最高次を探す
  int Qx = Q.Dx - 1;
  while (Qx > 0) {
    bool zero = true;
    for (int j = 0; j < Q.Dy; j++) if (Q[Qx][j] != mint(0)) { zero = false; break; }
    if (!zero) break;
    Qx--;
  }
  int K = Qx, Dy = (int)max(P.Dy, Q.Dy);
  FPS2D<mint> P = P_in;
    mint extra = 0;

    if (P.Dx > K) {
        // x方向多項式除算: P(x,y) = Q(x,y)*S(x,y) + R(x,y)
        // x を主変数として各係数は y の多項式 (FPS<mint>)
        // P を x の多項式とみなし、最高次から順に div_fps で割る

        int PDx = P.Dx;
        // 各行を FPS<mint>(Dy) として保持
        vector<FPS<mint>> Prows(PDx, FPS<mint>(Dy, 0));
        for (int i = 0; i < PDx; i++)
            for (int j = 0; j < P.Dy; j++) Prows[i][j] = P.at(i, j);

        vector<FPS<mint>> Qrows(K + 1, FPS<mint>(Dy, 0));
        for (int i = 0; i <= K; i++)
            for (int j = 0; j < Q.Dy; j++) Qrows[i][j] = Q.at(i, j);

        // leading係数 qK(y) の逆元が必要
        // div_fps(f, g) は f = g*q + r を返すが、
        // ここでは x 方向の割り算なので行ごとに処理する
        // S の x^{i-K} の係数 = Prows[i] / Qrows[K]  (y方向FPS除算)
        // ただし "y方向FPS除算" ではなく、x多項式として
        // 最高次から引いていく筆算方式で実装する

        // S_rows[i] = x^i の係数 (i = 0..PDx-K-1)
        vector<FPS<mint>> S_rows(PDx - K, FPS<mint>(Dy, 0));

        for (int i = PDx - 1; i >= K; i--) {
            // si(y) = Prows[i](y) / Qrows[K](y)  → div_fps で商だけ使う
            // Qrows[K] は y の多項式で定数項が非ゼロ必要
            auto [si, rem_y] = div_fps(Prows[i], Qrows[K]);
            // rem_y は y 方向の余り (通常 Qrows[K][0]!=0 なら 0 になるはず)
            // si が S の x^{i-K} 係数
            S_rows[i - K] = si;

            // Prows[i-K .. i] -= si * Qrows[0..K]
            for (int k = 0; k <= K; k++) {
                auto sub = NTT::mul(si, Qrows[k]);
                sub.resize(Dy, 0);
                for (int j = 0; j < Dy; j++)
                    Prows[i - K + k][j] -= sub[j];
            }
        }

        // S の [x^N y^M] 寄与 (N < PDx-K のときのみ非ゼロ)
        if (N < (long long)S_rows.size() && M < Dy)
            extra = S_rows[N][M];

        // R = Prows の下 K 行
        P = FPS2D<mint>(K, Dy);
        for (int i = 0; i < K; i++)
            for (int j = 0; j < Dy; j++)
                P[i][j] = Prows[i][j];
    }

    vector<FPS<mint>> Pv(K, FPS<mint>(Dy, 0));
    vector<FPS<mint>> Qv(K + 1, FPS<mint>(Dy, 0));
    for (int i = 0; i < K && i < P.Dx; i++)
        for (int j = 0; j < Dy && j < P.Dy; j++) Pv[i][j] = P[i][j];
    for (int i = 0; i <= K && i < Q.Dx; i++)
        for (int j = 0; j < Dy && j < Q.Dy; j++) Qv[i][j] = Q[i][j];

    return extra + BostanMori2D_impl(Pv, Qv, N, (int)M, Dy);
}

// ================================================================
//  BerlekampMassey2D : Sakata algorithm
//
//  入力: s[i][j]  (i=0..H-1, j=0..W-1) の 2次元数列
//  出力: 最小接続多項式の集合 F = {(f, delta)} を返す
//        各 f は Sparse2D<mint> で表現される接続多項式
//        delta = (di, dj) はその多項式の "leading monomial" の次数差
//
//  性質:
//   返された各 f について
//     Σ_{(p,q)} f[p][q] * s[i-p][j-q] = 0  (十分大きい (i,j) で)
//   が成立する
//
//  Sakata algorithm の概要:
//   Berlekamp-Massey の 2変数拡張
//   更新候補集合 F (接続多項式の集合) を管理し、
//   各点 (i,j) での discrepancy d = Σ f[p][q]*s[i-p][j-q] が
//   非ゼロになったとき F を更新する
//
//  計算量: O(H W |F|)  (|F| は出力の接続多項式の数)
//
//  注意:
//   - 2変数の場合、最小接続多項式は一般に一意でなくイデアルを形成する
//   - nth_term2D で使う場合は接続多項式 Q(x,y) として最初の要素を使う
//     (実用上は漸化式の shape が矩形の場合が多い)
// ================================================================

// 点の辞書順比較 (Sakata で使う σ 次数 = 点群の大きさ)
// (i1,j1) <_lex (i2,j2): i1<i2, または i1==i2 かつ j1<j2
inline bool lex_lt(pair<int,int> a, pair<int,int> b) {
  return a.first != b.first ? a.first < b.first : a.second < b.second;
}

// 接続多項式の "σ-degree" = その多項式が接続する点集合の大きさ
// ここでは簡略化として leading monomial の (di+dj) で代用
// (競プロ的には矩形漸化式が主目的なので十分)

template <typename mint>
struct SakataState {
  // F: 現在の接続多項式候補集合
  // 各要素: (f の係数 sparse, leading pos (di,dj), 最後に更新された点, discrepancy)
  struct Poly {
    Sparse2D<mint> f;  // f[0][0] = 1 を想定; Σ f[p][q]*s[i-p][j-q] = 0
    pair<int,int> lead; // leading monomial の位置 (正確には sigma-degree に関わる位置)
    pair<int,int> last; // 最後に discrepancy != 0 だった点
    mint d_last;        // そのときの discrepancy
  };
  vector<Poly> F;
};

// ----------------------------------------------------------------
//  BerlekampMassey2D 本体
//  s: H×W の 2次元数列 (s[i][j])
//  戻り値: 接続多項式の sparse リスト (複数返ることがある)
//          nth_term2D では先頭の最小次数のものを使う
// ----------------------------------------------------------------
template <typename mint>
vector<Sparse2D<mint>> BerlekampMassey2D(const vector<vector<mint>>& s) {
  int H = (int)s.size(), W = H ? (int)s[0].size() : 0;
  if (!H || !W) return {};

  // 接続多項式の評価: f を点 (i,j) で評価
  // disc(f, i, j) = Σ_{(p,q) in f, p<=i, q<=j} f[p][q] * s[i-p][j-q]
  auto disc = [&](const Sparse2D<mint>& f, int i, int j) -> mint {
    mint val = 0;
    for (auto& [p, q, c] : f) {
      if (p > i || q > j) continue;
      val += c * s[i - p][j - q];
    }
    return val;
  };

  // 接続多項式の "σ-degree" (Sakata の定義に従うが、
  // 競プロ簡略版として leading monomial の p+q で代用)
  auto sdeg = [](const Sparse2D<mint>& f) -> int {
    int d = 0;
    for (auto& [p, q, c] : f) d = max(d, p + q);
    return d;
  };

  // 初期状態: f = 1 (定数多項式)
  using Poly = typename SakataState<mint>::Poly;
  Poly init;
  init.f = make_sparse2d<mint>({{0, 0, mint(1)}});
  init.lead = {0, 0};
  init.last = {-1, -1};
  init.d_last = mint(1);
  vector<Poly> F = {init};

  // 各点 (i,j) を辞書順に走査
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      // 各接続多項式の discrepancy を計算
      vector<mint> D;
      D.reserve(F.size());
      for (auto& poly : F) D.push_back(disc(poly.f, i, j));

      // 更新が必要な多項式 (D[k] != 0) を処理
      // 更新前の F のコピーを保持
      vector<Poly> F_new = F;
      for (int k = 0; k < (int)F.size(); k++) {
        if (D[k] == mint(0)) continue;
        // g = F[k].f - (D[k]/D[best]) * x^{di} y^{dj} * F[best].f
        // best: 最後に更新された点が (i,j) より前で sdeg が最小のもの
        int best = -1;
        for (int l = 0; l < (int)F.size(); l++) {
          if (F[l].last == make_pair(-1,-1)) continue; 
          if (F[l].last.first > i || (F[l].last.first == i && F[l].last.second >= j)) continue;
          // F[l] の last が (i,j) より厳密に前
          if (best == -1 || sdeg(F[l].f) < sdeg(F[best].f)) best = l;
        }
        if (best == -1) {
          // 更新できる候補がない → F[k] の last を更新するだけ
          F_new[k].last = {i, j};
          F_new[k].d_last = D[k];
          continue;
        }
        // delta = (i - F[best].last.i, j - F[best].last.j)
        int di = i - F[best].last.first;
        int dj = j - F[best].last.second;
        mint coef = D[k] / F[best].d_last;
        // F_new[k].f = F[k].f - coef * x^{di} y^{dj} * F[best].f
        // sparse 演算で実装
        vector<tuple<int,int,mint>> terms;
        for (auto& [p, q, c] : F_new[k].f) terms.emplace_back(p, q, c);
        for (auto& [p, q, c] : F[best].f)  terms.emplace_back(p + di, q + dj, -coef * c);
        F_new[k].f = make_sparse2d(terms);
        // sdeg が増えた場合は last を更新
        if (sdeg(F_new[k].f) > sdeg(F[k].f)) {
          F_new[k].last = {i, j};
          F_new[k].d_last = D[k];
        }
      }
      F = F_new;
    }
  }

  // 結果: 重複を除いた接続多項式を返す (sdeg 昇順)
  sort(F.begin(), F.end(), [&](auto& a, auto& b) {
    return sdeg(a.f) < sdeg(b.f);
  });
  vector<Sparse2D<mint>> res;
  for (auto& poly : F) {
    // f[0][0] == 1 かつ sdeg >= 1 のもの (定数多項式は除く)
    bool has_const = false;
    for (auto& [p,q,c] : poly.f) if (p==0&&q==0) { has_const=true; break; }
    if (!has_const) continue;
    if (sdeg(poly.f) == 0) continue; // 定数のみは除く
    res.push_back(poly.f);
  }
  // 重複除去
  sort(res.begin(), res.end(), [](auto& a, auto& b) { return a < b; });
  res.erase(unique(res.begin(), res.end()), res.end());
  if (res.empty() && !F.empty()) {
    // fallback: sdeg>=1 のものを全部入れる
    for (auto& poly : F) if (sdeg(poly.f) >= 1) res.push_back(poly.f);
  }
  return res;
}

// ================================================================
//  nth_term2D : s[n][m] を高速に求める
//
//  手順:
//   1. BerlekampMassey2D(s) で接続多項式 Q(x,y) を求める
//      Q(x,y) = Σ c_{p,q} x^p y^q,  Q[0][0]=1
//      漸化式: Σ c_{p,q} * s[i-p][j-q] = 0
//
//   2. 母関数の分母 Q(x,y) を構成:
//      A(x,y) = Σ s[i][j] x^i y^j として
//      P(x,y) = A(x,y) * Q(x,y)  の低次部分 (初期条件部分)
//      deg_x(P) < deg_x(Q), deg_y(P) < deg_y(Q) を満たす
//
//   3. BostanMori2D(P, Q, n, m) で [x^n y^m] P/Q を計算
//
//  入力:
//   s    : 2次元数列 (十分な初期値を含む H×W グリッド)
//   n, m : 求めたい添字
//
//  必要サンプル数の目安:
//   接続多項式の x 次数を Kx, y 次数を Ky とすると
//   H >= 2*Kx+1, W >= 2*Ky+1 程度あれば BM が正しく動く
// ================================================================
template <typename mint>
mint nth_term2D(const vector<vector<mint>>& s, long long n, long long m) {
  int H = (int)s.size(), W = H ? (int)s[0].size() : 0;
  assert(H > 0 && W > 0);

  // n,m が既知範囲内なら直接返す
  if (n < H && m < W) return s[n][m];

  // 1. Berlekamp-Massey 2D で接続多項式を求める
  auto polys = BerlekampMassey2D(s);
  assert(!polys.empty());

  // 接続多項式のうち x 次数と y 次数が両方最小のものを選ぶ
  // (矩形サポートを持つものが理想)
  auto& Q_sp = polys[0]; // sdeg 最小のものを使う

  // Q_sp から FPS2D の Q を構成 (サイズ Kx+1 × Ky+1)
  int Kx = 0, Ky = 0;
  for (auto& [p, q, c] : Q_sp) { Kx = max(Kx, p); Ky = max(Ky, q); }

  // 2. P = (A * Q) の初期条件部分を抽出
  //    A は s の有限切り出し、P は deg_x < Kx かつ deg_y < Ky の部分
  //    具体的: P[i][j] = Σ_{p<=i, q<=j} Q[p][q] * s[i-p][j-q]
  //    ただし (i < Kx) または (j < Ky) の範囲のみ保持
  //    (漸化式が成立する範囲では P[i][j] = 0 になる)
  int PDx = Kx, PDy = Ky; // P の有効範囲 (x 次数 < Kx, y 次数 < Ky)
  // ただし一般の2変数の場合 P の次数範囲は Q の形に依存するため
  // 安全のため PDx = Kx, PDy = max(W-1, Ky) として初期条件を抽出
  PDx = min(H - 1, Kx);          // deg_x(P) < Kx
  PDy = min(W - 1, Kx + Ky);     // L字の張り出し分を考慮


  FPS2D<mint> Qf(Kx + 1, Ky + 1);
  for (auto& [p, q, c] : Q_sp) Qf[p][q] = c;

  // P = A * Q の (PDx × PDy) 切り出し
  // P[i][j] = Σ_{0<=p<=min(i,Kx), 0<=q<=min(j,Ky)} Q[p][q] * s[i-p][j-q]
  FPS2D<mint> Pf(PDx + 1, PDy + 1);
  for (int i = 0; i <= PDx; i++) {
    for (int j = 0; j <= PDy; j++) {
      if (i >= Kx && j >= Ky) continue;
      mint val = 0;
      for (auto& [p, q, c] : Q_sp) {
        
        if (p > i || q > j) continue;
        int si = i - p, sj = j - q;
        if (si < H && sj < W) val += c * s[si][sj];
      }
      Pf[i][j] = val;
    }
  }

  // 漸化式が成立する範囲 (i >= Kx かつ j >= Ky) では P[i][j] = 0 のはず
  // ゼロでない部分だけ残す (初期条件)
  // P を deg_x(Q)-1 = Kx-1 以下に切り詰め (BostanMori の前提)
  Pf = Pf.pre(Kx, PDy + 1);

  // 3. BostanMori2D で [x^n y^m] P/Q を計算
  return BostanMori2D(Pf, Qf, n, m);
}

// ================================================================
//  使用例
// ================================================================
/*
using mint = Fp<998244353>;
using FPS2 = FPS2D<mint>;

// 例1: (1 + x + y)^{-1} の [x^3 y^2]
{
  FPS2 f(5, 5);
  f[0][0] = 1; f[1][0] = 1; f[0][1] = 1;
  auto g = f.inv();
  cout << g[3][2] << "\n";
}

// 例2: 2変数 BostanMori — [x^{10} y^{5}] 1/(1-x-y) = C(15,5) = 3003
{
  FPS2 P(1, 1); P[0][0] = 1;
  FPS2 Q(2, 2); Q[0][0] = 1; Q[1][0] = -1; Q[0][1] = -1;
  cout << BostanMori2D(P, Q, 10, 5) << "\n";
}

// 例3: exp — e^{x+y} mod x^4 y^4,  g[i][j] = 1/(i! j!)
{
  FPS2 f(4, 4); f[1][0] = 1; f[0][1] = 1;
  auto g = f.exp();
}

// 例4: sparse 乗算 — f * (1 - x - y)
{
  FPS2 f(5, 5); f[0][0] = 1;
  Sparse2D<mint> g = make_sparse2d<mint>({{0,0,1},{1,0,-1},{0,1,-1}});
  f *= g;
}

// 例5: inv_sparse — 1/(1-x-y),  g[i][j] = C(i+j, i)
{
  Sparse2D<mint> f = make_sparse2d<mint>({{0,0,1},{1,0,-1},{0,1,-1}});
  auto g = inv_sparse(f, 6, 6);
  cout << g[3][2] << "\n"; // C(5,3) = 10
}

// 例6: exp_sparse — exp(x + y + xy)
{
  Sparse2D<mint> f = make_sparse2d<mint>({{1,0,1},{0,1,1},{1,1,1}});
  auto g = exp_sparse(f, 5, 5);
}

// 例7: pow_sparse — (1 + x + y)^100
{
  Sparse2D<mint> f = make_sparse2d<mint>({{0,0,1},{1,0,1},{0,1,1}});
  auto g = pow_sparse(f, 100, 50, 50);
}

// 例8: nth_term2D — s[i][j] = C(i+j, i) の s[100][200]
//   漸化式: s[i][j] = s[i-1][j] + s[i][j-1]
//   接続多項式 Q(x,y) = 1 - x - y を自動検出
{
  // 初期値 (十分な量を用意)
  int H = 10, W = 10;
  vector<vector<mint>> s(H, vector<mint>(W));
  for (int i = 0; i < H; i++)
    for (int j = 0; j < W; j++)
      s[i][j] = (i==0||j==0) ? 1 : s[i-1][j].val()+s[i][j-1].val();
  cout << nth_term2D(s, 100, 200) << "\n"; // C(300,100)
}

// 例9: nth_term2D — 3項漸化式  s[i][j] = s[i-1][j] + s[i][j-1] - s[i-1][j-1]
//   (接続多項式 Q = 1 - x - y + xy = (1-x)(1-y) → s[i][j] = i+j+1 みたいな数列)
{
  int H = 8, W = 8;
  vector<vector<mint>> s(H, vector<mint>(W));
  for (int i = 0; i < H; i++)
    for (int j = 0; j < W; j++) {
      s[i][j] = 0;
      if (i > 0) s[i][j] += s[i-1][j];
      if (j > 0) s[i][j] += s[i][j-1];
      if (i > 0 && j > 0) s[i][j] -= s[i-1][j-1];
      if (i == 0 && j == 0) s[i][j] = 1;
    }
  cout << nth_term2D(s, 50, 70) << "\n";
}
*/
//なるべくFPS2Dはsize(x)<size(y)にしよう！転置したりして

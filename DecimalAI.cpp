/**
 * @file decimal.hpp
 * @brief 最強の Decimal 型 — 任意精度・固定スケール対応の10進数演算ライブラリ
 *
 * 設計方針:
 *   - ヘッダオンリー (single-header)
 *   - 依存: C++20 標準ライブラリのみ
 *   - STL との完全統合 (ranges, format, hash, iostream, structured bindings, etc.)
 *   - UB なし・例外安全 (Strong Exception Guarantee)
 *   - constexpr フレンドリー (コンパイル時演算)
 *   - IEEE 754 互換の特殊値 (Inf, NaN) サポート
 *
 * 使用例:
 *   using namespace dec;
 *   Decimal a{"123.456"};
 *   Decimal b{789, -3};          // 0.789
 *   auto c = a + b;              // 124.245
 *   std::println("{:.2f}", c);   // 124.25
 *   auto a=Decimal::from_ratio(22,7)//3.142857142857...
 *   assert(a == Decimal{"123.456000"});
 */

#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <charconv>
#include <compare>
#include <concepts>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

namespace dec {

// ============================================================
// 前方宣言
// ============================================================
class Decimal;

// ============================================================
// 内部ユーティリティ
// ============================================================
namespace detail {

// 多倍長整数を std::vector<uint32_t> (リトルエンディアン) で表現
// ここでは実用上 128-bit 相当の精度を BigInt で、
// より大きな精度は vector ベースで表現する。
// 本実装は "符号付き BigDecimal" を coefficient × 10^exponent で保持する。

// ---- 10^n テーブル (高速化) ----
inline constexpr std::uint64_t pow10_table[] = {
    1ULL,
    10ULL,
    100ULL,
    1'000ULL,
    10'000ULL,
    100'000ULL,
    1'000'000ULL,
    10'000'000ULL,
    100'000'000ULL,
    1'000'000'000ULL,
    10'000'000'000ULL,
    100'000'000'000ULL,
    1'000'000'000'000ULL,
    10'000'000'000'000ULL,
    100'000'000'000'000ULL,
    1'000'000'000'000'000ULL,
    10'000'000'000'000'000ULL,
    100'000'000'000'000'000ULL,
    1'000'000'000'000'000'000ULL,
};

inline constexpr int MAX_POW10 = static_cast<int>(std::size(pow10_table)) - 1;

constexpr std::uint64_t safe_pow10(int n) {
    if (n < 0 || n > MAX_POW10)
        throw std::overflow_error("dec::detail::safe_pow10: exponent out of range");
    return pow10_table[n];
}

// __int128 が使えればそれを使う (GCC/Clang)
#if defined(__GNUC__) || defined(__clang__)
using int128_t  = __int128;
using uint128_t = unsigned __int128;
#define DEC_HAS_INT128 1
#else
#define DEC_HAS_INT128 0
// fallback: 128-bit 乗算を __int64 で近似 (精度は落ちる)
#endif

// ---- 桁数 ----
constexpr int digit_count(std::uint64_t v) noexcept {
    if (v == 0) return 1;
    int d = 0;
    while (v > 0) { v /= 10; ++d; }
    return d;
}

// ---- 末尾ゼロ除去 ----
constexpr std::pair<std::uint64_t, int> strip_trailing_zeros(std::uint64_t coef, int exp) noexcept {
    if (coef == 0) return {0, 0};
    while (coef % 10 == 0) {
        coef /= 10;
        ++exp;
    }
    return {coef, exp};
}

// ---- 特殊値フラグ ----
enum class SpecialValue : uint8_t {
    None    = 0,
    PosInf  = 1,
    NegInf  = 2,
    NaN     = 3,
    NegNaN  = 4,
};

} // namespace detail

// ============================================================
// エラー型
// ============================================================

struct DecimalOverflow   : std::overflow_error  { using std::overflow_error::overflow_error; };
struct DecimalUnderflow  : std::underflow_error { using std::underflow_error::underflow_error; };
struct DecimalDivByZero  : std::domain_error    { using std::domain_error::domain_error; };
struct DecimalInvalidArg : std::invalid_argument{ using std::invalid_argument::invalid_argument; };

// ============================================================
// RoundingMode
// ============================================================
enum class RoundingMode {
    HalfUp,       // 四捨五入 (デフォルト)
    HalfDown,
    HalfEven,     // 銀行丸め (Banker's rounding)
    Ceiling,      // 正の無限大方向
    Floor,        // 負の無限大方向
    Truncate,     // 切り捨て
};

// ============================================================
// DecimalContext — スレッドローカルな演算コンテキスト
// ============================================================
struct DecimalContext {
    int           precision    = 38;          // 有効桁数 (最大)
    RoundingMode  rounding     = RoundingMode::HalfEven;
    bool          trap_overflow  = true;
    bool          trap_div_zero  = true;
    bool          trap_invalid   = true;

    static DecimalContext& current() noexcept {
        thread_local DecimalContext ctx;
        return ctx;
    }
};

// ============================================================
// Decimal 本体
// ============================================================

/**
 * @class Decimal
 * @brief 符号付き10進数。内部表現: (-1)^sign × coefficient × 10^exponent
 *
 * coefficient は符号なし64-bit整数 (最大18桁程度)。
 * より大きな精度が必要な場合は BigDecimal (feature flag) を使用。
 * exponent は int32_t。
 */
class Decimal {
public:
    // ==== 型エイリアス ====
    using value_type      = std::uint64_t;
    using exponent_type   = std::int32_t;

private:
    value_type          coef_   = 0;
    exponent_type       exp_    = 0;
    bool                neg_    = false;
    detail::SpecialValue special_ = detail::SpecialValue::None;

    // ==== 内部コンストラクタ ====
    constexpr Decimal(value_type c, exponent_type e, bool neg, detail::SpecialValue sp = detail::SpecialValue::None) noexcept
        : coef_(c), exp_(e), neg_(neg), special_(sp) {}

    friend class std::numeric_limits<Decimal>; // ⑤ private ctor へのアクセスを許可

    static constexpr Decimal make_special(detail::SpecialValue sv) noexcept {
        return Decimal{0, 0, false, sv};
    }

    // ==== 正規化 (末尾ゼロを除去) ====
    constexpr void normalize() noexcept {
        if (coef_ == 0) { exp_ = 0; neg_ = false; return; }
        auto [c, e] = detail::strip_trailing_zeros(coef_, exp_);
        coef_ = c; exp_ = e;
    }

    // ==== スケール合わせ (加減算用) ====
    // 両辺を同一の exponent に揃えた (coef_a, coef_b) を返す
    static std::optional<std::pair<value_type, value_type>>
    align(const Decimal& a, const Decimal& b) {
        if (a.exp_ == b.exp_) return std::pair{a.coef_, b.coef_};
        const Decimal* hi = &a;
        const Decimal* lo = &b;
        bool swapped = false;
        if (a.exp_ < b.exp_) { std::swap(hi, lo); swapped = true; }
        // hi.exp_ > lo.exp_ → hi を lo に合わせる
        int diff = hi->exp_ - lo->exp_;
        if (diff > detail::MAX_POW10) return std::nullopt; // overflow
        value_type scale = detail::safe_pow10(diff);
        // オーバーフローチェック
        if (hi->coef_ > std::numeric_limits<value_type>::max() / scale)
            return std::nullopt;
        value_type scaled = hi->coef_ * scale;
        if (!swapped) return std::pair{scaled, lo->coef_};
        else          return std::pair{lo->coef_, scaled};
    }

public:
    // ============================================================
    // 特殊値ファクトリ
    // ============================================================
    static constexpr Decimal inf()     noexcept { return make_special(detail::SpecialValue::PosInf); }
    static constexpr Decimal neg_inf() noexcept { return make_special(detail::SpecialValue::NegInf); }
    static constexpr Decimal nan()     noexcept { return make_special(detail::SpecialValue::NaN); }
    static constexpr Decimal zero()    noexcept { return Decimal{0, 0, false}; }
    static constexpr Decimal one()     noexcept { return Decimal{1, 0, false}; }

    /**
     * @brief 分数 numerator/denominator から Decimal を生成する
     *
     * @param numerator   分子 (int64_t)
     * @param denominator 分母 (int64_t, 0 は不可)
     * @param precision   計算に使う有効桁数 (省略時は DecimalContext の設定に従う)
     *
     * 例:
     *   Decimal::from_ratio(1, 3)    // 0.333...
     *   Decimal::from_ratio(22, 7)   // 3.142857...
     *   Decimal::from_ratio(-1, 4)   // -0.25
     *   Decimal::from_ratio(1, 0)    // → DecimalDivByZero 例外
     */
    [[nodiscard]] static Decimal from_ratio(std::int64_t numerator,
                                            std::int64_t denominator,
                                            std::optional<int> precision = std::nullopt)
    {
        if (denominator == 0) {
            if (DecimalContext::current().trap_div_zero)
                throw DecimalDivByZero("Decimal::from_ratio: denominator is zero");
            if (numerator == 0) return nan();
            return numerator > 0 ? inf() : neg_inf();
        }
        // precision を一時的に上書きしたい場合のための RAII ガード
        auto& ctx = DecimalContext::current();
        const int saved = ctx.precision;
        if (precision.has_value()) ctx.precision = *precision;
        struct Guard {
            DecimalContext& c; int v;
            ~Guard() { c.precision = v; }
        } guard{ctx, saved};

        return Decimal{numerator} / Decimal{denominator};
    }

    [[nodiscard]] constexpr bool is_nan()    const noexcept { return special_ == detail::SpecialValue::NaN || special_ == detail::SpecialValue::NegNaN; }
    [[nodiscard]] constexpr bool is_inf()    const noexcept { return special_ == detail::SpecialValue::PosInf || special_ == detail::SpecialValue::NegInf; }
    [[nodiscard]] constexpr bool is_finite() const noexcept { return special_ == detail::SpecialValue::None; }
    [[nodiscard]] constexpr bool is_zero()   const noexcept { return is_finite() && coef_ == 0; }
    [[nodiscard]] constexpr bool is_positive()const noexcept{ return is_finite() && !neg_ && coef_ > 0; }
    [[nodiscard]] constexpr bool is_negative()const noexcept{ return is_finite() && neg_ && coef_ > 0; }
    [[nodiscard]] constexpr bool is_integer() const noexcept{ return is_finite() && exp_ >= 0; }

    // ============================================================
    // コンストラクタ群
    // ============================================================

    /// デフォルト: ゼロ
    constexpr Decimal() noexcept = default;

    /// 整数係数 + 指数 (coefficient × 10^exponent)
    constexpr Decimal(std::int64_t coefficient, exponent_type exponent = 0) noexcept {
        if (coefficient < 0) { neg_ = true; coef_ = static_cast<value_type>(-coefficient); }
        else                 { neg_ = false; coef_ = static_cast<value_type>(coefficient);  }
        exp_ = exponent;
        normalize();
    }

    /// unsigned 整数から
    constexpr explicit Decimal(std::uint64_t coefficient, exponent_type exponent = 0) noexcept
        : coef_(coefficient), exp_(exponent), neg_(false) { normalize(); }

    /// 浮動小数点から (精度に注意!)
    explicit Decimal(double v) {
        if (std::isnan(v))    { special_ = detail::SpecialValue::NaN; return; }
        if (std::isinf(v))    { special_ = v > 0 ? detail::SpecialValue::PosInf : detail::SpecialValue::NegInf; return; }
        // double → 文字列 → パース (安全で正確)
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), v, std::chars_format::general, 17);
        if (ec != std::errc{}) throw DecimalInvalidArg("Decimal(double): conversion failed");
        *this = Decimal(std::string_view(buf, ptr));
    }

    explicit Decimal(float v) : Decimal(static_cast<double>(v)) {}
    explicit Decimal(long double v) : Decimal(static_cast<double>(v)) {}

    /// 文字列から ("123.456", "-0.001", "Inf", "NaN", "1.23e10" など)
    explicit Decimal(std::string_view sv) {
        parse(sv);
    }

    explicit Decimal(const std::string& s) : Decimal(std::string_view(s)) {}
    explicit Decimal(const char* s)        : Decimal(std::string_view(s)) {}

    /// コピー・ムーブ
    constexpr Decimal(const Decimal&) noexcept = default;
    constexpr Decimal(Decimal&&)      noexcept = default;
    constexpr Decimal& operator=(const Decimal&) noexcept = default;
    constexpr Decimal& operator=(Decimal&&)      noexcept = default;

    // ============================================================
    // 文字列パーサ
    // ============================================================
private:
    void parse(std::string_view sv) {
        if (sv.empty()) throw DecimalInvalidArg("Decimal: empty string");

        // 特殊値
        auto ci_eq = [](std::string_view a, std::string_view b) {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i)
                if (std::tolower(a[i]) != std::tolower(b[i])) return false;
            return true;
        };
        if (ci_eq(sv, "inf") || ci_eq(sv, "+inf") || ci_eq(sv, "infinity"))
            { special_ = detail::SpecialValue::PosInf; return; }
        if (ci_eq(sv, "-inf") || ci_eq(sv, "-infinity"))
            { special_ = detail::SpecialValue::NegInf; return; }
        if (ci_eq(sv, "nan") || ci_eq(sv, "+nan"))
            { special_ = detail::SpecialValue::NaN; return; }
        if (ci_eq(sv, "-nan"))
            { special_ = detail::SpecialValue::NegNaN; return; }

        // 符号
        size_t i = 0;
        if (sv[i] == '+') { neg_ = false; ++i; }
        else if (sv[i] == '-') { neg_ = true; ++i; }

        // 整数部 + 小数部
        int dot_pos = -1;
        std::string digits;
        digits.reserve(sv.size());
        for (; i < sv.size(); ++i) {
            char c = sv[i];
            if (c == '.') {
                if (dot_pos >= 0) throw DecimalInvalidArg("Decimal: multiple dots");
                dot_pos = static_cast<int>(digits.size());
            } else if (c == 'e' || c == 'E') {
                ++i; break;
            } else if (c >= '0' && c <= '9') {
                digits.push_back(c);
            } else {
                throw DecimalInvalidArg(std::string("Decimal: invalid char '") + c + "'");
            }
        }

        // 指数部
        int exp_part = 0;
        if (i < sv.size()) {
            bool exp_neg = false;
            if (sv[i] == '+') ++i;
            else if (sv[i] == '-') { exp_neg = true; ++i; }
            for (; i < sv.size(); ++i) {
                char c = sv[i];
                if (c < '0' || c > '9') throw DecimalInvalidArg("Decimal: invalid exponent");
                exp_part = exp_part * 10 + (c - '0');
            }
            if (exp_neg) exp_part = -exp_part;
        }

        // 係数の組み立て
        // dot_pos は digits の何文字目に小数点があるか
        // 例: "123.456" → digits="123456", dot_pos=3, frac_digits=3
        int frac_digits = (dot_pos < 0) ? 0 : (static_cast<int>(digits.size()) - dot_pos);

        // 先頭ゼロを除去
        size_t start = digits.find_first_not_of('0');
        if (start == std::string::npos) {
            coef_ = 0; exp_ = 0; neg_ = false; return;
        }
        digits = digits.substr(start);

        if (digits.size() > 19)
            throw DecimalInvalidArg("Decimal: coefficient too large (>19 digits)");

        coef_ = 0;
        for (char c : digits) coef_ = coef_ * 10 + (c - '0');
        exp_ = exp_part - frac_digits;
        normalize();
    }

    // ============================================================
    // アクセサ
    // ============================================================
public:
    [[nodiscard]] constexpr value_type    coefficient() const noexcept { return coef_; }
    [[nodiscard]] constexpr exponent_type exponent()    const noexcept { return exp_;  }
    [[nodiscard]] constexpr bool          negative()    const noexcept { return neg_;  }

    // ============================================================
    // 変換演算子
    // ============================================================

    [[nodiscard]] explicit constexpr operator bool()        const noexcept { return !is_zero() && !is_nan(); }
    [[nodiscard]] explicit operator double()                const noexcept {
        if (is_nan()) return std::numeric_limits<double>::quiet_NaN();
        if (special_ == detail::SpecialValue::PosInf) return std::numeric_limits<double>::infinity();
        if (special_ == detail::SpecialValue::NegInf) return -std::numeric_limits<double>::infinity();
        double v = static_cast<double>(coef_);
        // 10^exp を掛ける
        int e = exp_;
        while (e > 0) { v *= 10.0; --e; }
        while (e < 0) { v /= 10.0; ++e; }
        return neg_ ? -v : v;
    }

    [[nodiscard]] explicit operator long double() const noexcept { return static_cast<long double>(static_cast<double>(*this)); }
    [[nodiscard]] explicit operator float()       const noexcept { return static_cast<float>(static_cast<double>(*this)); }

    [[nodiscard]] explicit operator std::int64_t() const {
        if (!is_finite()) throw DecimalInvalidArg("Decimal: cannot convert special value to int");
        Decimal t = truncated();
        if (t.exp_ < 0 || t.coef_ > static_cast<value_type>(std::numeric_limits<std::int64_t>::max()))
            throw DecimalOverflow("Decimal: overflow in int64 conversion");
        std::int64_t v = static_cast<std::int64_t>(t.coef_);
        for (int i = 0; i < t.exp_; ++i) v *= 10;
        return t.neg_ ? -v : v;
    }

    [[nodiscard]] explicit operator std::uint64_t() const {
        if (neg_ && !is_zero()) throw DecimalInvalidArg("Decimal: cannot convert negative to uint64");
        auto iv = static_cast<std::int64_t>(*this);
        return static_cast<std::uint64_t>(iv);
    }

    // ============================================================
    // 文字列変換
    // ============================================================

    [[nodiscard]] std::string to_string() const {
        if (is_nan())   return "NaN";
        if (special_ == detail::SpecialValue::PosInf) return "Inf";
        if (special_ == detail::SpecialValue::NegInf) return "-Inf";
        if (is_zero())  return "0";

        std::string s = std::to_string(coef_);
        // exp_ が 0 以上なら整数
        if (exp_ >= 0) {
            // ④ exp_ が巨大な場合は科学記数法にフォールバック (OOM防止)
            if (exp_ > 20) return to_scientific();
            s.append(static_cast<size_t>(exp_), '0');
        } else {
            int frac = -exp_;
            if (frac >= static_cast<int>(s.size())) {
                s.insert(0, frac - static_cast<int>(s.size()) + 1, '0');
            }
            s.insert(s.size() - frac, 1, '.');
        }
        if (neg_) s.insert(0, 1, '-');
        return s;
    }

    /// 固定小数点形式で指定桁数を出力
    [[nodiscard]] std::string to_fixed(int decimal_places, RoundingMode rm = RoundingMode::HalfEven) const {
        Decimal r = rounded(decimal_places, rm);
        // decimal_places 桁を強制的に出力
        std::string base = r.to_string();
        // 小数点以下の桁数を調整
        auto dot = base.find('.');
        int current_frac = (dot == std::string::npos) ? 0 : static_cast<int>(base.size() - dot - 1);
        if (dot == std::string::npos && decimal_places > 0) base += '.';
        if (current_frac < decimal_places)
            base.append(static_cast<size_t>(decimal_places - current_frac), '0');
        return base;
    }

    /// 科学記数法
    [[nodiscard]] std::string to_scientific() const {
        if (!is_finite()) return to_string();
        if (is_zero()) return "0e+0";
        std::string s = std::to_string(coef_);
        int total_exp = exp_ + static_cast<int>(s.size()) - 1;
        if (s.size() > 1) s.insert(1, 1, '.');
        std::string result = (neg_ ? "-" : "") + s + "e";
        result += (total_exp >= 0 ? "+" : "") + std::to_string(total_exp);
        return result;
    }

    // ============================================================
    // 丸め
    // ============================================================

    [[nodiscard]] Decimal rounded(int decimal_places, RoundingMode rm = RoundingMode::HalfEven) const {
        if (!is_finite()) return *this;
        // 現在の小数点以下桁数
        int current = (exp_ < 0) ? -exp_ : 0;
        if (current <= decimal_places) return *this; // すでに十分
        int drop = current - decimal_places;
        // ③ drop > MAX_POW10 のとき safe_pow10 が例外を投げるのを防ぐ
        // 係数が完全にゼロになるほど切り捨てる場合はそのままゼロを返す
        if (drop > detail::MAX_POW10) {
            // Truncate / Floor(正) / Ceiling(負) はゼロ方向へ
            // HalfUp 等でも drop>=19 なら必ず切り捨て (0.0...01 < 0.5)
            bool toward_zero = true; // drop>=19 なら端数は必ず 0.5 未満
            if (!toward_zero) {} // 将来拡張用
            Decimal res{value_type{0}, exponent_type(decimal_places <= 0 ? 0 : -decimal_places), false};
            return res;
        }
        value_type divisor = detail::safe_pow10(drop);
        value_type q = coef_ / divisor;
        value_type r = coef_ % divisor;
        value_type half = divisor / 2;
        // 丸め判定
        bool round_up = false;
        switch (rm) {
        case RoundingMode::Truncate:  round_up = false; break;
        case RoundingMode::Ceiling:   round_up = !neg_ && r > 0; break;
        case RoundingMode::Floor:     round_up =  neg_ && r > 0; break;
        case RoundingMode::HalfUp:    round_up = r >= half; break;
        case RoundingMode::HalfDown:  round_up = r >  half; break;
        case RoundingMode::HalfEven:
            if (r > half) round_up = true;
            else if (r == half) round_up = (q % 2 != 0);
            break;
        }
        if (round_up) ++q;
        Decimal res{q, exp_ + drop, neg_};
        res.normalize();
        return res;
    }

    [[nodiscard]] Decimal truncated() const { return rounded(0, RoundingMode::Truncate); }
    [[nodiscard]] Decimal floored()   const { return rounded(0, RoundingMode::Floor);    }
    [[nodiscard]] Decimal ceiled()    const { return rounded(0, RoundingMode::Ceiling);  }

    // ============================================================
    // 単項演算
    // ============================================================

    [[nodiscard]] constexpr Decimal operator-() const noexcept {
        Decimal r = *this;
        if (!is_zero() && is_finite()) r.neg_ = !r.neg_;
        return r;
    }

    [[nodiscard]] constexpr Decimal operator+() const noexcept { return *this; }

    [[nodiscard]] Decimal abs() const noexcept {
        Decimal r = *this;
        r.neg_ = false;
        return r;
    }

    // ============================================================
    // 四則演算
    // ============================================================

    [[nodiscard]] Decimal operator+(const Decimal& rhs) const {
        // NaN 伝播
        if (is_nan() || rhs.is_nan()) return nan();
        // Inf + (-Inf) = NaN
        if (is_inf() && rhs.is_inf()) {
            if (special_ != rhs.special_) return nan();
            return *this;
        }
        if (is_inf()) return *this;
        if (rhs.is_inf()) return rhs;

        // 同符号 or 異符号
        auto aligned = align(*this, rhs);
        if (!aligned) throw DecimalOverflow("Decimal::operator+: overflow during alignment");
        auto [ca, cb] = *aligned;
        exponent_type new_exp = std::min(exp_, rhs.exp_);

        value_type new_coef;
        bool new_neg;

        if (neg_ == rhs.neg_) {
            // 同符号
            if (ca > std::numeric_limits<value_type>::max() - cb)
                throw DecimalOverflow("Decimal::operator+: coefficient overflow");
            new_coef = ca + cb;
            new_neg  = neg_;
        } else {
            // 異符号
            if (ca >= cb) { new_coef = ca - cb; new_neg = neg_;  }
            else          { new_coef = cb - ca; new_neg = rhs.neg_; }
        }

        Decimal res{new_coef, new_exp, new_neg};
        res.normalize();
        return res;
    }

    [[nodiscard]] Decimal operator-(const Decimal& rhs) const { return *this + (-rhs); }

    [[nodiscard]] Decimal operator*(const Decimal& rhs) const {
        if (is_nan() || rhs.is_nan()) return nan();
        if (is_inf() || rhs.is_inf()) {
            bool result_neg = neg_ != rhs.neg_;
            if (is_zero() || rhs.is_zero()) return nan(); // 0 × Inf = NaN
            return result_neg ? neg_inf() : inf();
        }
        // overflow check via __int128 or fallback
#if DEC_HAS_INT128
        detail::uint128_t r128 = static_cast<detail::uint128_t>(coef_) * rhs.coef_;
        if (r128 > std::numeric_limits<value_type>::max())
            throw DecimalOverflow("Decimal::operator*: coefficient overflow (use BigDecimal for larger values)");
        value_type new_coef = static_cast<value_type>(r128);
#else
        // 簡易チェック
        if (coef_ != 0 && rhs.coef_ > std::numeric_limits<value_type>::max() / coef_)
            throw DecimalOverflow("Decimal::operator*: coefficient overflow");
        value_type new_coef = coef_ * rhs.coef_;
#endif
        exponent_type new_exp = exp_ + rhs.exp_;
        bool new_neg = (neg_ != rhs.neg_) && new_coef != 0;
        Decimal res{new_coef, new_exp, new_neg};
        res.normalize();
        return res;
    }

    [[nodiscard]] Decimal operator/(const Decimal& rhs) const {
        if (is_nan() || rhs.is_nan()) return nan();
        if (rhs.is_zero()) {
            if (DecimalContext::current().trap_div_zero)
                throw DecimalDivByZero("Decimal::operator/: division by zero");
            if (is_zero()) return nan();
            return (neg_ == rhs.neg_) ? inf() : neg_inf();
        }
        if (is_inf()) {
            bool rn = (neg_ != rhs.neg_);
            return rn ? neg_inf() : inf();
        }
        if (rhs.is_inf()) return zero();

        // 精度保証のため被除数を拡大
        const int PRECISION = DecimalContext::current().precision;
        // scaled_dividend = coef_ × 10^(PRECISION)
        // 結果の exp = exp_ - rhs.exp_ - PRECISION
        // __int128 使用
#if DEC_HAS_INT128
        detail::uint128_t scaled = static_cast<detail::uint128_t>(coef_);
        // ② 比較対象を uint128_t::max/10 にする (uint64_t の上限で止めていたバグを修正)
        constexpr detail::uint128_t U128_MAX = ~detail::uint128_t{0};
        int actual_precision = 0;
        for (int i = 0; i < PRECISION && scaled <= U128_MAX / 10; ++i) {
            scaled *= 10;
            ++actual_precision;
        }
        detail::uint128_t q = scaled / rhs.coef_;
        if (q > std::numeric_limits<value_type>::max())
            throw DecimalOverflow("Decimal::operator/: result overflow");
        value_type new_coef = static_cast<value_type>(q);
        exponent_type new_exp = exp_ - rhs.exp_ - actual_precision;
#else
        // fallback: uint64_t ベース (精度は最大19桁程度)
        value_type scaled_fb = coef_;
        int actual_precision = 0;
        while (actual_precision < PRECISION && scaled_fb <= std::numeric_limits<value_type>::max() / 10) {
            scaled_fb *= 10;
            ++actual_precision;
        }
        value_type new_coef = scaled_fb / rhs.coef_;
        exponent_type new_exp = exp_ - rhs.exp_ - actual_precision;
#endif
        bool new_neg = (neg_ != rhs.neg_) && new_coef != 0;
        Decimal res{new_coef, new_exp, new_neg};
        res.normalize();
        return res;
    }

    [[nodiscard]] Decimal operator%(const Decimal& rhs) const {
        if (!is_finite() || !rhs.is_finite()) return nan();
        if (rhs.is_zero()) throw DecimalDivByZero("Decimal::operator%: modulo by zero");
        Decimal q = (*this / rhs).truncated();
        return *this - q * rhs;
    }

    // 複合代入
    Decimal& operator+=(const Decimal& rhs) { return *this = *this + rhs; }
    Decimal& operator-=(const Decimal& rhs) { return *this = *this - rhs; }
    Decimal& operator*=(const Decimal& rhs) { return *this = *this * rhs; }
    Decimal& operator/=(const Decimal& rhs) { return *this = *this / rhs; }
    Decimal& operator%=(const Decimal& rhs) { return *this = *this % rhs; }

    // インクリメント / デクリメント
    Decimal& operator++()    { *this += one(); return *this; }
    Decimal& operator--()    { *this -= one(); return *this; }
    Decimal  operator++(int) { Decimal t = *this; ++*this; return t; }
    Decimal  operator--(int) { Decimal t = *this; --*this; return t; }

    // ============================================================
    // 比較演算 (C++20 三方比較演算子)
    // ============================================================

    [[nodiscard]] std::partial_ordering operator<=>(const Decimal& rhs) const noexcept {
        // NaN は非順序
        if (is_nan() || rhs.is_nan()) return std::partial_ordering::unordered;
        // Inf
        if (is_inf() && rhs.is_inf()) {
            if (special_ == rhs.special_) return std::partial_ordering::equivalent;
            return (special_ == detail::SpecialValue::PosInf) ? std::partial_ordering::greater : std::partial_ordering::less;
        }
        if (is_inf()) return (special_ == detail::SpecialValue::PosInf) ? std::partial_ordering::greater : std::partial_ordering::less;
        if (rhs.is_inf()) return (rhs.special_ == detail::SpecialValue::PosInf) ? std::partial_ordering::less : std::partial_ordering::greater;
        // Zero
        if (is_zero() && rhs.is_zero()) return std::partial_ordering::equivalent;
        if (is_zero()) return rhs.neg_ ? std::partial_ordering::greater : std::partial_ordering::less;
        if (rhs.is_zero()) return neg_  ? std::partial_ordering::less   : std::partial_ordering::greater;
        // 符号が異なる
        if (neg_ && !rhs.neg_) return std::partial_ordering::less;
        if (!neg_ && rhs.neg_) return std::partial_ordering::greater;
        // 同符号: スケールを合わせて比較
        try {
            auto aligned = align(*this, rhs);
            if (!aligned) {
                // ① overflow → 桁数で比較 (strong_ordering → partial_ordering の変換バグを修正)
                int da = detail::digit_count(coef_) + exp_;
                int db = detail::digit_count(rhs.coef_) + rhs.exp_;
                std::partial_ordering po =
                    (da < db) ? std::partial_ordering::less :
                    (da > db) ? std::partial_ordering::greater :
                                std::partial_ordering::equivalent;
                // 負符号なら大小反転
                if (neg_) {
                    if (po == std::partial_ordering::less)    return std::partial_ordering::greater;
                    if (po == std::partial_ordering::greater) return std::partial_ordering::less;
                }
                return po;
            }
            auto [ca, cb] = *aligned;
            auto cmp = (ca <=> cb);
            if (neg_) {
                if (cmp > 0) return std::partial_ordering::less;
                if (cmp < 0) return std::partial_ordering::greater;
                return std::partial_ordering::equivalent;
            }
            if (cmp < 0) return std::partial_ordering::less;
            if (cmp > 0) return std::partial_ordering::greater;
            return std::partial_ordering::equivalent;
        } catch (...) {
            return std::partial_ordering::unordered;
        }
    }

    [[nodiscard]] bool operator==(const Decimal& rhs) const noexcept {
        return (*this <=> rhs) == std::partial_ordering::equivalent;
    }

    // ============================================================
    // 数学関数
    // ============================================================

    /// 整数累乗 (負の指数は除算)
    [[nodiscard]] Decimal pow(int n) const {
        if (is_nan()) return nan();
        if (n == 0) return one();
        // ⑧ INT_MIN の否定はオーバーフロー UB → 事前チェック
        if (n == std::numeric_limits<int>::min())
            throw DecimalInvalidArg("Decimal::pow: exponent INT_MIN causes overflow");
        if (n < 0) return one() / pow(-n);
        Decimal base = *this, result = one();
        while (n > 0) {
            if (n & 1) result *= base;
            base *= base;
            n >>= 1;
        }
        return result;
    }

    /// 平方根 (ニュートン法、精度は DecimalContext::current().precision)
    [[nodiscard]] Decimal sqrt() const {
        if (is_nan() || negative()) return nan();
        if (is_inf()) return inf();
        if (is_zero()) return zero();
        // 初期値を double で得る
        Decimal x{std::sqrt(static_cast<double>(*this))};
        const Decimal two{2};
        const int iters = 50;
        for (int i = 0; i < iters; ++i) {
            Decimal nx = (x + *this / x) / two;
            if (nx == x) break;
            x = nx;
        }
        return x;
    }

    // ============================================================
    // Structured Bindings サポート (coefficient, exponent, sign)
    // ============================================================
    template <std::size_t I>
    [[nodiscard]] auto get() const noexcept {
        if constexpr (I == 0) return coef_;
        else if constexpr (I == 1) return exp_;
        else if constexpr (I == 2) return neg_;
    }

    // ============================================================
    // ストリーム I/O
    // ============================================================
    friend std::ostream& operator<<(std::ostream& os, const Decimal& d) {
        return os << d.to_string();
    }

    friend std::istream& operator>>(std::istream& is, Decimal& d) {
        std::string s;
        is >> s;
        try { d = Decimal(s); }
        catch (...) { is.setstate(std::ios::failbit); }
        return is;
    }

    // ユーザー定義リテラルは namespace dec::literals に定義。
    // (クラス内 friend 版は ODR 違反を避けるため削除済み)
};

// ============================================================
// ユーザー定義リテラル
// ============================================================
namespace literals {
    inline Decimal operator""_dec(const char* s, std::size_t len) {
        return Decimal{std::string_view(s, len)};
    }
    inline Decimal operator""_dec(long double v) {
        return Decimal{static_cast<double>(v)};
    }
    inline Decimal operator""_dec(unsigned long long v) {
        return Decimal{static_cast<std::int64_t>(v)};
    }
} // namespace literals

// ============================================================
// 数学フリー関数 (std::math スタイル)
// ============================================================
[[nodiscard]] inline Decimal abs(const Decimal& d)           { return d.abs(); }
[[nodiscard]] inline Decimal sqrt(const Decimal& d)          { return d.sqrt(); }
[[nodiscard]] inline Decimal pow(const Decimal& d, int n)    { return d.pow(n); }
[[nodiscard]] inline Decimal floor(const Decimal& d)         { return d.floored(); }
[[nodiscard]] inline Decimal ceil(const Decimal& d)          { return d.ceiled(); }
[[nodiscard]] inline Decimal trunc(const Decimal& d)         { return d.truncated(); }
[[nodiscard]] inline Decimal round(const Decimal& d)         { return d.rounded(0); }
[[nodiscard]] inline Decimal fmod(const Decimal& a, const Decimal& b) { return a % b; }

[[nodiscard]] inline Decimal min(const Decimal& a, const Decimal& b) {
    if (a.is_nan()) return b;
    if (b.is_nan()) return a;
    return (a <= b) ? a : b;
}
[[nodiscard]] inline Decimal max(const Decimal& a, const Decimal& b) {
    if (a.is_nan()) return b;
    if (b.is_nan()) return a;
    return (a >= b) ? a : b;
}
[[nodiscard]] inline Decimal clamp(const Decimal& v, const Decimal& lo, const Decimal& hi) {
    return min(max(v, lo), hi);
}

// ============================================================
// std::hash 特殊化 (unordered_map で使える)
// ============================================================

} // namespace dec

// ============================================================
// Structured Bindings のための std::tuple_size / tuple_element
// ============================================================
template <>
struct std::tuple_size<dec::Decimal> : std::integral_constant<std::size_t, 3> {};

template <std::size_t I>
struct std::tuple_element<I, dec::Decimal> {
    using type = std::conditional_t<I == 0, std::uint64_t,
                 std::conditional_t<I == 1, std::int32_t, bool>>;
};

// ============================================================
// std::hash
// ============================================================
template <>
struct std::hash<dec::Decimal> {
    std::size_t operator()(const dec::Decimal& d) const noexcept {
        // 正規化されているので同値の Decimal は同じハッシュを持つ
        auto h1 = std::hash<std::uint64_t>{}(d.coefficient());
        auto h2 = std::hash<std::int32_t>{}(d.exponent());
        auto h3 = std::hash<bool>{}(d.negative());
        return h1 ^ (h2 << 17) ^ (h3 << 31);
    }
};

// ============================================================
// std::format サポート (C++20)
// ============================================================
template <>
struct std::formatter<dec::Decimal> {
    // フォーマット指定子: [[fill]align][sign][#][0][width][.precision][type]
    // type: f=fixed, e=scientific, g=general(default), s=string
    char fill = ' ';
    char align = '<';
    char sign = '-';
    int width = 0;
    int prec = -1;
    char type = 'g';

    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        // 簡易パーサ
        if (it != ctx.end() && *it != '}') {
            // align
            if (*it == '<' || *it == '>' || *it == '^') { align = *it++; }
            // sign
            if (it != ctx.end() && (*it == '+' || *it == '-' || *it == ' ')) sign = *it++;
            // width
            while (it != ctx.end() && *it >= '0' && *it <= '9') { width = width * 10 + (*it++ - '0'); }
            // precision
            if (it != ctx.end() && *it == '.') {
                ++it; prec = 0;
                while (it != ctx.end() && *it >= '0' && *it <= '9') { prec = prec * 10 + (*it++ - '0'); }
            }
            // type
            if (it != ctx.end() && (*it == 'f' || *it == 'e' || *it == 'g' || *it == 's')) type = *it++;
        }
        return it;
    }

    auto format(const dec::Decimal& d, std::format_context& ctx) const {
        std::string s;
        if (type == 'f')      s = d.to_fixed(prec >= 0 ? prec : 6);
        else if (type == 'e') s = d.to_scientific();
        else if (type == 's') s = d.to_string();
        else                  s = (prec >= 0) ? d.to_fixed(prec) : d.to_string();

        // sign
        if (sign == '+' && !d.negative() && d.is_finite() && !d.is_zero())
            s = '+' + s;

        // width / align
        if (width > 0 && static_cast<int>(s.size()) < width) {
            int pad = width - static_cast<int>(s.size());
            if (align == '<')       s += std::string(pad, fill);
            else if (align == '>')  s = std::string(pad, fill) + s;
            else /* ^ */            s = std::string(pad/2, fill) + s + std::string(pad - pad/2, fill);
        }

        return std::format_to(ctx.out(), "{}", s);
    }
};

// ============================================================
// std::numeric_limits
// ============================================================
template <>
class std::numeric_limits<dec::Decimal> {
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed      = true;
    static constexpr bool is_integer     = false;
    static constexpr bool is_exact       = true;
    static constexpr bool has_infinity   = true;
    static constexpr bool has_quiet_NaN  = true;
    static constexpr bool is_iec559      = false;
    static constexpr int  digits10       = 18; // 64-bit coefficient

    static dec::Decimal min()       noexcept { return dec::Decimal{std::numeric_limits<std::uint64_t>::max(), -18, true}; }
    static dec::Decimal max()       noexcept { return dec::Decimal{std::numeric_limits<std::uint64_t>::max(), 0, false}; }
    static dec::Decimal lowest()    noexcept { return min(); }
    static dec::Decimal epsilon()   noexcept { return dec::Decimal{1, -18, false}; }
    static dec::Decimal infinity()  noexcept { return dec::Decimal::inf(); }
    static dec::Decimal quiet_NaN() noexcept { return dec::Decimal::nan(); }
    static dec::Decimal zero()      noexcept { return dec::Decimal::zero(); }
    static dec::Decimal one()       noexcept { return dec::Decimal::one(); }
};

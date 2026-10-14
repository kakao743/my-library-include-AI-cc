#include <iostream>
#include <vector>
#include <array>
#include <source_location>
#include <type_traits>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <cstdlib>

namespace dbg {
    struct debug_context {
        std::source_location loc;
        std::size_t depth = 0;
        std::array<std::size_t, 16> sizes{};
        std::array<std::ptrdiff_t, 16> accesses{};
    };

    [[noreturn]] inline void report_error(const debug_context& ctx) {
        std::ostringstream ss;
        ss << "\n[BOUNDS ERROR] " << ctx.loc.file_name() << ":(" << ctx.loc.line() << ":" << ctx.loc.column() << "):" << ctx.loc.function_name() << "\n";
        ss << "out of range: dp size is (";
        for (std::size_t i = 0; i < ctx.depth; ++i) {
            ss << ctx.sizes[i] << (i + 1 == ctx.depth ? "" : ", ");
        }
        ss << ") but your access is (";
        for (std::size_t i = 0; i < ctx.depth; ++i) {
            ss << ctx.accesses[i] << (i + 1 == ctx.depth ? "" : ", ");
        }
        ss << ")\n";

        std::cerr << ss.str() << std::endl; 
        std::abort();                       
    }

    template<class T, class Allocator> class vector;

    template<class T> struct is_vector_impl : std::false_type {};
    template<class U, class A> struct is_vector_impl<std::vector<U, A>> : std::true_type {};
    template<class U, class A> struct is_vector_impl<vector<U, A>> : std::true_type {};

    template<class T>
    struct is_vector_type : is_vector_impl<std::remove_cvref_t<T>> {};

    template <class T>
    inline constexpr bool is_vector_type_v = is_vector_type<T>::value;

    // ① 左辺値（通常の配列）用プロキシ
    template<class VectorType>
    class vector_access_proxy {
        VectorType& vec;
        debug_context ctx;

    public:
        vector_access_proxy(VectorType& vec_, debug_context ctx_) 
            : vec(vec_), ctx(ctx_) {}

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub) {
            if (ctx.depth >= 16) report_error(ctx);

            ctx.sizes[ctx.depth] = vec.size();
            ctx.accesses[ctx.depth] = sub;
            ctx.depth++;

            if (sub < 0 || static_cast<std::size_t>(sub) >= vec.size()) {
                report_error(ctx);
            }

            using value_type = typename VectorType::value_type;
            
            if constexpr (is_vector_type_v<value_type>) {
                return vector_access_proxy<value_type>(vec[sub], ctx);
            } else {
                return vec[sub]; 
            }
        }
    };

    // ② const左辺値用プロキシ
    template<class VectorType>
    class const_vector_access_proxy {
        const VectorType& vec;
        debug_context ctx;

    public:
        const_vector_access_proxy(const VectorType& vec_, debug_context ctx_) 
            : vec(vec_), ctx(ctx_) {}

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub) const {
            if (ctx.depth >= 16) report_error(ctx);

            auto mutable_ctx = ctx;
            mutable_ctx.sizes[mutable_ctx.depth] = vec.size();
            mutable_ctx.accesses[mutable_ctx.depth] = sub;
            mutable_ctx.depth++;

            if (sub < 0 || static_cast<std::size_t>(sub) >= vec.size()) {
                report_error(mutable_ctx);
            }

            using value_type = typename VectorType::value_type;
            if constexpr (is_vector_type_v<value_type>) {
                return const_vector_access_proxy<value_type>(vec[sub], mutable_ctx);
            } else {
                return vec[sub]; 
            }
        }
    };

    // ③ 右辺値（一時オブジェクト）専用プロキシ
    template<class VectorType>
    class rvalue_vector_access_proxy {
        VectorType vec; 
        debug_context ctx;

    public:
        rvalue_vector_access_proxy(VectorType&& vec_, debug_context ctx_) 
            : vec(std::move(vec_)), ctx(ctx_) {}

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub) && {
            if (ctx.depth >= 16) report_error(ctx);

            ctx.sizes[ctx.depth] = vec.size();
            ctx.accesses[ctx.depth] = sub;
            ctx.depth++;

            if (sub < 0 || static_cast<std::size_t>(sub) >= vec.size()) {
                report_error(ctx);
            }

            using value_type = typename VectorType::value_type;
            if constexpr (is_vector_type_v<value_type>) {
                return rvalue_vector_access_proxy<value_type>(std::move(vec[sub]), ctx);
            } else {
                // ★【ここだけ修正】
                // もし最深層が vector<bool> だった場合、プロキシクラス（std::vector<bool>::reference）を「値」としてコピーして返す。
                // これにより、消え去る予定の vec からビット情報を安全に救出して呼び出し元に届けます。
                if constexpr (std::is_same_v<value_type, bool>) {
                    return vec[sub]; // decltype(auto) により参照ではなく「値」が返ります
                } else {
                    return std::move(vec[sub]); // 通常の型はこれまで通りムーブ
                }
            }
        }
    };

    // --- 本体のラッパー ---
    template<class T, class Allocator = std::allocator<T>>
    class vector final : public std::vector<T, Allocator> {
        using base_type = std::vector<T, Allocator>;

    public:
        using base_type::base_type; 

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub, std::source_location loc = std::source_location::current()) & {
            debug_context ctx{ .loc = loc, .depth = 1 };
            ctx.sizes[0] = this->size();
            ctx.accesses[0] = sub;
            
            if (sub < 0 || static_cast<std::size_t>(sub) >= this->size()) {
                report_error(ctx);
            }

            if constexpr (is_vector_type_v<T>) {
                return vector_access_proxy<T>((*this).base_type::operator[](sub), ctx);
            } else {
                return (*this).base_type::operator[](sub); 
            }
        }

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub, std::source_location loc = std::source_location::current()) const & {
            debug_context ctx{ .loc = loc, .depth = 1 };
            ctx.sizes[0] = this->size();
            ctx.accesses[0] = sub;

            if (sub < 0 || static_cast<std::size_t>(sub) >= this->size()) {
                report_error(ctx);
            }

            if constexpr (is_vector_type_v<T>) {
                return const_vector_access_proxy<T>((*this).base_type::operator[](sub), ctx);
            } else {
                return (*this).base_type::operator[](sub);
            }
        }

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub, std::source_location loc = std::source_location::current()) && {
            debug_context ctx{ .loc = loc, .depth = 1 };
            ctx.sizes[0] = this->size();
            ctx.accesses[0] = sub;

            if (sub < 0 || static_cast<std::size_t>(sub) >= this->size()) {
                report_error(ctx);
            }

            if constexpr (is_vector_type_v<T>) {
                return rvalue_vector_access_proxy<T>(std::move((*this).base_type::operator[](sub)), ctx);
            } else {
                // 本体（1次元目）が vector<bool> だった場合の配慮
                if constexpr (std::is_same_v<T, bool>) {
                    return (*this).base_type::operator[](sub);
                } else {
                    return std::move((*this).base_type::operator[](sub));
                }
            }
        }

        [[nodiscard]] constexpr decltype(auto) at(std::ptrdiff_t sub, std::source_location loc = std::source_location::current()) {
            return (*this).operator[](sub, loc);
        }
        [[nodiscard]] constexpr decltype(auto) at(std::ptrdiff_t sub, std::source_location loc = std::source_location::current()) const {
            return (*this).operator[](sub, loc);
        }
    };
}

namespace std {
    namespace dbg = ::dbg;
}
#define vector dbg::vector

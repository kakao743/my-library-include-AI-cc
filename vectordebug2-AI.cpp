#include <iostream>
#include <vector>
#include <array>
#include <source_location>
#include <type_traits>
#include <format>  
#include <cstdlib>
#include <utility>
#include <string_view>

namespace dbg {
    struct debug_context {
        std::source_location loc;
        std::string_view name;
        std::size_t depth = 0;
        std::array<std::size_t, 16> sizes{};
        std::array<std::ptrdiff_t, 16> accesses{};
    };

    [[noreturn]] inline void report_error(const debug_context& ctx) {
        std::string msg = std::format(
            "\n[BOUNDS ERROR] {}:({}:{}):{}\n",
            ctx.loc.file_name(), ctx.loc.line(), ctx.loc.column(), ctx.loc.function_name()
        );

        std::string sizes_str, accesses_str;
        for (std::size_t i = 0; i < ctx.depth; ++i) {
            sizes_str += std::format("{}{}", ctx.sizes[i], (i + 1 == ctx.depth ? "" : ", "));
            accesses_str += std::format("{}{}", ctx.accesses[i], (i + 1 == ctx.depth ? "" : ", "));
        }

        msg += std::format("out of range: '{}' size is ({}) but your access is (崩壊値: {})\n", 
                          ctx.name, sizes_str, accesses_str);

        std::cerr << msg << std::flush; 
        std::abort();
    }

    template<class T> struct is_vector_impl : std::false_type {};
    template<class U, class A> struct is_vector_impl<std::vector<U, A>> : std::true_type {};
    template<class T> inline constexpr bool is_vector_v = is_vector_impl<std::remove_cvref_t<T>>::value;

    template <typename RawContainer, bool IsConst>
    class bounds_view {
        using container_ref_type = std::conditional_t<IsConst, const RawContainer&, RawContainer&>;
        
        container_ref_type container; 
        debug_context ctx;

    public:
        constexpr bounds_view(container_ref_type c, debug_context cx) 
            : container(c), ctx(cx) {}

        [[nodiscard]] constexpr decltype(auto) operator[](std::ptrdiff_t sub) {
            if (ctx.depth >= 16) report_error(ctx);

            if (sub < 0 || static_cast<std::size_t>(sub) >= container.size()) {
                ctx.sizes[ctx.depth] = container.size();
                ctx.accesses[ctx.depth] = sub;
                ctx.depth++;
                report_error(ctx);
            }

            ctx.sizes[ctx.depth] = container.size();
            ctx.accesses[ctx.depth] = sub;
            ctx.depth++;

            // ここで container[sub] の生の戻り値型を完全に維持する
            decltype(auto) element = container[sub];
            using element_type = decltype(container[sub]); // 元の operator[] の正確な返り値型
            using next_raw_type = std::remove_cvref_t<element_type>;

            if constexpr (is_vector_v<next_raw_type>) {
                return bounds_view<next_raw_type, IsConst || std::is_const_v<std::remove_reference_t<element_type>>>(element, ctx);
            } else {
                // 4. 【バグ修正】 型ベースによる完璧な最終フォワード
                // 返り値が参照型（T&）なら参照のまま返し、std::vector<bool>::reference のような
                // 一時オブジェクト（値）なら、右辺値参照の罠をバイパスして「値」として完全に安全に転送する！
                return static_cast<element_type>(element);
            }
        }
    };

    template <typename Container>
    [[nodiscard]] constexpr decltype(auto) make_view(Container& c, std::string_view name, std::source_location loc) {
        using raw_type = std::remove_cvref_t<Container>;
        debug_context ctx{ .loc = loc, .name = name, .depth = 0 };
        return bounds_view<raw_type, std::is_const_v<Container>>(c, ctx);
    }

    template <typename Container>
    void make_view(Container&& c, std::string_view name, std::source_location loc) = delete;
}

#define dbg_verify(grid) (::dbg::make_view((grid), #grid, std::source_location::current()))

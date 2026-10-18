//
// Created by griffith on 18/10/2026.
//

#pragma once

#include <rasi/support/arena.hh>
#include <rasi/ir/function.hh>

namespace rasi::ir {
    //! @brief A module which owns the arena and interned strings.
    class IRModule {
    public:
        explicit IRModule(const std::size_t arena_size = support::Arena::default_slab_size)
            : m_arena{ arena_size }
        {}

        IRModule(IRModule&&) noexcept            = default;
        IRModule& operator=(IRModule&&) noexcept = default;

        IRModule(const IRModule&)            = delete;
        IRModule& operator=(const IRModule&) = delete;

        [[nodiscard]] FunctionRef declare(
            const std::string_view        name,
            const Type                    ret,
            const std::span<const Type>   params = {}
        ) {
            auto* fn             = m_arena.alloc<IRFunction>();
            const auto id        = static_cast<uint32_t>(m_functions.size());
            fn->arena            = &m_arena;
            fn->self             = FunctionRef{ id };
            fn->name             = intern(name).first;
            fn->sig.ret          = ret;
            fn->sig.params_count = static_cast<uint16_t>(params.size());
            m_functions.push_back(fn);
            return FunctionRef{ id };
        }

        [[nodiscard]] FunctionRef declare(
            const std::string_view                  name,
            const Type                              ret,
            const std::initializer_list<const Type> params = {}
        ) {
            auto* fn             = m_arena.alloc<IRFunction>();
            const auto id        = static_cast<uint32_t>(m_functions.size());
            fn->arena            = &m_arena;
            fn->self             = FunctionRef{ id };
            fn->name             = intern(name).first;
            fn->sig.ret          = ret;
            fn->sig.params_count = static_cast<uint16_t>(params.size());
            m_functions.push_back(fn);
            return FunctionRef{ id };
        }

        //! @brief Intern a string; returns a StringRef + view pair.
        [[nodiscard]] std::pair<StringRef, std::string_view> intern(const std::string_view sv) {
            auto view = m_arena.alloc_str(sv);
            const auto id = static_cast<uint32_t>(m_strings.size());
            m_strings.push_back(view);
            return { StringRef{ id }, view };
        }

        //! @brief Resolve a StringRef to its string_view.
        [[nodiscard]] std::string_view string(StringRef ref) const noexcept {
            return m_strings[ref.id];
        }

        [[nodiscard]] IRFunction* get(const FunctionRef ref) noexcept {
            return m_functions[ref.id];
        }

        [[nodiscard]] const IRFunction* get(const FunctionRef ref) const noexcept {
            return m_functions[ref.id];
        }

        [[nodiscard]] std::span<IRFunction* const> functions() const noexcept {
            return m_functions;
        }

        [[nodiscard]] support::Arena& arena() noexcept { return m_arena; }
    private:
        support::Arena                  m_arena;
        std::vector<IRFunction*>        m_functions;
        std::vector<std::string_view>   m_strings;
        std::size_t                     m_string_count{};
    };
}

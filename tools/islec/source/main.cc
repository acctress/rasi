#include "checker.hh"
#include "dec_tree.hh"
#include "emitter.hh"
#include "parser.hh"

#include <exception>
#include <iostream>
#include <string_view>
#include <vector>
#include <print>

int main()
{
    constexpr std::string_view source = R"(
(type Value (primitive))

(decl extractor iadd  (Value Value) Value)
(decl extractor isub  (Value Value) Value)
(decl extractor imul  (Value Value) Value)
(decl extractor idiv  (Value Value) Value)
(decl extractor neg  (Value) Value)
(decl extractor load (Value) Value)

(decl constructor emit_add  (Value Value) Value)
(decl constructor emit_sub  (Value Value) Value)
(decl constructor emit_mul  (Value Value) Value)
(decl constructor emit_div  (Value Value) Value)
(decl constructor emit_neg  (Value) Value)
(decl constructor emit_load (Value) Value)

(rule 100 (iadd lhs rhs)
          (emit_add lhs rhs))

(rule 90 (isub lhs rhs)
         (emit_sub lhs rhs))

(rule 80 (imul lhs rhs)
         (emit_mul lhs rhs))

(rule 70 (idiv lhs rhs)
         (emit_div lhs rhs))

(rule 60 (neg value)
         (emit_neg value))

(rule 50 (load address)
         (emit_load address))
)";

    std::println("\x1b[38;5;217mbasic_rule_test.isle:\x1b[0m\n{}", source);
    std::println("\x1b[38;5;217mgenerated matches:\x1b[0m");

    try
    {
        parser p { source };
        const auto ast = p.parse();

        checker ch { ast };
        ch.check();

        std::vector<const rule_node*> rules;

        for (const auto& node : ast)
        {
            if (const auto* rule = std::get_if<rule_node>(&node))
                rules.push_back(rule);
        }

        const auto root = dec_tree::build(std::move(rules));

        emitter out { root, std::cout };
        out.emit();
    }
    catch (const std::exception& error)
    {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
#include <xiao/ir/builder.hh>
#include <xiao/ir/verifier.hh>
#include <xiao/ir/module.hh>
#include <print>

#include "xiao/isa/x86/abi.hh"
#include "xiao/isa/x86/lower.hh"

using namespace xiao::ir;

int main() {
    IRModule module;

    const auto ref = module.declare("add", Type::i64, {Type::i64, Type::i64});
    IRBuilder b{ *module.get(ref) };

    const auto entry = b.add_block({Type::i64, Type::i64});
    b.set_block(entry);

    auto x = b.param(0);
    auto y = b.param(1);
    auto z = b.iadd(x, y);
    b.ret(z);
    b.finalise();

    IRVerifier verifier;
    verifier.visit_module(module);
    if (!verifier.ok())
        for (const auto& [msg] : verifier.errs) std::println("\x1b[38;5;198m[error]\x1b[0m {}", msg);

    std::println("ok");

    xiao::codegen::MachineEnv mach_env {};
    xiao::x86_64::X86ABISpec abi_spec {};
    xiao::x86_64::IRLowerer lowerer { mach_env, abi_spec };

    auto fn = module.get(ref);
    auto vcode = lowerer.lower(*fn, xiao::codegen::CallConv::SystemV);


    std::println("~~ ir function ~~\n{}", *fn);

    std::println("~~ vcode ~~");
    for (const auto& i : vcode.insts)
        std::println("{}", i.to_string());
}
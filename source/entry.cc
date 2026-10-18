#include <rasi/ir/builder.hh>
#include <print>

#include "rasi/ir/module.hh"

using namespace rasi::ir;

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

}
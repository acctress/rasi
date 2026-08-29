#pragma once

#include <rasi/asm/x86_64/assembler.hpp>
#include <rasi/regalloc/regalloc.hh>
#include <rasi/vcode/vcode.hh>
#include <vector>

namespace rasi::azm::x86_64
{
    [[nodiscard]] x86::Buffer emit( const VCode& vcode, const regalloc::AllocContext& ctx );
}
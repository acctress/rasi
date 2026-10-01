#pragma once

#include <rasi/vcode/vcode.hh>
#include <rasi/regalloc/regalloc.hh>
#include <ostream>

namespace rasi::vcode
{
    void print_vcode( std::ostream &os, const VCode &vcode, const regalloc::AllocContext &ctx );
}
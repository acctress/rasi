#pragma once

#include <rasi/asm/vcode.hh>
#include <rasi/codegen/x86_64_assembler.hpp>
#include <rasi/regalloc/linear_scan.hh>
#include <rasi/support/buffer.hh>

#include <unordered_map>

namespace rasi::codegen
{
    using namespace rasi::asm_;
    using namespace rasi::regalloc;

    class CodeGen
    {
    public:
        void emit ( const VCode& vcode, const std::unordered_map< u32, AllocResult > &alloc, Buffer& buf );
    private:
        static void emit_prologue( Buffer &buf );
        static void emit_epilogue( Buffer &buf );

        static x86_64::Reg resolve( VReg vreg, const std::unordered_map< u32, AllocResult > &alloc );
    };
} // namespace rasi::codegen
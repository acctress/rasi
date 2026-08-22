#pragma once

#include <rasi/asm/vcode.hh>
#include <rasi/ir/function.hh>

namespace rasi::isle
{
    using namespace rasi::asm_;

    class IsleContext
    {
    public:
        explicit IsleContext( Function &fn, VCode &code ) : m_fn( fn ), m_vcode( code ) { }

        VReg arg( const Inst& inst, std::size_t idx ) const;

        VReg emit_iadd( VReg lhs, VReg rhs ) const;
        VReg emit_isub( VReg lhs, VReg rhs ) const;
        VReg emit_imul( VReg lhs, VReg rhs ) const;
        VReg emit_isub( VReg lhs, VReg rhs );
        VReg emit_imul( VReg lhs, VReg rhs );
        void emit_ret( VReg val ) const;

    private:
        Function &m_fn;
        VCode    &m_vcode;
    };
} // namespace rasi::isle
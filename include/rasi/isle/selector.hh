#pragma once

#include <rasi/asm/vcode.hh>
#include <rasi/ir/function.hh>
#include <unordered_map>

namespace rasi::isle
{
    using namespace rasi::asm_;

    class Selector
    {
    public:
        explicit Selector( Function &fn, VCode &code ) : m_fn( fn ), m_vcode( code )
        {
            m_vcode.call_conv = fn.call_conv;
        }

        void select( const Function &fn );
        void lower_args( const Function &fn );

        [[nodiscard]] VReg arg( const Inst &inst, std::size_t idx ) const;

        VReg lower_iadd( VReg lhs, VReg rhs ) const;
        VReg lower_isub( VReg lhs, VReg rhs ) const;
        VReg lower_imul( VReg lhs, VReg rhs ) const;
        void lower_ret( VReg val ) const;

    private:
        Function                       &m_fn;
        VCode                          &m_vcode;
        std::unordered_map< u32, VReg > m_value_map;
    };
} // namespace rasi::isle
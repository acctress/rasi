#pragma once
#include "rasi/asm/operand.hh"
#include "rasi/asm/vcode.hh"

#include <unordered_map>
#include <vector>

using namespace rasi::asm_;

namespace rasi::regalloc
{
    struct LiveInterval
    {
        VReg    reg;
        PhysReg assigned;
        u32     start;
        u32     end;
    };

    class LinearScan
    {
    public:
        std::unordered_map< u32, PhysReg > run( const VCode &vcode );

    private:
        std::vector< LiveInterval >        m_intervals;
        std::unordered_map< u32, PhysReg > m_alloc;

        void build_intervals( const VCode &vcode );
        void alloc( );
    };

} // namespace rasi::regalloc
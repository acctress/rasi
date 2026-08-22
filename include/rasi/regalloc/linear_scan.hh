#pragma once
#include "rasi/asm/operand.hh"
#include "rasi/asm/vcode.hh"

#include <unordered_map>
#include <variant>
#include <vector>

using namespace rasi::asm_;

namespace rasi::regalloc
{
    using AllocResult = std::variant< PhysReg, SpillSlot >;

    struct LiveInterval
    {
        VReg        reg;
        u32         start;
        u32         end;
        AllocResult assign;
    };

    class LinearScan
    {
    public:
        std::unordered_map< u32, AllocResult > run( const VCode &vcode );

    private:
        std::vector< LiveInterval > m_intervals;
        u32                         m_nx_spill_offset {};

        void build_intervals( const VCode &vcode );
        void alloc( );
    };

} // namespace rasi::regalloc
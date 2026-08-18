#pragma once

#include "instruction.hh"
#include "rasi/support/types.hh"

namespace rasi
{
    /// @brief A linear sequence of instructions, final instruction must be a terminator.
    struct BasicBlock
    {
        BlockRef ref{};
        u32      instructions_offset{};
        u16      instructions_count{};
        u32      params_offset{};
        u16      params_count{};
    };
}
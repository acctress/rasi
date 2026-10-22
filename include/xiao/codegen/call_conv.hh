#pragma once

#include <cstdint>

namespace xiao::codegen {
    //! @brief Calling convention tag
    enum class CallConv : uint8_t {
        SystemV,
        WindowsFastcall
    };
}
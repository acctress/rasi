#pragma once

#include <cstdint>

namespace rasi::codegen {
    //! @brief Calling convention tag
    enum class CallConv : uint8_t {
        SystemV,
        WindowsFastcall
    };
}
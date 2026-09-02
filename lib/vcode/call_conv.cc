#include <rasi/vcode/call_conv.hh>

using namespace rasi;
using namespace rasi::registers;

static constexpr u8 sys_v_allocatable[] = {
    RAX,  RCX,  RDX,  RSI,  RDI,  R8,   R9,   R10,  R11,  RBX,   R12,   R13,   R14,   R15,   XMM0,
    XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
};

static constexpr u8 sys_v_args_int[]   = { RDI, RSI, RDX, RCX, R8, R9 };
static constexpr u8 sys_v_args_float[] = { XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7 };
static constexpr u8 sys_v_ret_int[]    = { RAX, RDX };
static constexpr u8 sys_v_ret_float[]  = { XMM0, XMM1 };
static constexpr u8 sys_v_callee[]     = { RBX, RBP, R12, R13, R14, R15 };

static constexpr u8 win64_allocatable[] = {
    RAX,  RCX,  RDX,  R8,   R9,   R10,  R11,  RBX,  RDI,  RSI,   R12,   R13,   R14,   R15,   XMM0,
    XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
};

static constexpr u8 win64_args_int[]   = { RCX, RDX, R8, R9 };
static constexpr u8 win64_args_float[] = { XMM0, XMM1, XMM2, XMM3 };
static constexpr u8 win64_ret_int[]    = { RAX };
static constexpr u8 win64_ret_float[]  = { XMM0 };
static constexpr u8 win64_callee[]
    = { RBX, RBP, RDI, RSI, R12, R13, R14, R15, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15 };

static constexpr auto make_reg_set = []( const auto &a ) -> RegSet
{
    return { a, static_cast< u8 >( std::size( a ) ) };
};

static constexpr CallConvRegs sys_v_regs {
    .allocatable  = make_reg_set( sys_v_allocatable ),
    .args_int     = make_reg_set( sys_v_args_int ),
    .args_float   = make_reg_set( sys_v_args_float ),
    .ret_int      = make_reg_set( sys_v_ret_int ),
    .ret_float    = make_reg_set( sys_v_ret_float ),
    .callee_saved = make_reg_set( sys_v_callee ),
};

static constexpr CallConvRegs win64_regs {
    .allocatable  = make_reg_set( win64_allocatable ),
    .args_int     = make_reg_set( win64_args_int ),
    .args_float   = make_reg_set( win64_args_float ),
    .ret_int      = make_reg_set( win64_ret_int ),
    .ret_float    = make_reg_set( win64_ret_float ),
    .callee_saved = make_reg_set( win64_callee ),
};

const CallConvRegs &rasi::conv_regs( const CallConv conv ) noexcept
{
    switch ( conv )
    {
        case CallConv::sys_v : return sys_v_regs;
        case CallConv::win_fastcall : return win64_regs;
        case CallConv::native :
        default :
#if defined( _WIN32 )
            return win64_regs;
#else
            return sys_v_regs;
#endif
    }
}

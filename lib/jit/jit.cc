#include <rasi/jit/jit.hh>

using namespace rasi::jit;
using namespace rasi::isle;

void *JIT::compile( Function &fn )
{
    VCode vcode( m_arena );
    Selector selector( fn, vcode );
    
}

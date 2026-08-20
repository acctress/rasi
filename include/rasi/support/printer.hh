#pragma once

#include "rasi/ir/module.hh"
#include <ostream>

namespace rasi
{
    class IRPrinter
    {
    public:
        explicit IRPrinter( std::ostream& os ) : m_stream( os ) { }

        void print( const Module& module );
    private:
        std::ostream& m_stream;

        void print_function     ( const Function& function ) const;
        void print_block        ( const Function &function, const BasicBlock &block ) const;
        void print_instruction  ( const Function &function, const Inst &inst ) const;

        static std::string_view type_str( Type type );
    };
}
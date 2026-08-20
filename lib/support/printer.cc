#include <rasi/support/printer.hh>

using namespace rasi;

void IRPrinter::print( const Module &module )
{
    for ( auto& func : module.functions.as_span( ) )
        print_function( func );
}

void IRPrinter::print_function( const Function &function ) const
{
    m_stream << "fn @" << function.name;
    m_stream << "(";

    for ( auto idx { 0uz }; idx < function.param_types.size( ); ++idx )
    {
        if ( idx > 0 ) m_stream << ", ";
        m_stream << type_str( function.param_types[idx] ) << " %" << idx;
    }

    m_stream << ") -> " << type_str( function.return_type) << " {\n";

    for ( auto& block : function.blocks.as_span( ) )
        print_block( function, block );

    m_stream << "}\n\n";
}

void IRPrinter::print_block( const Function& function, const BasicBlock &block ) const
{
    m_stream << "bb" << block.ref.id << ":\n";
    for ( auto i { block.instructions_offset }; i < block.instructions_offset + block.instructions_count; ++i )
        print_instruction( function, function.instructions[ i ] );
}

void IRPrinter::print_instruction( const Function &function, const Inst &inst ) const
{
    const bool has_result = inst.kind != InstKind::ret && inst.kind != InstKind::br && inst.kind != InstKind::cbr;

    m_stream << "    ";
    if ( has_result ) m_stream << "%" << inst.result.id << " = ";

    switch ( inst.kind )
    {
        case InstKind::iadd:  m_stream << "iadd"; break;
        case InstKind::isub:  m_stream << "isub"; break;
        case InstKind::imul:  m_stream << "imul"; break;
        case InstKind::sdiv:  m_stream << "sdiv"; break;
        case InstKind::udiv:  m_stream << "udiv"; break;
        case InstKind::srem:  m_stream << "srem"; break;
        case InstKind::urem:  m_stream << "urem"; break;
        case InstKind::fadd:  m_stream << "fadd"; break;
        case InstKind::fsub:  m_stream << "fsub"; break;
        case InstKind::fmul:  m_stream << "fmul"; break;
        case InstKind::fdiv:  m_stream << "fdiv"; break;
        case InstKind::and_:  m_stream << "and"; break;
        case InstKind::or_:   m_stream << "or"; break;
        case InstKind::xor_:  m_stream << "xor"; break;
        case InstKind::shl:   m_stream << "shl"; break;
        case InstKind::shr:   m_stream << "shr"; break;
        case InstKind::sar:   m_stream << "sar"; break;
        case InstKind::icmp:  m_stream << "icmp"; break;
        case InstKind::fcmp:  m_stream << "fcmp"; break;
        case InstKind::load:  m_stream << "load"; break;
        case InstKind::store: m_stream << "store"; break;
        case InstKind::alloca_: m_stream << "alloca"; break;
        case InstKind::call:  m_stream << "call"; break;
        case InstKind::ret:   m_stream << "ret"; break;
        case InstKind::br:    m_stream << "br"; break;
        case InstKind::cbr:   m_stream << "cbr"; break;
    }

    for ( auto i = inst.operand_offset; i < inst.operand_offset + inst.operand_count; ++i )
    {
        if ( i > inst.operand_offset ) m_stream << ",";
        m_stream << " %" << function.operands[ i ].id;
    }

    if ( inst.kind == InstKind::br || inst.kind == InstKind::cbr )
        m_stream << " bb" << inst.target_block.id;

    if ( inst.kind == InstKind::cbr )
        m_stream << " bb" << inst.false_block.id;

    m_stream << "\n";
}

std::string_view IRPrinter::type_str( const Type type )
{
    switch ( type )
    {
        case Type::i8:    return "i8";
        case Type::i16:   return "i16";
        case Type::i32:   return "i32";
        case Type::i64:   return "i64";
        case Type::i128:  return "i128";
        case Type::f32:   return "f32";
        case Type::f64:   return "f64";
        case Type::ptr:   return "ptr";
        case Type::void_: return "void";
    }

    return "unknown";
}

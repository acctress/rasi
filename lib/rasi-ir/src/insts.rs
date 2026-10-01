use crate::block::Block;
use crate::types::Type;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Value(pub u32);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct ConstRef(pub u32);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct FuncRef(pub u32);

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IntCC { Eq, Ne, Slt, Sle, Sgt, Sge, Ult, Ule, Ugt, Uge }


#[derive(Debug, Clone)]
pub enum Opcode {
    Iconst(ConstRef),
    Iadd,
    Isub,
    Imul,
    Udiv,
    Sdiv,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Sar,
    Icmp(IntCC),
    Br(Block, Vec<Value>),
    Brif(Value, Block, Vec<Value>, Block, Vec<Value>),
    Call(FuncRef),
    Ret,
    Load(i32),
    Store(i32),
    Srem,
    Urem,
    Sext,
    Zext,
    Trunc,
}

#[derive(Debug, Clone)]
pub struct Inst {
    pub opcode: Opcode,
    pub args: Vec<Value>,
    pub result: Option<Value>,
    pub ty: Type
}

impl Inst {
    pub fn new(opcode: Opcode, args: Vec<Value>, result: Option<Value>, ty: Type) -> Self {
        Self { opcode, args, result, ty }
    }

    pub fn is_terminator(&self) -> bool {
        matches!(self.opcode, Opcode::Br(..) | Opcode::Brif(..) | Opcode::Ret)
    }
}

impl Opcode {
    pub fn name(&self) -> &'static str {
        use Opcode::*;

        match self {
            Iconst(..) => "iconst",
            Iadd => "iadd",
            Isub => "isub",
            Imul => "imul",
            Sdiv => "sdiv",
            Udiv => "udiv", 
            And => "and", 
            Or => "or", 
            Xor => "xor",
            Shl => "shl", 
            Shr => "shr", 
            Sar => "sar", 
            Icmp(..) => "icmp",
            Br(..) => "br", 
            Brif(..) => "brif", 
            Call(..) => "call", 
            Ret => "ret",
            Load(..) => "load", 
            Store(..) => "store",
            Srem => "srem",
            Urem => "urem",
            Sext => "sext",
            Zext => "zext",
            Trunc => "trunc",
        }
    }
}
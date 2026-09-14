use crate::types::Type;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Value(pub u32);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct ConstRef(pub u32);


#[derive(Debug, Clone)]
pub enum Opcode {
    Iconst(ConstRef),
    Iadd, Isub, Imul,
    Idiv,
    And, Or, Xor,
    Shl, Shr, Sar,
    Icmp,
    Br, Brif,
    Call,
    Ret,
    Load, Store
}

#[derive(Debug, Clone)]
pub struct Inst {
    pub opcode: Opcode,
    pub args: Vec<Value>,
    pub ty: Type
}

impl Inst {
    pub fn new(opcode: Opcode, args: Vec<Value>, ty: Type) -> Self {
        Self { opcode, args, ty }
    }
}
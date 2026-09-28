use crate::{encode::emit_rm, operand::*, regs::*};

#[derive(Default)]
pub struct X86Assembler {
    pub buf: Vec<u8>
}

impl X86Assembler {
    pub fn new() -> Self { Self::default() }
}
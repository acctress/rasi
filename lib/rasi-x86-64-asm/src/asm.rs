use crate::{encode::emit_rm, operand::*, regs::*};

#[derive(Default)]
pub struct X86Assembler {
    pub buf: Vec<u8>
}

impl X86Assembler {
    pub fn new() -> Self { Self::default() }

    fn alu(&mut self, n: u8, size: Size, dst: Rm, src: RegImm) {
        let byte = |s: Size| if s == Size::S8 { 0 } else { 1 };

        match src {
            RegImm::Reg(r) => emit_rm(&mut self.buf, size, &[n * 8 + byte(size)], r.num(), &dst, false),

            RegImm::Imm(i) if i8::try_from(i).is_ok() => {
                emit_rm(&mut self.buf, size, &[0x83], n, &dst, false);
                self.buf.push(i as i8 as u8);
            }

            RegImm::Imm(i) => {
                emit_rm(&mut self.buf, size, &[0x81], n, &dst, false);
                self.buf.extend_from_slice(&i.to_le_bytes());
            }
        }
    }

    pub fn mov(&mut self, size: Size, dst: impl Into<Rm>, src: impl Into<RegImm>) {
        let dst: Rm = dst.into();
        let src: RegImm = src.into();
        
        match src {
            RegImm::Reg(r) => emit_rm(&mut self.buf, size, &[0x89], r.num(), &dst, false),
            RegImm::Imm(_) => todo!()
        }
    }
}

macro_rules! alu { ($($name:ident = $n:expr),* $(,)?) => {
    impl X86Assembler {$(
        pub fn $name(&mut self, size: Size, dst: impl Into<Rm>, src: impl Into<RegImm>) {
            self.alu($n, size, dst.into(), src.into());
        }
    )*
    }
}}

alu!(add = 0, sub = 5);
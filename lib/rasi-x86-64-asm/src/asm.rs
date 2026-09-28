use crate::{encode::emit_rm, operand::*, regs::*};
use crate::encode::rex;

#[derive(Default)]
pub struct X86Assembler {
    pub buf: Vec<u8>
}

fn wbit(size: Size) -> u8 { u8::from(size != Size::S8) }

impl X86Assembler {
    pub fn new() -> Self { Self::default() }

    fn imm(&mut self, size: Size, value: i32) {
        match size {
            Size::S8 => {
                debug_assert!((i8::MIN..=i8::MAX).contains(&(value as i8)));
                self.buf.push(value as u8);
            }

            Size::S16 => {
                debug_assert!((i16::MIN..=i16::MAX).contains(&(value as i16)));
                self.buf.extend_from_slice(&(value as u16).to_le_bytes());
            }

            Size::S32 | Size::S64 => self.buf.extend_from_slice(&value.to_le_bytes())
        }
    }

    fn alu(&mut self, n: u8, size: Size, dst: Rm, src: RegImm) {
        match src {
            RegImm::Reg(r) => emit_rm(&mut self.buf, size, &[n * 8 + wbit(size)], r, &dst),

            RegImm::Imm(i) if size != Size::S8 && i8::try_from(i).is_ok() => {
                emit_rm(&mut self.buf, size, &[0x83], n, &dst);
                self.buf.push(i as u8);
            }

            RegImm::Imm(i) => {
                let op = if size == Size::S8 { 0x80 } else { 0x81 };
                emit_rm(&mut self.buf, size, &[op], n, &dst);
                self.imm(size, i);
            }
        }
    }

    pub fn mov(&mut self, size: Size, dst: impl Into<Rm>, src: impl Into<RegImm>) {
        let dst: Rm = dst.into();
        match src.into() {
            RegImm::Reg(r) => emit_rm(&mut self.buf, size, &[0x88 | wbit(size)], r, &dst),
            RegImm::Imm(i) => {
                let op = if size == Size::S8 { 0xC6 } else { 0xC7 };
                emit_rm(&mut self.buf, size, &[op], 0u8, &dst);
                self.imm(size, i);
            }
        }
    }

    pub fn mov_load(&mut self, size: Size, dst: Gpr, src: impl Into<Rm>) {
        emit_rm(&mut self.buf, size, &[0x8A | wbit(size)], dst, &src.into());
    }

    pub fn movabs(&mut self, dst: Gpr, imm: i64) {
        self.buf.push(rex(true, false, false, dst.hi()));
        self.buf.push(0xB8 + dst.enc());
        self.buf.extend_from_slice(&imm.to_le_bytes());
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

alu!(add = 0, or = 1, adc = 2, sbb = 3, and = 4, sub = 5, xor = 6, cmp = 7);
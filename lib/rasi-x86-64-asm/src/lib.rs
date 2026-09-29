pub mod regs;
pub mod operand;
pub mod encode;
pub mod asm;

#[cfg(test)]
mod tests {
    use crate::{asm::X86Assembler, operand::{Mem, Scale}, regs::*};
    use Size::*;

    fn enc(f: impl FnOnce(&mut X86Assembler)) -> Vec<u8> {
        let mut a = X86Assembler::new();
        f(&mut a);
        a.buf
    }

    #[test]
    fn alu() {
        assert_eq!(enc(|a| a.add(S64, RAX, RCX)), [0x48, 0x01, 0xC8]);
        assert_eq!(enc(|a| a.or (S64, RAX, RCX)), [0x48, 0x09, 0xC8]);
        assert_eq!(enc(|a| a.adc(S64, RAX, RCX)), [0x48, 0x11, 0xC8]);
        assert_eq!(enc(|a| a.sbb(S64, RAX, RCX)), [0x48, 0x19, 0xC8]);
        assert_eq!(enc(|a| a.and(S64, RAX, RCX)), [0x48, 0x21, 0xC8]);
        assert_eq!(enc(|a| a.sub(S64, RAX, RCX)), [0x48, 0x29, 0xC8]);
        assert_eq!(enc(|a| a.xor(S64, RAX, RCX)), [0x48, 0x31, 0xC8]);
        assert_eq!(enc(|a| a.cmp(S64, RAX, RCX)), [0x48, 0x39, 0xC8]);
        assert_eq!(enc(|a| a.add(S64, RAX, 8)), [0x48, 0x83, 0xC0, 0x08]);
        assert_eq!(enc(|a| a.or (S64, RAX, 8)), [0x48, 0x83, 0xC8, 0x08]);
        assert_eq!(enc(|a| a.adc(S64, RAX, 8)), [0x48, 0x83, 0xD0, 0x08]);
        assert_eq!(enc(|a| a.sbb(S64, RAX, 8)), [0x48, 0x83, 0xD8, 0x08]);
        assert_eq!(enc(|a| a.and(S64, RAX, 8)), [0x48, 0x83, 0xE0, 0x08]);
        assert_eq!(enc(|a| a.xor(S64, RAX, 8)), [0x48, 0x83, 0xF0, 0x08]);
        assert_eq!(enc(|a| a.cmp(S64, RAX, 8)), [0x48, 0x83, 0xF8, 0x08]);
        assert_eq!(enc(|a| a.and(S32, RAX, -16)), [0x83, 0xE0, 0xF0]);
        assert_eq!(enc(|a| a.cmp(S32, Mem::base(RAX), 5)),          [0x83, 0x38, 0x05]);
        assert_eq!(enc(|a| a.xor(S64, R9, R10)),                    [0x4D, 0x31, 0xD1]);
        assert_eq!(enc(|a| a.add(S8, Mem::base(RBX), RSI)),         [0x40, 0x00, 0x33]);
    }

    #[test]
    fn alu_with_size_imms() {
        assert_eq!(enc(|a| a.sub(S64, RAX, 8)),      [0x48, 0x83, 0xE8, 0x08]);
        assert_eq!(enc(|a| a.sub(S64, RAX, 0x1000)), [0x48, 0x81, 0xE8, 0x00, 0x10, 0x00, 0x00]);
        assert_eq!(enc(|a| a.add(S32, RAX, 0x1000)), [0x81, 0xC0, 0x00, 0x10, 0x00, 0x00]);
        assert_eq!(enc(|a| a.add(S16, RAX, RCX)),    [0x66, 0x01, 0xC8]);
        assert_eq!(enc(|a| a.add(S16, RAX, 8)),      [0x66, 0x83, 0xC0, 0x08]);
        assert_eq!(enc(|a| a.add(S16, RAX, 0x1000)), [0x66, 0x81, 0xC0, 0x00, 0x10]);
        assert_eq!(enc(|a| a.add(S8,  RAX, RCX)),    [0x00, 0xC8]);
        assert_eq!(enc(|a| a.sub(S8,  RAX, 5)),      [0x80, 0xE8, 0x05]);
    }

    #[test]
    fn mov() {
        assert_eq!(enc(|a| a.mov(S64, RAX, RCX)), [0x48, 0x89, 0xC8]);
        assert_eq!(enc(|a| a.mov(S64, R9, R10)),  [0x4D, 0x89, 0xD1]);
        assert_eq!(enc(|a| a.mov(S32, R9, R10)),  [0x45, 0x89, 0xD1]);
        assert_eq!(enc(|a| a.mov(S32, RAX, RCX)), [0x89, 0xC8]);
        assert_eq!(enc(|a| a.mov(S16, RAX, RCX)), [0x66, 0x89, 0xC8]);
        assert_eq!(enc(|a| a.mov(S8,  RAX, RCX)), [0x88, 0xC8]);
        assert_eq!(enc(|a| a.mov(S64, Mem::base(RSP).disp(8), RAX)), [0x48, 0x89, 0x44, 0x24, 0x08]);
        assert_eq!(enc(|a| a.mov(S64, Mem::base(RBP), RAX)),         [0x48, 0x89, 0x45, 0x00]);
    }

    #[test]
    fn mov_imms_loads() {
        assert_eq!(enc(|a| a.mov(S64, RAX, 1)),  [0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00]);
        assert_eq!(enc(|a| a.mov(S8,  RAX, 1)),  [0xC6, 0xC0, 0x01]);
        assert_eq!(enc(|a| a.mov(S64, Mem::base(RSP).disp(8), -1)),
                   [0x48, 0xC7, 0x44, 0x24, 0x08, 0xFF, 0xFF, 0xFF, 0xFF]);
        assert_eq!(enc(|a| a.mov_load(S64, RCX, Mem::base(RSP).disp(8))), [0x48, 0x8B, 0x4C, 0x24, 0x08]);
        assert_eq!(enc(|a| a.mov_load(S8,  RSI, Mem::base(RAX))),         [0x40, 0x8A, 0x30]);
        assert_eq!(enc(|a| a.movabs(RAX, 0x1122334455667788)),
                   [0x48, 0xB8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11]);
        assert_eq!(enc(|a| a.movabs(R9, 0x1122334455667788)),
                   [0x49, 0xB9, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11]);
    }

    #[test]
    fn byte_regs_nr() {
        assert_eq!(enc(|a| a.add(S8, RSI, RDI)), [0x40, 0x00, 0xFE]);
        assert_eq!(enc(|a| a.sub(S8, RSI, 1)),   [0x40, 0x80, 0xEE, 0x01]);
        assert_eq!(enc(|a| a.mov(S8, Mem::base(RSP).disp(8), RSI)), [0x40, 0x88, 0x74, 0x24, 0x08]);
    }

    #[test]
    fn addressing() {
        assert_eq!(enc(|a| a.sub(S64, Mem::base(RSP).disp(8), RAX)), [0x48, 0x29, 0x44, 0x24, 0x08]);
        assert_eq!(enc(|a| a.sub(S64, Mem::base(RSP), RAX)),         [0x48, 0x29, 0x04, 0x24]);
        assert_eq!(enc(|a| a.sub(S64, Mem::base(RBP), RAX)),         [0x48, 0x29, 0x45, 0x00]);
        assert_eq!(enc(|a| a.sub(S64, Mem::base(R12), R13)),         [0x4D, 0x29, 0x2C, 0x24]);
        assert_eq!(enc(|a| a.sub(S64, Mem::base(R13), RAX)),         [0x49, 0x29, 0x45, 0x00]);
        assert_eq!(
            enc(|a| a.sub(S64, Mem::base(RAX).index(RCX, Scale::X4).disp(8), RDX)),
            [0x48, 0x29, 0x54, 0x88, 0x08]
        );
    }

    #[test]
    fn unary_all() {
        assert_eq!(enc(|a| a.not (S64, RAX)), [0x48, 0xF7, 0xD0]);
        assert_eq!(enc(|a| a.neg (S64, RAX)), [0x48, 0xF7, 0xD8]);
        assert_eq!(enc(|a| a.mul (S64, RAX)), [0x48, 0xF7, 0xE0]);
        assert_eq!(enc(|a| a.imul1(S64, RAX)), [0x48, 0xF7, 0xE8]);
        assert_eq!(enc(|a| a.div (S64, RAX)), [0x48, 0xF7, 0xF0]);
        assert_eq!(enc(|a| a.idiv(S64, RAX)), [0x48, 0xF7, 0xF8]);
        assert_eq!(enc(|a| a.not(S32, RCX)),  [0xF7, 0xD1]);
        assert_eq!(enc(|a| a.not(S16, RAX)),  [0x66, 0xF7, 0xD0]);
        assert_eq!(enc(|a| a.neg(S8,  RAX)),  [0xF6, 0xD8]);
        assert_eq!(enc(|a| a.not(S8,  RSI)),  [0x40, 0xF6, 0xD6]);
        assert_eq!(enc(|a| a.neg(S64, R9)),                 [0x49, 0xF7, 0xD9]);
        assert_eq!(enc(|a| a.div(S32, Mem::base(RBX))),     [0xF7, 0x33]);
    }

    #[test]
    fn fixed_all() {
        assert_eq!(enc(|a| a.ret()),   [0xC3]);
        assert_eq!(enc(|a| a.cqo()),   [0x48, 0x99]);
        assert_eq!(enc(|a| a.cdq()),   [0x99]);
        assert_eq!(enc(|a| a.nop()),   [0x90]);
        assert_eq!(enc(|a| a.int3()),  [0xCC]);
        assert_eq!(enc(|a| a.leave()), [0xC9]);
        assert_eq!(enc(|a| a.ud2()),   [0x0F, 0x0B]);
    }
}
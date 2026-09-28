use crate::operand::Mem;
use crate::regs::{Size, R10, R9, RAX, RBP, RCX, RSP};

pub mod regs;
pub mod operand;
pub mod encode;
pub mod asm;

#[test]
fn alu_encodings() {
    use crate::asm::X86Assembler;
    use crate::operand::Mem;
    use crate::regs::*;

    let enc = |f: fn(&mut X86Assembler)| { let mut a = X86Assembler::new(); f(&mut a); a.buf };

    assert_eq!(enc(|a| a.add(Size::S64, RAX, RCX)),                    [0x48, 0x01, 0xC8]);
    assert_eq!(enc(|a| a.sub(Size::S64, RAX, RCX)),                    [0x48, 0x29, 0xC8]);
    assert_eq!(enc(|a| a.sub(Size::S64, RAX, 8)),                      [0x48, 0x83, 0xE8, 0x08]);
    assert_eq!(enc(|a| a.sub(Size::S64, RAX, 0x1000)),                 [0x48, 0x81, 0xE8, 0x00, 0x10, 0x00, 0x00]);
    assert_eq!(enc(|a| a.sub(Size::S64, Mem::base(RSP).disp(8), RAX)), [0x48, 0x29, 0x44, 0x24, 0x08]);
    assert_eq!(enc(|a| a.sub(Size::S64, Mem::base(RSP), RAX)),         [0x48, 0x29, 0x04, 0x24]);
    assert_eq!(enc(|a| a.sub(Size::S64, Mem::base(R12), R13)),         [0x4D, 0x29, 0x2C, 0x24]);
}

#[test]
fn mov_encodings() {
    use crate::asm::X86Assembler;
    use crate::operand::Mem;
    use crate::regs::*;

    let enc = |f: fn(&mut X86Assembler)| { let mut a = X86Assembler::new(); f(&mut a); a.buf };
    
    assert_eq!(enc(|a|a.mov(Size::S64, RAX, RCX)),                     [0x48, 0x89, 0xC8]);
    assert_eq!(enc(|a|a.mov(Size::S64, R9, R10)),                      [0x4D, 0x89, 0xD1]);
    assert_eq!(enc(|a|a.mov(Size::S64, Mem::base(RSP).disp(8), RAX)),  [0x48, 0x89, 0x44, 0x24, 0x08]);
    assert_eq!(enc(|a|a.mov(Size::S64, Mem::base(RBP), RAX)),          [0x48, 0x89, 0x45, 0x00]);
}
use crate::regs::Gpr;

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum Scale { X1 = 0, X2 = 1, X4 = 2, X8 = 3 }

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub struct Mem {
    pub base: Gpr,
    pub index: Option<(Gpr, Scale)>,
    pub disp: i32,
}

impl Mem {
    pub const fn base(base: Gpr) -> Self { Self { base, index: None, disp: 0 } }
    pub const fn disp(mut self, d: i32) -> Self { self.disp = d; self }
    pub const fn index(mut self, i: Gpr, s: Scale) -> Self { assert!(i.num() != 4); self.index = Some((i, s)); self }
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum Rm {
    Reg(Gpr),
    Mem(Mem)
}

impl From<Gpr> for Rm { fn from(value: Gpr) -> Self { Rm::Reg(value) } }
impl From<Mem> for Rm { fn from(value: Mem) -> Self { Rm::Mem(value) } }

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum RegImm {
    Reg(Gpr),
    Imm(i32)
}

impl From<Gpr> for RegImm { fn from(value: Gpr) -> Self { RegImm::Reg(value) } }
impl From<i32> for RegImm { fn from(value: i32) -> Self { RegImm::Imm(value) } }

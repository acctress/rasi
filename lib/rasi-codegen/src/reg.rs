#[derive(Clone, Copy, PartialEq, Eq, Hash, Debug)]
pub enum RegClass { Int, Float }

#[derive(Clone, Copy, PartialEq, Eq, Hash, Debug)]
pub struct PReg { pub class: RegClass, pub hw: u8 }

#[derive(Clone, Copy, PartialEq, Eq, Hash, Debug)]
pub struct VReg(pub u32);

#[derive(Clone, Copy, PartialEq, Eq, Hash, Debug)]
pub enum Reg { V(VReg), P(PReg) }

impl PReg {
    pub const fn new(class: RegClass, hw: u8) -> Self { Self { class, hw } }
    pub const fn int(hw: u8) -> Self { Self::new(RegClass::Int, hw) }
    pub const fn float(hw: u8) -> Self { Self::new(RegClass::Float, hw) }
}

impl Reg {
    pub fn as_preg(self) -> Option<PReg> { if let Reg::P(p) = self { Some(p) } else { None } }
    pub fn as_vreg(self) -> Option<VReg> { if let Reg::V(p) = self { Some(p) } else { None } }
    pub fn preg(self) -> PReg { self.as_preg().expect("vreg survbived regalloc") }
}

impl From<VReg> for Reg { fn from(value: VReg) -> Self { Reg::V(value) } }
impl From<PReg> for Reg { fn from(value: PReg) -> Self { Reg::P(value) } }

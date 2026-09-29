pub mod buffer;
pub mod reg;
pub mod exec;
pub mod operand;
pub mod machinst;

pub use buffer::{Buffer, FixupKind, Label};
pub use reg::{PReg, Reg, RegClass, VReg};
pub use exec::ExecBuf;
pub use operand::*;
pub use machinst::MachInst;

#[cfg(test)]
mod tests {
    use crate::buffer::*;
    use crate::reg::*;

    #[test]
    fn forward_ref() {
        let mut b = Buffer::new();
        let l = b.new_label();
        b.put(&[0xE9]);
        b.use_label(l, FixupKind::Rel32);
        b.put(&[0x90]);
        b.bind(l);
        assert_eq!(b.finish(), [0xE9, 1, 0, 0, 0, 0x90]);
    }

    #[test]
    fn backward_ref() {
        let mut b = Buffer::new();
        let l = b.new_label();
        b.bind(l);
        b.put(&[0x90, 0x0F, 0x85]);
        b.use_label(l, FixupKind::Rel32);
        assert_eq!(b.finish(), [0x90, 0x0F, 0x85, 0xF9, 0xFF, 0xFF, 0xFF]);
    }

    #[test]
    fn reg_basics() {
        let p: Reg = PReg::int(3).into();
        let v: Reg = VReg(7).into();
        assert_eq!(p.as_preg(), Some(PReg::int(3)));
        assert_eq!(v.as_preg(), None);
        assert_eq!(v.as_vreg(), Some(VReg(7)));
        assert_ne!(PReg::int(1), PReg::float(1));
    }
}
use crate::reg::{PReg, Reg};

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum OpKind { Use, Def, UseDef }

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum Constraint { Any, Fixed(PReg) }

pub trait RegVisitor {
    fn reg(&mut self, r: &mut Reg, kind: OpKind, c: Constraint);
    fn clobber(&mut self, p: PReg);
}
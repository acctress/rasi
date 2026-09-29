use rasi_x86_64_asm::{asm::X86Assembler, operand::RegImm, regs::{Gpr, Size}};
use rasi_codegen::{
    operand::{Constraint, OpKind, RegVisitor},
    Buffer, MachInst, PReg, Reg,
};

pub const RAX: PReg = PReg::int(0);
pub const RCX: PReg = PReg::int(1);
pub const RDX: PReg = PReg::int(2);

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum AluOp { Add, Or, Adc, Sbb, And, Sub, Xor, Cmp }

#[derive(Clone, Copy, Debug)]
pub enum RegOImm { Reg(Reg), Imm(i32) }

#[derive(Clone, Debug)]
pub enum X86Inst {
    MovImm { size: Size, dst: Reg, imm: i32 },
    Alu { op: AluOp, size: Size, dst: Reg, src: RegOImm },
    Ret,
}

fn gpr(r: Reg) -> Gpr { Gpr::name(r.preg().hw) }

impl MachInst for X86Inst {
    fn visit_regs(&mut self, v: &mut impl RegVisitor) {
        match self {
            X86Inst::MovImm { dst, .. } => v.reg(dst, OpKind::Def, Constraint::Any),
            X86Inst::Alu { op, dst, src, .. } => {
                let opkind = if *op == AluOp::Cmp { OpKind::Use } else { OpKind::UseDef };
                v.reg(dst, opkind, Constraint::Any);

                if let RegOImm::Reg(r) = src { v.reg(r, OpKind::Use, Constraint::Any); }
            }

            X86Inst::Ret => {}
        }
    }

    fn emit(&self, buf: &mut Buffer) {
        let mut a = X86Assembler::new(buf);

        match *self {
            X86Inst::MovImm { size, dst, imm } => a.mov(size, gpr(dst), imm),
            X86Inst::Alu { op, size, dst, src } => {
                let d = gpr(dst);
                let s = match src {
                    RegOImm::Reg(r) => RegImm::Reg(gpr(r)),
                    RegOImm::Imm(i) => RegImm::Imm(i),
                };

                match op {
                    AluOp::Add => a.add(size, d, s),
                    AluOp::Or  => a.or(size, d, s),
                    AluOp::Adc => a.adc(size, d, s),
                    AluOp::Sbb => a.sbb(size, d, s),
                    AluOp::And => a.and(size, d, s),
                    AluOp::Sub => a.sub(size, d, s),
                    AluOp::Xor => a.xor(size, d, s),
                    AluOp::Cmp => a.cmp(size, d, s),
                }
            }

            X86Inst::Ret => a.ret(),
        }
    }
}
pub mod inst;
pub mod lower;

pub use inst::*;
pub use lower::*;

#[cfg(all(test, target_arch = "x86_64"))]
mod tests {
    use super::*;
    use rasi_codegen::{Buffer, ExecBuf, MachInst, Reg, operand::Collector, VReg, OpKind, Constraint};
    use rasi_codegen::Constraint::Any;
    use rasi_codegen::OpKind::{Def, Use, UseDef};
    use rasi_ir::builder::IRBuilder;
    use rasi_ir::function::Function;
    use rasi_ir::types::Type;
    use rasi_regalloc::interval::LiveIntervals;
    use rasi_regalloc::liveness::{Liveness};
    use rasi_x86_64_asm::regs::Size;
    use crate::AluOp::And;

    #[test]
    fn jit_machinst() {
        let rax = Reg::P(RAX);

        let p = [

            X86Inst::MovImm { size: Size::S32, dst: rax, imm: 23 },
            X86Inst::Alu { op: AluOp::Add, size: Size::S32, dst: rax, src: RegOImm::Imm(78) },
            X86Inst::Ret

        ];

        let mut buf = Buffer::new();
        for i in &p { i.emit(&mut buf); }

        let bytes = buf.finish();
        println!("{}", rasi_x86_64_asm::dis::dis(&bytes));
        let code = ExecBuf::new(&bytes);
        let f: extern "C" fn() -> i32 = unsafe { code.as_fn() };
        assert_eq!(f(), 101);
    }

    #[test]
    fn pipeline_test() {
        let mut func = Function::new("main", &[], Type::I32);
        let mut b = IRBuilder::new(&mut func);

        let x = b.iconst(23i32);
        let y = b.iconst(78i32);
        let s = b.iadd(x, y);
        b.ret(Some(s));
        
        println!("{func}");

        let mut vc = lower_function(&func);
        // assign_pregs(&mut vc);
        
        println!();
        println!("{}", &vc);

        let liveness = Liveness::compute(&vc);
        println!("{:?}", liveness);
        
        let intervals = LiveIntervals::build(&vc, &liveness);
        println!("{:?}", intervals);

        //
        // let mut buf = Buffer::new();
        // for i in &vc.insts { i.emit(&mut buf); }
        // let bytes = buf.finish();
        //
        // let code = ExecBuf::new(&bytes);
        // let f: extern "C" fn() -> i32 = unsafe { code.as_fn() };
        // assert_eq!(f(), 101);
        // println!("result = {}", f());
    }

    fn ops(mut i: X86Inst) -> Vec<(OpKind, Constraint)> {
        let mut c = Collector::default();
        i.visit_regs(&mut c);
        c.ops.into_iter().map(|(_, k, c)| (k, c)).collect()
    }

    const A: Reg = Reg::V(VReg(0));
    const B: Reg = Reg::V(VReg(1));

    #[test]
    fn contract() {
        let s = Size::S32;

        assert_eq!(ops(X86Inst::MovImm { size: s, dst: A, imm: 1 }), [(Def, Any)]);
        assert_eq!(ops(X86Inst::Alu { op: AluOp::Add, size: s, dst: A, src: RegOImm::Reg(B) }),
                   [(UseDef, Any), (Use, Any)]);
        assert_eq!(ops(X86Inst::Alu { op: AluOp::Add, size: s, dst: A, src: RegOImm::Imm(1) }),
                   [(UseDef, Any)]);
        assert_eq!(ops(X86Inst::Alu { op: AluOp::Cmp, size: s, dst: A, src: RegOImm::Reg(B) }),
                   [(Use, Any), (Use, Any)]);
        assert!(ops(X86Inst::Ret).is_empty());
    }
}
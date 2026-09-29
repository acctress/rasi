pub mod inst;

pub use inst::*;

#[cfg(all(test, target_arch = "x86_64"))]
mod tests {
    use super::*;
    use rasi_codegen::{Buffer, ExecBuf, MachInst, Reg};
    use rasi_x86_64_asm::regs::Size;

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
}
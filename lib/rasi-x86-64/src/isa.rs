use std::fmt;
use std::fmt::{write, Formatter};
use rasi_codegen::{Buffer, CompiledCode, Flags, MachInst, TargetIsa};
use rasi_ir::function::Function;
use crate::{lower_function, RegAlloc, X86Inst};

pub struct X86Isa {
    flags: Flags
}

impl X86Isa {
    pub fn new(flags: Flags) -> Self {
        Self { flags }
    }
}

impl TargetIsa for X86Isa {
    fn compile(&self, func: &Function) -> CompiledCode {
        let mut vc = lower_function(func);
        RegAlloc::run(&mut vc);
        
        println!("{}", vc);

        let mut buf = Buffer::new();
        for inst in &vc.insts {
            inst.emit(&mut buf);
        }

        CompiledCode { bytes: buf.finish(), relocs: vec![] }
    }
}

impl fmt::Display for X86Isa {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        write!(f, "x86_64")
    }
}
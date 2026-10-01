use rasi_ir::{function::Function, insts::{Opcode, Value}, types::Type};
use rasi_codegen::{Block, Reg, RegClass, VCode, VCodeBuilder};
use rasi_x86_64_asm::regs::Size;
use std::collections::HashMap;
use rasi_ir::insts::{ConstRef, Inst};
use crate::{AluOp, RegOImm, X86Inst, RAX};

pub struct LowerCtx {
    builder: VCodeBuilder<X86Inst>,
    vregs: HashMap<u32, Reg>,
}

impl LowerCtx {
    fn new() -> Self { Self { builder: VCodeBuilder::new(), vregs: HashMap::new() } }

    fn class_of(ty: Type) -> RegClass {
        if matches!(ty, Type::F32 | Type::F64) { RegClass::Float } else { RegClass::Int }
    }

    fn value(&mut self, v: Value, ty: Type) -> Reg {
        let b = &mut self.builder;
        *self.vregs.entry(v.0).or_insert_with(|| Reg::V(b.new_vreg(Self::class_of(ty))))
    }

    fn bind(&mut self, v: Value, reg: Reg) {
        self.vregs.insert(v.0, reg);
    }

    fn tmp(&mut self, ty: Type) -> Reg {
        Reg::V(self.builder.new_vreg(Self::class_of(ty)))
    }

    fn push(&mut self, inst: X86Inst) { self.builder.push(inst); }
    fn start_block(&mut self) -> Block { self.builder.start_block() }
    fn end_block(&mut self, succs: &[Block]) { self.builder.end_block(succs) }
    fn finish(self) -> VCode<X86Inst> { self.builder.finish() }
}

fn size_of(ty: Type) -> Size {
    match ty {
        Type::I8 => Size::S8, Type::I16 => Size::S16,
        Type::I32 => Size::S32, Type::I64 => Size::S64,
        t => panic!("unsupported type for int lowering: {t:?}"),
    }
}

fn lower_iconst(ctx: &mut LowerCtx, func: &Function, inst: &Inst, cref: ConstRef) {
    let dst = ctx.value(inst.result.unwrap(), inst.ty);
    let imm = func.constant(cref) as i32;
    ctx.push(X86Inst::MovImm { size: size_of(inst.ty), dst, imm })
}

fn lower_binop(ctx: &mut LowerCtx, inst: &Inst, op: AluOp) {
    let a = ctx.value(inst.args[0], inst.ty);
    let s = ctx.value(inst.args[1], inst.ty);
    let size = size_of(inst.ty);
    let dst = ctx.tmp(inst.ty);

    ctx.push(X86Inst::Mov { size, dst, src: a });
    ctx.push(X86Inst::Alu { op, size, dst, src: RegOImm::Reg(s) });
    ctx.bind(inst.result.unwrap(), dst);
}

fn lower_ret(ctx: &mut LowerCtx, func: &Function, inst: &Inst) {
    if let Some(&val) = inst.args.first() {
        let ty = func.value_type(val);
        let src = ctx.value(val, ty);
        let size = size_of(ty);

        ctx.push(X86Inst::Mov { size, dst: Reg::P(RAX), src });
    }

    ctx.push(X86Inst::Ret);
}

pub fn lower_function(func: &Function) -> VCode<X86Inst> {
    assert_eq!(func.blocks.len(), 1, "lower_function: multiple blocks not implemented");

    let mut ctx = LowerCtx::new();
    ctx.start_block();

    for inst in &func.block(func.entry).insts {
        match &inst.opcode {
            Opcode::Iconst(cref) => lower_iconst(&mut ctx, func, inst, *cref),
            Opcode::Iadd         => lower_binop (&mut ctx, inst, AluOp::Add),
            Opcode::Isub         => lower_binop (&mut ctx, inst, AluOp::Sub),
            Opcode::Ret          => lower_ret   (&mut ctx, func, inst),
            op                   => panic!("lower_function: unhandled op {op:?}"),
        }
    }

    ctx.end_block(&[]);
    ctx.finish()
}
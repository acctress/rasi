use crate::{
    block::Block,
    function::Function,
    insts::{Inst, Opcode, Value},
    types::Type
};

pub trait IntoConst {
    fn ty(&self) -> Type;
    fn as_i64(self) -> i64;
}

impl IntoConst for i32 {
    fn ty(&self) -> Type { Type::I32 }
    fn as_i64(self) -> i64 { self as i64 }
}

impl IntoConst for i64 {
    fn ty(&self) -> Type { Type::I64 }
    fn as_i64(self) -> i64 { self }
}

pub struct IRBuilder<'a> {
    func: &'a mut Function,
    current: Block,
}

impl<'a> IRBuilder<'a> {
    pub fn new(func: &'a mut Function) -> Self {
        let entry = func.entry;
        Self { func, current: entry }
    }

    pub fn switch(&mut self, block: Block) {
        self.current = block;
    }

    pub fn iconst(&mut self, val: impl IntoConst) -> Value {
        let ty = val.ty();
        let cref = self.func.alloc_const(val.as_i64());
        let v = self.func.alloc_value(ty);
        self.push(Inst::new(Opcode::Iconst(cref), vec![], ty ));
        v
    }

    pub fn iadd(&mut self, a: Value, b: Value) -> Value {
        let ty = self.func.value_type(a);
        let val = self.func.alloc_value(ty);
        self.push(Inst::new(Opcode::Iadd, vec![a, b], ty ));
        val
    }

    pub fn ret(&mut self, val: Option<Value>) {
        let args = val.map(|v| vec![v]).unwrap_or_default();
        self.push(Inst::new(Opcode::Ret, args, Type::Void));
    }

    fn push(&mut self, inst: Inst) {
        self.func.blocks.get_mut(&self.current).unwrap().insts.push(inst);
    }
}
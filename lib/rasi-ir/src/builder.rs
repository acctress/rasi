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
    pub func: &'a mut Function,
    pub current: Block,
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
        self.emit(Opcode::Iconst(cref), &[], ty)
    }

    pub fn iadd(&mut self, a: Value, b: Value) -> Value {
        let ty = self.func.value_type(a);
        self.emit(Opcode::Iadd, &[a, b], ty)
    }

    pub fn ret(&mut self, val: Option<Value>) {
        self.emit_void(Opcode::Ret, val.as_slice());
    }

    fn emit(&mut self, op: Opcode, args: &[Value], ty: Type) -> Value {
        debug_assert!(!self.func.is_terminated(self.current));

        let res = self.func.alloc_value(ty);
        self.func.push(self.current, Inst::new(op, Vec::from(args), Some(res), ty));
        res
    }

    fn emit_void(&mut self, op: Opcode, args: &[Value]) {
        debug_assert!(!self.func.is_terminated(self.current));

        self.func.push(self.current, Inst::new(op, Vec::from(args), None, Type::Void));
    }
}
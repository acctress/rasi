use crate::{block::{Block, BasicBlock}, insts::{Value, ConstRef}, types::Type};
use std::collections::HashMap;

pub struct Function {
    pub name: String,
    pub params: Vec<Type>,
    pub ret: Type,
    pub blocks: HashMap<Block, BasicBlock>,
    pub entry: Block,
    pub constants: Vec<i64>,
    pub value_types: Vec<Type>,
    nx_block: u32,
    nx_value: u32,
}

impl Function {
    pub fn new(name: impl Into<String>, params: Vec<Type>, ret: Type) -> Self {
        let entry = Block(0);
        let mut blocks = HashMap::new();

        blocks.insert(entry, BasicBlock::new());

        Self {
            name: name.into(),
            params,
            ret,
            blocks,
            entry,
            constants: vec![],
            value_types: vec![],
            nx_block: 1,
            nx_value: 0,
        }
    }

    pub fn value_type(&self, val: Value) -> Type {
        self.value_types[val.0 as usize]
    }

    pub fn alloc_block( &mut self ) -> Block {
        let blk = Block(self.nx_block);
        self.nx_block += 1;
        self.blocks.insert(blk, BasicBlock::new());

        blk
    }
    pub fn alloc_const(&mut self, val: i64) -> ConstRef {
        let c = ConstRef(self.constants.len() as u32);
        self.constants.push(val);

        c
    }

    pub fn alloc_value(&mut self, ty: Type) -> Value {
        let v = Value(self.nx_value);
        self.value_types.push(ty);
        self.nx_value += 1;

        v
    }
}
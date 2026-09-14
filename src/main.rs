use rasi_ir::builder::IRBuilder;
use rasi_ir::function::Function;
use rasi_ir::types::Type;

fn main() {
    let mut func = Function::new("foo", vec![ Type::I64 ], Type::I64 );

    let mut builder = IRBuilder::new(&mut func);

    let a = builder.iconst(42i32);
    let b = builder.iconst(89i32);
    let c = builder.iadd(a, b);
    builder.ret(Option::from(c));
}

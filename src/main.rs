use rasi_ir::{builder::IRBuilder, function::Function, types::Type};

fn main() {
    let mut func = Function::new("foo", &[ Type::I32 ], Type::I32 );
    let mut builder = IRBuilder::new(&mut func);

    let a = builder.iconst(42i32);
    let b = builder.iconst(89i64);
    let c = builder.iadd(a, b);
    builder.ret(Some(c));

    println!("{}", builder.func);
}

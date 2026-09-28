# rasi

[![CI](https://github.com/acctress/rasi/actions/workflows/rust.yml/badge.svg?branch=rust)](https://github.com/acctress/rasi/actions/workflows/rust.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![Rust](https://img.shields.io/badge/rust-2024-orange?logo=rust)
![Targets](https://img.shields.io/badge/target-x86--64-informational)
![ABI](https://img.shields.io/badge/ABI-SysV%20%7C%20Win64-informational)
![Last commit](https://img.shields.io/github/last-commit/acctress/rasi)

A retargetable compiler backend suite in Rust, targeting x86-64.

## Example

```rust
use rasi_ir::{builder::IRBuilder, function::Function, types::Type};

fn main() {
    let mut func = Function::new("foo", &[ Type::I32 ], Type::I32 );
    let mut builder = IRBuilder::new(&mut func);

    let a = builder.iconst(42i32);
    let b = builder.iconst(89i32);
    let c = builder.iadd(a, b);
    builder.ret(Some(c));

    println!("{}", builder.func);
}
```

```
define @foo(i32) -> i32 {
block0(%0: i32):
    %1 = iconst i32 42
    %2 = iconst i32 89
    %3 = iadd i32 %1, %2
    ret i32 %3
}
```

```
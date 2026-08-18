# Rasi IR Specification

## 1. Overview

Rasi IR is an SSA IR, it takes the form of functions with basic blocks. It is designed for both JIT and AOT compilation. The IR guarantees every value is defined once, and every block must end with a terminator.

All IR entities are referenced by **typed integer handles** rather than pointers. e.g.
```
struct ValueRef { u32 id; }
```

## 2. Type System

Rasi IR has a simple flat type system, signedness is not a property of types; but it is expressed by instructions.

### 2.1 Primitive Types

| Type   | Description                        |
|--------|------------------------------------|
| `i8`   | 8-bit integer                      |
| `i16`  | 16-bit integer                     |
| `i32`  | 32-bit integer                     |
| `i64`  | 64-bit integer                     |
| `i128` | 128-bit integer                    |
| `f32`  | 32-bit IEEE 754 floating point     |
| `f64`  | 64-bit IEEE 754 floating point     |

### 2.2 Pointer Types

`ptr` - a pointer width integer.

### 2.3 Type Rules

- Every value has **exactly** one type, which is assigned at definition and is immutable.
- Instructions define the types of their result values.
- Type mismatches between instruction operands and expected types are invalid IR.

## 3. Values

### 3.1 Definition

A value is the result of an instruction, which is identified by a `ValueRef`. It has exactly one type and is never reassigned.

### 3.2 SSA Invariant

Every value is defined once and is never reassigned.

## 4. Instructions

All instructions produce zero or one result value, the result type is determined by the instruction and its operands.

### 4.1 Arithmetic

| Instructions | Operands     | Result | Description               |
|--------------|--------------|--------|---------------------------|
| `iadd a, b`  | int, int     | int    | Integer addition          |
| `isub a, b`  | int, int     | int    | Integer subtraction       |
| `imul a, b`  | int, int     | int    | Integer multiplication    |
| `sdiv a, b`  | int, int     | int    | Signed integer division   |
| `udiv a, b`  | int, int     | int    | Unsigned integer division |
| `srem a, b`  | int, int     | int    | Signed remainder          |
| `urem a, b`  | int, int     | int    | Unsigned remainder        |
| `fadd a, b`  | float, float | float  | Float addition            |
| `fsub a, b`  | float, float | float  | Float subtraction         |
| `fmul a, b`  | float, float | float  | Float multiplication      |
| `fdiv a, b`  | float, float | float  | Float division            |
| `and a, b`   | int, int     | int    | Bitwise AND               |
| `or a, b`    | int, int     | int    | Bitwise OR                |
| `xor a, b`   | int, int     | int    | Bitwise XOR               |
| `shl a, b`   | int, int     | int    | Logical shift left        |
| `shr a, b`   | int, int     | int    | Logical shift right       |
| `sar a, b`   | int, int     | int    | Arithemetic shift right   |

### 4.2 Comparison

All comparison instructions produce `i8`

| Instructions    | Operands     | Result | Description                 |
|-----------------|--------------|--------|-----------------------------|
| `icmp.eq a, b`  | int, int     | i8     | Equal                       |
| `icmp.ne a, b`  | int, int     | i8     | Not equal                   |
| `icmp.slt a, b` | int, int     | i8     | Signed less than            |
| `icmp.sle a, b` | int, int     | i8     | Signed less than or equal   |
| `icmp.ult a, b` | int, int     | i8     | Unsigned less than          |
| `icmp.ule a, b` | int, int     | i8     | Unsigned less than or equal |
| `fcmp.eq a, b`  | float, float | i8     | Float less                  |
| `fcmp.lt a, b`  | float, float | i8     | Float less than             |
| `fcmp.le a, b`  | float, float | i8     | Float less than or equal    |

### 4.3 Memory

| Instructions        | Operands | Result | Description             |
|---------------------|----------|--------|-------------------------|
| `load.T addr`       | ptr      | T      | Load value of type T    |
| `store.T val, addr` | T, ptr   | N/A    | Store value to address  |
| `alloca.T`          | N/A      | ptr    | Allocate T on the stack |

### 4.4 Control Flow

| Instructions      | Operands        | Result   | Description                             |
|-------------------|-----------------|----------|-----------------------------------------|
| `call f, args...` | func, values... | T or N/A | Call function and returns result if any |

### 4.5 Terminators

| Instructions        | Operands         | Description                          |
|---------------------|------------------|--------------------------------------|
| `ret val`           | value or N/A     | Return from function                 |
| `br block, args...` | block, values... | Unconditional branch with block args |
| `cbr cond, b1, b2`  | i8, block, block | Conditional branch, no args          |

## 5. Basic Blocks

A basic block is a linear sequence of instructions ending with one terminator instruction.

### 5.1 Block Parameters

Basic blocks accept typed parameters instead of phi nodes, a branch to a block must supply values for all of its parameters.

### 5.2 Terminator Rules

- Every block must end with one terminator.
- A terminator must be the last instruction in a basic block.
- Branch targets must be basic blocks in the same function.

### 5.3 Validity

- It contains at least one instruction.
- Its final instruction is a terminator.
- All values used are defined and dominate their use.

## 6. Functions

### 6.1 Signature

A function has a name, a list of typed parameters, and a return type (or not return).

Signature essentially define the calling contract of the function.

### 6.2 Calling Convention

The default calling convention follows the platform ABI:
* System V AMD64 (Linux)
* Microsoft x64 (Windows)

### 6.3 Entry Block

Every function has exactly one entry block, the entry block receives the functions parameters as block parameters.

## 7. Modules

### 7.1 Contents

A module is a top level compilation unit, it contains a collection of functions and the module is passed to the JIT or AOT compiler.

### 7.2 Linkage

Functions have one of two linkage kinds:
- `public`, visible and callable from outside the module.
- `private`, internal to the module, not externally visible.
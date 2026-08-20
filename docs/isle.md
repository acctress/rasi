# ISLE

ISLE, the Instruction Selection Lowering Engine.

Rasi uses a custom ISLE system for instruction selection, opposed to hand writing codegen for every IR instruction, ISLE lets us declare pattern matching rules in a small DSL.

The `islec` tool will read `.isle` files and emit C++, which would be a generated match function that is compiled directly into the codegen pipeline, adding new instructions then becomes a matter of writing new rules and not touching the C++ codegen code at all.

## Architecture

```
           Rasi SSA (IR)
                |
                |
 ISLE matcher (x86_64_isle_gen.hh)
                |
                |
  calls extractors which is Rasi IR
calls constructors which is the assembler
                |
                |
          Assembler (x86_64)
                |
                |
    encoding.hh / assembler.hh
                |
                |
              Buffer
                |
            ExecMemory
```

## DSL

### Type Declarations
```lisp
;; primitives, opaque to ISLE
(type Value     (primitive))    ; ValueRef in IR
(type Reg       (primitive))    ; virtual or physical reg
(type VReg      (primitive))    ; virtual reg before regalloc
(type I64       (primitive))    ; 64 bit signed immediate
(type Type      (primitive))    ; rasi::Type

;; enums which ISLE can match on directly
(type CC (enum eq ne lt le gt ge ult ule ugt uge))
```
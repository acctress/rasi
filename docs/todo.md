# todo

## IR
- [x] isub, imul, sdiv, udiv, srem, urem
- [x] fadd, fsub, fmul, fdiv
- [x] and, or, xor, shl, shr, sar
- [x] icmp
- [x] fcmp
- [x] load, store, alloca
- [x] call
- [x] ret, br, cbr
- [x] IR formatter

## Codegen
- [ ] x86-64 instruction encoder
- [ ] Instruction selection
- [ ] Linear scan register allocator
- [ ] prologue/epilogue emission
- [x] exec memory (with mmap or virtualalloc)
- [ ] JitCompiler

## Pass pipeline
- [ ] Pass manager shell
- [ ] DCE
- [ ] Constant folding

## Tooling
- [ ] rasic CLI
- [ ] IR text parser

## ISLE
- [x] Lexer
- [x] Parser
- [x] Checker
- [ ] Decision tree
- [ ] C++ emitter
- [ ] CMake regen on `.isle`
- [ ] `x86_64.isle`
- [ ] `IsleContext` x86-64s

## Tests
- [ ] Instruction tests for all IR ops
- [ ] compile and execute jit tests
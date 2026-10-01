use std::collections::HashMap;
use rasi_codegen::{Constraint, MachInst, OpKind, PReg, Reg, RegVisitor, VCode};
use rasi_regalloc::interval::LiveIntervals;
use rasi_regalloc::linear::LinearScan;
use rasi_regalloc::liveness::Liveness;
use crate::{RAX, RCX, RDX};
use crate::X86Inst;

struct Rw<'a>(&'a HashMap<u32, PReg>);
impl RegVisitor for Rw<'_> {
    fn reg(&mut self, r: &mut Reg, kind: OpKind, c: Constraint) {
        if let Reg::V(v) = *r { *r = Reg::P(self.0[&v.0]); }
    }

    fn clobber(&mut self, p: PReg) {}
}

pub struct RegAlloc;

impl RegAlloc {
    pub fn run(vc: &mut VCode<X86Inst>) {
        let liveness = Liveness::compute(vc);
        let intervals = LiveIntervals::build(vc, &liveness);
        let scan = LinearScan::new(intervals.intervals, vec![RAX, RCX, RDX], vec![]);

        let (assignments, spilled) = scan.run();
        assert!(spilled.is_empty(), "spilling not impld");

        for inst in &mut vc.insts {
            inst.visit_regs(&mut Rw(&assignments));
        }
    }
}
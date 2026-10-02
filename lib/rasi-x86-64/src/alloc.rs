use std::collections::HashMap;
use rasi_codegen::{Constraint, MachInst, OpKind, PReg, Reg, RegVisitor, VCode};
use rasi_regalloc::interval::LiveIntervals;
use rasi_regalloc::linear::LinearScan;
use rasi_regalloc::liveness::Liveness;
use rasi_x86_64_asm::regs::Size;
use crate::{RAX, RCX, RDX};
use crate::frame::FrameLayout;
use crate::X86Inst;

const SCRATCH: PReg = PReg::int(11);

struct SpillRw<'a> {
    assignments: &'a HashMap<u32, PReg>,
    spill_slots: &'a HashMap<u32, usize>,
    frame: &'a FrameLayout,
    reloads: Vec<X86Inst>,
    stores: Vec<X86Inst>,
}

impl RegVisitor for SpillRw<'_> {
    fn reg(&mut self, r: &mut Reg, kind: OpKind, _c: Constraint) {
        if let Reg::V(v) = *r {
            if let Some(&idx) = self.spill_slots.get(&v.0) {
                let dsp = self.frame.spill_offset(idx);
                *r = Reg::P(SCRATCH);

                match kind {
                    OpKind::Use => self.reloads.push(X86Inst::MovFromMem { size: Size::S64, dst: Reg::P(SCRATCH), dsp }),
                    OpKind::Def => self.stores.push(X86Inst::MovToMem { size: Size::S64, dsp, src: Reg::P(SCRATCH) }),
                    OpKind::UseDef => {
                        self.reloads.push(X86Inst::MovFromMem { size: Size::S64, dst: Reg::P(SCRATCH), dsp });
                        self.stores.push(X86Inst::MovToMem { size: Size::S64, dsp, src: Reg::P(SCRATCH) });
                    }
                }
            } else {
                *r = Reg::P(self.assignments[&v.0]);
            }
        }
    }

    fn clobber(&mut self, _p: PReg) {}
}

pub struct RegAlloc;

impl RegAlloc {
    pub fn run(vc: &mut VCode<X86Inst>, frame: &FrameLayout) -> i32 {
        let liveness = Liveness::compute(vc);
        let intervals = LiveIntervals::build(vc, &liveness);
        let scan = LinearScan::new(intervals.intervals, vec![RAX, RCX, RDX], vec![]);

        let (assignments, spilled) = scan.run();

        let mut spill_slots: HashMap<u32, usize> = HashMap::new();
        for v in &spilled {
            let idx = spill_slots.len();
            spill_slots.entry(*v).or_insert(idx);
        }

        let mut out = Vec::with_capacity(vc.insts.len());
        for mut inst in vc.insts.drain(..) {
            let mut rw = SpillRw {
                assignments: &assignments,
                spill_slots: &spill_slots,
                frame,
                reloads: Vec::new(),
                stores: Vec::new(),
            };

            inst.visit_regs(&mut rw);

            out.extend(rw.reloads);
            out.push(inst);
            out.extend(rw.stores);
        }

        vc.insts = out;
        frame.total_size(spill_slots.len())
    }
}
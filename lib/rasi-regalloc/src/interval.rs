use std::collections::{HashMap, HashSet};
use std::ops::Range;
use rasi_codegen::{machinst::MachInst, Block, Collector, OpKind, RegClass, VCode};
use crate::liveness::Liveness;

pub type Point = u32;

#[derive(Debug)]
pub struct LiveInterval {
    pub vreg: u32,
    pub class: RegClass,
    pub ranges: Vec<Range<Point>>,
}

#[derive(Debug)]
pub struct LiveIntervals {
    pub intervals: Vec<LiveInterval>
}

impl LiveIntervals {
    pub fn build<I: MachInst + Clone>(vc: &VCode<I>, liveness: &Liveness) -> Self {
        let mut ranges: HashMap<u32, Vec<Range<Point>>> = HashMap::new();

        for b in 0..vc.n_blocks() {
            let block = Block(b as u32);
            let range = &vc.blocks[b];

            let mut first_def: HashMap<u32, Point> = HashMap::new();
            let mut last_use: HashMap<u32, Point> = HashMap::new();

            for (idx, mut inst) in vc.block_insts(block).iter().cloned().enumerate() {
                let abs: Point = range.start + idx as u32;
                let mut c = Collector::default();

                inst.visit_regs(&mut c);
                for (reg, kind, _const) in &c.ops {
                    let Some(vr) = reg.as_vreg() else { continue };

                    let is_use = matches!(kind, OpKind::Use | OpKind::UseDef);
                    let is_def = matches!(kind, OpKind::Def | OpKind::UseDef);

                    if is_use { last_use.insert(vr.0, abs);           }
                    if is_def { first_def.entry(vr.0).or_insert(abs); }
                }
            }

            let live_in = &liveness.blocks[b].live_in;
            let live_out = &liveness.blocks[b].live_out;

            let mut tch: HashSet<u32> = HashSet::new();
            tch.extend(first_def.keys());
            tch.extend(last_use.keys());
            tch.extend(live_in.iter());
            tch.extend(live_out.iter());

            for &vr in &tch {
                let start = if live_in.contains(&vr)  { range.start } else { first_def[&vr]    };
                let end   = if live_out.contains(&vr) { range.end   } else { last_use[&vr] + 1 };

                ranges.entry(vr).or_default().push(start..end);
            }
        }

        let intervals = ranges.into_iter()
            .map(|(vreg, ranges)| LiveInterval { vreg, class: vc.vclass[vreg as usize], ranges })
            .collect();

        LiveIntervals { intervals }
    }
}
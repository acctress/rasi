use std::collections::HashMap;
use rasi_codegen::{PReg, RegClass};
use crate::interval::{LiveInterval, Point};

pub struct LinearScan {
    intervals: Vec<LiveInterval>,
    ints_pool: Vec<PReg>,
    floats_pool: Vec<PReg>,
    active: Vec<(u32, PReg, Point)>,
    assignments: HashMap<u32, PReg>,
    spilled: Vec<u32>
}

impl LinearScan {
    pub fn new(intervals: Vec<LiveInterval>, ints_pool: Vec<PReg>, floats_pool: Vec<PReg>) -> Self {
        Self {
            intervals,
            ints_pool,
            floats_pool,
            active: Vec::new(),
            assignments: HashMap::new(),
            spilled: Vec::new(),
        }
    }

    pub fn run(mut self) -> (HashMap<u32, PReg>, Vec<u32>) {
        self.intervals.sort_by_key(|v| v.start());

        for interval in &self.intervals {
            let mut curr_active: Vec<(u32, PReg, Point)> = Vec::new();
            for (vreg, preg, end) in &self.active {
                if *end <= interval.start() {
                    match preg.class {
                        RegClass::Int   => self.ints_pool.push(*preg),
                        RegClass::Float => self.floats_pool.push(*preg),
                    }
                } else { curr_active.push((*vreg, *preg, *end)); }
            }

            self.active = curr_active;

            let pool = match interval.class {
                RegClass::Int   => &mut self.ints_pool,
                RegClass::Float => &mut self.floats_pool
            };

            if let Some(preg) = pool.pop() {
                self.assignments.insert(interval.vreg, preg);
                self.active.push((interval.vreg, preg, interval.end()));
            } else {
                let vic_idx = self.active.iter().enumerate()
                    .filter(|(_, (_, preg, _))| preg.class == interval.class)
                    .max_by_key(|(_, (_, _, end))| *end)
                    .map(|(i, _)| i);

                match vic_idx {
                    None      => self.spilled.push(interval.vreg),
                    Some(idx) => {
                        let (vic_vreg, vic_preg, vic_end) = self.active[idx];

                        if interval.end() > vic_end {
                            self.spilled.push(interval.vreg);
                        } else {
                            self.active.remove(idx);
                            self.spilled.push(vic_vreg);
                            self.assignments.insert(interval.vreg, vic_preg);
                            self.active.push((interval.vreg, vic_preg, interval.end()));
                        }
                    }
                }
            }
        }

        (self.assignments, self.spilled)
    }
}
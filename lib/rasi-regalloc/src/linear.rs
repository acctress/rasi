use std::collections::HashMap;
use rasi_codegen::PReg;
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

        }

        (self.assignments, self.spilled)
    }
}
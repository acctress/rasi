use std::ops::Range;
use rasi_codegen::RegClass;

pub type Point = u32;

pub struct LiveInterval {
    pub vreg: u32,
    pub class: RegClass,
    pub ranges: Vec<Range<Point>>
}
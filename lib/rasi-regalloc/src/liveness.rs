use rasi_codegen::{machinst::MachInst, VCode};

pub struct BlockLiveness {
    pub live_in: Vec<u32>,
    pub live_out: Vec<u32>,
}

pub struct Liveness {
    pub blocks: Vec<BlockLiveness>
}

pub fn compute_liveness<I: MachInst>(vc: &VCode<I>) -> Liveness {
    todo!()
}
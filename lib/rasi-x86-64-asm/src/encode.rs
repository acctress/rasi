use crate::{operand::{Mem, Rm}, regs::Size};

pub const fn rex(w: bool, r: bool, x: bool, b: bool) -> u8 {
    0x40 | ((w as u8) << 3) | ((r as u8) << 2) | ((x as u8) << 1) | b as u8
}

pub const fn modrm(md: u8, reg: u8, rm: u8) -> u8 {
    (md << 6) | ((reg & 7) << 3) | (rm & 7)
}

pub const fn sib(scale: u8, index: u8, base: u8) -> u8 {
    (scale << 6) | ((index & 7) << 3) | (base & 7)
}

pub fn emit_rm(buf: &mut Vec<u8>, size: Size, opcode: &[u8], reg: u8, rm: &Rm, frex: bool) {
    if size == Size::S16 { buf.push(0x66); }

    let (x, b) = match rm {
        Rm::Reg(r) => (false, r.hi()),
        Rm::Mem(m) => (m.index.is_some_and(|(i, _)| i.hi()), m.base.hi()),
    };

    let (w, r) = (size == Size::S64, reg >= 8);

    if w || r || x || b || frex { buf.push(rex(w, r, x, b)); }
    buf.extend_from_slice(opcode);

    match rm {
        Rm::Reg(r) => buf.push(modrm(0b11, reg, r.enc())),
        Rm::Mem(m) => emit_mem(buf, reg, m),
    }
}

fn emit_mem(buf: &mut Vec<u8>, reg: u8, m: &Mem) {
    let md = if m.disp == 0 && m.base.enc() != 5  { 0b00 }
             else if i8::try_from(m.disp).is_ok() { 0b01 }
             else                                 { 0b10 };

    match m.index {
        None if m.base.enc() == 4 => {
            buf.push(modrm(md, reg, 4));
            buf.push(sib(0, 4, m.base.enc()));
        }

        None => buf.push(modrm(md, reg, m.base.enc())),

        Some((idx, sc)) => {
            buf.push(modrm(md, reg, 4));
            buf.push(sib(sc as u8, idx.enc(), m.base.enc()));
        }
    }

    match md {
        0b01 => buf.push(m.disp as i8 as u8),
        0b10 => buf.extend_from_slice(&m.disp.to_le_bytes()),
        _    => {}
    }
}


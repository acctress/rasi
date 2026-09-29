use std::ffi::c_void;

const PAGE: usize = 4096;

#[cfg(windows)]
mod sys {
    use super::*;

    const MEM_COMMIT: u32 = 0x1000;
    const MEM_RESERVE: u32 = 0x2000;
    const MEM_RELEASE: u32 = 0x8000;
    const PAGE_READWRITE: u32 = 0x04;
    const PAGE_EXECUTE_READ: u32 = 0x20;

    unsafe extern "system" {
        fn VirtualAlloc(addr: *mut c_void, size: usize, ty: u32, prot: u32) -> *mut c_void;
        fn VirtualProtect(addr: *mut c_void, size: usize, new: u32, old: *mut u32) -> i32;
        fn VirtualFree(addr: *mut c_void, size: usize, ty: u32) -> i32;
    }

    pub unsafe fn allow_rw(len: usize) -> *mut u8 {
        unsafe { VirtualAlloc(std::ptr::null_mut(), len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) as *mut u8 }
    }

    pub unsafe fn make_rx(p: *mut u8, len: usize) -> bool {
        let mut old = 0u32; unsafe { VirtualProtect(p as *mut c_void, len, PAGE_EXECUTE_READ, &mut old) != 0 }
    }

    pub unsafe fn free(p: *mut u8, _len: usize) {
        unsafe { VirtualFree(p as *mut c_void, 0, MEM_RELEASE); }
    }
}

#[cfg(unix)]
mod sys {
    use super::*;

    const PROT_READ: i32 = 1;
    const PROT_WRITE: i32 = 2;
    const PROT_EXEC: i32 = 4;
    const MAP_PRIVATE: i32 = 0x02;

    #[cfg(target_os = "linux")]
    const MAP_ANONYMOUS: i32 = 0x20;

    #[cfg(target_os = "macos")]
    const MAP_ANONYMOUS: i32 = 0x1000;

    unsafe extern "C" {
        fn mmap(addr: *mut c_void, len: usize, prot: i32, flags: i32, fd: i32, off: i64) -> *mut c_void;
        fn mprotect(addr: *mut c_void, len: usize, prot: i32) -> i32;
        fn munmap(addr: *mut c_void, len: usize) -> i32;
    }

    pub unsafe fn alloc_rw(len: usize) -> *mut u8 {
        let p = unsafe { mmap(std::ptr::null_mut(), len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) };
        if p as isize == -1 { std::ptr::null_mut() } else { p as *mut u8 }
    }

    pub unsafe fn make_rx(p: *mut u8, len: usize) -> bool {
        unsafe { mprotect(p as *mut c_void, len, PROT_READ | PROT_EXEC) == 0 }
    }

    pub unsafe fn free(p: *mut u8, len: usize) {
        unsafe { munmap(p as *mut c_void, len); }
    }
}

pub struct ExecBuf { ptr: *mut u8, len: usize }

impl ExecBuf {
    pub fn new(code: &[u8]) -> Self {
        let len = code.len().max(1).next_multiple_of(PAGE);

        unsafe {
            let ptr = sys::allow_rw(len);
            assert!(!ptr.is_null(), "exec memory alloc failed");

            std::ptr::copy_nonoverlapping(code.as_ptr(), ptr, code.len());
            assert!(sys::make_rx(ptr, len), "failed to make mem executable");

            Self { ptr, len }
        }
    }

    pub fn as_ptr(&self) -> *const u8 { self.ptr }

    pub unsafe fn as_fn<F: Copy>(&self) -> F {
        debug_assert_eq!(size_of::<F>(), size_of::<usize>());
        unsafe { std::mem::transmute_copy(&self.ptr) }
    }
}

impl Drop for ExecBuf {
    fn drop(&mut self) { unsafe { sys::free(self.ptr, self.len) } }
}
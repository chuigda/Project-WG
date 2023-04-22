#[cfg(windows)] pub mod msg_box;
#[cfg(windows)] pub mod combo_box;

#[cfg(windows)] pub fn convert_u8_u16(input: &str) -> Box<[u16]> {
    input.encode_utf16().chain(Some(0)).collect()
}

#[cfg(not(windows))] pub mod msg_box {
    #[macro_export] macro_rules! message_box {
        ($title:expr, $message:expr) => {}
    }
}

#[cfg(windows)] pub fn set_dpi_awareness() {
    unsafe { winapi::um::winuser::SetProcessDPIAware(); }
}

#[cfg(not(windows))] #[inline(always)] pub const fn set_dpi_awareness() {}

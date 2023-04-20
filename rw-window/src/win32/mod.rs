#[cfg(windows)] pub mod msg_box;
#[cfg(windows)] pub mod combo_box;

#[cfg(windows)]
fn convert_u8_u16(input: &str) -> Box<[u16]> {
    input.encode_utf16().chain(Some(0)).collect()
}

#[cfg(not(windows))] pub mod msg_box {
    #[macro_export] macro_rules! message_box {
        ($title:expr, $message:expr) => {}
    }
}

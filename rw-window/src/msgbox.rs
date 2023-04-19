use std::ffi::c_void;

#[cfg(windows)]
extern "system" {
    fn MessageBoxW(
        h_wnd: *mut c_void,
        lp_text: *const u16,
        lp_captain: *const u16,
        u_type: u32,
    ) -> i32;

    fn MultiByteToWideChar(
        code_page: u32,
        dw_flags: u32,
        lp_multi_byte_str: *const i8,
        cb_multi_byte: i32,
        lp_wide_char_str: *mut u16,
        cch_wide_char: i32,
    ) -> i32;
}

#[cfg(windows)]
fn convert_u8_u16(input: &str) -> Box<[u16]> {
    assert_eq!(input.as_bytes().last(), Some(&0));

    let mut output = vec![0u16; input.len() + 1];
    let len = unsafe {
        MultiByteToWideChar(
            65001, // UTF-8
            0,
            input.as_ptr() as *const i8,
            input.len() as i32,
            output.as_mut_ptr(),
            output.len() as i32,
        )
    };
    output.truncate(len as usize);
    output.shrink_to_fit();
    output.into_boxed_slice()
}

#[cfg_attr(not(windows), allow(unused_variables))]
pub fn message_box(title: &str, message: &str) {
    #[cfg(windows)]
    {
        let title = convert_u8_u16(title);
        let message = convert_u8_u16(message);

        unsafe {
            MessageBoxW(
                std::ptr::null_mut(),
                message.as_ptr(),
                title.as_ptr(),
                0
            );
        }
    }
}

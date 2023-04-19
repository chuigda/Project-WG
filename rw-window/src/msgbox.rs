#[cfg(windows)]
extern "system" {
    fn MessageBoxA(
        h_wnd: *mut std::ffi::c_void,
        lp_text: *const i8,
        lp_captain: *const i8,
        u_type: u32,
    ) -> i32;
}

pub fn message_box(title: &str, message: &str) {
    assert_eq!(title.as_bytes().last(), Some(&b'\0'));
    assert_eq!(message.as_bytes().last(), Some(&b'\0'));

    #[cfg(windows)]
    unsafe {
        MessageBoxA(
            std::ptr::null_mut(),
            message.as_ptr() as *const i8,
            title.as_ptr() as *const i8,
            0,
        );
    }
}

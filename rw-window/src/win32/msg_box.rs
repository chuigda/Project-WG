use winapi::um::winuser::MessageBoxW;

pub fn message_box_impl(title: &str, message: &str) {
    let title = super::convert_u8_u16(title);
    let message = super::convert_u8_u16(message);

    unsafe {
        MessageBoxW(
            std::ptr::null_mut(),
            message.as_ptr(),
            title.as_ptr(),
            0
        );
    }
}

#[macro_export] macro_rules! message_box {
    ($title:expr, $message:expr) => {
        // ensure that the string is null-terminated
        $crate::win32::msg_box::message_box_impl($title, $message)
    }
}

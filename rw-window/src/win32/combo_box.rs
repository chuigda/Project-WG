use std::mem::zeroed;
use std::ptr::{null, null_mut};
use once_cell::sync::OnceCell;
use winapi::shared::minwindef::{LRESULT, WPARAM, LPARAM, UINT, HINSTANCE};
use winapi::shared::windef::{HBRUSH, HWND};
use winapi::um::libloaderapi::GetModuleHandleW;
use winapi::um::winuser::{MSG, WNDCLASSW};
use winapi::um::winuser::{
    CreateWindowExW,
    DispatchMessageW,
    GetMessageW,
    LoadCursorW,
    LoadIconW,
    RegisterClassW,
    ShowWindow,
    TranslateMessage,
    UpdateWindow,
    DestroyWindow,
    PostQuitMessage,
    DefWindowProcW
};
use winapi::um::winuser::{
    COLOR_WINDOW,
    CW_USEDEFAULT,
    IDI_APPLICATION,
    WS_EX_CLIENTEDGE,
    WS_OVERLAPPEDWINDOW,
    WM_CLOSE,
    WM_DESTROY
};
use crate::win32::convert_u8_u16;

#[derive(Debug, Clone)]
pub struct ComboBoxOption {
    title: String,
    description: String
}

const WND_CLASS_NAME: OnceCell<Box<[u16]>> = OnceCell::new();

pub unsafe fn combo_box_impl(title: &str, _options: Vec<ComboBoxOption>) -> usize {
    let binding = WND_CLASS_NAME;
    let wnd_class_name: &Box<[u16]> = binding.get_or_init(|| convert_u8_u16("rw-combobox"));
    let wnd_class_name: *const u16 = wnd_class_name.as_ptr();

    // assert!(options.len() >= 1);

    let h_instance: HINSTANCE = GetModuleHandleW(null());

    let mut wnd_class: WNDCLASSW = zeroed();
    wnd_class.lpszClassName = wnd_class_name;
    wnd_class.style = 0;
    wnd_class.lpfnWndProc = Some(wnd_proc);
    wnd_class.hInstance = h_instance;
    wnd_class.hIcon = LoadIconW(h_instance, IDI_APPLICATION);
    wnd_class.hCursor = LoadCursorW(h_instance, IDI_APPLICATION);
    wnd_class.hbrBackground = COLOR_WINDOW as HBRUSH;

    RegisterClassW(&wnd_class);

    let h_wnd: HWND = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        wnd_class_name,
        convert_u8_u16(title).as_ptr(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        240,
        null_mut(),
        null_mut(),
        h_instance,
        null_mut()
    );

    ShowWindow(h_wnd, 1);
    UpdateWindow(h_wnd);

    let mut msg: MSG = zeroed();
    while GetMessageW(&mut msg, null_mut(), 0, 0) > 0 {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    msg.wParam as _
}

unsafe extern "system" fn wnd_proc(
    h_wnd: HWND,
    msg: UINT,
    w_param: WPARAM,
    l_param: LPARAM
) -> LRESULT {
    match msg {
        WM_CLOSE => {
            DestroyWindow(h_wnd);
            0
        },
        WM_DESTROY => {
            PostQuitMessage(0);
            0
        },
        _ => DefWindowProcW(h_wnd, msg, w_param, l_param)
    }
}

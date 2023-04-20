use std::mem::zeroed;
use std::ptr::{null, null_mut};
use once_cell::sync::OnceCell;
use winapi::shared::minwindef::{LRESULT, WPARAM, LPARAM, UINT, HINSTANCE, LOWORD, HIWORD};
use winapi::shared::windef::{HBRUSH, HWND, HFONT};
use winapi::um::wingdi::{
    CreateFontW,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_CHARSET,
    DEFAULT_PITCH,
    DEFAULT_QUALITY,
    FF_DONTCARE,
    FW_HEAVY,
    OUT_DEFAULT_PRECIS
};
use winapi::um::libloaderapi::GetModuleHandleW;
use winapi::um::winuser::{MSG, WNDCLASSW};
use winapi::um::winuser::{
    CreateWindowExW,
    GetWindowLongPtrW,
    GetWindowLongW,
    SetWindowLongW,
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
    DefWindowProcW,
    SetWindowLongPtrW,
    SendMessageW,
    SetWindowTextW,
    InvalidateRect,
    EnumChildWindows
};
use winapi::um::winuser::{
    COLOR_WINDOW,
    CW_USEDEFAULT,
    IDI_APPLICATION,
    WS_EX_CLIENTEDGE,
    WS_OVERLAPPED,
    WS_CAPTION,
    WS_SYSMENU,
    WM_CLOSE,
    WM_DESTROY,
    WS_CHILD,
    WS_VISIBLE,
    WS_DISABLED,
    WM_SETFONT,
    ES_LEFT,
    ES_READONLY,
    ES_MULTILINE,
    CBS_DROPDOWNLIST,
    GWLP_HINSTANCE,
    GWLP_USERDATA,
    GWL_ID,
    GWL_STYLE,
    CB_ADDSTRING,
    CBN_SELCHANGE,
    CB_GETCURSEL,
    WM_COMMAND
};

use winapi::um::commctrl::{
    WC_COMBOBOX,
    WC_EDIT,
    WC_BUTTONA
};
use crate::win32::convert_u8_u16;

#[derive(Debug, Clone)]
pub struct ComboBoxOption {
    pub title: Box<[u16]>,
    pub description: Box<[u16]>
}

impl ComboBoxOption {
    pub fn new(title: &str, description: &str) -> Self {
        Self {
            title: convert_u8_u16(title),
            description: convert_u8_u16(description)
        }
    }
}

#[derive(Debug)]
struct ComboBoxExtra {
    h_combobox: HWND,
    h_textbox: HWND,
    h_confirm_button: HWND,
    h_cancel_button: HWND,
    selected_item: Option<usize>,
    confirmed: bool,
    options: Vec<ComboBoxOption>
}

const WND_CLASS_NAME: OnceCell<Box<[u16]>> = OnceCell::new();

pub unsafe fn combo_box_impl(title: &str, options: Vec<ComboBoxOption>) -> Option<usize> {
    let binding = WND_CLASS_NAME;
    let wnd_class_name: &Box<[u16]> = binding.get_or_init(|| convert_u8_u16("rw-combobox"));
    let wnd_class_name: *const u16 = wnd_class_name.as_ptr();

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
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        480,
        360,
        null_mut(),
        null_mut(),
        h_instance,
        null_mut()
    );

    let wnd_instance: HINSTANCE = GetWindowLongPtrW(h_wnd, GWLP_HINSTANCE) as HINSTANCE;

    let h_combobox = CreateWindowExW(
        0,
        convert_u8_u16(WC_COMBOBOX).as_ptr(),
        null(),
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        5,
        5,
        450,
        300,
        h_wnd,
        null_mut(),
        wnd_instance,
        null_mut()
    );
    SetWindowLongW(h_combobox, GWL_ID, 1);

    for option in &options {
        SendMessageW(
            h_combobox,
            CB_ADDSTRING,
            0,
            option.title.as_ptr() as LPARAM
        );
    }

    let h_textbox = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        convert_u8_u16(WC_EDIT).as_ptr(),
        null(),
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_READONLY,
        5,
        36,
        450,
        225,
        h_wnd,
        null_mut(),
        wnd_instance,
        null_mut()
    );
    SetWindowLongW(h_textbox, GWL_ID, 2);

    let h_cancel_button = CreateWindowExW(
        0,
        convert_u8_u16(WC_BUTTONA).as_ptr(),
        convert_u8_u16("取消").as_ptr(),
        WS_CHILD | WS_VISIBLE,
        5,
        265,
        80,
        40,
        h_wnd,
        null_mut(),
        wnd_instance,
        null_mut()
    );
    SetWindowLongW(h_cancel_button, GWL_ID, 3);

    let h_confirm_button = CreateWindowExW(
        0,
        convert_u8_u16(WC_BUTTONA).as_ptr(),
        convert_u8_u16("确认").as_ptr(),
        WS_CHILD | WS_VISIBLE | WS_DISABLED,
        375,
        265,
        80,
        40,
        h_wnd,
        null_mut(),
        wnd_instance,
        null_mut()
    );
    SetWindowLongW(h_confirm_button, GWL_ID, 4);

    let user_data_ptr = Box::into_raw(Box::new(ComboBoxExtra {
        h_combobox,
        h_textbox,
        h_confirm_button,
        h_cancel_button,
        selected_item: None,
        confirmed: false,
        options
    }));

    SetWindowLongPtrW(
        h_wnd,
        GWLP_USERDATA,
        user_data_ptr as _
    );

    let font: HFONT = CreateFontW(
        24, 0, 0, 0, FW_HEAVY, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, convert_u8_u16("System").as_ptr()
    );
    EnumChildWindows(h_wnd, Some(set_font), font as _);

    ShowWindow(h_wnd, 1);
    UpdateWindow(h_wnd);

    let mut msg: MSG = zeroed();
    while GetMessageW(&mut msg, null_mut(), 0, 0) > 0 {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Box::from_raw(user_data_ptr).selected_item
}

unsafe extern "system" fn set_font(child: HWND, font: LPARAM) -> i32 {
    SendMessageW(child, WM_SETFONT, font as _, 1);
    1
}

unsafe extern "system" fn wnd_proc(
    h_wnd: HWND,
    msg: UINT,
    w_param: WPARAM,
    l_param: LPARAM
) -> LRESULT {
    match msg {
        WM_COMMAND => {
            let user_data_ptr = GetWindowLongPtrW(h_wnd, GWLP_USERDATA) as *mut ComboBoxExtra;
            let user_data = &mut *user_data_ptr;

            let id = LOWORD(w_param as _);
            let code = HIWORD(w_param as _);

            if id == GetWindowLongW(user_data.h_confirm_button, GWL_ID) as u16 {
                user_data.confirmed = true;
                DestroyWindow(h_wnd);
            } else if id == GetWindowLongW(user_data.h_cancel_button, GWL_ID) as u16 {
                DestroyWindow(h_wnd);
            } else if id == GetWindowLongW(user_data.h_combobox, GWL_ID) as u16 {
                if code == CBN_SELCHANGE {
                    let index = SendMessageW(
                        user_data.h_combobox,
                        CB_GETCURSEL,
                        0,
                        0
                    ) as usize;
                    if index >= user_data.options.len() {
                        return 0;
                    }

                    user_data.selected_item = Some(index);
                    let option = &user_data.options[index];
                    SetWindowTextW(user_data.h_textbox, option.description.as_ptr());
                    SetWindowLongW(user_data.h_confirm_button, GWL_STYLE, (WS_CHILD | WS_VISIBLE) as _);
                    InvalidateRect(h_wnd, null(), 1);
                }
            }
            0
        },
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

#[macro_export] macro_rules! combo_box {
    ($title:expr ; $(($opt_title:expr, $opt_desc:expr)),*) => {
        {
            let options = vec![$(
                $crate::win32::combo_box::ComboBoxOption {
                    title: $crate::win32::convert_u8_u16($opt_title),
                    description: $crate::win32::convert_u8_u16($opt_desc)
                }
            ),*];
            unsafe { $crate::win32::combo_box::combo_box_impl($title, options) }
        }
    };
    ($title:expr, $options:expr) => {
        unsafe { $crate::win32::combo_box::combo_box_impl($title, $options) }
    }
}

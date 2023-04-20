mod win32;

use std::env;
use std::process::exit;
use vulkano::instance::{Instance, InstanceCreateInfo};
use vulkano::VulkanLibrary;
use vulkano_win::VkSurfaceBuild;
use winit::event::{Event, WindowEvent};
use winit::event_loop::{EventLoop, ControlFlow};
use winit::window::WindowBuilder;
use crate::win32::combo_box::{combo_box_impl, ComboBoxOption};

fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "info");
    }
    tracing_subscriber::fmt::init();

    #[cfg(windows)] unsafe { winapi::um::winuser::SetProcessDPIAware(); }

    unsafe {
        combo_box_impl("选择 Vulkan 设备", vec![ComboBoxOption::new(
            "DefaultDevice",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\r
Sed auctor, nisl eget ultricies lacinia, nisl nisl aliquet nisl,\r
eget aliquet nisl nisl eget nisl. Sed auctor, nisl eget ultricies lacinia,\r
nisl nisl aliquet nisl, eget aliquet nisl nisl eget nisl. Sed auctor,\r
nisl eget ultricies lacinia, nisl nisl aliquet nisl, eget aliquet nisl nisl\r
eget nisl. Sed auctor, nisl eget ultricies lacinia, nisl nisl aliquet nisl,\r
eget aliquet nisl nisl eget nisl. Sed auctor, nisl eget ultricies lacinia,\r
nisl nisl aliquet nisl, eget aliquet nisl nisl eget nisl. Sed auctor\r
\r
Cain and Abel\r
\r
1 And Adam knew his wife Eve, \r"
        )]);
    }
    return;

    let library = match VulkanLibrary::new() {
        Ok(library) => library,
        Err(e) => {
            tracing::error!("无法初始化 Vulkan Library: {e}");
            message_box!("错误\0", &format!("无法初始化 Vulkan Library: \r\n{e}\0"));
            exit(-1)
        }
    };
    tracing::info!("Vulkan Library 初始化完成");

    let required_extensions = vulkano_win::required_extensions(&library);
    let vk_instance = Instance::new(library, InstanceCreateInfo {
        enabled_extensions: required_extensions,
        ..Default::default()
    });
    let vk_instance = match vk_instance {
        Ok(instance) => instance,
        Err(e) => {
            tracing::error!("无法初始化 Vulkan 实例: {e}");
            message_box!("错误\0", &format!("无法初始化 Vulkan 实例: \r\n{e}\0"));
            exit(-1)
        }
    };
    tracing::info!("Vulkan 实例初始化完成");

    let event_loop = EventLoop::new();
    let surface = WindowBuilder::new()
        .build_vk_surface(&event_loop, vk_instance.clone());
    match surface {
        Ok(_) => {},
        Err(e) => {
            tracing::error!("无法初始化 winit 窗口: {e}");
            message_box!("错误\0", &format!("无法初始化 winit 窗口: \r\n{e}\0"));
            exit(-1)
        }
    }

    event_loop.run(|event, _, control_flow| {
        match event {
            Event::WindowEvent {
                event: WindowEvent::CloseRequested,
                ..
            } => {
                *control_flow = ControlFlow::Exit;
            },
            _ => ()
        }
    });
}

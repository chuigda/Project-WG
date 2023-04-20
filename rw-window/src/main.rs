mod win32;

use std::env;
use std::process::exit;
use std::sync::Arc;
use vulkano::device::physical::{PhysicalDevice, PhysicalDeviceType};
use vulkano::device::{Device, DeviceCreateInfo, QueueCreateInfo, QueueFlags};
use vulkano::instance::{Instance, InstanceCreateInfo, InstanceExtensions};
use vulkano::swapchain::Surface;
use vulkano::VulkanLibrary;
use vulkano_win::VkSurfaceBuild;
use winit::event::{Event, WindowEvent};
use winit::event_loop::{EventLoop, ControlFlow};
use winit::window::WindowBuilder;

fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "info");
    }
    tracing_subscriber::fmt::init();

    #[cfg(windows)] unsafe { winapi::um::winuser::SetProcessDPIAware(); }

    let library: Arc<VulkanLibrary> = match VulkanLibrary::new() {
        Ok(library) => library,
        Err(e) => {
            tracing::error!("无法初始化 Vulkan Library: {e}");
            message_box!("错误", &format!("无法初始化 Vulkan Library: \r\n{e}"));
            exit(-1)
        }
    };
    tracing::info!("Vulkan Library 初始化完成");

    let required_extensions: InstanceExtensions = vulkano_win::required_extensions(&library);
    let vk_instance: Result<Arc<Instance>, _> = Instance::new(library, InstanceCreateInfo {
        enabled_extensions: required_extensions,
        ..Default::default()
    });
    let vk_instance: Arc<Instance> = match vk_instance {
        Ok(instance) => instance,
        Err(e) => {
            tracing::error!("无法初始化 Vulkan 实例: {e}");
            message_box!("错误", &format!("无法初始化 Vulkan 实例: \r\n{e}"));
            exit(-1)
        }
    };
    tracing::info!("Vulkan 实例初始化完成");

    let physical_device: Arc<PhysicalDevice> = choose_vulkan_device(&vk_instance);
    tracing::info!("选中了 Vulkan 设备: {}", physical_device.properties().device_name);

    let Some(queue_family_index) = physical_device
        .queue_family_properties()
        .iter()
        .enumerate()
        .position(|(_queue_family_index, queue_family_properties)| {
            queue_family_properties.queue_flags.contains(QueueFlags::GRAPHICS)
        }) else
    {
        tracing::error!("选中的设备没有可用的图形队列");
        message_box!("错误", "选中的设备没有可用的图形队列");
        exit(-1)
    };

    let device_creation_result: Result<_, _> = Device::new(
        physical_device,
        DeviceCreateInfo {
            // here we pass the desired queue family to use by index
            queue_create_infos: vec![QueueCreateInfo {
                queue_family_index: queue_family_index as u32,
                ..Default::default()
            }],
            ..Default::default()
        }
    );
    let (device, mut queues) = match device_creation_result {
        Ok(res) => res,
        Err(e) => {
            tracing::error!("无法创建 Vulkan 设备: {e}");
            message_box!("错误", &format!("无法创建 Vulkan 设备: \r\n{e}"));
            exit(-1)
        }
    };

    let event_loop: EventLoop<_> = EventLoop::new();
    let surface: Result<Arc<Surface>, _> = WindowBuilder::new()
        .build_vk_surface(&event_loop, vk_instance.clone());
    match surface {
        Ok(_) => {},
        Err(e) => {
            tracing::error!("无法初始化 winit 窗口: {e}");
            message_box!("错误", &format!("无法初始化 winit 窗口: \r\n{e}"));
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

fn choose_vulkan_device(instance: &Arc<Instance>) -> Arc<PhysicalDevice> {
    use crate::win32::combo_box::ComboBoxOption;

    let physical_devices = (match instance.enumerate_physical_devices() {
        Ok(devices) => devices,
        Err(e) => {
            message_box!("错误", &format!("无法枚举可用的 Vulkan 物理设备: \r\n{e}"));
            exit(-1)
        }
    }).filter(|device| {
        device.queue_family_properties()
            .iter()
            .any(|family| family.queue_flags.contains(QueueFlags::GRAPHICS))
    }).collect::<Box<[_]>>();

    if physical_devices.is_empty() {
        message_box!("错误", "没有可用的 Vulkan 物理设备");
        exit(-1)
    }

    let device_options = physical_devices.iter().map(|device| {
        let properties = device.properties();
        let name = &properties.device_name;

        ComboBoxOption::new(name, &format!(
            "设备 ID: {}\r\n\
            设备类别: {}\r\n\
            Vulkan 版本: {}\r\n\
            驱动程序: {}\r\n\
            驱动程序内部版本号: {}",
            properties.device_id,
            vulkan_device_type_name(properties.device_type),
            properties.api_version,
            properties.driver_name.as_ref().map_or("未知驱动程序", String::as_str),
            properties.driver_version
        ))
    }).collect::<Vec<_>>();

    let Some(choice) = combo_box!("选择 Vulkan 物理设备以继续", device_options) else {
        exit(0)
    };

    physical_devices[choice].clone()
}

fn vulkan_device_type_name(device_type: PhysicalDeviceType) -> &'static str {
    match device_type {
        PhysicalDeviceType::IntegratedGpu => "集成显卡",
        PhysicalDeviceType::DiscreteGpu => "独立显卡",
        PhysicalDeviceType::VirtualGpu => "虚拟 GPU",
        PhysicalDeviceType::Cpu => "CPU",
        _ => "未知设备"
    }
}

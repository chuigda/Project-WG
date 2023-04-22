use std::convert::Infallible;
use std::process::exit;
use std::sync::Arc;
use vulkano::device::physical::PhysicalDeviceType;
use vulkano::device::QueueFlags;
use vulkano::instance::{Instance, InstanceCreateInfo, InstanceExtensions};
use vulkano::VulkanLibrary;
use crate::{combo_box, message_box};
use crate::win32::combo_box::ComboBoxOption;

pub fn init_vk_library() -> Arc<VulkanLibrary> {
    match VulkanLibrary::new() {
        Ok(library) => library,
        Err(e) => {
            tracing::error!("无法初始化 Vulkan Library: {e}");
            message_box!("错误", &format!("无法初始化 Vulkan Library: \r\n{e}"));
            exit(-1)
        }
    }
}

pub fn create_vk_instance(library: Arc<VulkanLibrary>) -> Arc<Instance> {
    let required_extensions: InstanceExtensions = vulkano_win::required_extensions(&library);
    let vk_instance: Result<Arc<Instance>, _> = Instance::new(library, InstanceCreateInfo {
        enabled_extensions: required_extensions,
        ..Default::default()
    });
    match vk_instance {
        Ok(instance) => instance,
        Err(e) => {
            tracing::error!("无法初始化 Vulkan 实例: {e}");
            message_box!("错误", &format!("无法初始化 Vulkan 实例: \r\n{e}"));
            exit(-1)
        }
    }
}

pub fn list_vulkan_devices(instance: &Arc<Instance>, use_log_file: bool) -> Infallible {
    let physical_devices = (match instance.enumerate_physical_devices() {
        Ok(devices) => devices,
        Err(e) => {
            tracing::error!("无法枚举可用的 Vulkan 物理设备: {e}");
            message_box!("错误", &format!("无法枚举可用的 Vulkan 物理设备: \r\n{e}"));
            exit(-1)
        }
    }).filter(|device| {
        device.queue_family_properties()
            .iter()
            .any(|family| family.queue_flags.contains(QueueFlags::GRAPHICS))
    }).collect::<Box<[_]>>();

    if physical_devices.is_empty() {
        tracing::error!("没有可用的 Vulkan 物理设备");
        message_box!("错误", "没有可用的 Vulkan 物理设备");
        exit(-1)
    }

    if use_log_file || cfg!(not(windows)) {
        tracing::info!("正在枚举所有可用的 Vulkan 物理设备");
        for (idx, device) in physical_devices.iter().enumerate() {
            let properties = device.properties();
            let name = &properties.device_name;

            tracing::info!(
                "找到了 Vulkan 物理设备\n\
                 \t设备序号: {}\n\
                 \t设备名称: {}\n\
                 \t设备 ID: {}\n\
                 \t设备类别: {}\n\
                 \tVulkan 版本: {}\n\
                 \t驱动程序: {}\n\
                 \t驱动程序内部版本号: {}\n",
                idx,
                name,
                properties.device_id,
                vulkan_device_type_name(properties.device_type),
                properties.api_version,
                properties.driver_name.as_ref().map_or("未知驱动程序", String::as_str),
                properties.driver_version);
        }
    }

    #[cfg(windows)] {
        let device_options = physical_devices.iter().enumerate().map(|(idx, device)| {
            let properties = device.properties();
            let name = &properties.device_name;

            ComboBoxOption::new(name, &format!(
                "设备序号: {}\r\n\
                 设备 ID: {}\r\n\
                 设备类别: {}\r\n\
                 Vulkan 版本: {}\r\n\
                 驱动程序: {}\r\n\
                 驱动程序内部版本号: {}",
                idx,
                properties.device_id,
                vulkan_device_type_name(properties.device_type),
                properties.api_version,
                properties.driver_name.as_ref().map_or("未知驱动程序", String::as_str),
                properties.driver_version
            ))
        }).collect::<Vec<_>>();

        combo_box!("可用的 Vulkan 设备", device_options, false);
    }

    exit(0)
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

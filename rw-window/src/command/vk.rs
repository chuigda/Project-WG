use std::io::Write;
use std::num::ParseIntError;
use std::process::exit;
use std::sync::Arc;
use vulkano::device::physical::{PhysicalDevice, PhysicalDeviceType};
use vulkano::device::QueueFlags;
use vulkano::instance::{Instance, InstanceCreateInfo, InstanceExtensions};
use vulkano::VulkanLibrary;
use crate::{combo_box, message_box};
use crate::config::RwVulkanConfig;
use crate::opt::RwOptions;

#[cfg(windows)] use crate::win32::combo_box::ComboBoxOption;

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

pub fn list_vulkan_devices(instance: &Arc<Instance>, use_log_file: bool) {
    let physical_devices: Box<[Arc<PhysicalDevice>]> = query_vulkan_physical_devices(instance);

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

    #[cfg(windows)] combo_box!(
        "可用的 Vulkan 设备",
        devices_to_combo_box_options(&physical_devices),
        false
    );

    exit(0)
}

pub fn select_vulkan_device(
    instance: &Arc<Instance>,
    options: &RwOptions,
    config: Option<&RwVulkanConfig>
) -> Arc<PhysicalDevice> {
    let physical_devices: Box<[Arc<PhysicalDevice>]> = query_vulkan_physical_devices(instance);

    let device_idx = options.vulkan_device_index.or(config.and_then(|config| { config.device_index }));
    if let Some(device_idx) = device_idx {
        if device_idx >= physical_devices.len() {
            tracing::error!("Vulkan 设备序号 {} 超出了可用的 Vulkan 设备数量", device_idx);
            message_box!("错误", &format!("Vulkan 设备序号 {} 超出了可用的 Vulkan 设备数量", device_idx));
            exit(-1)
        }

        return physical_devices[device_idx].clone();
    }

    let device_id = options.vulkan_device_id.or(config.and_then(|config| { config.device_id }));
    if let Some(device_id) = device_id {
        for device in physical_devices.iter() {
            if device.properties().device_id == device_id {
                return device.clone();
            }
        }

        tracing::error!("没有找到 Vulkan 设备 ID 为 {} 的设备", device_id);
        message_box!("错误", &format!("没有找到 Vulkan 设备 ID 为 {} 的设备", device_id));
        exit(-1)
    }

    #[cfg(windows)] let device_idx = combo_box!(
        "选择 Vulkan 设备",
        devices_to_combo_box_options(&physical_devices)
    ).unwrap_or_else(|| {
        exit(-1)
    });

    #[cfg(not(windows))] let device_idx = {
        println!("请选择 Vulkan 设备");
        for (idx, device) in physical_devices.iter().enumerate() {
            let properties = device.properties();
            let name = &properties.device_name;

            println!(
                "\t- [{}] 设备名称: {}\n\
                 \t      设备 ID: {}\n\
                 \t      设备类别: {}\n\
                 \t      Vulkan 版本: {}\n\
                 \t      驱动程序: {}\n\
                 \t      驱动程序内部版本号: {}\n",
                idx,
                name,
                properties.device_id,
                vulkan_device_type_name(properties.device_type),
                properties.api_version,
                properties.driver_name.as_ref().map_or("未知驱动程序", String::as_str),
                properties.driver_version);
        }

        let mut input = String::new();
        loop {
            print!("选择 Vulkan 设备序号> ");
            let _ = std::io::stdout().flush();
            match std::io::stdin().read_line(&mut input) {
                Ok(size) => {
                    if size == 0 {
                        tracing::error!("无法从用户输入选择 Vulkan 设备");
                        exit(-1)
                    }
                    match input.trim().parse::<usize>() {
                        Ok(idx) => {
                            if idx >= physical_devices.len() {
                                println!("Vulkan 设备序号 {} 超出了可用的 Vulkan 设备数量", idx);
                                input.clear();
                                continue;
                            }

                            break idx;
                        }
                        Err(_) => {
                            continue;
                        }
                    }
                }
                Err(e) => {
                    tracing::error!("无法从用户输入选择 Vulkan 设备: {e}");
                    exit(-1)
                }
            }
        }
    };

    physical_devices[device_idx].clone()
}

fn query_vulkan_physical_devices(instance: &Arc<Instance>) -> Box<[Arc<PhysicalDevice>]> {
    let physical_devices = (match instance.enumerate_physical_devices() {
        Ok(devices) => devices,
        Err(e) => {
            tracing::error!("无法枚举可用的 Vulkan 物理设备: {e}");
            message_box!("错误", &format!("无法枚举可用的 Vulkan 物理设备: \r\n{e}"));
            exit(-1)
        }
    }).filter(|device| {
        device.properties().api_version.minor >= 3 &&
            device.queue_family_properties()
                .iter()
                .any(|family| family.queue_flags.contains(QueueFlags::GRAPHICS))
    }).collect::<Box<[_]>>();

    if physical_devices.is_empty() {
        tracing::error!("没有可用的 Vulkan 物理设备");
        message_box!("错误", "没有可用的 Vulkan 物理设备");
        exit(-1)
    }

    physical_devices
}

#[cfg(windows)]
fn devices_to_combo_box_options(physical_devices: &[Arc<PhysicalDevice>]) -> Vec<ComboBoxOption> {
    physical_devices.iter().enumerate().map(|(idx, device)| {
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
    }).collect::<Vec<_>>()
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

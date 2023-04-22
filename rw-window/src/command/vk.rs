use std::process::exit;
use std::sync::Arc;
use vulkano::buffer::{C, Subbuffer};
use vulkano::command_buffer::PrimaryAutoCommandBuffer;
use vulkano::device::physical::{PhysicalDevice, PhysicalDeviceType};
use vulkano::device::{Device, DeviceCreateInfo, DeviceExtensions, Queue, QueueCreateInfo, QueueFlags};
use vulkano::image::{ImageUsage, SwapchainImage};
use vulkano::image::view::ImageView;
use vulkano::instance::{Instance, InstanceCreateInfo, InstanceExtensions};
use vulkano::memory::allocator::MemoryAllocator;
use vulkano::pipeline::GraphicsPipeline;
use vulkano::render_pass::{Framebuffer, FramebufferCreateInfo, RenderPass};
use vulkano::swapchain::{Surface, Swapchain, SwapchainCreateInfo};
use vulkano::VulkanLibrary;
use winit::window::Window;
use rw_render::triangle::MeinVertex;
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
    Instance::new(library, InstanceCreateInfo {
        enabled_extensions: required_extensions,
        ..Default::default()
    })
    .unwrap_or_else(|e| {
        tracing::error!("无法初始化 Vulkan 实例: {e}");
        message_box!("错误", &format!("无法初始化 Vulkan 实例: \r\n{e}"));
        exit(-1)
    })
}

pub fn list_vulkan_devices(
    instance: &Arc<Instance>,
    use_log_file: bool,
    surface: &Arc<Surface>
) {
    let physical_devices: Box<[Arc<PhysicalDevice>]>
        = query_vulkan_physical_devices(instance, surface);

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
                properties.driver_version
            );
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
    config: Option<&RwVulkanConfig>,
    surface: &Arc<Surface>
) -> Arc<PhysicalDevice> {
    let physical_devices: Box<[Arc<PhysicalDevice>]>
        = query_vulkan_physical_devices(instance, surface);

    let device_idx = options.vulkan_device_index
        .or(config.and_then(|config| { config.device_index }));
    if let Some(device_idx) = device_idx {
        if device_idx >= physical_devices.len() {
            tracing::error!("Vulkan 设备序号 {} 超出了可用的 Vulkan 设备数量", device_idx);
            message_box!(
                "错误",
                &format!("Vulkan 设备序号 {} 超出了可用的 Vulkan 设备数量", device_idx)
            );
            exit(-1)
        }

        return physical_devices[device_idx].clone();
    }

    let device_id = options.vulkan_device_id
        .or(config.and_then(|config| { config.device_id }));
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

pub fn create_vulkan_device(
    physical_device: &Arc<PhysicalDevice>,
    surface: &Arc<Surface>,
) -> (Arc<Device>, Arc<Queue>) {
    let queue_family_index = physical_device
        .queue_family_properties()
        .iter()
        .enumerate()
        .position(|(_, family)| {
            family.queue_flags.contains(QueueFlags::GRAPHICS)
            && physical_device.surface_support(family.queue_count, surface).unwrap_or(false)
        })
        .unwrap_or_else(|| {
            tracing::error!("选中的设备没有可用的图形队列");
            message_box!("错误", "选中的设备没有可用的图形队列");
            exit(-1)
        });

    let (device, mut queues) = Device::new(
        physical_device.clone(),
        DeviceCreateInfo {
            // here we pass the desired queue family to use by index
            queue_create_infos: vec![QueueCreateInfo {
                queue_family_index: queue_family_index as u32,
                ..Default::default()
            }],
            enabled_extensions: DeviceExtensions {
                khr_swapchain: true,
                ..Default::default()
            },
            ..Default::default()
        }
    )
    .unwrap_or_else(|e| {
        tracing::error!("无法创建 Vulkan 设备: {e}");
        message_box!("错误", &format!("无法创建 Vulkan 设备: \r\n{e}"));
        exit(-1)
    });

    let queue = queues.next().unwrap_or_else(|| {
        tracing::error!("无法获取 Vulkan 设备的图形队列");
        message_box!("错误", "无法获取 Vulkan 设备的图形队列");
        exit(-1)
    });

    (device, queue)
}

fn query_vulkan_physical_devices(
    instance: &Arc<Instance>,
    surface: &Arc<Surface>
) -> Box<[Arc<PhysicalDevice>]> {
    let khr_swapchain_extension: DeviceExtensions = DeviceExtensions {
        khr_swapchain: true,
        ..Default::default()
    };

    let physical_devices = instance
        .enumerate_physical_devices()
        .unwrap_or_else(|e| {
            tracing::error!("无法枚举可用的 Vulkan 物理设备: {e}");
            message_box!("错误", &format!("无法枚举可用的 Vulkan 物理设备: \r\n{e}"));
            exit(-1)
        })
        .filter(|device| {
            device.properties().api_version.minor >= 3 &&
                device.supported_extensions().contains(&khr_swapchain_extension) &&
                device.queue_family_properties()
                    .iter()
                    .any(|family| {
                        family.queue_flags.contains(QueueFlags::GRAPHICS)
                        && device.surface_support(family.queue_count, surface).unwrap_or(false)
                    })
        }).collect::<Box<[_]>>();

    if physical_devices.is_empty() {
        tracing::error!("没有可用的 Vulkan 物理设备");
        message_box!("错误", "没有可用的 Vulkan 物理设备");
        exit(-1)
    }

    physical_devices
}

pub fn create_swapchain(
    physical_device: &Arc<PhysicalDevice>,
    device: &Arc<Device>,
    surface: &Arc<Surface>
) -> (Arc<Swapchain>, Vec<Arc<SwapchainImage>>) {
    let caps = physical_device
        .surface_capabilities(&surface, Default::default())
        .unwrap_or_else(|e| {
            tracing::error!("无法获取 Vulkan 表面的能力: {e}");
            message_box!("错误", &format!("无法获取 Vulkan 表面的能力: \r\n{e}"));
            exit(-1)
        });

    let window = Arc::downcast::<Window>(surface.object().unwrap().clone()).unwrap();
    let dimensions = window.inner_size();
    let composite_alpha = caps.supported_composite_alpha
        .into_iter()
        .next()
        .unwrap_or_else(|| {
            tracing::error!("无法获取 Vulkan 表面的支持的 Alpha 组合");
            message_box!("错误", &format!("无法获取 Vulkan 表面的支持的 Alpha 组合:"));
            exit(-1)
        });
    let image_formats = physical_device
        .surface_formats(&surface, Default::default())
        .unwrap_or_else(|e| {
            tracing::error!("无法获取 Vulkan 表面的支持的图像格式: {e}");
            message_box!("错误", &format!("无法获取 Vulkan 表面的支持的图像格式: \r\n{e}"));
            exit(-1)
        });
    if image_formats.len() == 0 {
        tracing::error!("Vulkan 表面不支持任何图像格式");
        message_box!("错误", "Vulkan 表面不支持任何图像格式");
        exit(-1)
    }

    let image_format = Some(image_formats[0].0);

    Swapchain::new(
        device.clone(),
        surface.clone(),
        SwapchainCreateInfo {
            min_image_count: caps.min_image_count + 1,
            image_format,
            image_extent: dimensions.into(),
            image_usage: ImageUsage::COLOR_ATTACHMENT,
            composite_alpha,
            ..Default::default()
        }
    )
    .unwrap_or_else(|e| {
        tracing::error!("无法创建 Vulkan 交换链: {e}");
        message_box!("错误", &format!("无法创建 Vulkan 交换链: \r\n{e}"));
        exit(-1)
    })
}

pub fn create_render_pass(device: Arc<Device>, swapchain: &Arc<Swapchain>) -> Arc<RenderPass> {
    vulkano::single_pass_renderpass!(
        device,
        attachments: {
            color: {
                load: Clear,
                store: Store,
                format: swapchain.image_format(),
                samples: 1,
            },
        },
        pass: {
            color: [color],
            depth_stencil: {},
        },
    )
    .unwrap_or_else(|e| {
        tracing::error!("无法创建 Vulkan 渲染管线: {e}");
        message_box!("错误", &format!("无法创建 Vulkan 渲染管线: \r\n{e}"));
        exit(-1)
    })
}

pub fn create_framebuffer(
    images: &[Arc<SwapchainImage>],
    render_pass: &Arc<RenderPass>
) -> Box<[Arc<Framebuffer>]> {
    images.iter()
        .map(|image| {
            let view = ImageView::new_default(image.clone()).unwrap();
            Framebuffer::new(
                render_pass.clone(),
                FramebufferCreateInfo {
                    attachments: vec![view],
                    ..Default::default()
                },
            ).unwrap_or_else(|e| {
                tracing::error!("无法创建 Vulkan 帧缓冲: {e}");
                message_box!("错误", &format!("无法创建 Vulkan 帧缓冲: \r\n{e}"));
                exit(-1)
            })
        })
        .collect()
}

pub fn create_command_buffer(
    _device: &Arc<Device>,
    _queue: &Arc<Queue>,
    _pipeline: &Arc<GraphicsPipeline>,
    _framebuffer: &[Arc<Framebuffer>],
    _vertex_buffer: &Arc<Subbuffer<[MeinVertex]>>
) -> Box<[Arc<PrimaryAutoCommandBuffer>]> {
    todo!()
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

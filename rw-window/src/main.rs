mod command;
mod config;
mod opt;
mod win32;

use std::env;
use std::process::exit;
use std::sync::Arc;
use structopt::StructOpt;
use vulkano::device::physical::PhysicalDevice;
use vulkano::instance::Instance;
use vulkano::swapchain::Surface;
use vulkano::VulkanLibrary;
use vulkano_win::VkSurfaceBuild;
use winit::event::{Event, WindowEvent};
use winit::event_loop::{EventLoop, ControlFlow};
use winit::window::WindowBuilder;

use crate::command::log::setup_logger;
use crate::command::vk::*;
use crate::config::{RwConfig, try_read_config};
use crate::opt::RwOptions;

fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "info");
    }
    win32::set_dpi_awareness();

    let options: RwOptions = RwOptions::from_args();

    let config: Option<RwConfig> = if let Some(config_file) = options.config_file.as_ref() {
        try_read_config(&config_file)
    } else {
        try_read_config("config.toml")
    };

    let use_log_file = setup_logger(config.as_ref(), &options);

    if let Some(config_file_name) = options.config_file.as_ref() {
        if config.is_none() {
            tracing::warn!("无法读取指定的配置文件: {config_file_name}");
            message_box!("警告", &format!("无法读取配置文件: \r\n{config_file_name}"));
        }
    }

    if let Some(config) = config.as_ref() {
        tracing::info!("读取了以下配置:\n{}", serde_json::to_string_pretty(&config).unwrap());
    }

    let library: Arc<VulkanLibrary> = init_vk_library();
    tracing::info!("Vulkan Library 初始化完成");

    let vk_instance: Arc<Instance> = create_vk_instance(library);
    tracing::info!("Vulkan 实例初始化完成");

    let event_loop: EventLoop<_> = EventLoop::new();
    let surface: Arc<Surface> = WindowBuilder::new()
        .build_vk_surface(&event_loop, vk_instance.clone())
        .unwrap_or_else(|e| {
            tracing::error!("无法初始化 winit 窗口: {e}");
            message_box!("错误", &format!("无法初始化 winit 窗口: \r\n{e}"));
            exit(-1)
        });
    tracing::info!("winit 窗口创建完成");

    if options.list_devices {
        list_vulkan_devices(&vk_instance, use_log_file, &surface);
    }

    let physical_device: Arc<PhysicalDevice> = select_vulkan_device(
        &vk_instance,
        &options,
        config.as_ref().and_then(|config| config.vulkan.as_ref()),
        &surface
    );
    tracing::info!("选中了 Vulkan 设备: {}", physical_device.properties().device_name);

    let (device, queue) = create_vulkan_device(&physical_device, &surface);
    tracing::info!("Vulkan 设备创建完成");

    let (swapchain, images) = create_swapchain(&physical_device, &device, &surface);
    tracing::info!("Vulkan Swapchain 创建完成");

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

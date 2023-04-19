pub mod msgbox;

use vulkano::instance::{Instance, InstanceCreateInfo};
use vulkano::VulkanLibrary;
use vulkano_win::VkSurfaceBuild;
use winit::event::{Event, WindowEvent};
use winit::event_loop::{EventLoop, ControlFlow};
use winit::window::WindowBuilder;

use rw_protocol::ClientRequest;

use msgbox::message_box;

fn main() {
    message_box("Hi\0", "This is a test dialog, that would only be shown on Windows.\0");

    let library = VulkanLibrary::new().expect("Unable to load local Vulkan library/DLL");
    let required_extensions = vulkano_win::required_extensions(&library);
    let vk_instance = Instance::new(library, InstanceCreateInfo {
        enabled_extensions: required_extensions,
        ..Default::default()
    }).expect("Unable to create Vulkan instance");

    let event_loop = EventLoop::new();
    let surface = WindowBuilder::new()
        .build_vk_surface(&event_loop, vk_instance.clone())
        .unwrap();

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

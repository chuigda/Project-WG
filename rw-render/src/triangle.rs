use std::sync::Arc;
use vulkano::buffer::BufferContents;
use vulkano::device::Device;
use vulkano::pipeline::graphics::input_assembly::InputAssemblyState;
use vulkano::pipeline::graphics::vertex_input::Vertex;
use vulkano::pipeline::graphics::viewport::{Viewport, ViewportState};
use vulkano::pipeline::GraphicsPipeline;
use vulkano::render_pass::{RenderPass, Subpass};
use vulkano::shader::ShaderModule;
use crate::error::RenderError;

#[derive(BufferContents, Vertex)]
#[repr(C)]
pub struct MeinVertex {
    #[format(R32G32_SFLOAT)]
    position: [f32; 2],

    #[format(R32G32B32_SFLOAT)]
    color: [f32; 3]
}

mod vs {
    vulkano_shaders::shader! {
        ty: "vertex",
        src: "
            #version 460

            layout(location = 0) in vec2 position;
            layout(location = 1) in vec3 color;

            layout(location = 0) out vec3 v_color;

            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                v_color = color;
            }
        "
    }
}

mod fs {
    vulkano_shaders::shader! {
        ty: "fragment",
        src: "
            #version 460
            layout(location = 0) in vec3 v_color;
            layout(location = 0) out vec4 f_color;

            void main() {
                f_color = vec4(v_color, 1.0);
            }
        "
    }
}

pub fn create_pipeline(
    device: Arc<Device>,
    render_pass: Arc<RenderPass>,
    viewport: Viewport
) -> Result<Arc<GraphicsPipeline>, RenderError> {
    let vs: Arc<ShaderModule> = vs::load(device.clone())?;
    let fs: Arc<ShaderModule> = fs::load(device.clone())?;

    GraphicsPipeline::start()
        .vertex_input_state(MeinVertex::per_vertex())
        .vertex_shader(vs.entry_point("main").unwrap(), ())
        .input_assembly_state(InputAssemblyState::new())
        .viewport_state(ViewportState::viewport_fixed_scissor_irrelevant([viewport]))
        .fragment_shader(fs.entry_point("main").unwrap(), ())
        .render_pass(Subpass::from(render_pass, 0).unwrap())
        .build(device)
        .map_err(RenderError::from)
}

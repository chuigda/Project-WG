use vulkano::pipeline::graphics::GraphicsPipelineCreationError;
use vulkano::shader::ShaderCreationError;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum RenderError {
    #[error("shader creation failed")]
    ShaderCreationError(#[from] ShaderCreationError),

    #[error("graphics pipeline creation failed")]
    GraphicsPipelineCreationError(#[from] GraphicsPipelineCreationError)
}

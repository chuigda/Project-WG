fn main() -> std::io::Result<()> {
    prost_build::compile_protos(&["src/protocol.proto"], &["src/"])?;
    Ok(())
}

use prost_build::Config;

fn main() -> std::io::Result<()> {
    // println!("cargo:rerun-if-changed=src/state.proto");
    // println!("cargo:rerun-if-changed=src/request.proto");
    // println!("cargo:rerun-if-changed=src/response.proto");
    // println!("cargo:rerun-if-changed=src/protocol.proto");
    //
    // let mut prost = Config::new();
    // prost.message_attribute(".rw.protocol.BasicState", "#[repr(C)]");
    // prost.compile_protos(&["src/protocol.proto"], &["src/"])?;
    Ok(())
}

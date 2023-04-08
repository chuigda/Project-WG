use std::fs::OpenOptions;
use std::io::Write;
use pr21::io_ctx::{IOContext, Type21};
use bis_anim::ctx::Context;

fn type21_to_ctype(input: Type21) -> &'static str {
    match input {
        Type21::Int32 => "std::int32_t",
        Type21::Float32 => "float",
    }
}

fn main() {
    let mut header_file = OpenOptions::new()
        .read(false)
        .write(true)
        .create(true)
        .truncate(true)
        .open("include/bis/BISContext.h")
        .unwrap();

    write!(header_file, "{}", include_str!("cdef_pre.h.txt")).unwrap();
    for (_, field, ty) in <Context as IOContext>::metadata() {
        writeln!(
            header_file,
            "  {} {};",
            type21_to_ctype(ty),
            field
        ).unwrap();
    }
    write!(header_file, "{}", include_str!("cdef_post.h.txt")).unwrap();

    let mut source_file = OpenOptions::new()
        .read(false)
        .write(true)
        .create(true)
        .truncate(true)
        .open("src/bis/BISContext.cc")
        .unwrap();
    write!(source_file, include_str!("cdef.c.txt"), std::mem::size_of::<Context>()).unwrap();
}

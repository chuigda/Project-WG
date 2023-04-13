def write_binseq(file, content: bytes):
    file.write(len(content).to_bytes(4, byteorder="little"))
    file.write(content)


def read_binseq(file):
    length = file.read(4)
    if length == b"":
        return None
    length = int.from_bytes(length, byteorder="little")
    content = file.read(length)
    if len(content) != length:
        raise Exception("Unexpected EOF")
    return content

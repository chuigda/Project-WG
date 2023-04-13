import zlib

Example_Data = """[{"name":"FaceAngleX","addedBy":"VTube Studio","value":45.78,"min":-30,"max":30,"defaultValue":0},{"name":"FacePositionX","addedBy":"VTube Studio","value":8.33,"min":-10,"max":10,"defaultValue":0},{"name":"FaceAngleY","addedBy":"VTube Studio","value":32.78,"min":-30,"max":30,"defaultValue":0},{"name":"FacePositionY","addedBy":"VTube Studio","value":48.33,"min":-10,"max":10,"defaultValue":0},{"name":"FaceAngleZ","addedBy":"VTube Studio","value":32.78,"min":-30,"max":30,"defaultValue":0},{"name":"FacePositionZ","addedBy":"VTube Studio","value":48.33,"min":-10,"max":10,"defaultValue":0},{"name":"LeftEye","addedBy":"VTube Studio","value":79.33,"min":0,"max":100,"defaultValue":0},{"name":"RightEye","addedBy":"VTube Studio","value":79.33,"min":0,"max":100,"defaultValue":0},{"name":"MouthOpen","addedBy":"VTube Studio","value":57.1633,"min":0,"max":100,"defaultValue":0}]"""

def compress(data: str):
    raw_bytes = data.encode("utf-8")
    return zlib.compress(raw_bytes, level=9)


def decompress(data: bytes):
    return zlib.decompress(data).decode("utf-8")

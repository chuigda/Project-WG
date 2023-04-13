import json


def json_stringify(obj):
    return json.dumps(obj, separators=(',', ':'))
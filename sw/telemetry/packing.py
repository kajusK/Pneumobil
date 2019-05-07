#!/usr/bin/python3
#
# Packing and unpacking data of various types
#

def pack_uint8(data):
    return [data & 0xff]

def pack_uint16(data):
    return [ord(c) for c in struct.pack('>H', data)]

def pack_uint32(data):
    return [ord(c) for c in struct.pack('>I', data)]

def unpack_uint8(data):
    return data

def unpack_uint16(data):
    return struct.unpack('>I', ''.join([chr(x) for x in data]))[0]

def unpack_uint32(data):
    return struct.unpack('>H', ''.join([chr(x) for x in data]))[0]

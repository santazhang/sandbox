#!/usr/bin/env python

import os
import sys
import struct
from common import *
from varint import varint


# fmt1: <tag1><field1>..<tagN><fieldN>

class Codec(object):
    def __init__(self):
        self.data = ""

    def _pack(self, fmt, data):
        self.data += struct.pack(fmt, data)
    
    def _unpack(self, fmt):
        u = struct.unpack_from(fmt, self.data)
        self.data = self.data[struct.calcsize(fmt):]
        return u[0]

    def write_bool(self, b):
        if b:
            self._pack('b', TRUE_WIRE)
        else:
            self._pack('b', FALSE_WIRE)

    def read_bool(self):
        res = self._unpack('b')
        if res == TRUE_WIRE:
            return True
        elif res == FALSE_WIRE:
            return False
        else:
            raise "%o is not bool wire type!" % res

    def write_i8(self, v):
        self._pack('b', FIX8_WIRE)
        self._pack('b', v)

    def read_i8(self):
        w = self._unpack('b')
        assert w == FIX8_WIRE
        return self._unpack('b')

    def _pack_size_t(self, v):
        vi = varint(v)
        print "-- packing varint:", v, "->", repr(vi.dump())
        self.data += vi.dump()

    def _unpack_size_t(self):
        sz = varint.decode_size_from_first_byte(bytearray(self.data[0])[0])
        d = self.data[:sz]
        self.data = self.data[sz:]
        return varint.load(d)

    def _fetch_bytes(self, count):
        b = self.data[0:count]
        self.data = self.data[count:]
        return b

    def write_str(self, s):
        self._pack_size_t(len(s))
        self.data += s
    
    def read_str(self):
        n = self._unpack_size_t()
        return self._fetch_bytes(n)

    def __str__(self):
        return repr(self.data) + "(len=%d)" % len(self.data)

def test_func(f):
    def wrap():
        print "------ " + f.__name__ + " -------"
        f()
        print "--------------------"
        print
    return wrap

@test_func
def test_bool():
    # a single bool message
    codec = Codec()
    codec.write_bool(True)
    print "true:", codec
    print codec.read_bool()
    codec = Codec()
    codec.write_bool(False)
    print "false:", codec
    print codec.read_bool()

@test_func
def test_i8():
    codec = Codec()
    codec.write_i8(0)
    print "after write 0", codec
    codec.write_i8(127)
    print "after write 127", codec
    codec.write_i8(-128)
    print "after write -128", codec
    print codec.read_i8()
    print codec
    print codec.read_i8()
    print codec
    print codec.read_i8()
    print codec

@test_func
def test_string():
    codec = Codec()
    codec.write_str("hello world")
    print "after write hello world", codec
    codec.write_str("this is a very long text--" * 4 + "." * 23) # 127 bytes
    print "after write long text", codec
    print codec.read_str()
    print codec
    print codec.read_str()
    print codec

def test_primitive():
    test_bool()
    test_i8()
    test_string()


if __name__ == "__main__":
    test_primitive()

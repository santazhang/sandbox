#!/usr/bin/env python
# python2 only

import struct
from varint import varint

BOOL_TYPE = 1
TRUE_TYPE = 1
FALSE_TYPE = 2
I8_TYPE = 3
I16_TYPE = 4
I32_TYPE = 5
I64_TYPE = 6
VARINT_TYPE = 7
FLOAT_TYPE = 8
DOUBLE_TYPE = 9
STRING_TYPE = 10
LIST_TYPE = 11
MAP_TYPE = 12
OBJ_TYPE = 13

TYPE_LIMIT = 16 # valid types: 0 to (TYPE_LIMIT - 1), inclusive

class NeoFmt(object):

    def __init__(self):
        self._data = ""

    def __str__(self):
        return "len=%d " % len(self._data) + repr(self._data)

    def _pack(self, fmt, data):
        self._data += struct.pack(fmt, data)

    def _unpack(self, fmt):
        u = struct.unpack_from(fmt, self._data)
        self._data = self._data[struct.calcsize(fmt):]
        return u[0]

    def _read_size_t(self):
        sz = varint.decode_size_from_first_byte(bytearray(self._data[0])[0])
        d = self._data[:sz]
        self._data = self._data[sz:]
        return varint.load(d)

    def _pack_size_t(self, v):
        vi = varint(v)
        self._data += vi.dump()

    def _read_i8(self):
        return self._unpack('b')
    
    def _read_i16(self):
        return self._unpack('h')

    def _read_i32(self):
        return self._unpack('i')

    def _read_i64(self):
        return self._unpack('q')
    
    def _read_varint(self):
        sz = varint.decode_size_from_first_byte(bytearray(self._data[0])[0])
        d = self._data[:sz]
        self._data = self._data[sz:]
        return varint.zigzag_decode(varint.load(d))

    def _read_float(self):
        return self._unpack('f')

    def _read_double(self):
        return self._unpack('d')

    def write_bool(self, b):
        if b:
            self._data += struct.pack('B', TRUE_TYPE)
        else:
            self._data += struct.pack('B', FALSE_TYPE)

    def write_i8(self, v):
        self._data += struct.pack('B', I8_TYPE)
        self._data += struct.pack('b', v)
    
    def write_i16(self, v):
        self._pack('B', I16_TYPE)
        self._pack('h', v)

    def write_i32(self, v):
        self._pack('B', I32_TYPE)
        self._pack('i', v)

    def write_i64(self, v):
        self._pack('B', I64_TYPE)
        self._pack('q', v)
    
    def write_varint(self, v):
        self._pack('B', VARINT_TYPE)
        u = varint.zigzag_encode(v)
        vi = varint(u)
        self._data += vi.dump()

    def write_float(self, v):
        self._pack('B', FLOAT_TYPE)
        self._pack('f', v)

    def write_double(self, v):
        self._pack('B', DOUBLE_TYPE)
        self._pack('d', v)

    def write_string(self, s):
        self._pack('B', STRING_TYPE)
        self._pack_size_t(len(s))
        self._data += s
    
    def _fetch_bytes(self, count):
        b = self._data[0:count]
        self._data = self._data[count:]
        return b
    
    def _read_string(self):
        n = self._read_size_t()
        return self._fetch_bytes(n)

    def write_i8_list(self, lst):
        self._pack('B', LIST_TYPE)
        self._pack('B', I8_TYPE)
        sub = ""
        for e in lst:
            sub += struct.pack('b', e)
        self._pack('I', len(sub))
        self._data += sub

    def dump(self):
        """print debug dump of content"""
        print self
        if len(self._data) == 0:
            print "(empty)"
        while len(self._data) > 0:
            v = self._read_size_t()
            if v == TRUE_TYPE:
                print "bool:", True
            elif v == FALSE_TYPE:
                print "bool:", False
            elif v == I8_TYPE:
                print "i8:", self._read_i8()
            elif v == I16_TYPE:
                print "i16:", self._read_i16()
            elif v == I32_TYPE:
                print "i32:", self._read_i32()
            elif v == I64_TYPE:
                print "i32:", self._read_i64()
            elif v == VARINT_TYPE:
                print "varint:", self._read_varint()
            elif v == FLOAT_TYPE:
                print "float:", self._read_float()
            elif v == DOUBLE_TYPE:
                print "double:", self._read_double()
            elif v == STRING_TYPE:
                print "string:", self._read_string()
            elif v == LIST_TYPE:
                print "list:",
                self.dump_list()
                print
            else:
                raise RuntimeError("oops")

    def write_with_type(self, type_info, val):
        pass

    def dump_with_type(self, type_info):
        pass

    def dump_list(self):
        ty = self._read_i8()
        if ty == I8_TYPE:
            byte_count = self._unpack('I')
            print '[',
            for i in range(byte_count):
                print self._unpack('b'),
            print ']',
        else:
            raise RuntimeError("oops in dumping list")


def main():
    fmt = NeoFmt()
    fmt.write_bool(True)
    fmt.write_bool(False)
    fmt.write_i8(0)
    fmt.write_i8(1)
    fmt.write_i8(-1)
    fmt.write_i8(-128)
    fmt.write_i8(127)
    fmt.write_i16(0)
    fmt.write_i16(2 ** 15 - 1)
    fmt.write_i16(-2 ** 15)
    fmt.write_i32(0)
    fmt.write_i32(2 ** 31 - 1)
    fmt.write_i32(-2 ** 31)
    fmt.write_i64(0)
    fmt.write_i64(2 ** 63 - 1)
    fmt.write_i64(-2 ** 63)
    fmt.write_varint(0)
    fmt.write_varint(-1)
    fmt.write_varint(128)
    fmt.write_varint(-128)
    fmt.write_varint(2 ** 63 - 1)
    fmt.write_varint(-2 ** 63)
    fmt.write_float(1.8)
    fmt.write_float(1.8e19)
    fmt.write_double(3.14)
    fmt.write_double(3.94 ** 3)
    fmt.write_string("hello world")
    fmt.write_string("hello world" * 20)
    fmt.write_i8_list([1, 2, 3, -128])
    fmt.write_with_type([LIST_TYPE, STRING_TYPE], ["hello", "world"])
    fmt.write_with_type([MAP_TYPE, STRING_TYPE, VARINT_TYPE], {"hello":384, "world":-93939})
    fmt.dump()
    print '--'
    fmt.dump()

if __name__ == "__main__":
    main()

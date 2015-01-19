# Variable size integers
# Yang Zhang <y@yzhang.net>, 2014
# See LICENSE file for copyright notice


from __future__ import division

import struct


class varint(object):
    """Variable size integer"""

    def __init__(self, value=0):
        """Constructor"""
#        assert -2**63 <= value and value < 2**63
        self.v = value

    def value(self):
        """Get integer value"""
        return self.v

    def dump(self):
        """Dump integer value to varint representation"""
        u = self.v
        b = bytearray(struct.pack("<Q", u))
        if u < 2**7:
            out = bytearray(1)
            out[0] = b[0]
        elif u < 2**14:
            out = bytearray(2)
            out[1:] = b[:1]
            out[0] = b[1] | 0x80
        elif u < 2**21:
            out = bytearray(3)
            out[1:] = b[:2]
            out[0] = b[2] | 0xC0
        elif u < 2**28:
            out = bytearray(4)
            out[1:] = b[:3]
            out[0] = b[3] | 0xE0
        elif u < 2**35:
            out = bytearray(5)
            out[1:] = b[:4]
            out[0] = b[4] | 0xF0
        elif u < 2**42:
            out = bytearray(6)
            out[1:] = b[:5]
            out[0] = b[5] | 0xF8
        elif u < 2**49:
            out = bytearray(7)
            out[1:] = b[:6]
            out[0] = b[6] | 0xFC
        elif u < 2**56:
            out = bytearray(8)
            out[1:] = b[:7]
            out[0] = 0xFE
        else:
            out = bytearray(9)
            out[1:] = b[:8]
            out[0] = 0xFF  # Special case
        return bytes(out)

    @staticmethod
    def decode_size_from_first_byte(byte0):
        """Given the first byte in varint, get the total size of varint"""
        if byte0 & 0x80 == 0:
            return 1
        elif byte0 & 0xC0 == 0x80:
            return 2
        elif byte0 & 0xE0 == 0xC0:
            return 3
        elif byte0 & 0xF0 == 0xE0:
            return 4
        elif byte0 & 0xF8 == 0xF0:
            return 5
        elif byte0 & 0xFC == 0xF8:
            return 6
        elif byte0 & 0xFE == 0xFC:
            return 7
        elif byte0 == 0xFE:
            return 8
        else:
            return 9

    @classmethod
    def value_to_memory_size(cls, value):
        """Given the integer value, get the total size of varint"""
        assert -2**63 <= value and value < 2**63
        u = value
        if u < 2**7:
            return 1
        elif u < 2**14:
            return 2
        elif u < 2**21:
            return 3
        elif u < 2**28:
            return 4
        elif u < 2**35:
            return 5
        elif u < 2**42:
            return 6
        elif u < 2**49:
            return 7
        elif u < 2**56:
            return 8
        else:
            return 9

    @classmethod
    def load(cls, data):
        """Dump the varint into integer value"""
        assert isinstance(data, bytes)
        msb = bytearray(data[0:1])[0]
        size = cls.decode_size_from_first_byte(msb)
        assert 0 < size and size <= 9
        if size < 9:
            b = bytearray(data[1:size])
            original_msb = msb & (0xFF >> size)
            b.append(original_msb)
            if len(b) < 8:
                b += bytearray(8 - len(b))
            u = struct.unpack("<Q", bytes(b))[0]
        else:  # size == 9
            u = struct.unpack("<Q", data[1:9])[0]
        return u

    # From google protocol buffer
    #
    #       int32 ->     uint32
    # -------------------------
    #           0 ->          0
    #          -1 ->          1
    #           1 ->          2
    #          -2 ->          3
    #         ... ->        ...
    #  2147483647 -> 4294967294
    # -2147483648 -> 4294967295
    #
    #        >> encode >>
    #
    #       (+) a -> 2a
    #       (-) a -> -2a - 1
    #
    #        << decode <<
    #
    #           b <- 2b
    #          -b <- 2b - 1
    @staticmethod
    def zigzag_encode(v):
        """Helper function to unify encoding of signed and unsigned integers"""
        if v < 0:
            return -2 * v - 1
        else:
            return 2 * v

    @staticmethod
    def zigzag_decode(v):
        """Helper function to unify encoding of signed and unsigned integers"""
        if v % 2 == 1:
            return -(v + 1) // 2
        else:
            return v // 2

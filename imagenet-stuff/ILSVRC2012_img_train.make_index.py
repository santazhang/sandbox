#!/usr/bin/env python

# based on tarindexer.py

from __future__ import print_function

import tarfile
import os

class FileSlice(object):
    def __init__(self, fname, offset, length):
        self.f = open(fname, 'rb')
        self.offset = offset
        self.length = length
        self.pos = self.offset
        self.f.seek(self.offset)

    def read(self, nbytes=None):
        if nbytes is None:
            ret = self.f.read(self.length - self.pos)
        else:
            if self.pos + nbytes > self.offset + self.length:
                nbytes = self.offset + self.length - self.pos
            ret = self.f.read(nbytes)
        self.pos += len(ret)
        return ret

    def close(self):
        if self.f:
            self.f.close()
        self.f = None


dbtarfile = "ILSVRC2012_img_train.tar"
indexfile = "ILSVRC2012_img_train.index"

filesize = os.path.getsize(dbtarfile)
lastpercent = 0

with tarfile.open(dbtarfile, 'r|') as db:
    with open(indexfile, 'w') as outfile:
        for tarinfo in db:
            # free ram...
            db.members = []
            fileslice = FileSlice(dbtarfile, tarinfo.offset_data, tarinfo.size)
            with tarfile.open(tarinfo.name, 'r|', fileslice) as db2:
                for tarinfo2 in db2:
                    # free ram...
                    db2.members = []
                    currentseek = tarinfo.offset_data + tarinfo2.offset_data
                    print("%s %d %d" % (tarinfo2.name,
                                        tarinfo.offset_data + tarinfo2.offset_data,
                                        tarinfo2.size))
            fileslice.close()

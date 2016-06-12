from __future__ import print_function

import os
import random
import hashlib

def run_cmd(cmd):
    print("+ %s" % cmd)
    os.system(cmd)

def random_token(size=5):
    token = ""
    alphabet = "abcdefghijklmnopqrst0123456789"
    for i in range(size):
        token += alphabet[random.randint(0, len(alphabet) - 1)]
    return token

def md5_of_str(s):
    md5 = hashlib.md5()
    md5.update(s)
    return md5.hexdigest()

def mkdir_p(dpath):
    if not os.path.exists(dpath):
        os.makedirs(dpath)

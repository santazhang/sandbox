#!/bin/bash

gcc -I/usr/lib/ruby/1.8/i686-linux/ c-call-ruby.c -c -o c-call-ruby.o

gcc c-call-ruby.o -lruby1.8 -o c-call-ruby



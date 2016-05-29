#!/bin/bash

/Users/santa/Downloads/caffe/build/tools/caffe test \
    -model layer.prototxt -weights _iter_36625.caffemodel \
        -iterations 1000

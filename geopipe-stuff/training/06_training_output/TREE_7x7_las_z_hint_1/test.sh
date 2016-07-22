#!/bin/bash

cd $(dirname $0)

../../../deps/_build/caffe/build/tools/caffe test \
    -model network.prototxt -weights _iter_250000.caffemodel

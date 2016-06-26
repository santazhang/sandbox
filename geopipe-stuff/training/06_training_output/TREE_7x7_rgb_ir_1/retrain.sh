#!/bin/bash

cd $(dirname $0)

rm *.solverstate *.caffemodel

../../../deps/_build/caffe/build/tools/caffe train \
    -solver solver.prototxt

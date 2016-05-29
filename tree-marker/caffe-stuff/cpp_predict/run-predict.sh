#!/bin/bash


LD_LIBRARY_PATH=/Users/santa/Downloads/caffe/build/lib/ ./predict \
	deploy.prototxt \
	tmp.caffemodel

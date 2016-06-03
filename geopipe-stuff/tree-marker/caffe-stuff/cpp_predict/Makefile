# WORKS ON SANTA-WHITE

.PHONY: all clean

all: predict

clean:
	rm -f predict

predict: predict.cc
	g++  $^ -I/Users/santa/Downloads/caffe/include/ \
	-I /Users/santa/Downloads/caffe/build/src \
	-I /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers/ \
	 -L/Users/santa/Downloads/caffe/build/lib/ -lcaffe -lboost_system -lglog -lprotobuf \
	 -o $@

run-predict: predict
	LD_LIBRARY_PATH=/Users/santa/Downloads/caffe/build/lib/ ./predict \
		deploy.prototxt \
		tmp.caffemodel


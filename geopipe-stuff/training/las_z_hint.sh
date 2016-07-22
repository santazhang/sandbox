#!/bin/bash

cd $(dirname $0)

source ../activate.sh

../build/las2stats $@

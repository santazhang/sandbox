#!/bin/bash

HADOOP_DIR="/Users/santa/Downloads/hadoop-2.7.2"
. $HADOOP_DIR/libexec/hadoop-config.sh

mkdir -p out/production

javac src/yarn_workflow_scheduler/*.java -cp $CLASSPATH:src \
    -d out/production

jar -cf yarn_workflow_scheduler.jar -C out/production/ .

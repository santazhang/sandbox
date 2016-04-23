#!/bin/bash

source $(dirname $0)/env.sh
. $HADOOP_HOME/libexec/hadoop-config.sh
mkdir -p out/production
javac $(find src -name "*.java") -cp $CLASSPATH:src -d out/production
jar -cf yarn_workflow.jar -C out/production/ .

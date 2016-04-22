#!/bin/bash

export JAVA_HOME="/Library/Java/JavaVirtualMachines/jdk1.8.0_51.jdk/Contents/Home"

source activate-hadoop-2.7.2.sh

(
    cd /Users/santa/Workspace/sandbox/yarn_workflow_scheduler;
    ./build_jar.sh
)

hadoop jar /Users/santa/Workspace/sandbox/yarn_workflow_scheduler/yarn_workflow_scheduler.jar yarn_workflow_scheduler.Client \
    -jar /Users/santa/Workspace/sandbox/yarn_workflow_scheduler/yarn_workflow_scheduler.jar \
    -num_containers 3 -workflow_dir /Users/santa/Workspace/sandbox/yarn_workflow_scheduler/workflow_example

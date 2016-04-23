#!/bin/bash

source $(dirname $0)/../../env.sh

cd $(dirname $0)/../..
./build.sh
cd -

hadoop jar $(dirname $0)/../../yarn_workflow.jar yarn_workflow.Client \
    -jar $(dirname $0)/../../yarn_workflow.jar \
    -workflow_dir $(dirname $0)/../../workflow_example \
    $@

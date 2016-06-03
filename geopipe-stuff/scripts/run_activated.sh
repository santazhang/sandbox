#!/bin/bash
#
# Run commands with activate.sh sourced
#
# Yang Zhang <y@yzhang.net>, 2015
# See LICENSE file for copyright notice

if [ -z "$WORKSPACE_GEOPIPE_STUFF" ]; then
    . $(dirname $0)/../activate.sh
fi

$@

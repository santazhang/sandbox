#!/bin/bash
# searches for the line number where finish the script and start the tar.gz
SKIP=`awk '/^__PAYLOAD_CONTENT__/ { print NR + 1; exit 0; }' $0`
# remember our file name
THIS=`pwd`/$0
# take the tarfile and pipe it into tar
tail -n +$SKIP $THIS | tar -xz
# Any script here will happen after the tar file extract.
if [ $# -gt 0 ]; then
    bash $@
fi
exit $?
# NOTE: Don't place any newline characters after the last line below.
__PAYLOAD_CONTENT__

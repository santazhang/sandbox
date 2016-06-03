# Setup environment
#
# Yang Zhang <y@yzhang.net>, 2014
# See LICENSE file for copyright notice

SOURCE="${BASH_SOURCE[0]}"
# Resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    # If $SOURCE was a relative symlink, we need to resolve it relative to the
    # path where the symlink file was located
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

export WORKSPACE_GEOPIPE_STUFF=$DIR
export PATH=$WORKSPACE_GEOPIPE_STUFF/build:$PATH

#!/bin/bash

# Script to install and configure SystemTap on Ubuntu.
# Needs root privilege.
#
# Author: Santa Zhang <santa1987@gmail.com>
#
# Usage: ./ubuntu_install_systemtap.sh

# check root privilege
if [[ $UID -ne 0 ]]; then
  echo "This script requires root privilege!"
  exit 1
fi

# check if distribution is really Ubuntu
if grep -q "Ubuntu" "/etc/issue" ; then
  echo # do nothing
else
  echo "Sorry, seems you are not running a Ubuntu distribution!"
  exit 1
fi

clear
echo
echo This script will install and configure SystemTap on Ubuntu.
echo Network connection is required since apt-get will be used to download packages.
echo
read -p "Press ENTER to start installation, or press Ctrl+C to quit..."
clear

# working dir
working_dir=ubuntu_install_systemtap.files

mkdir -p ${working_dir}
pushd ${working_dir} > /dev/null

# exit handler, removes dirty files
function exit_script() {
  popd > /dev/null
  rm -rf ${working_dir}
  exit $1
}

# install systemtap
apt-get install -y systemtap || exit_script 1
apt-get install -y systemtap-sdt-dev || exit_script 1

# install build dependency for linux
apt-get install -y dpkg-dev debhelper gawk || exit_script 1

# preparing build env for linux
apt-get build-dep -y --no-install-recommends linux-image-$(uname -r) || exit_script 1
apt-get source -y linux-image-$(uname -r) || exit_script 1
cd linux-*
fakeroot debian/rules clean
AUTOBUILD=1 fakeroot debian/rules binary-generic skipdbg=false
dpkg -i ../linux-image-*.ddeb

exit_script 0


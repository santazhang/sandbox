#!/bin/bash

if [[ $UID -ne 0 ]]; then
  echo "This script requires root privilege!"
  exit 1
fi

echo
echo "This script automatically configures Ubuntu to my style (server mode)."
echo "It will install some frequently used packages, and personalize vim editor."
echo

read -p "Press any key to start installation..."

# phase 1: update apt-get source

echo
echo "Phase 1: updating apt-get source..."
apt-get update

# phase 2: install debs

echo
echo "Phase 2: installing packages..."

DEBS_LIST_FILE=$(readlink -f ./debs.list.server)
all_debs=( $( cat $DEBS_LIST_FILE ) )
(yes | apt-get install ${all_debs[@]}) || exit 1

# phase 3: config system, start the ruby script

echo
echo "Phase 3: configuring system..."

ruby my_ubuntu.rb server

echo 
echo "All job done. Enjoy!"


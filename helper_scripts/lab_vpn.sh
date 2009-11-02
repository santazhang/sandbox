#!/bin/bash
#########################################################################
# Author: Santa Zhang
# Created Time: 2009-11-02 19:51:15
# File Name: lab_vpn.sh
# Description: connect to lab vpn 
#########################################################################

if [[ $UID -ne 0 ]]; then
  echo "$0 must be run as root!"
  exit 1
fi

pptpsetup --create myvpn --server 166.111.131.55 --username fit --password vpn@fit --start
route add -net 0.0.0.0 gw 10.0.11.254

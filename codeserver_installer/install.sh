#!/bin/bash

# where is the script?
SCRIPT_ROOT=$(dirname $0)

# show info
echo
echo "This package provides a simple code server. It runs redmine & gitosis, and supports svn & git."
echo "The installer script does the following jobs:"
echo
echo "  * apt-get install: subversion, git-core, ruby, rails, rake, rubygems, mongrel, python-setuptools, openssh-server, lftp"
echo "  * gem install: rack(1.0.1)"
echo "  * adduser: git"
echo
read -p "Press ENTER to continue, or cancel by Ctrl-C..."
clear

# install deb packages
echo "Installing deb packages..."
sudo apt-get install subversion git-core ruby rails rake rubygems mongrel python-setuptools openssh-server lftp

# install gem packages
echo "Installing gem packages..."
pushd $SCRIPT_ROOT/data > /dev/null
sudo gem install -l *.gem --no-ri --no-rdoc
popd > /dev/null

# add the "git" user
sudo adduser --system --shell /bin/sh --gecos 'git version control' --group --disabled-password --home /home/git git

ruby $SCRIPT_ROOT/install_stage2.rb

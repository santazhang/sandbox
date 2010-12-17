#!/bin/bash

# This script will install ffmpeg and many encoding libraries.
#
# Author::  Santa Zhang (santa1987@gmail.com)

# check root privilege
if [[ $UID -ne 0 ]]; then
  echo "This script requires root privilege!"
  exit 1
fi

clear
echo
echo This script will install ffmpeg and many encoding libraries.
echo
read -p "Press ENTER to start installation, or press Ctrl+C to quit..."
clear

# where is the script?
SCRIPT_ROOT=$(dirname $0)
RUNNING_ROOT=$(pwd)

echo ==================================
echo Phase 1: install faad
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/faad2-2.7.tar.gz /tmp
cd /tmp
tar zxf faad2-2.7.tar.gz
cd faad2-2.7
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf faad2-2.7 faad2-2.7.tar.gz
cd $RUNNING_ROOT

echo ==================================
echo Phase 2: install faac
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/faac-1.28.tar.gz /tmp
cd /tmp
tar zxf faac-1.28.tar.gz
cd faac-1.28
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf faac-1.28 faac-1.28.tar.gz
cd $RUNNING_ROOT

echo ==================================
echo Phase 3: install lame
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/lame-398-2.tar.gz /tmp
cd /tmp
tar zxf lame-398-2.tar.gz
cd lame-398-2
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf lame-398-2 lame-398-2.tar.gz
cd $RUNNING_ROOT

echo ==================================
echo Phase 4: install libogg
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/libogg-1.2.2.tar.gz /tmp
cd /tmp
tar zxf libogg-1.2.2.tar.gz
cd libogg-1.2.2
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf libogg-1.2.2 libogg-1.2.2.tar.gz
cd $RUNNING_ROOT

echo ==================================
echo Phase 5: install libvorbis
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/libvorbis-1.3.1.tar.bz2 /tmp
cd /tmp
bunzip2 libvorbis-1.3.1.tar.bz2
tar xf libvorbis-1.3.1.tar
cd libvorbis-1.3.1
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf libvorbis-1.3.1 libvorbis-1.3.1.tar
cd $RUNNING_ROOT

echo ==================================
echo Phase 6: install libtheora
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/libtheora-1.1.1.tar.bz2 /tmp
cd /tmp
bunzip2 libtheora-1.1.1.tar.bz2
tar xf libtheora-1.1.1.tar
cd libtheora-1.1.1
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf libtheora-1.1.1 libtheora-1.1.1.tar
cd $RUNNING_ROOT

echo ==================================
echo Phase 7: install libvpx
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/libvpx-v0.9.5.tar.bz2 /tmp
cd /tmp
bunzip2 libvpx-v0.9.5.tar.bz2
tar xf libvpx-v0.9.5.tar
cd libvpx-v0.9.5
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf libvpx-v0.9.5 libvpx-v0.9.5.tar
cd $RUNNING_ROOT

echo ==================================
echo Phase 8: install xvidcore
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/xvidcore-1.2.2.tar.gz /tmp
cd /tmp
tar zxf xvidcore-1.2.2.tar.gz
cd xvidcore/build/generic
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf xvidcore xvidcore-1.2.2.tar.gz
cd $RUNNING_ROOT

echo ==================================
echo Phase 9: install yasm
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/yasm-1.1.0.tar.gz /tmp
cd /tmp
tar zxf yasm-1.1.0.tar.gz
cd yasm-1.1.0
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf yasm-1.1.0 yasm-1.1.0.tar.gz
cd $RUNNING_ROOT

echo ==================================
echo Phase 10: install h264
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/x264-snapshot-20101216-2245.tar.bz2 /tmp
cd /tmp
bunzip2 x264-snapshot-20101216-2245.tar.bz2
tar xf x264-snapshot-20101216-2245.tar
cd x264-snapshot-20101216-2245
./configure --prefix=/usr
make
make install
cd /tmp
rm -rf x264-snapshot-20101216-2245 x264-snapshot-20101216-2245.tar
cd $RUNNING_ROOT

echo ==================================
echo Phase 11: install ffmpeg
echo ==================================
cd $RUNNING_ROOT
cp $SCRIPT_ROOT/ffmpeg-0.6.1.tar.gz /tmp
cd /tmp
tar zxf ffmpeg-0.6.1.tar.gz
cd ffmpeg-0.6.1
./configure --prefix=/usr --enable-gpl --enable-nonfree --enable-pthreads --enable-zlib --enable-bzlib --enable-libfaac --enable-libfaad --enable-libmp3lame --enable-libtheora --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libxvid
make
make install
cd /tmp
rm -rf ffmpeg-0.6.1 ffmpeg-0.6.1.tar.gz
cd $RUNNING_ROOT

echo Everything done!


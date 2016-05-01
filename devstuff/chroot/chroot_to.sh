#!/bin/bash

NEW_ROOT=$1

if [ "$NEW_ROOT" = "" ]; then
    echo "USAGE: $0 NEW_ROOT"
    exit 1
fi

#set -x
#set -e

copy_files() {
    local from=$1
    local to="$NEW_ROOT/$from"
    local to_dir=$(dirname $to)
    mkdir -p "$to_dir"
    rm -rf "$to"
    cp -rv "$from" "$to"
}

mkdir -p $NEW_ROOT

copy_files /usr/lib/dyld
copy_files /bin/sh
copy_files /bin/bash
copy_files /bin/ls
copy_files /bin/mkdir

#./otool-ls-r $NEW_ROOT/bin/bash
copy_files /usr/lib/libauto.dylib
copy_files /usr/lib/libc++.1.dylib
copy_files /usr/lib/libc++abi.dylib
copy_files /usr/lib/libDiagnosticMessagesClient.dylib
copy_files /usr/lib/libncurses.5.4.dylib
copy_files /usr/lib/libobjc.A.dylib
copy_files /usr/lib/libSystem.B.dylib
copy_files /usr/lib/libutil.dylib
copy_files /usr/lib/system/libcache.dylib
copy_files /usr/lib/system/libcommonCrypto.dylib
copy_files /usr/lib/system/libcompiler_rt.dylib
copy_files /usr/lib/system/libcopyfile.dylib
copy_files /usr/lib/system/libcorecrypto.dylib
copy_files /usr/lib/system/libdispatch.dylib
copy_files /usr/lib/system/libdyld.dylib
copy_files /usr/lib/system/libkeymgr.dylib
copy_files /usr/lib/system/liblaunch.dylib
copy_files /usr/lib/system/libmacho.dylib
copy_files /usr/lib/system/libquarantine.dylib
copy_files /usr/lib/system/libremovefile.dylib
copy_files /usr/lib/system/libsystem_asl.dylib
copy_files /usr/lib/system/libsystem_blocks.dylib
copy_files /usr/lib/system/libsystem_c.dylib
copy_files /usr/lib/system/libsystem_configuration.dylib
copy_files /usr/lib/system/libsystem_coreservices.dylib
copy_files /usr/lib/system/libsystem_coretls.dylib
copy_files /usr/lib/system/libsystem_dnssd.dylib
copy_files /usr/lib/system/libsystem_info.dylib
copy_files /usr/lib/system/libsystem_kernel.dylib
copy_files /usr/lib/system/libsystem_m.dylib
copy_files /usr/lib/system/libsystem_malloc.dylib
copy_files /usr/lib/system/libsystem_network.dylib
copy_files /usr/lib/system/libsystem_networkextension.dylib
copy_files /usr/lib/system/libsystem_notify.dylib
copy_files /usr/lib/system/libsystem_platform.dylib
copy_files /usr/lib/system/libsystem_pthread.dylib
copy_files /usr/lib/system/libsystem_sandbox.dylib
copy_files /usr/lib/system/libsystem_secinit.dylib
copy_files /usr/lib/system/libsystem_trace.dylib
copy_files /usr/lib/system/libunc.dylib
copy_files /usr/lib/system/libunwind.dylib
copy_files /usr/lib/system/libxpc.dylib

sudo chroot -g staff -u santa $NEW_ROOT #/bin/bash

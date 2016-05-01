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
copy_files /usr/local/bin/lua

#./otool-ls-r $NEW_ROOT/bin/bash, ls, mkdir...
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

# httphere (go program)
copy_files /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
copy_files /System/Library/Frameworks/Security.framework/Versions/A/Security
copy_files /usr/lib/libicucore.A.dylib
copy_files /usr/lib/libz.1.dylib
copy_files /usr/lib/libbsm.0.dylib
copy_files /usr/lib/libxar.1.dylib
copy_files /usr/lib/libsqlite3.dylib
copy_files /usr/lib/libpam.2.dylib
copy_files /System/Library/Frameworks/IOKit.framework/Versions/A/IOKit
copy_files /usr/lib/libbz2.1.0.dylib
copy_files /usr/lib/libxml2.2.dylib
copy_files /usr/lib/libenergytrace.dylib
copy_files /usr/lib/system/libkxld.dylib

# wine
# __files_to_copy=(
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Accelerate
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vImage.framework/Versions/A/vImage
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libBLAS.dylib
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libLAPACK.dylib
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libLinearAlgebra.dylib
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libSparseBLAS.dylib
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libvDSP.dylib
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libvMisc.dylib
#     /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/vecLib
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/ApplicationServices
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/ATS.framework/Versions/A/ATS
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/ATS.framework/Versions/A/Resources/libFontParser.dylib
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/ATS.framework/Versions/A/Resources/libFontRegistry.dylib
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/ColorSync.framework/Versions/A/ColorSync
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/HIServices.framework/Versions/A/HIServices
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/LangAnalysis.framework/Versions/A/LangAnalysis
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/PrintCore.framework/Versions/A/PrintCore
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/QD.framework/Versions/A/QD
#     /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/SpeechSynthesis.framework/Versions/A/SpeechSynthesis
#     /System/Library/Frameworks/AudioToolbox.framework/Versions/A/AudioToolbox
#     /System/Library/Frameworks/AudioUnit.framework/Versions/A/AudioUnit
#     /System/Library/Frameworks/CFNetwork.framework/Versions/A/CFNetwork
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Carbon
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/CommonPanels.framework/Versions/A/CommonPanels
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/HIToolbox
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/Help.framework/Versions/A/Help
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/ImageCapture.framework/Versions/A/ImageCapture
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/Ink.framework/Versions/A/Ink
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/OpenScripting.framework/Versions/A/OpenScripting
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/Print.framework/Versions/A/Print
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/SecurityHI.framework/Versions/A/SecurityHI
#     /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/SpeechRecognition.framework/Versions/A/SpeechRecognition
#     /System/Library/Frameworks/CoreAudio.framework/Versions/A/CoreAudio
#     /System/Library/Frameworks/CoreBluetooth.framework/Versions/A/CoreBluetooth
#     /System/Library/Frameworks/CoreData.framework/Versions/A/CoreData
#     /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
#     /System/Library/Frameworks/CoreGraphics.framework/Versions/A/CoreGraphics
#     /System/Library/Frameworks/CoreImage.framework/Versions/A/CoreImage
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/AE
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/CarbonCore
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/DictionaryServices.framework/Versions/A/DictionaryServices
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/FSEvents.framework/Versions/A/FSEvents
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/LaunchServices
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/Metadata.framework/Versions/A/Metadata
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/OSServices.framework/Versions/A/OSServices
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/SearchKit.framework/Versions/A/SearchKit
#     /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/SharedFileList.framework/Versions/A/SharedFileList
#     /System/Library/Frameworks/CoreText.framework/Versions/A/CoreText
#     /System/Library/Frameworks/CoreVideo.framework/Versions/A/CoreVideo
#     /System/Library/Frameworks/CoreWLAN.framework/Versions/A/CoreWLAN
#     /System/Library/Frameworks/DiskArbitration.framework/Versions/A/DiskArbitration
#     /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
#     /System/Library/Frameworks/GSS.framework/Versions/A/GSS
#     /System/Library/Frameworks/IOBluetooth.framework/Versions/A/IOBluetooth
#     /System/Library/Frameworks/IOKit.framework/Versions/A/IOKit
#     /System/Library/Frameworks/IOSurface.framework/Versions/A/IOSurface
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/ImageIO
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libGIF.dylib
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libJP2.dylib
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libJPEG.dylib
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libPng.dylib
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libRadiance.dylib
#     /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libTIFF.dylib
#     /System/Library/Frameworks/Kerberos.framework/Versions/A/Kerberos
#     /System/Library/Frameworks/Metal.framework/Versions/A/Metal
#     /System/Library/Frameworks/NetFS.framework/Versions/A/NetFS
#     /System/Library/Frameworks/OpenCL.framework/Versions/A/OpenCL
#     /System/Library/Frameworks/OpenDirectory.framework/Versions/A/Frameworks/CFOpenDirectory.framework/Versions/A/CFOpenDirectory
#     /System/Library/Frameworks/OpenDirectory.framework/Versions/A/OpenDirectory
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libCVMSPluginSupport.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libCoreFSCache.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libCoreVMClient.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGFXShared.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGLImage.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGLU.dylib
#     /System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL
#     /System/Library/Frameworks/QuartzCore.framework/Versions/A/QuartzCore
#     /System/Library/Frameworks/Security.framework/Versions/A/Security
#     /System/Library/Frameworks/SecurityFoundation.framework/Versions/A/SecurityFoundation
#     /System/Library/Frameworks/ServiceManagement.framework/Versions/A/ServiceManagement
#     /System/Library/Frameworks/SystemConfiguration.framework/Versions/A/SystemConfiguration
#     /System/Library/PrivateFrameworks/Apple80211.framework/Versions/A/Apple80211
#     /System/Library/PrivateFrameworks/AppleJPEG.framework/Versions/A/AppleJPEG
#     /System/Library/PrivateFrameworks/AppleVPA.framework/Versions/A/AppleVPA
#     /System/Library/PrivateFrameworks/ChunkingLibrary.framework/Versions/A/ChunkingLibrary
#     /System/Library/PrivateFrameworks/CommonAuth.framework/Versions/A/CommonAuth
#     /System/Library/PrivateFrameworks/CoreUI.framework/Versions/A/CoreUI
#     /System/Library/PrivateFrameworks/CoreWiFi.framework/Versions/A/CoreWiFi
#     /System/Library/PrivateFrameworks/CrashReporterSupport.framework/Versions/A/CrashReporterSupport
#     /System/Library/PrivateFrameworks/DesktopServicesPriv.framework/Versions/A/DesktopServicesPriv
#     /System/Library/PrivateFrameworks/FaceCore.framework/Versions/A/FaceCore
#     /System/Library/PrivateFrameworks/GPUCompiler.framework/libmetal_timestamp.dylib
#     /System/Library/PrivateFrameworks/GenerationalStorage.framework/Versions/A/GenerationalStorage
#     /System/Library/PrivateFrameworks/Heimdal.framework/Versions/A/Heimdal
#     /System/Library/PrivateFrameworks/IOAccelerator.framework/Versions/A/IOAccelerator
#     /System/Library/PrivateFrameworks/IconServices.framework/Versions/A/IconServices
#     /System/Library/PrivateFrameworks/LanguageModeling.framework/Versions/A/LanguageModeling
#     /System/Library/PrivateFrameworks/MultitouchSupport.framework/Versions/A/MultitouchSupport
#     /System/Library/PrivateFrameworks/NetAuth.framework/Versions/A/NetAuth
#     /System/Library/PrivateFrameworks/Sharing.framework/Versions/A/Sharing
#     /System/Library/PrivateFrameworks/SpeechRecognitionCore.framework/Versions/A/SpeechRecognitionCore
#     /System/Library/PrivateFrameworks/TCC.framework/Versions/A/TCC
#     /System/Library/PrivateFrameworks/login.framework/Versions/A/Frameworks/loginsupport.framework/Versions/A/loginsupport
#     /usr/lib/libCRFSuite.dylib
#     /usr/lib/libChineseTokenizer.dylib
#     /usr/lib/libDiagnosticMessagesClient.dylib
#     /usr/lib/libFosl_dynamic.dylib
#     /usr/lib/libOpenScriptingUtil.dylib
#     /usr/lib/libSystem.B.dylib
#     /usr/lib/libarchive.2.dylib
#     /usr/lib/libauto.dylib
#     /usr/lib/libbsm.0.dylib
#     /usr/lib/libbz2.1.0.dylib
#     /usr/lib/libc++.1.dylib
#     /usr/lib/libc++abi.dylib
#     /usr/lib/libcmph.dylib
#     /usr/lib/libcompression.dylib
#     /usr/lib/libcups.2.dylib
#     /usr/lib/libenergytrace.dylib
#     /usr/lib/libextension.dylib
#     /usr/lib/libheimdal-asn1.dylib
#     /usr/lib/libiconv.2.dylib
#     /usr/lib/libicucore.A.dylib
#     /usr/lib/liblangid.dylib
#     /usr/lib/liblzma.5.dylib
#     /usr/lib/libmarisa.dylib
#     /usr/lib/libmecabra.dylib
#     /usr/lib/libobjc.A.dylib
#     /usr/lib/libpam.2.dylib
#     /usr/lib/libresolv.9.dylib
#     /usr/lib/libsqlite3.dylib
#     /usr/lib/libxar.1.dylib
#     /usr/lib/libxml2.2.dylib
#     /usr/lib/libxslt.1.dylib
#     /usr/lib/libz.1.dylib
#     /usr/lib/system/libcache.dylib
#     /usr/lib/system/libcommonCrypto.dylib
#     /usr/lib/system/libcompiler_rt.dylib
#     /usr/lib/system/libcopyfile.dylib
#     /usr/lib/system/libcorecrypto.dylib
#     /usr/lib/system/libdispatch.dylib
#     /usr/lib/system/libdyld.dylib
#     /usr/lib/system/libkeymgr.dylib
#     /usr/lib/system/libkxld.dylib
#     /usr/lib/system/liblaunch.dylib
#     /usr/lib/system/libmacho.dylib
#     /usr/lib/system/libquarantine.dylib
#     /usr/lib/system/libremovefile.dylib
#     /usr/lib/system/libsystem_asl.dylib
#     /usr/lib/system/libsystem_blocks.dylib
#     /usr/lib/system/libsystem_c.dylib
#     /usr/lib/system/libsystem_configuration.dylib
#     /usr/lib/system/libsystem_coreservices.dylib
#     /usr/lib/system/libsystem_coretls.dylib
#     /usr/lib/system/libsystem_dnssd.dylib
#     /usr/lib/system/libsystem_info.dylib
#     /usr/lib/system/libsystem_kernel.dylib
#     /usr/lib/system/libsystem_m.dylib
#     /usr/lib/system/libsystem_malloc.dylib
#     /usr/lib/system/libsystem_network.dylib
#     /usr/lib/system/libsystem_networkextension.dylib
#     /usr/lib/system/libsystem_notify.dylib
#     /usr/lib/system/libsystem_platform.dylib
#     /usr/lib/system/libsystem_pthread.dylib
#     /usr/lib/system/libsystem_sandbox.dylib
#     /usr/lib/system/libsystem_secinit.dylib
#     /usr/lib/system/libsystem_trace.dylib
#     /usr/lib/system/libunc.dylib
#     /usr/lib/system/libunwind.dylib
#     /usr/lib/system/libxpc.dylib
#
#     /usr/share/terminfo/78/xterm-256color
# )
# for file1 in "${__files_to_copy[@]}"; do
#     copy_files $file1
# done

# lua
copy_files /usr/local/Cellar/lua/5.2.4_3/lib/liblua.5.2.dylib
copy_files /usr/lib/libedit.3.dylib

# python
copy_files /usr/local/bin/python
copy_files /usr/local/Cellar/python/2.7.11/Frameworks/Python.framework/Versions/2.7/Python
copy_files /usr/local/Cellar/python/2.7.11/Frameworks/Python.framework/Versions/2.7/Resources/Python.app/Contents/MacOS/Python
touch /usr/local/Cellar/python/2.7.11/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site.py

copy_files /usr/bin/touch

# ruby
copy_files /usr/bin/ruby
copy_files /System/Library/Frameworks/Ruby.framework/Versions/2.0/usr/lib/libruby.2.0.0.dylib
copy_files /System/Library/Frameworks/Ruby.framework/Versions/2.0/usr/lib/ruby/2.0.0/

sudo chroot -g staff -u santa $NEW_ROOT /bin/bash

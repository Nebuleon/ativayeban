#!/bin/sh

# usage: path/to/make_opk.sh [<binary directory> <data directory>]
# The parameters are needed, because cmake supports out-of-source builds.
# When missing, a in-source build is assumed and default directories are set.

mydir=`dirname $0`

if [ -z "$1" ]; then
  datadir=${mydir}/..
else
  datadir=$1
fi

# to prevent interfering with an existing desktop build, build in a separate directory
mkdir -p "${datadir}/pkg/gcw_build"
cd "${datadir}/pkg/gcw_build"
# this is important, otherwise the existing CMakeCache will be used (and overwritten)
touch CMakeCache.txt

cmake -DCMAKE_TOOLCHAIN_FILE="/opt/gcw0-toolchain/usr/share/buildroot/toolchainfile.cmake" -DCMAKE_BUILD_TYPE=Release ../../
make clean
make
cd -

bin="${datadir}/pkg/gcw_build/falling_time"

data="${datadir}/data ${datadir}/COPYRIGHT"

gcwzdata="${mydir}/default.gcw0.desktop"
gcwzdata="${gcwzdata} ${datadir}/data/graphics/icon.png"

alldata="${gcwzdata} ${bin} ${data}"

/opt/gcw0-toolchain/usr/bin/mksquashfs ${alldata} falling_time.opk -all-root -noappend -no-exports -no-xattrs

#!/bin/bash
##############################################################################
#
#
# Install Target RunTime Libraries
#
##############################################################################

if [ "${ECEA5305_PRJROOT}" == "x" ]
then
    echo "ECEA5305_PRJROOT is not set."
    exit 1
fi

echo -n "Installing Target RunTime Libraries..."

if [ -L "${ECEA5305_PRJROOT}"/tools/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libc.so.6 ]
then
    cp -a "${ECEA5305_PRJROOT}"/tools/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib/*.so* \
        fs/lib/.
    cp -a "${ECEA5305_PRJROOT}"/tools/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/*.so* \
        fs/lib64/.
    echo " Done."
else
    echo " Cannont Find C-Runtime."
fi


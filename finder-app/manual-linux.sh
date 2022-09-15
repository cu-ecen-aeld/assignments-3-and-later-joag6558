#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64

echo "Assignment 3 part 2h"

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"

echo "Making rootfs directory"
mkdir -p ${OUTDIR}/rootfs
mkdir -p ${OUTDIR}/rootfs/scripts
mkdir -p ${OUTDIR}/rootfs/assignments
mkdir -p ${OUTDIR}/rootfs/assignments/conf

if [ ! -e "${OUTDIR}/rootfs/Makefile" ]
then

   echo "copying rootfs files "

   cp ${FINDER_APP_DIR}/assignments/rootfs/Makefile ${OUTDIR}/rootfs/
   cp ${FINDER_APP_DIR}/assignments/rootfs/Makefile.ecea5305 ${OUTDIR}/rootfs/
   cp -r ${FINDER_APP_DIR}/assignments/rootfs/scripts/* ${OUTDIR}/rootfs/scripts/
   cp -r /__w/assignments-3-and-later-joag6558/assignments-3-and-later-joag6558/conf/* ${OUTDIR}/rootfs/assignments/conf/
   cp -r ${FINDER_APP_DIR}/* ${OUTDIR}/rootfs/assignments/



   make -C ${OUTDIR}/rootfs OUTDIR="${OUTDIR}" all
fi


if [ ! -d "${OUTDIR}/kernel" ]
then
    echo "making kernel directory"
    mkdir -p ${OUTDIR}/kernel
fi

if [ ! -d "${OUTDIR}/sysapps" ]
then
    echo "making kernel directory"
    mkdir -p ${OUTDIR}/sysapps
fi

cd "$OUTDIR"

if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e "${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image" ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    cp ${FINDER_APP_DIR}/assignments/kernel/Makefile ${OUTDIR}/kernel/
    cp ${FINDER_APP_DIR}/assignments/kernel/linux-5.1.10-ecea5305_defconfig ${OUTDIR}/kernel/
    make -C ${OUTDIR}/kernel OUTDIR="${OUTDIR}" all
    make -C ${OUTDIR}/kernel OUTDIR="${OUTDIR}" install
      
fi


cd "$OUTDIR"

if [ ! -d "${OUTDIR}/busybox" ]
then
    git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    cp ${FINDER_APP_DIR}/assignments/sysapps/Makefile ${OUTDIR}/sysapps/
    cp ${FINDER_APP_DIR}/assignments/sysapps/busybox-1.33.1-arm64_defconfig ${OUTDIR}/
    make -C ${OUTDIR}/sysapps OUTDIR="${OUTDIR}" all
    make -C ${OUTDIR}/sysapps OUTDIR="${OUTDIR}" install
   
fi


echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs/stage" ]
then
   echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
   sudo rm  -rf ${OUTDIR}/rootfs/stage
fi

make -C ${OUTDIR}/rootfs OUTDIR="${OUTDIR}" firmware

cp -a ${OUTDIR}/rootfs/stage/initramfs.cpio.gz ${OUTDIR}/


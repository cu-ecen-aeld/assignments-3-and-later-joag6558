#!/bin/bash
#########################################################################
#
# www.kernel.org/doc/Documentation/filesystems/ramfs-rootfs-initramfs.txt
# 
# Copyright 2006 Rob Landley <rob@landley.net> and TimeSys Corporation.
# 
# Licensed under GPL version 2
#
#########################################################################

if [ $# -ne 2 ]
then
  echo "usage: mkinitramfs directory imagename.cpio.gz"
  exit 1
fi

if [ -d "$1" ]
then
  echo "creating $2 from $1"
  (cd "$1"; find . | cpio -o -H newc --owner root:root | gzip) > "$2"
else
  echo "First argument must be a directory"
  exit 1
fi

exit 0

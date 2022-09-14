#!/bin/sh
##############################################################################
#
# Script to find a string in the given directory
#
# To Use:
#
#       ./finder.sh /tmp/aesd/assignment1 linux
##############################################################################

set -e
set -u

#echo "number of arguments ${#}"

if [ $# -lt 2 ]
then
    echo "finder.sh scripts needs to 2 arguments, see example below."
    echo "finder.sh /tmp/aesd/assignment1 linux"
    exit 1
else
	DIR_NAME=$1
	echo "finder dir: ${DIR_NAME}"
	STR_NAME=$2
	echo "finder string: ${STR_NAME}"
fi

NUM_FILES=$(ls "${DIR_NAME}" | wc -l)
NUM_LINES=$(grep -rio "${STR_NAME}" "${DIR_NAME}" | wc -l)

#echo "number of files: ${NUM_FILES}"
#echo "number of line: ${NUM_LINES}"

echo "The number of files are ${NUM_FILES} and the number of matching lines are ${NUM_LINES}"


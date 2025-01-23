#!/bin/bash
# This is a simple test script
SCRIPTNAME=$(basename $0)
MYDIR=$(dirname $(realpath $0))
LOGFILE=${MYDIR}/${SCRIPTNAME}.log

# echo the arguments of the script to the logfile 
echo  $(date) $@ >> $LOGFILE 


#!/bin/sh
addr=`nm boot2.exe | grep $1 | cut -d " " -f 1`
echo "0x$addr"

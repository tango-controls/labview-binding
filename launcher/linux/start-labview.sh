#!/bin/sh

LVB_PATH=$HOME/github/labview-binding/runtime/linux/lib/x86/tango-9.2.2-gcc-4.3
export LVB_PATH

LD_LIBRARY_PATH=$LVB_PATH/runtime:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

/usr/local/natinst/LabVIEW-2014/labview


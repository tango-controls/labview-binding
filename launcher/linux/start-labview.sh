#!/bin/sh

LVB_PREFIX=/home/nicolas/bindings/labview-binding@github

LVB_PATH=$LVB_PREFIX/runtime/linux/lib/x86/tango-9.2.2-gcc-4.3
export LVB_PATH

LD_LIBRARY_PATH=$LVB_PATH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

/usr/local/natinst/LabVIEW-2014/labview


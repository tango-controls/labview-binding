#!/bin/sh

#set -- libname.so.5.1.1

for baselib in "$@"
do
    shortlib=$baselib
    while extn=$(echo $shortlib | sed -n '/\.[0-9][0-9]*$/s/.*\(\.[0-9][0-9]*\)$/\1/p')
          [ -n "$extn" ]
    do
        shortlib=$(basename $shortlib $extn)
        if [ ! -f  $shortlib ]; then
          echo ln -s $baselib $shortlib
          ln -s $baselib $shortlib
        fi
    done
done

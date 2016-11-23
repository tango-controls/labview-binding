@ECHO off

::------------------------------------------------------------------------
:: ======== TANGO BINDING PATHS ======== 
::------------------------------------------------------------------------
set BINDING_PATH=C:\mystuffs\dev\bindings\labview-bindings-github

set RUNTIME_PATH_X86=%BINDING_PATH%\runtime\windows
set RUNTIME_PATH_X64=%BINDING_PATH%\runtime\windows

set PATH=%RUNTIME_PATH_X64%\lib\vc12_dll;%PATH%
set PATH=%RUNTIME_PATH_X86%\lib\vc10_dll;%PATH%
set PATH=%BINDING_PATH%\vis;%PATH%

::------------------------------------------------------------------------
:: ======== NI-LABVIEW PATHS ======== 
::------------------------------------------------------------------------
set LV_ROOT32=C:\Program Files (x86)\National Instruments\LabVIEW 2015
set LV_INC32=%LV_ROOT32%\cintools
set LV_LIB32=%LV_ROOT32%\cintools
SET LV_LIB32_LIST=labview.lib

set LV_ROOT64=C:\Program Files\National Instruments\LabVIEW 2014
set LV_INC64=%LV_ROOT64%\cintools
set LV_LIB64=%LV_ROOT64%\cintools
SET LV_LIB64_LIST=labview.lib

::------------------------------------------------------------------------
:: ======== YAT32 ======== 
::------------------------------------------------------------------------
set YAT_ROOT32=%RUNTIME_PATH_X86%
set YAT_INC32=%YAT_ROOT32%\include\vc10
set YAT_LIB32=%YAT_ROOT32%\lib\vc10
set YAT_SHARED_LIB32=%YAT_ROOT32%\lib\vc10_dll
set YAT_LIB32_LIST=yat.lib
set YAT_DBG_LIB32_LIST=yatd.lib

::------------------------------------------------------------------------
:: ======== YAT64 ======== 
::------------------------------------------------------------------------
set YAT_ROOT64=%RUNTIME_PATH_X64%
set YAT_INC64=%YAT_ROOT64%\include\vc12
set YAT_LIB64=%YAT_ROOT64%\lib\vc12_dll
set YAT_SHARED_LIB64=%YAT_ROOT64%\lib\vc12_dll
set YAT_LIB64_LIST=yat.lib
set YAT_DBG_LIB64_LIST=yatd.lib

::------------------------------------------------------------------------
:: ======== YAT4TANGO32 ======== 
::------------------------------------------------------------------------
set YAT4TANGO_ROOT32=%RUNTIME_PATH_X86%
set YAT4TANGO_INC32=%YAT4TANGO_ROOT32%\include
set YAT4TANGO_LIB32=%YAT4TANGO_ROOT32%\lib\vc10
set YAT4TANGO_SHARED_LIB32=%YAT4TANGO_ROOT32%\lib\vc10_dll
set YAT4TANGO_LIB32_LIST=yat4tango.lib
set YAT4TANGO_DBG_LIB32_LIST=yat4tangod.lib

::------------------------------------------------------------------------
:: ======== YAT4TANGO64 ======== 
::------------------------------------------------------------------------
set YAT4TANGO_ROOT64=%RUNTIME_PATH_X64%
set YAT4TANGO_INC64=%YAT_ROOT64%\include\vc12
set YAT4TANGO_LIB64=%YAT_ROOT64%\lib\vc12_dll
set YAT4TANGO_SHARED_LIB64=%YAT_ROOT64%\lib\vc12_dll
set YAT4TANGO_LIB64_LIST=yat4tango.lib
set YAT4TANGO_DBG_LIB64_LIST=yat4tangod.lib

::------------------------------------------------------------------------
:: ======== LOG4TANGO ======== 
::------------------------------------------------------------------------
SET LOG4TANGO_ROOT32=C:\mystuffs\dev\tango-9.2.2-x86-vc10
SET LOG4TANGO_INC32=%LOG4TANGO_ROOT32%\include\vc10
SET LOG4TANGO_LIB32=%LOG4TANGO_ROOT32%\lib\vc10_dll
SET LOG4TANGO_BIN32=%LOG4TANGO_ROOT32%\lib\vc10_dll
SET LOG4TANGO_LIB32_LIST=log4tango.lib
SET PATH=%LOG4TANGO_BIN32%;%PATH%

SET LOG4TANGO_ROOT64=%RUNTIME_PATH_X64%
SET LOG4TANGO_INC64=%LOG4TANGO_ROOT64%\include\vc12
SET LOG4TANGO_LIB64=%LOG4TANGO_ROOT64%\lib\vc12_dll
SET LOG4TANGO_BIN64=%LOG4TANGO_ROOT64%\lib\vc12_dll
SET LOG4TANGO_LIB64_LIST=log4tango.lib
SET PATH=%LOG4TANGO_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== ZMQ ======== 
::------------------------------------------------------------------------
SET ZMQ_ROOT32=%RUNTIME_PATH_X86%
SET ZMQ_INC32=%ZMQ_ROOT32%\include\vc10
SET ZMQ_LIB32=%ZMQ_ROOT32%\lib\vc10_dll
SET ZMQ_BIN32=%ZMQ_ROOT32%\lib\vc10_dll
SET ZMQ_LIB32_LIST=zmq.lib
SET PATH=%ZMQ_BIN32%;%PATH%

SET ZMQ_ROOT64=%RUNTIME_PATH_X64%
SET ZMQ_INC64=%ZMQ_ROOT64%\include\vc12
SET ZMQ_LIB64=%ZMQ_ROOT64%\lib\vc12_dll
SET ZMQ_BIN64=%ZMQ_ROOT64%\lib\vc12_dll
SET ZMQ_LIB64_LIST=zmq.lib
SET PATH=%ZMQ_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== OMNIORB ======== 
::------------------------------------------------------------------------
SET OMNIORB_ROOT32=%RUNTIME_PATH_X86%
SET OMNIORB_INC32=%OMNIORB_ROOT32%\include\vc10
SET OMNIORB_LIB32=%OMNIORB_ROOT32%\lib\vc10_dll
SET OMNIORB_BIN32=%OMNIORB_ROOT32%\lib\vc10_dll
SET OMNIORB_LIB32_LIST=omnithread40_rt.lib;omniORB421_rt.lib;COS421_rt.lib;omniDynamic421_rt.lib
SET PATH=%OMNIORB_BIN32%;%PATH%

SET OMNIORB_ROOT64=%RUNTIME_PATH_X64%
SET OMNIORB_INC64=%OMNIORB_ROOT64%\include\vc12
SET OMNIORB_LIB64=%OMNIORB_ROOT64%\lib\vc12_dll
SET OMNIORB_BIN64=%OMNIORB_ROOT64%\lib\vc12_dll
SET OMNIORB_LIB64_LIST=omnithread40_rt.lib;omniORB421_rt.lib;COS421_rt.lib;omniDynamic421_rt.lib
SET PATH=%OMNIORB_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== TANGO ======== 
::------------------------------------------------------------------------
SET TANGO_ROOT32=%RUNTIME_PATH_X86%
SET TANGO_INC32=%TANGO_ROOT32%\include\vc10
SET TANGO_LIB32=%TANGO_ROOT32%\lib\vc10_dll
SET TANGO_BIN32=%TANGO_ROOT32%\lib\vc10_dll
SET TANGO_LIB32_LIST=tango.lib
SET PATH=%TANGO_BIN32%;%PATH%

SET TANGO_ROOT64=%RUNTIME_PATH_X64%
SET TANGO_INC64=%TANGO_ROOT64%\include\vc12
SET TANGO_LIB64=%TANGO_ROOT64%\lib\vc12_dll
SET TANGO_BIN64=%TANGO_ROOT64%\lib\vc12_dll
SET TANGO_LIB64_LIST=tango.lib
SET PATH=%TANGO_BIN64%;%PATH%


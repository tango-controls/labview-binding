@ECHO off

::------------------------------------------------------------------------
:: SETUP THE RUNTIME ENV.
::------------------------------------------------------------------------
set PATH=C:\mystuffs\dev\bindings\labview-cvs-soleil-trunk\vis;%PATH%
set PATH=C:\mystuffs\dev\tango-9.2.1-x64-vc12\win64\lib\vc12_dll;%PATH%
set PATH=C:\Program Files\National Instruments\LabVIEW 2014;%PATH%

start LabVIEW.exe

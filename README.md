[![logo](http://www.tango-controls.org/static/tango/img/logo_tangocontrols.png)](http://www.tango-controls.org)

# About the LabVIEW binding for Tango

This is the LabVIEW client and server API for [Tango](http://tango-controls.org). 

# Documentation

The provided 'quick start guide' explains how to implement a Tango device server in LabVIEW. 

# How to build and install on Windows using Microsoft Visual C++ 
- software requirements 
  * Microsoft Visual C++ >= 2013 (a.k.a msvc12) 
  * LabVIEW x86 and/or x64 >= 2014
  
- clone the github repository in a directory of your choice (here we use c:\github)
```
cd c:\github 
git clone git@github.com:tango-controls/labview-binding.git
```

- edit the `compil-env-setup.bat` file
```
{edit} c:\github\labview-binding\windows\msvcxx\compil-env-setup.bat
```

- set BINDING_PATH to the labview-binding clone location  
```
set BINDING_PATH=c:\github\labview-binding 
```

- set LV_ROOT32 and/or LV_ROOT64 according to your local LabVIEW installation
```
set LV_ROOT32=C:\Program Files (x86)\National Instruments\LabVIEW 2015
set LV_ROOT64=C:\Program Files\National Instruments\LabVIEW 2014
```

- save the `compil-env-setup.bat` file then open the Visual C++ project
```
{double-click-on} c:\github\labview-binding\windows\msvcxx\compil-env-setup.bat
```

- select the configration you want to compile then build the solution
```
{select one of the following combination} [release or debug] / [win32 or x64]
{build}
```

- the compilation will hopefully succeed...
```
{check-for-presence-of} c:\github\labview-binding\vis\tango_binding.dll
 ```

# How to build and install on Linux using cmake

This this a work in progress. Stay tuned.

# Links

[Tango Controls web page](http://tango-controls.org)

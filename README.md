[![logo](http://www.tango-controls.org/static/tango/img/logo_tangocontrols.png)](http://www.tango-controls.org)

# About the LabVIEW binding for Tango

This is the LabVIEW client and server API for [Tango](http://tango-controls.org). 
It runs on Linux and Windows and supports both the x86 and x64 version of LabVIEW.

# Documentation

The provided 'quick start guide' explains how to implement a Tango device server in LabVIEW.
See `../doc/quick-start-guide/Writing a Tango Device Server in LabVIEW.pdf`

# How to build on Windows using Microsoft Visual C++ 
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

- set LV_ROOT32 and/or LV_ROOT64 according to your local LabVIEW installation(s)
```
set LV_ROOT32=C:\Program Files (x86)\National Instruments\LabVIEW 2015
set LV_ROOT64=C:\Program Files\National Instruments\LabVIEW 2014
```

- save the `compil-env-setup.bat` file then open the Visual C++ project
```
{double-click-on} c:\github\labview-binding\windows\msvcxx\run-me-to-compile.bat
```

- select the configuration you want to compile then build the solution
```
{select one of the following combination} [release or debug] / [win32 or x64]
{build}
```

- the compilation will hopefully succeed
```
{safely ignore} warning LNK4088: image being generated due to /FORCE option; image may not run
{check for presence of} c:\github\labview-binding\vis\tango_binding.dll
```

- you can optionally edit the provided LabVIEW launcher and modify the PATH env. variable
```
{edit} c:\github\labview-binding\launcher\windows\start-labview.bat

set PATH=c:\github\labview-binding\vis;%PATH%
set PATH=c:\github\labview-binding\runtime\windows\lib\vc12_dll;%PATH%
set PATH=c:\Program Files\National Instruments\LabVIEW 2014;%PATH%

{save then double-click on} c:\github\labview-binding\launcher\windows\start-labview.bat
```

- in case you want to use your own Tango runtime, just edit the `compil-env-setup.bat`, modify the paths then rebuild the solution

# How to build on Linux using gcc 
The main constraint we have is to compile the binding with a gcc version producing a shared library which is binary compatible with the LabVIEW version we plan to use. It means we have to identify the proper gcc version to use before trying to compile the code. Fortunately, the NI website contains [a page providing this critical information]( http://digital.ni.com/public.nsf/allkb/4596349739E988088625761C005B197E). The provided runtime for linux has been compiled with gcc-4.3 for LabVIEW 2013 & 2014.

- clone the github repository in a directory of your choice (here we use $HOME/github)
```
cd $HOME/github
git clone git@github.com:tango-controls/labview-binding.git
```

- optionally, recompile the whole Tango software chain with the appropriate gcc version
  * see [cppTango](https://github.com/tango-controls/cppTango) for details
  * using your own Tango runtime requires `$HOME/github/labview-binding/src/Makefile` to be adapted 
  
- compile the binding 
```
cd $HOME/github/labview-binding/src
make
```

- the compilation will hopefully succeed
```
{check for presence of} $HOME/github/labview-binding/vis/tango_binding.so
```

- you can optionally edit the provided LabVIEW launcher and modify the LVB_PATH variable
```
{make the launcher executable} chmod 755 $HOME/github/labview-binding/launcher/linux/start-labview.sh

{edit} $HOME/github/labview-binding/launcher/linux/start-labview.sh

{set LVB_PATH} LVB_PATH=$HOME/github/labview-binding/runtime/linux/lib/x86/tango-9.2.2-gcc-4.3

{save then execute} $HOME/github/labview-binding/launcher/linux/start-labview.sh
```

# How to build on Linux using cmake

This is the top priority task of the todo list. Stay tuned.

# Links

[Tango Controls web page](http://tango-controls.org)

@ECHO OFF

CALL .\compil-env-setup.bat 

set IDE="C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\WDExpress"

%IDE% .\TangoBindingForLabView.sln


@echo off

set XIO_ENV_ROOT=%cd%

@call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

mkdir build
cd build

%comspec% /k
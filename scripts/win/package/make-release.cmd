@echo off

setlocal enableextensions enabledelayedexpansion

set BRANCH=%1
set ARCH=%2
shift
shift

if "%BRANCH%" == "" (
  set BRANCH=master
)

if "%ARCH%" == "" (
  set ARCH=x64
)

set ROOT="%HOME%\Shaderkit"
set BUILD="%HOME%\Shaderkit-build-%ARCH%"
set QT_HOME=C:\Qt\4.8.1\sdk71-%ARCH%
set S3=C:\s3\s3.exe

echo Building %BRANCH% (%ARCH%) in %BUILD% from %ROOT% with %QT_HOME% at %DATE% %TIME%

call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /win7 /%ARCH%

set PATH=%PATH%;%QT_HOME%\bin

pushd %ROOT%

call git checkout .
call git clean -f -d -q -x
call git fetch origin
call git checkout %BRANCH%
call git merge origin/%BRANCH%

rd /s /q %BUILD%
mkdir %BUILD%
cd /D %BUILD%

echo %PATH%
qmake -config release -r %ROOT%
nmake
if %errorlevel% neq 0 exit /B %errorlevel%

%ROOT%\ext\win32\bin\ruby.exe %ROOT%\scripts\win\installer\make-build.rb %BUILD%
if %errorlevel% neq 0 exit /B %errorlevel%

%S3% put shaderkit-releases/windows/%ARCH%/ %BUILD%\install\setup\*.exe /nogui

echo Build finished at %DATE% %TIME%

popd
endlocal

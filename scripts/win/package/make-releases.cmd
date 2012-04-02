@echo off

set S3=C:\s3\s3.exe
set ORIG_ROOT=%cd%

if not exist build-logs mkdir build-logs

For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
For /f "tokens=1-3 delims=/:." %%a in ("%TIME%") do (set mytime=%%a%%b%%c)

set LOGFILE=build-logs/build-x86-%mydate%_%mytime%.log

call make-release master x86 > %LOGFILE% 2>&1
if %errorlevel% NEQ 0 goto end

For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
For /f "tokens=1-3 delims=/:." %%a in ("%TIME%") do (set mytime=%%a%%b%%c)

set LOGFILE2=build-logs/build-x64-%mydate%_%mytime%.log

call make-release master x64 > %LOGFILE2% 2>&1

cd %ORIG_ROOT%
"C:\Program Files (x86)\git\bin\gzip.exe" %LOGFILE2%

:end
cd %ORIG_ROOT%
"C:\Program Files (x86)\git\bin\gzip.exe" %LOGFILE%

%S3% put shaderkit-logs/windows/ build-logs\ /sync /sub /nogui

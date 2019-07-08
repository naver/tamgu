rem Compiling Windows version for Visual 2013: call to vcvars64.bat
rem You need to set the JAVA_HOME and the ANT_HOME to your personnal installation.
rem The first step must BE the compiling of tamgudll via tamgu.sln, within Visual 2013

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64\vcvars64.bat"

set JAVA_HOME=C:\Program Files\Java\jdk1.8.0_161
set ANT_HOME=C:\Users\roux\Documents\apache-ant-1.9.10
set Path=%JAVA_HOME%\bin;C:\Program Files\Debugging Tools for Windows (x64);%ANT_HOME%\bin;%Path%


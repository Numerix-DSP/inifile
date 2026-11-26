@echo off

rem Batch file for creating SA application for Microsoft Visual Studio C++ Community Edition

rem Create the bakefile
bakefile -f msvc project.bkl

rem Build the application
nmake -f makefile.vc BUILD=release RUNTIME_LIBS=static WX_DEBUG=0

echo Done ...

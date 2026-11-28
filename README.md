# Simple .ini File Reader/Writer

This simple set of functions reads and writes .ini files.
There is a pure C based version with pure C and C++ examples, there is also a version for wxWidgets.

wxWidgets version tested with Microsoft Visual Studio C++ Community Edition


## Pure C

gcc -std=c11 -Wall -Wextra test.c inifile.c -o test_c; ./test_c

# Pure C++

gcc -std=c11 -c inifile.c -o inifile.o; g++ -std=c++11 test_cpp.cpp inifile.o -o test_cpp; ./test_cpp

## C++ With wxWidgets

wxbuild.bat


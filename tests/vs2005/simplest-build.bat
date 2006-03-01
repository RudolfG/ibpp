@MKDIR bin
@cl /DIBPP_WINDOWS /DIBPP_MSVC /W4 /EHsc /I..\..\core /Fobin\ ..\tests.cpp ..\..\core\*.cpp advapi32.lib

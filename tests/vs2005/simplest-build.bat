@MKDIR bin
@cl /DIBPP_WINDOWS /DIBPP_MSVC /I..\..\core /EHsc /Fobin\ ..\tests.cpp ..\..\core\*.cpp advapi32.lib

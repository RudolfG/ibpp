@MKDIR bin
@cl /DIBPP_WINDOWS /O2 /Oi /W4 /EHsc /I..\..\core /Fobin\ ..\tests.cpp ..\..\core\all_at_once.cpp advapi32.lib

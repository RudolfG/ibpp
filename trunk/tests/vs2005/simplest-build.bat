@REM $Id$
@MKDIR bin
@cl /DIBPP_WINDOWS /O2 /Oi /W4 /EHsc /I..\..\core /Fobin\ ..\tests.cpp ..\..\core\all_in_one.cpp advapi32.lib

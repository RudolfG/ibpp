@REM $Id$
@MKDIR bin
@bcc32 -DIBPP_WINDOWS -O2 -w -I..\..\core -nbin ..\tests.cpp ..\..\core\all_in_one.cpp

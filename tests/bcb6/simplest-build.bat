@MKDIR bin
@bcc32 -DIBPP_WINDOWS -O2 -w -I..\..\core -nbin ..\tests.cpp ..\..\core\all_at_once.cpp

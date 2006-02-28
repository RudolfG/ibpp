copy /Y ibpp.h L:\AddOns\IBPP
REM
copy /Y release\win32_msvc\ibpp.lib L:\AddOns\IBPP
copy /Y debug\win32_msvc\ibppd.lib L:\AddOns\IBPP
copy /Y debug\win32_msvc\ibppd.pdb L:\AddOns\IBPP
copy /Y tests\release\win32_msvc\tests.exe L:\AddOns\IBPP
REM
copy /Y release\win32_bcc\ibpp.lib L:\AddOns\IBPP\ibpp_b.lib
copy /Y debug\win32_bcc\ibppd.lib L:\AddOns\IBPP\ibpp_bd.lib

@REM Build a source-code distribution zipfile
@REM Run this script from *above* the IBPP root folder
@REM and pass the ibpp subdirectory name as parameter
@REM Example: ibpp-2-6\packaging\buildzip ibpp-2-6

@DEL /Q %1%-src.zip
@DEL /Q list.txt
@REM
@ECHO %1%\license.txt >list.txt
@ECHO %1%\releasenotes.txt >>list.txt
@ECHO %1%\howtobuild.txt >>list.txt
@ECHO %1%\changelog.txt >>list.txt
@ECHO %1%\todo.txt >>list.txt
@REM
@ECHO %1%\core\_ibpp.h >>list.txt
@ECHO %1%\core\ibase.h >>list.txt
@ECHO %1%\core\iberror.h >>list.txt
@ECHO %1%\core\ibpp.h >>list.txt
@REM
@ECHO %1%\core\_dpb.cpp >>list.txt
@ECHO %1%\core\_ibpp.cpp >>list.txt
@ECHO %1%\core\_ibs.cpp >>list.txt
@ECHO %1%\core\_rb.cpp >>list.txt
@ECHO %1%\core\_spb.cpp >>list.txt
@ECHO %1%\core\_tpb.cpp >>list.txt
@ECHO %1%\core\array.cpp >>list.txt
@ECHO %1%\core\blob.cpp >>list.txt
@ECHO %1%\core\database.cpp >>list.txt
@ECHO %1%\core\date.cpp >>list.txt
@ECHO %1%\core\dbkey.cpp >>list.txt
@ECHO %1%\core\events.cpp >>list.txt
@ECHO %1%\core\exception.cpp >>list.txt
@ECHO %1%\core\row.cpp >>list.txt
@ECHO %1%\core\service.cpp >>list.txt
@ECHO %1%\core\statement.cpp >>list.txt
@ECHO %1%\core\time.cpp >>list.txt
@ECHO %1%\core\transaction.cpp >>list.txt
@ECHO %1%\core\user.cpp >>list.txt
@ECHO %1%\core\all_in_one.cpp >>list.txt
@REM
@ECHO %1%\tests\tests.cpp >>list.txt
@REM
@ECHO %1%\tests\unixes\Makefile >>list.txt
@ECHO %1%\tests\unixes\simplest-build.sh >>list.txt
@REM
@ECHO %1%\tests\bcb6\simplest-build.bat >>list.txt
@ECHO %1%\tests\bcb6\tests.bpf >>list.txt
@ECHO %1%\tests\bcb6\tests.bpr >>list.txt
@ECHO %1%\tests\bcb6\tests.res >>list.txt
@REM
@ECHO %1%\tests\vs2005\simplest-build.bat >>list.txt
@ECHO %1%\tests\vs2005\tests.sln >>list.txt
@ECHO %1%\tests\vs2005\tests.vcproj >>list.txt
@REM
@ECHO %1%\tests\vs6\tests.dsp >>list.txt
@REM
@ECHO %1%\packaging\debian.diff >>list.txt
@ECHO %1%\packaging\buildzip.cmd >>list.txt
@REM
@"C:\Program Files\7-Zip\7z.exe" a -tzip %1%-src.zip @list.txt
@DEL /Q list.txt

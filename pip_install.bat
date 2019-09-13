call %1 %2
set BIN_DIR=bin
rmdir /q/s %BIN_DIR%

cmake -H. -B%BIN_DIR% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBRARY=1 -G "NMake Makefiles"
IF %ERRORLEVEL% NEQ 0 ( 
   EXIT /B %ERRORLEVEL%
)

cd %BIN_DIR%
nmake
IF %ERRORLEVEL% NEQ 0 ( 
   EXIT /B %ERRORLEVEL%
)
cd ..

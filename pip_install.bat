call %1 %2
set BIN_DIR=bin
rmdir /q/s %BIN_DIR%

cmake -H. -B%BIN_DIR% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBRARY=1 -G "NMake Makefiles" || goto :error

cd %BIN_DIR%
nmake || goto :error
cd ..

:error
exit /b %errorlevel%

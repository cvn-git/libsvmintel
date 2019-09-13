call %1 %2
set BIN_DIR=bin
rmdir /q/s %BIN_DIR%
cmake -H. -B%BIN_DIR% -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles"
cd %BIN_DIR%
nmake
cd ..

cmake -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -H. -Bbuild/
cd build/
make VERBOSE=1
pause
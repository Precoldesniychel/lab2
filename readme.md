cd C:\Labs\lab2-master
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release -j 8
ctest --verbose
.\lab2.exe
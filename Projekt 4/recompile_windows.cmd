rmdir /s /q build
mkdir build
cd build
cmake .. -A x64
cmake --build .
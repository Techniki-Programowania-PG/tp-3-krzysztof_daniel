cmake_minimum_required(VERSION 4.0.2)
cmake_policy(VERSION 4.0.2)
project(signal-analizer)

### DLA WINDOWS
set(PYTHON_LIBRARY "C:/msys64/ucrt64/lib/libpython3.12.dll.a")
set(PYTHON_INCLUDE_DIR "C:/msys64/ucrt64/include/python3.12")

### Ustawienia do komendy cmake --build .
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)

include(FetchContent)

FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.12
)

FetchContent_MakeAvailable(pybind11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pybind11_add_module(signal_analyzer src/signal_analyzer.cpp)
set_target_properties(signal_analyzer PROPERTIES OUTPUT_NAME "signal_analyzer")
set_target_properties(signal_analyzer PROPERTIES PREFIX "" SUFFIX ".pyd")
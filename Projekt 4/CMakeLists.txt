cmake_minimum_required(VERSION 3.10)
project(RobotArmSim)

set(CMAKE_CXX_STANDARD 17)

# This flag tells CMake to build a WIN32 (GUI) executable, which will
# look for WinMain / wWinMain instead of main().
add_executable(RobotArmSim WIN32 main.cpp)

# link to Gdiplus
target_link_libraries(RobotArmSim PRIVATE gdiplus)
target_link_options(RobotArmSim PRIVATE -mwindows -municode)
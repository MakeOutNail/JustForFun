cmake_minimum_required(VERSION 3.28.3)
project(ProjectX LANGUAGES CXX) # C++ Only

set(CMAKE_CXX_STANDARD 20) # Select C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # do not fall back
set(CMAKE_CXX_EXTENSIONS OFF) # Do not allow GNU-specific extensions


add_executable(ProjectX main.cpp
        files/rom.cpp
        files/rom.h
        files/header.cpp
        files/header.h
        files/cpustate.cpp
        files/cpustate.h
        files/cpu.cpp
        files/cpu.h
        files/ram.cpp
        files/ram.h
        files/memorybus.cpp
        files/memorybus.h)


# Enable Warnings
# Private only this target and no one else receives this compiler options
# Target is a type of identifier
# Different Warning Packages
target_compile_options(ProjectX PRIVATE -Wall -Wextra -Wpedantic)
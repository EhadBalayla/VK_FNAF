cmake_minimum_required(VERSION 3.16)
project(VK_FNAF LANGUAGES C CXX) #the CXX is for VMA as its a C++ library but with a C interface

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(Game)
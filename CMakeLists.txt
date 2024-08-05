cmake_minimum_required(VERSION 3.5)

project(refl CXX)

set(CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

add_executable(refl source/main.cpp)
set_property(TARGET refl PROPERTY CXX_STANDARD 23)

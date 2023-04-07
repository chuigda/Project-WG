cmake_minimum_required(VERSION 3.25)
project(bis_anim CXX)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(bis_anim_placeholder
               include/bis_anim.h
               src/placeholder.cc)

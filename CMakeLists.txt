cmake_minimum_required(VERSION 3.10)

project(ffb_example)

add_executable(ffb_example ffb_example.cpp)
target_link_libraries(ffb_example dinput8 dxguid)

cmake_minimum_required(VERSION 3.4)
project(dtrace_memaccess)

add_executable(memaccess exploit.cpp shared.cpp shared.h)
target_link_libraries(memaccess dtrace)

add_executable(target target.cpp shared.cpp shared.h)

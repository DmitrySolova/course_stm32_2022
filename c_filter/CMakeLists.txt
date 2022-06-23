cmake_minimum_required(VERSION 3.20)
project(c_filter C)

set(CMAKE_C_STANDARD 99)

add_executable(c_filter main.c)

target_link_libraries(c_filter
        m
)

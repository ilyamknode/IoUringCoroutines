cmake_minimum_required(VERSION 3.27)
project(ucontext_test2 C)

set(CMAKE_C_STANDARD 11)

add_executable(ucontext_test2 main.c
        context.c
        context.h
        ring.c
        ring.h
)

target_link_libraries(ucontext_test2 PRIVATE -luring)
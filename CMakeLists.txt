cmake_minimum_required(VERSION 2.8) 

project(stepper)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O3")

set(GCC_LINKER_FLAGS "-lpthread -lpigpio")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_LINKER_FLAGS}")

add_definitions(
    #-D CMAKE_BUILD_TYPE=Debug
    -D _GNU_SOURCE
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/worker_thread.c
)

add_executable(stepper src/posix_rt.c ${SOURCES})

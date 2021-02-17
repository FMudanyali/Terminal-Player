cmake_minimum_required(VERSION 3.13)
#set(CMAKE_TOOLCHAIN_FILE "toolchain-mingw32.cmake" CACHE PATH "toolchain file")
project(termplayer)

set(CMAKE_C_FLAGS "-std=gnu11 -o3")
set(CMAKE_CXX_FLAGS "-std=c++11 -o3")

include_directories(
)

link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(${PROJECT_NAME}
    main.c
)

target_link_libraries(${PROJECT_NAME}
    m
    ncurses
    avformat
)
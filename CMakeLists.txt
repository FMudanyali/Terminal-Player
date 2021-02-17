cmake_minimum_required(VERSION 3.13)
OPTION(WSL OFF)
IF(WSL)
add_definitions(-DWSL)
ENDIF()
#set(CMAKE_TOOLCHAIN_FILE "toolchain-mingw32.cmake" CACHE PATH "toolchain file")
project(termplayer)

set(CMAKE_C_FLAGS "-std=gnu11 -o3")
set(CMAKE_CXX_FLAGS "-std=c++11 -o3")

include_directories(
    /opt/local/include
)

link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    /opt/local/lib
)

add_executable(${PROJECT_NAME}
    main.c
)

target_link_libraries(${PROJECT_NAME}
    m
    ncurses
    avformat
)
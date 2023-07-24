cmake_minimum_required(VERSION 3.23)
project(tinywm C)

set(CMAKE_C_STANDARD 23)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

# add src every file in /src
file(GLOB SOURCE_FILES src/*.c)

add_executable(tinywm ${SOURCE_FILES})

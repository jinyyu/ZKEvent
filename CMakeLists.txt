cmake_minimum_required(VERSION 3.5)
project(ZKEvent-distribution)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-Wall)
add_definitions(-Wno-unused-function)

set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${HEADER_DIR})

find_package(PkgConfig REQUIRED)


pkg_check_modules(GLIB2 glib-2.0>=2.10 REQUIRED)
include_directories(${GLIB2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLIB2_LIBRARIES})


set(HEADERS
    ${HEADER_DIR}/ZKEvent/Slice.h
    ${HEADER_DIR}/ZKEvent/Callback.h
    ${HEADER_DIR}/ZKEvent/ZKEvent.h
    )

set(SRCS
    src/ZKEvent.cpp
    src/ZKClient.cpp
    src/Status.cpp
    src/Event.h)


add_library(zkevent
            ${HEADERS}
            ${SRCS}
            )
target_link_libraries(zkevent
                      pthread
                      zookeeper_mt)

add_subdirectory(example)
add_subdirectory(tests)



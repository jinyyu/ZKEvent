cmake_minimum_required(VERSION 3.5)
project(zkclient-distribution)

set(CMAKE_CXX_STANDARD 11)

set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${HEADER_DIR})

set(zkclient_HEADERS
    ${HEADER_DIR}/ZkClient/ZkClient.h
    )

set(zkclient_SRCS
    src/DebugLog.cpp
    src/ZkClient.cpp
    )


add_library(zkclient
            ${zkclient_HEADERS}
            ${zkclient_SRCS}
            )
target_link_libraries(zkclient
                      pthread
                      zookeeper_mt)

add_subdirectory(example)



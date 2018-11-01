cmake_minimum_required(VERSION 3.5)
project(zkclient-distribution)

set(CMAKE_CXX_STANDARD 11)

set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${HEADER_DIR})

set(zkclient_HEADERS
    ${HEADER_DIR}/ZkClient/Slice.h
    ${HEADER_DIR}/ZkClient/Callback.h
    ${HEADER_DIR}/ZkClient/ZKClient.h
    )

set(zkclient_SRCS
    src/ZKClient.cpp
    src/ClientImpl.cpp
    )


add_library(zkclient
            ${zkclient_HEADERS}
            ${zkclient_SRCS}
            )
target_link_libraries(zkclient
                      pthread
                      zookeeper_mt)


find_package(Boost COMPONENTS system thread)
if (Boost_FOUND)
    set(LIBS
        ${LIBS} Boost::system Boost::thread)
endif (Boost_FOUND)

add_subdirectory(example)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GoogleTest gtest gtest_main)
if (GoogleTest_FOUND)
    include_directories(${GoogleTest_INCLUDE_DIRS})
    set(LIBS
        ${LIBS} ${GoogleTest_LIBRARIES} pthread)
    add_subdirectory(tests)
endif (GoogleTest_FOUND)



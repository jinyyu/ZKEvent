set(LIBS ${LIBS} zkevent)

add_executable(test_client test_client.cpp)
target_link_libraries(test_client ${LIBS})
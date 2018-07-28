
add_executable(example_subscribe example_subscribe.cpp)
target_link_libraries(example_subscribe ${LIBS} zkclient)
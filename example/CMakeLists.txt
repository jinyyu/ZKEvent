set(LIBS ${LIBS} zkevent)

add_executable(usage usage.cpp)
target_link_libraries(usage ${LIBS})
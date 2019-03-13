cmake_minimum_required (VERSION 3.0)

project(ecs)

set(
    SRCS
    ecs.cpp
)

set(
    HEADERS
    ecs.hpp
)

add_library(ecs STATIC ${SRCS} ${HEADERS})

add_executable(ecs_example main.cpp)
target_link_libraries(ecs_example ecs)

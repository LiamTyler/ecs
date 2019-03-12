cmake_minimum_required (VERSION 3.0)

project(ecs)

set(
    SRCS
    entity.cpp
    ecs.cpp
)

set(
    HEADERS
    entity.hpp
    ecs.hpp
)

add_library(ecs STATIC ${SRCS} ${HEADERS})

add_executable(ecs_example main.cpp)
target_link_libraries(ecs_example ecs)

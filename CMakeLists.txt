cmake_minimum_required (VERSION 3.0)

project(ecs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    SRCS
)

set(
    HEADERS
)

#add_library(ecs STATIC ${SRCS} ${HEADERS})

add_executable(ecs_example main.cpp)
#target_link_libraries(ecs_example ecs)

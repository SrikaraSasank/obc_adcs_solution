
cmake_minimum_required(VERSION 3.10)
project(adcs_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DADCS_STANDALONE=1)

add_library(adcs_core
    src/math_quat.hpp
    src/udp_link.hpp
    src/udp_link.cpp
    src/adcs.hpp
    src/adcs.cpp
)

add_executable(adcs_app
    src/main.cpp
)

target_include_directories(adcs_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(adcs_app PRIVATE adcs_core)

# For cFS builds, compile this directory as a static lib and link from a cFE app wrapper.

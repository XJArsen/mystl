# cmake_minimum_required(VERSION 3.5.0)
# project(mystl VERSION 0.1.0 LANGUAGES C CXX)

# add_executable(mystl main.cpp)
cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

project(mystl LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Werror=return-type)

file(GLOB sources CONFIGURE_DEPENDS *.cpp)
foreach (source IN ITEMS ${sources})
    get_filename_component(name "${source}" NAME_WLE)
    add_executable(${name} ${name}.cpp)
endforeach()

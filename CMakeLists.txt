cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)

project(mystl LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Werror=return-type)
# 添加gtest gmock
find_package(PkgConfig REQUIRED)
pkg_check_modules(GMOCK REQUIRED gmock)
pkg_check_modules(GTEST REQUIRED gtest)

include_directories(${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
link_directories(${GMOCK_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS})

file(GLOB sources CONFIGURE_DEPENDS *.cpp)

# 为每个源文件创建测试目标并链接库
foreach(source IN ITEMS ${sources})
    get_filename_component(name "${source}" NAME_WLE)
    add_executable(${name}_test ${source})

    target_link_libraries(${name}_test ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})

    add_test(NAME ${name}_test COMMAND ${name}_test)
endforeach()

enable_testing()

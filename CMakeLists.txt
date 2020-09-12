cmake_minimum_required(VERSION 3.8)
project(xcspp LANGUAGES CXX)

file(GLOB_RECURSE sources src/*.cpp)
add_library(xcspp STATIC ${sources})

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

target_compile_features(xcspp PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(xcspp PRIVATE /W4)
else()
    target_compile_options(xcspp PRIVATE -O2 -Wall)
endif()
target_include_directories(xcspp PUBLIC ${PROJECT_SOURCE_DIR}/include)

enable_testing()
add_subdirectory(test)

export(TARGETS xcspp FILE ${CMAKE_CURRENT_BINARY_DIR}/xcsppConfig.cmake)

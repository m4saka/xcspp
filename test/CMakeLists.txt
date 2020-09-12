add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)
enable_testing()
include(GoogleTest)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})

add_executable(SymbolTest symbol_test.cpp)
target_compile_features(SymbolTest PRIVATE cxx_std_17)
target_link_libraries(SymbolTest gtest gtest_main xcspp)
gtest_discover_tests(SymbolTest)

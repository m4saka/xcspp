include(CTest)

# Include googletest
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)

add_executable(SymbolTest symbol_test.cpp)
target_compile_features(SymbolTest PRIVATE cxx_std_17)
target_link_libraries(SymbolTest gtest gtest_main xcspp)
add_test(SymbolTest SymbolTest)

add_executable(ConditionTest condition_test.cpp)
target_compile_features(ConditionTest PRIVATE cxx_std_17)
target_link_libraries(ConditionTest gtest gtest_main xcspp)
add_test(ConditionTest ConditionTest)

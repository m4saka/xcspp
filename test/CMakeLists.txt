include(CTest)

# Include googletest
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)

add_subdirectory(xcs)
add_subdirectory(xcsr)

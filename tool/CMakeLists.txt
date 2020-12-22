file(GLOB common_sources
    ${PROJECT_SOURCE_DIR}/tool/common/*.cpp)

file(GLOB xcs_sources
    ${PROJECT_SOURCE_DIR}/tool/xcs/*.cpp)
add_executable(xcs ${common_sources} ${xcs_sources})
target_compile_features(xcs PRIVATE cxx_std_17)
if (MSVC)
    target_compile_options(xcs PRIVATE /W4)
else()
    target_compile_options(xcs PRIVATE -O2 -Wall)
endif()
target_include_directories(xcs PRIVATE
    ${PROJECT_SOURCE_DIR}/tool/cxxopts/include
    ${PROJECT_SOURCE_DIR}/tool)
target_link_libraries(xcs xcspp)

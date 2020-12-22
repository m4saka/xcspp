file(GLOB common_sources
    ${PROJECT_SOURCE_DIR}/tool/common/*.cpp)

foreach(target IN ITEMS xcs xcsr)
    file(GLOB sources
        ${PROJECT_SOURCE_DIR}/tool/${target}/*.cpp)
    add_executable(${target} ${common_sources} ${sources})
    target_compile_features(${target} PRIVATE cxx_std_17)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -O2 -Wall)
    endif()
    target_include_directories(${target} PRIVATE
        ${PROJECT_SOURCE_DIR}/tool/cxxopts/include
        ${PROJECT_SOURCE_DIR}/tool)
    target_link_libraries(${target} xcspp)
endforeach()

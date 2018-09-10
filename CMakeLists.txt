cmake_minimum_required(VERSION 3.0.0)
project(MetaStuff)

set(CMAKE_CXX_STANDARD 14)

add_library(MetaStuff INTERFACE)
target_include_directories(MetaStuff INTERFACE "${MetaStuff_SOURCE_DIR}/include")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(buildExamplesDefault ON)
else()
    set(buildExamplesDefault OFF)
endif()

option(METASTUFF_BUILD_EXAMPLES "Build MetaStuff examples" ${buildExamplesDefault})

if(METASTUFF_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

cmake_minimum_required(VERSION 3.0)
project(MetaStuff VERSION 0.0.0.1 LANGUAGES CXX)

option(WITH_EXAMPLES "Build examples" ON)

include(GNUInstallDirs)

# copy the header files
install(
    DIRECTORY include
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}
    ) 

if(WITH_EXAMPLES)
    add_subdirectory(example)
endif(WITH_EXAMPLES)

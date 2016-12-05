cmake_minimum_required(VERSION 3.1)

project(MetaStuffExample)

# Bit hacky but whatever it works for examples.
set(MetaStuffExample_SOURCES
    JsonCast.cpp
    JsonCast.h
    JsonCast.inl
    MovieInfo.h
    Person.h
    StringCast.cpp
    StringCast.h
    jsoncpp/jsoncpp.cpp)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/jsoncpp/
    ${PROJECT_SOURCE_DIR}/../include/)

add_executable(MetaStuffExample main.cpp ${MetaStuffExample_SOURCES})

target_include_directories(MetaStuffExample PRIVATE
    ${INCLUDE_DIRS})

set_target_properties(MetaStuffExample PROPERTIES DEBUG_POSTFIX _d)

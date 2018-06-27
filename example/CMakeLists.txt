cmake_minimum_required( VERSION 2.8 )
project( MetaStuffTest)

set(exename meta-stuff-test)

add_executable(${exename} main.cpp)
target_include_directories(${exename} PRIVATE
    nlohmann_json # json in subdir
    ../include    # meta in parent tree
    )
target_sources(${exename} PRIVATE
    StringCast.cpp
    StringCast.h
    JsonCast.h
    MovieInfo.h
    Person.h
    JsonCast.inl
    )

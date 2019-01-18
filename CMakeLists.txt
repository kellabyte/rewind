cmake_minimum_required(VERSION 2.6)
include(CMakeToolsHelpers OPTIONAL)
include("common.cmake")

# ----------------------------------------
# Rewind
# ----------------------------------------
project(rewind C CXX)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set (CMAKE_CXX_STANDARD 14)

#add_definitions(-std=gnu99)
#add_definitions(-msse4.1)
add_definitions(-mavx)
add_definitions(-pedantic)
add_definitions(-O3)
add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wcast-align)
add_definitions(-w)

if (UNIX)
    add_definitions(-DUNIX)
endif (UNIX)

file(GLOB_RECURSE REWIND_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rewind/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rewind/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rewind/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rewind/*.cpp)

list(SORT REWIND_SOURCES)
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/src" ${REWIND_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/lib/lmdb/libraries/liblmdb)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

add_library(rewind STATIC ${REWIND_SOURCES})

target_link_libraries (rewind ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_SOURCE_DIR}/lib/lmdb/libraries/liblmdb/liblmdb.a)

GET_PROPERTY(rewind_location TARGET rewind PROPERTY LOCATION)

# ----------------------------------------
# Tests
# ----------------------------------------
file(GLOB_RECURSE TESTS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.cpp)

list(SORT TESTS_SOURCES)
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/src" ${TESTS_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

add_executable (tests
        ${TESTS_SOURCES})

add_dependencies(tests rewind)

# Libraries to link in reverse order because that's what ld requires.
target_link_libraries (tests
        ${rewind_location}
        ${CMAKE_SOURCE_DIR}/lib/lmdb/libraries/liblmdb/liblmdb.a
        ${CMAKE_THREAD_LIBS_INIT})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (tests rt)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# ----------------------------------------
# Benchmarks
# ----------------------------------------
file(GLOB_RECURSE BENCHMARKS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarks/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarks/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarks/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarks/*.cpp)

list(SORT BENCHMARKS_SOURCES)
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/src" ${BENCHMARKS_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

add_executable (benchmarks
        ${BENCHMARKS_SOURCES})

add_dependencies(benchmarks rewind)

# Libraries to link in reverse order because that's what ld requires.
target_link_libraries (benchmarks
        ${rewind_location}
        ${CMAKE_SOURCE_DIR}/lib/lmdb/libraries/liblmdb/liblmdb.a
        ${CMAKE_THREAD_LIBS_INIT})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (benchmarks rt)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
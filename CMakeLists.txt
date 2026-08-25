cmake_minimum_required(VERSION 3.21)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} VERSION 0.1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE RASI_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hh"
)

file(GLOB_RECURSE RASI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cc"
)

add_library(rasi STATIC ${RASI_SOURCES} ${RASI_HEADERS})

target_include_directories(rasi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(rasi PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -fno-exceptions -fno-rtti
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX /MP
    >
)

set_target_properties(rasi PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib"
)

file(GLOB_RECURSE RASIC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tools/rasic/*.cc"
)

if (RASIC_SOURCE)
    add_executable(rasic ${RASIC_SOURCES})
    target_link_libraries(rasic PRIVATE rasi)
    set_target_properties(rasic PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/exe"
    )
endif()

add_subdirectory(tools/islec)

option(RASI_BUILD_TESTS "build tests" ON)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.6.0
)

FetchContent_MakeAvailable(Catch2)

if (RASI_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc"
    )
    add_executable(rasi_tests ${TEST_SOURCES})
    target_link_libraries(rasi_tests PRIVATE rasi Catch2::Catch2WithMain)
    set_target_properties(rasi_tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/exe"
    )
endif()

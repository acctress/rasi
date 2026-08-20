file(GLOB_RECURSE headers CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.hh"
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc"
)

add_executable(islec ${headers} ${sources})

target_compile_features(islec PRIVATE cxx_std_23)
target_include_directories(islec PRIVATE source)
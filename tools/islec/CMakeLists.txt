add_executable(islec
    source/main.cc
    source/lexer.cc
    source/parser.cc
    source/checker.cc
    source/dec_tree.cc
    source/emitter.cc
)

target_compile_features(islec PRIVATE cxx_std_23)
target_include_directories(islec PRIVATE source)
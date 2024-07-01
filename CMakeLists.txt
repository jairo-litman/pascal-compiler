cmake_minimum_required(VERSION 3.5)
project(
    PascalLexicalAnalyzer
    VERSION 0.1.0 
    LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)

add_executable(PascalLexicalAnalyzer main.c)

target_link_libraries(PascalLexicalAnalyzer PRIVATE PascalLexer PascalToken REPL)
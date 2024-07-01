cmake_minimum_required(VERSION 3.5)
project(
    PascalSyntaxAnalyzer
    VERSION 0.1.0 
    LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)

add_executable(PascalSyntaxAnalyzer main.c)

target_link_libraries(PascalSyntaxAnalyzer PRIVATE PascalLexer PascalToken PascalREPL HashMap PascalAST PascalParser Hash ErrorList)

# if windows
if(WIN32)
    target_link_libraries(PascalSyntaxAnalyzer PRIVATE WinFuncs)
endif()
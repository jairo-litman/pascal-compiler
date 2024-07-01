file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_library(PascalToken token.c ${INCLUDE_DIR}/token.h)
add_library(PascalLexer lexer.c ${INCLUDE_DIR}/lexer.h)
add_library(PascalREPL repl.c ${INCLUDE_DIR}/repl.h)
add_library(HashMap hashmap.c ${INCLUDE_DIR}/hashmap.h)
add_library(PascalAST ast.c ${INCLUDE_DIR}/ast.h)
add_library(PascalParser parser.c ${INCLUDE_DIR}/parser.h)
add_library(Hash hash.c ${INCLUDE_DIR}/hash.h)
add_library(ErrorList error.c ${INCLUDE_DIR}/error.h)
if (WIN32)
    add_library(WinFuncs winfuncs.c ${INCLUDE_DIR}/winfuncs.h)
endif()


target_include_directories(PascalToken PUBLIC ${INCLUDE_DIR})
target_include_directories(PascalLexer PUBLIC ${INCLUDE_DIR})
target_include_directories(PascalREPL PUBLIC ${INCLUDE_DIR})
target_include_directories(HashMap PUBLIC ${INCLUDE_DIR})
target_include_directories(PascalAST PUBLIC ${INCLUDE_DIR})
target_include_directories(PascalParser PUBLIC ${INCLUDE_DIR})
target_include_directories(Hash PUBLIC ${INCLUDE_DIR})
target_include_directories(ErrorList PUBLIC ${INCLUDE_DIR})
if (WIN32)
    target_include_directories(WinFuncs PUBLIC ${INCLUDE_DIR})
endif()

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_library(PascalToken token.c ${INCLUDE_DIR}/token.h)
add_library(PascalLexer lexer.c ${INCLUDE_DIR}/lexer.h)
add_library(REPL repl.c ${INCLUDE_DIR}/repl.h)

target_include_directories(PascalToken PUBLIC ${INCLUDE_DIR})
target_include_directories(PascalLexer PUBLIC ${INCLUDE_DIR})
target_include_directories(REPL PUBLIC ${INCLUDE_DIR})

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
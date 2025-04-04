cmake_minimum_required (VERSION 3.28)

project(OBJtoBSP C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(
    -std=c99
    -Wpedantic
    -Wextra
    -Werror
    -Wall
    -Wstrict-aliasing=3
    -Wwrite-strings
    -Wvla
    -Wcast-align=strict
    -Wstrict-prototypes
    -Wstringop-overflow=4
    -Wshadow
    -fanalyzer
    -g
    -DDEBUG_BUILD
)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SRC_FILES
    "${SRC_DIR}/dynamic_arr.c"
    "${SRC_DIR}/utils.c"
    "${SRC_DIR}/main.c"
    "${SRC_DIR}/obj/file.c"
    "${SRC_DIR}/bsp_tree/tree.c"
)

set(INCL_DIR "${PROJECT_SOURCE_DIR}/include")
set(INCL_FILES
    "${INCL_DIR}/dynamic_arr.h"
    "${INCL_DIR}/utils.h"
    "${INCL_DIR}/log.h"
    "${INCL_DIR}/obj/file.h"
    "${INCL_DIR}/bsp_tree/tree.h"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR})

#FIXME: we shouldn't be relying on preinstalled libs for this.
# download all of these (except for libm) locally and store it in ./libs/
find_library(GNU_SCI_LIB gsl REQUIRED)
target_link_libraries(${PROJECT_NAME} ${GNU_SCI_LIB})
find_library(GNU_SCI_LIB_CBLAS gslcblas REQUIRED)
target_link_libraries(${PROJECT_NAME} ${GNU_SCI_LIB_CBLAS})
find_library(MAT_LIB m REQUIRED)
target_link_libraries(${PROJECT_NAME} ${MAT_LIB})

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -style=GNU -i ${SRC_FILES} ${INCL_FILES}
        COMMENT "Running clang-format on source files."
    )

    add_dependencies(${PROJECT_NAME} format)
else()
    message(WARNING "clang-format not found. Code formatting skipped.")
endif()

find_program(VALGRIND_EXECUTABLE NAMES valgrind)
if(VALGRIND_EXECUTABLE)
    add_custom_target(test
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=yes ./${PROJECT_NAME} test.obj
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running valgrind leak check on executable."
    )

    add_custom_target(full
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target test
        COMMENT "Running full build (clean, format, build, & test)."
    )
else()
    message(WARNING "valgrind not found. Memory leak check skipped.")
endif()

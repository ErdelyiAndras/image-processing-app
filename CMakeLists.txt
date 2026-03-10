cmake_minimum_required(VERSION 3.21)
project(image-processing-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(
        /W4
        /WX
        /w44388
        /w44365
        /w44267
        /w44242
        /w44244
        /w44018
        /w44389
        /permissive-
        /analyze
        /analyze:external-
        /experimental:external
        /external:anglebrackets
        /external:W0
        /external:templates-
    )
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL:NO>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror
        -Wsign-conversion -Wsign-compare -Wconversion
        -Wshadow -Wnull-dereference
        -Wuninitialized -Wmaybe-uninitialized
        -Wdouble-promotion -Wfloat-conversion
        -Wlogical-op
        -Wold-style-cast -Wcast-qual -Wcast-align
        -Wstrict-overflow=5
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror
        -Wsign-conversion -Wsign-compare -Wconversion
        -Wshadow -Wnull-dereference
        -Wuninitialized
        -Wdouble-promotion -Wfloat-conversion
        -Wunreachable-code
        -Wold-style-cast -Wcast-qual -Wcast-align
    )
endif()

option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

if (WARNINGS_AS_ERRORS)
    if (MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE image_processing_lib)

if (MSVC)
    get_filename_component(MSVC_TOOLS_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB ASAN_DLLS "${MSVC_TOOLS_BIN}/clang_rt.asan*dynamic*.dll")
    foreach(DLL ${ASAN_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E $<IF:$<CONFIG:Debug>,copy_if_different,true>
                "${DLL}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying ASan DLL: ${DLL}"
        )
    endforeach()
endif()

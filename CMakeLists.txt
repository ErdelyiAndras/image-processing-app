cmake_minimum_required(VERSION 3.21)
project(image-processing-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;WarningsAsErrors" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS_WARNINGSASERRORS             "${CMAKE_CXX_FLAGS_DEBUG}"             CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_WARNINGSASERRORS      "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"      CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_WARNINGSASERRORS   "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}"   CACHE STRING "" FORCE)

set(IS_DEBUG_LIKE           "$<OR:$<CONFIG:Debug>,$<CONFIG:WarningsAsErrors>>")
set(IS_MSVC                 "$<CXX_COMPILER_ID:MSVC>")
set(IS_CLANG_CL             "$<CXX_COMPILER_ID:Clang>")
set(IS_DEBUG_LIKE_AND_MSVC     "$<AND:${IS_DEBUG_LIKE},${IS_MSVC}>")
set(IS_DEBUG_LIKE_AND_CLANG_CL "$<AND:${IS_DEBUG_LIKE},${IS_CLANG_CL}>")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_compile_options(
    # warnings — both compilers
    $<${IS_DEBUG_LIKE}:/W4>
    $<${IS_DEBUG_LIKE}:/w44388>
    $<${IS_DEBUG_LIKE}:/w44365>
    $<${IS_DEBUG_LIKE}:/w44267>
    $<${IS_DEBUG_LIKE}:/w44242>
    $<${IS_DEBUG_LIKE}:/w44244>
    $<${IS_DEBUG_LIKE}:/w44018>
    $<${IS_DEBUG_LIKE}:/w44389>
    $<${IS_DEBUG_LIKE}:/permissive->
    $<$<CONFIG:WarningsAsErrors>:/WX>

    # clang-cl extra warnings
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wextra>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wconversion>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wshadow>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wnull-dereference>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wdouble-promotion>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wunreachable-code>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wold-style-cast>

    # MSVC only — static analysis and ASan
    $<${IS_DEBUG_LIKE_AND_MSVC}:/analyze>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/analyze:external->
    $<${IS_DEBUG_LIKE_AND_MSVC}:/experimental:external>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/external:anglebrackets>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/external:W0>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/external:templates->
    $<${IS_DEBUG_LIKE_AND_MSVC}:/fsanitize=address>
)

add_link_options($<${IS_DEBUG_LIKE_AND_MSVC}:/INCREMENTAL:NO>)

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE image_processing_lib)

get_filename_component(MSVC_TOOLS_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
file(GLOB ASAN_DLLS "${MSVC_TOOLS_BIN}/clang_rt.asan*dynamic*.dll")
foreach(DLL ${ASAN_DLLS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
            $<IF:${IS_DEBUG_LIKE_AND_MSVC},copy_if_different,true>
            "${DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endforeach()

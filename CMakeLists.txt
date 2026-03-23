cmake_minimum_required(VERSION 3.21)
project(image-processing-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;WarningsAsErrors" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS_WARNINGSASERRORS             "${CMAKE_CXX_FLAGS_DEBUG}"             CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_WARNINGSASERRORS      "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"      CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_WARNINGSASERRORS   "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}"   CACHE STRING "" FORCE)

set(IS_DEBUG_LIKE      "$<OR:$<CONFIG:Debug>,$<CONFIG:WarningsAsErrors>>")
set(IS_RELEASE         "$<CONFIG:Release>")
set(IS_MSVC            "$<CXX_COMPILER_ID:MSVC>")
set(IS_CLANG_CL        "$<CXX_COMPILER_ID:Clang>")

set(IS_DEBUG_LIKE_AND_MSVC     "$<AND:${IS_DEBUG_LIKE},${IS_MSVC}>")
set(IS_DEBUG_LIKE_AND_CLANG_CL "$<AND:${IS_DEBUG_LIKE},${IS_CLANG_CL}>")
set(IS_RELEASE_AND_MSVC        "$<AND:${IS_RELEASE},${IS_MSVC}>")
set(IS_RELEASE_AND_CLANG_CL    "$<AND:${IS_RELEASE},${IS_CLANG_CL}>")

set(CMAKE_MSVC_RUNTIME_LIBRARY
    "$<IF:$<CXX_COMPILER_ID:Clang>,MultiThreadedDLL,$<IF:$<CONFIG:Release>,MultiThreadedDLL,MultiThreadedDebugDLL>>"
)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "WarningsAsErrors")
            set(_CLANG_TIDY_BASE
                "${CLANG_TIDY_EXE};--extra-arg=--driver-mode=cl;--extra-arg=/EHsc"
            )
            if(CMAKE_BUILD_TYPE STREQUAL "WarningsAsErrors")
                set(CMAKE_CXX_CLANG_TIDY "${_CLANG_TIDY_BASE};--warnings-as-errors=*")
            else()
                set(CMAKE_CXX_CLANG_TIDY "${_CLANG_TIDY_BASE}")
            endif()
            message(STATUS "clang-tidy: ${CLANG_TIDY_EXE}")
        endif()
    else()
        message(STATUS "clang-tidy: found but skipped for MSVC (/analyze is used instead)")
    endif()
else()
    message(WARNING "clang-tidy: not found - static analysis disabled for clang-cl builds")
endif()

set(_ASAN_CLANG_AVAILABLE FALSE)
set(_UBSAN_AVAILABLE      FALSE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    get_filename_component(_COMPILER_BIN  "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(_COMPILER_ROOT "${_COMPILER_BIN}" DIRECTORY)

    file(GLOB_RECURSE _ASAN_DYN_LIB_CANDIDATES
        "${_COMPILER_ROOT}/lib/clang_rt.asan_dynamic-x86_64.lib"
    )

    if(NOT _ASAN_DYN_LIB_CANDIDATES)
        message(WARNING
            "clang-cl ASan runtime not found under ${_COMPILER_ROOT}/lib/\n"
            "Debug builds will work but ASan memory checking will be disabled.\n"
            "To enable: VS installer -> Individual Components -> 'C++ AddressSanitizer'."
        )
    else()
        set(_ASAN_RT_DIR "")
        foreach(_CANDIDATE IN LISTS _ASAN_DYN_LIB_CANDIDATES)
            get_filename_component(_CANDIDATE_DIR "${_CANDIDATE}" DIRECTORY)
            if(EXISTS "${_CANDIDATE_DIR}/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib")
                set(_ASAN_RT_DIR "${_CANDIDATE_DIR}")
                break()
            endif()
        endforeach()

        if(NOT _ASAN_RT_DIR)
            message(WARNING
                "clang-cl ASan thunk library not found alongside the dynamic library.\n"
                "Debug builds will work but ASan memory checking will be disabled.\n"
                "To enable: VS installer -> Individual Components -> 'C++ AddressSanitizer'."
            )
        else()
            set(_ASAN_DYN_LIB   "${_ASAN_RT_DIR}/clang_rt.asan_dynamic-x86_64.lib")
            set(_ASAN_THUNK_LIB "${_ASAN_RT_DIR}/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib")
            set(_ASAN_DYN_DLL   "${_ASAN_RT_DIR}/clang_rt.asan_dynamic-x86_64.dll")
            set(_ASAN_CLANG_AVAILABLE TRUE)
            message(STATUS "clang-cl ASan runtime: ${_ASAN_RT_DIR}")
        endif()
    endif()

    file(GLOB_RECURSE _UBSAN_LIB_CANDIDATES
        "${_COMPILER_ROOT}/lib/clang_rt.ubsan_standalone-x86_64.lib"
    )
    if(NOT _UBSAN_LIB_CANDIDATES)
        message(WARNING
            "clang-cl UBSan runtime not found under ${_COMPILER_ROOT}/lib/\n"
            "Debug builds will work but UBSan undefined-behaviour checking will be disabled.\n"
            "To enable: VS installer -> Individual Components -> 'C++ Clang Compiler for Windows'."
        )
    else()
        list(GET _UBSAN_LIB_CANDIDATES 0 _UBSAN_FIRST)
        get_filename_component(_UBSAN_RT_DIR "${_UBSAN_FIRST}" DIRECTORY)
        set(_UBSAN_LIB     "${_UBSAN_RT_DIR}/clang_rt.ubsan_standalone-x86_64.lib")
        set(_UBSAN_CXX_LIB "${_UBSAN_RT_DIR}/clang_rt.ubsan_standalone_cxx-x86_64.lib")
        set(_UBSAN_AVAILABLE TRUE)
        message(STATUS "clang-cl UBSan runtime: ${_UBSAN_RT_DIR}")
    endif()
endif()

set(_ASAN_MSVC_AVAILABLE FALSE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    get_filename_component(_MSVC_COMPILER_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB _MSVC_ASAN_DLLS "${_MSVC_COMPILER_BIN}/clang_rt.asan*dynamic*.dll")
    if(NOT _MSVC_ASAN_DLLS)
        message(WARNING
            "MSVC ASan runtime DLL not found - ASan disabled for MSVC debug builds.\n"
            "To enable: VS installer -> Individual Components -> 'C++ AddressSanitizer'."
        )
    else()
        set(_ASAN_MSVC_AVAILABLE TRUE)
    endif()
endif()

option(ENABLE_LTO "Enable Link-Time Optimization for Release builds" ON)

set(IS_LTO_MSVC     "$<AND:${IS_RELEASE_AND_MSVC},$<BOOL:${ENABLE_LTO}>>")
set(IS_LTO_CLANG_CL "$<AND:${IS_RELEASE_AND_CLANG_CL},$<BOOL:${ENABLE_LTO}>>")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_compile_options(
    $<${IS_MSVC}:/MP>

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

    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wextra>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wconversion>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wshadow>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wnull-dereference>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wdouble-promotion>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wunreachable-code>
    $<${IS_DEBUG_LIKE_AND_CLANG_CL}:-Wold-style-cast>

    $<${IS_LTO_CLANG_CL}:-flto=thin>

    $<${IS_DEBUG_LIKE_AND_MSVC}:/analyze>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/analyze:external->
    $<${IS_DEBUG_LIKE_AND_MSVC}:/experimental:external>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/external:anglebrackets>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/external:W0>
    $<${IS_DEBUG_LIKE_AND_MSVC}:/external:templates->
    $<${IS_DEBUG_LIKE_AND_MSVC}:/sdl>

    $<${IS_MSVC}:/guard:cf>
    $<${IS_MSVC}:/Gy>
    $<${IS_MSVC}:/Gw>

    $<${IS_LTO_MSVC}:/GL>
)

if(_ASAN_CLANG_AVAILABLE)
    add_compile_options($<${IS_DEBUG_LIKE_AND_CLANG_CL}:-fsanitize=address>)
endif()
if(_UBSAN_AVAILABLE)
    add_compile_options($<${IS_DEBUG_LIKE_AND_CLANG_CL}:-fsanitize=undefined>)
endif()
if(_ASAN_MSVC_AVAILABLE)
    add_compile_options($<${IS_DEBUG_LIKE_AND_MSVC}:/fsanitize=address>)
endif()

add_link_options(
    $<${IS_DEBUG_LIKE_AND_MSVC}:/INCREMENTAL:NO>
    $<${IS_MSVC}:/GUARD:CF>
    $<${IS_RELEASE_AND_MSVC}:/OPT:REF,ICF>
    $<${IS_LTO_MSVC}:/LTCG>
    $<${IS_LTO_CLANG_CL}:-flto=thin>
)

if(_ASAN_CLANG_AVAILABLE)
    add_link_options(
        "$<${IS_DEBUG_LIKE_AND_CLANG_CL}:${_ASAN_DYN_LIB}>"
        "$<${IS_DEBUG_LIKE_AND_CLANG_CL}:/wholearchive:${_ASAN_THUNK_LIB}>"
    )
endif()
if(_UBSAN_AVAILABLE)
    add_link_options(
        "$<${IS_DEBUG_LIKE_AND_CLANG_CL}:${_UBSAN_LIB}>"
        "$<${IS_DEBUG_LIKE_AND_CLANG_CL}:${_UBSAN_CXX_LIB}>"
    )
endif()

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE image_processing_lib)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(_ASAN_CLANG_AVAILABLE AND EXISTS "${_ASAN_DYN_DLL}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E
                $<IF:${IS_DEBUG_LIKE},copy_if_different,true>
                "${_ASAN_DYN_DLL}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(_ASAN_MSVC_AVAILABLE)
        foreach(DLL ${_MSVC_ASAN_DLLS})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                    $<IF:${IS_DEBUG_LIKE},copy_if_different,true>
                    "${DLL}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            )
        endforeach()
    endif()
endif()

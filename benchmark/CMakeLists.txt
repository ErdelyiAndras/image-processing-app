include(FetchContent)

FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(BENCHMARK_ENABLE_TESTING     OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL     OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googlebenchmark)

foreach(_bm_target IN ITEMS benchmark benchmark_main)
    if(TARGET ${_bm_target})
        target_compile_options(${_bm_target} PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:
                -Wno-double-promotion
                -Wno-old-style-cast
                -Wno-conversion
                -Wno-shadow
                -Wno-null-dereference
                -Wno-unreachable-code
                -Wno-extra
            >
        )
    endif()
endforeach()

add_executable(image-processing-app-benchmark ComponentBenchmarks.cpp)

target_link_libraries(image-processing-app-benchmark
    PRIVATE
    benchmark::benchmark
    image_processing_lib
    delayimp
)

target_include_directories(image-processing-app-benchmark PRIVATE
    ${CMAKE_SOURCE_DIR}/src/components/denoising/gaussian-blur/cpu
    ${CMAKE_SOURCE_DIR}/src/components/denoising/gaussian-blur/gpu
    ${CMAKE_SOURCE_DIR}/src/components/denoising/tv-denoising/cpu
    ${CMAKE_SOURCE_DIR}/src/components/denoising/tv-denoising/gpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/sobel/cpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/sobel/gpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/canny/cpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/canny/gpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-line/cpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-line/gpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-circle/cpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-circle/gpu
)

target_compile_definitions(image-processing-app-benchmark PRIVATE
    "BENCHMARK_IMAGES_DIR=\"${CMAKE_SOURCE_DIR}/reference-images\""
)

target_link_options(image-processing-app-benchmark PRIVATE
    $<$<CXX_COMPILER_ID:MSVC,Clang>:/DELAYLOAD:OpenCL.dll>
)

get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi_config)
    foreach(_cfg IN ITEMS Debug Release WarningsAsErrors Tests)
        string(TOUPPER "${_cfg}" _cfg_upper)
        set_target_properties(image-processing-app-benchmark PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${_cfg_upper}" "${CMAKE_BINARY_DIR}/${_cfg}")
    endforeach()
else()
    set_target_properties(image-processing-app-benchmark PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(_ASAN_CLANG_AVAILABLE AND EXISTS "${_ASAN_DYN_DLL}")
        add_custom_command(TARGET image-processing-app-benchmark POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E
                $<IF:${IS_DEBUG_LIKE},copy_if_different,true>
                "${_ASAN_DYN_DLL}"
                "$<TARGET_FILE_DIR:image-processing-app-benchmark>"
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(_ASAN_MSVC_AVAILABLE)
        foreach(DLL ${_MSVC_ASAN_DLLS})
            add_custom_command(TARGET image-processing-app-benchmark POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                    $<IF:${IS_DEBUG_LIKE},copy_if_different,true>
                    "${DLL}"
                    "$<TARGET_FILE_DIR:image-processing-app-benchmark>"
            )
        endforeach()
    endif()
endif()

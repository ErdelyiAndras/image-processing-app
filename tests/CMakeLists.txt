include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

foreach(_gtest_target IN ITEMS gtest gtest_main)
    if(TARGET ${_gtest_target})
        target_compile_options(${_gtest_target} PRIVATE
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

        if(MSVC)
            get_target_property(_gtest_flags ${_gtest_target} COMPILE_FLAGS)
            if(_gtest_flags)
                string(REGEX REPLACE "[-/]WX" "" _gtest_flags "${_gtest_flags}")
                string(STRIP "${_gtest_flags}" _gtest_flags)
                set_target_properties(${_gtest_target} PROPERTIES
                    COMPILE_FLAGS "${_gtest_flags}")
            endif()
        endif()
    endif()
endforeach()

enable_testing()

add_subdirectory(pipeline)
add_subdirectory(components)

add_executable(image-processing-app-tests
    pipeline/PipelineTests.cpp
    components/ComponentRegressionTests.cpp
)

target_link_libraries(image-processing-app-tests
    PRIVATE
    GTest::gtest_main
    image_processing_lib
    delayimp
)

target_include_directories(image-processing-app-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pipeline
    ${CMAKE_SOURCE_DIR}/src/components/denoising/gaussian-blur/cpu
    ${CMAKE_SOURCE_DIR}/src/components/denoising/tv-denoising/cpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/sobel/cpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/canny/cpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-line/cpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-circle/cpu
)

target_compile_definitions(image-processing-app-tests PRIVATE
    "REFERENCE_IMAGES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/components/reference-images\""
)

target_link_options(image-processing-app-tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC,Clang>:/DELAYLOAD:OpenCL.dll>
)

get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi_config)
    foreach(_cfg IN ITEMS Debug Release WarningsAsErrors Tests)
        string(TOUPPER "${_cfg}" _cfg_upper)
        set_target_properties(image-processing-app-tests PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${_cfg_upper}" "${CMAKE_BINARY_DIR}/${_cfg}")
    endforeach()
else()
    set_target_properties(image-processing-app-tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(_ASAN_CLANG_AVAILABLE AND EXISTS "${_ASAN_DYN_DLL}")
        add_custom_command(TARGET image-processing-app-tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E
                $<IF:${IS_DEBUG_LIKE},copy_if_different,true>
                "${_ASAN_DYN_DLL}"
                "$<TARGET_FILE_DIR:image-processing-app-tests>"
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(_ASAN_MSVC_AVAILABLE)
        foreach(DLL ${_MSVC_ASAN_DLLS})
            add_custom_command(TARGET image-processing-app-tests POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                    $<IF:${IS_DEBUG_LIKE},copy_if_different,true>
                    "${DLL}"
                    "$<TARGET_FILE_DIR:image-processing-app-tests>"
            )
        endforeach()
    endif()
endif()

include(GoogleTest)
gtest_discover_tests(image-processing-app-tests)

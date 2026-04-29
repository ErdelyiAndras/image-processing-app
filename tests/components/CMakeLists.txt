add_executable(component_tests
    ComponentRegressionTests.cpp
)

target_link_libraries(component_tests
    PRIVATE
    GTest::gtest_main
    image_processing_lib
)

target_include_directories(component_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/components/denoising/gaussian-blur/cpu
    ${CMAKE_SOURCE_DIR}/src/components/denoising/tv-denoising/cpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/sobel/cpu
    ${CMAKE_SOURCE_DIR}/src/components/edge-detection/canny/cpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-line/cpu
    ${CMAKE_SOURCE_DIR}/src/components/shape-detection/hough-circle/cpu
)

target_link_options(component_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC,Clang>:/DELAYLOAD:OpenCL.dll>
)
target_link_libraries(component_tests PRIVATE delayimp)

target_compile_definitions(component_tests PRIVATE
    "REFERENCE_IMAGES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/reference-images\""
)

include(GoogleTest)
gtest_discover_tests(component_tests)

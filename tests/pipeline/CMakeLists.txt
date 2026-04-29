add_executable(pipeline_tests
    PipelineTests.cpp
)

target_link_libraries(pipeline_tests
    PRIVATE
    GTest::gtest_main
    image_processing_lib
)

target_link_options(pipeline_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC,Clang>:/DELAYLOAD:OpenCL.dll>
)
target_link_libraries(pipeline_tests PRIVATE delayimp)

include(GoogleTest)
gtest_discover_tests(pipeline_tests)

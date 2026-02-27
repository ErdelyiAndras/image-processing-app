add_library(canny_gpu OBJECT
    CannyEdgeDetectionGPU.cpp
)

target_include_directories(canny_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(canny_gpu PUBLIC
    core
    config
)

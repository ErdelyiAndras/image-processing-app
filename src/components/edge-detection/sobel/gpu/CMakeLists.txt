add_library(sobel_gpu OBJECT
    SobelEdgeDetectionGPU.cpp
)

target_include_directories(sobel_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(sobel_gpu PUBLIC
    core
    config
)

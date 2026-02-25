add_library(sobel_cpu OBJECT
    SobelEdgeDetectionCPU.cpp
)

target_include_directories(sobel_cpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(sobel_cpu PUBLIC
    core
    config
)

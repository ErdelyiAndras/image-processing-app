add_library(gaussian_blur_gpu OBJECT
    GaussianBlurGPU.cpp
)

target_include_directories(gaussian_blur_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(gaussian_blur_gpu PUBLIC
    core
    config
)

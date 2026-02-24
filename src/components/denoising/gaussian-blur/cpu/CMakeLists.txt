add_library(gaussian_blur_cpu OBJECT
    GaussianBlurCPU.cpp
)

target_include_directories(gaussian_blur_cpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(gaussian_blur_cpu PUBLIC
    core
    config
)

add_subdirectory(cpu)
add_subdirectory(gpu)

add_library(gaussian_blur OBJECT
    GaussianBlurComponent.cpp
)

target_include_directories(gaussian_blur PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(gaussian_blur PUBLIC
    core
    config
    gaussian_blur_cpu
    gaussian_blur_gpu
)

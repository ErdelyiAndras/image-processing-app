add_subdirectory(tv-denoising)
add_subdirectory(gaussian-blur)

add_library(denoising STATIC)

target_include_directories(denoising PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(denoising PUBLIC
    core
    config
    tv_denoising
    gaussian_blur
)

target_sources(denoising PRIVATE
    DenoisingComponent.cpp
    $<TARGET_OBJECTS:tv_denoising>
    $<TARGET_OBJECTS:tv_denoising_cpu>
    $<TARGET_OBJECTS:tv_denoising_gpu>
    $<TARGET_OBJECTS:gaussian_blur>
    $<TARGET_OBJECTS:gaussian_blur_cpu>
    $<TARGET_OBJECTS:gaussian_blur_gpu>
)

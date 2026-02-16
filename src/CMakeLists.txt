add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(components)

set(DENOISING_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/kernels/denoising/TVDenoising.cl")

add_library(image_processing_lib INTERFACE)
target_link_libraries(image_processing_lib INTERFACE
    utils
    core
    components
)

target_compile_definitions(image_processing_lib INTERFACE
    DENOISING_KERNEL_PATH="${DENOISING_KERNEL_PATH}"
)

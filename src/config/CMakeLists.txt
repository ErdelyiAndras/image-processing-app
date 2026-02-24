add_library(config INTERFACE)

target_include_directories(config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

set(TV_DENOISING_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../kernels/denoising/TVDenoising.cl")
set(GAUSSIAN_BLUR_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../kernels/denoising/GaussianBlur.cl")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(config INTERFACE
        ENABLE_LOGGING=1
    )
else()
    target_compile_definitions(config INTERFACE
        ENABLE_LOGGING=0
    )
endif()

target_compile_definitions(config INTERFACE
    TV_DENOISING_KERNEL_PATH="${TV_DENOISING_KERNEL_PATH}"
    GAUSSIAN_BLUR_KERNEL_PATH="${GAUSSIAN_BLUR_KERNEL_PATH}"
)

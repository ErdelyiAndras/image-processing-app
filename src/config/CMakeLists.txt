add_library(config INTERFACE)

target_include_directories(config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

set(DENOISING_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../kernels/denoising/TVDenoising.cl")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(config INTERFACE ENABLE_LOGGING=1 DENOISING_KERNEL_PATH="${DENOISING_KERNEL_PATH}")
else()
    target_compile_definitions(config INTERFACE ENABLE_LOGGING=0 DENOISING_KERNEL_PATH="${DENOISING_KERNEL_PATH}")
endif()

add_library(denoising
    tv-denoising/gpu/TVDenoisingGPU.cpp
    tv-denoising/cpu/TVDenoisingCPU.cpp
)

target_include_directories(denoising PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tv-denoising/gpu
    ${CMAKE_CURRENT_SOURCE_DIR}/tv-denoising/cpu
)

target_link_libraries(denoising PUBLIC
    core
)

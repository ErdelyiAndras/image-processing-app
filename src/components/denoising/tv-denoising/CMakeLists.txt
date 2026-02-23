add_subdirectory(cpu)
add_subdirectory(gpu)

add_library(tv_denoising OBJECT
    TVDenoisingComponent.cpp
)

target_include_directories(tv_denoising PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(tv_denoising PUBLIC
    core
    config
    tv_denoising_cpu
    tv_denoising_gpu
)

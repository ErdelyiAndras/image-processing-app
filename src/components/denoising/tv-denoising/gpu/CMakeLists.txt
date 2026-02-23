add_library(tv_denoising_gpu OBJECT
    TVDenoisingGPU.cpp
)

target_include_directories(tv_denoising_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(tv_denoising_gpu PUBLIC
    core
    config
)

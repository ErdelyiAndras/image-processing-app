add_library(tv_denoising_cpu OBJECT
    TVDenoisingCPU.cpp
)

target_include_directories(tv_denoising_cpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(tv_denoising_cpu PUBLIC
    core
    config
)

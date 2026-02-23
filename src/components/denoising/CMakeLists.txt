add_subdirectory(tv-denoising)

add_library(denoising STATIC)

target_include_directories(denoising PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(denoising PUBLIC
    core
    config
    tv_denoising
)

target_sources(denoising PRIVATE
    DenoisingComponent.cpp
)

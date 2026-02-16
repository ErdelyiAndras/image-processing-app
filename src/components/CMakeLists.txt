add_subdirectory(denoising)

add_library(components INTERFACE)
target_link_libraries(components INTERFACE
    denoising
)

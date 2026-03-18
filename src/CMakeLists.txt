add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(components)
add_subdirectory(config)
add_subdirectory(pipeline)

add_library(image_processing_lib INTERFACE)
target_link_libraries(image_processing_lib INTERFACE
    utils
    core
    components
    config
    pipeline
)

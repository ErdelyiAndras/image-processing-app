add_library(cli STATIC
    PipelineController.cpp
)

target_include_directories(cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cli PUBLIC
    image_processing_lib
)

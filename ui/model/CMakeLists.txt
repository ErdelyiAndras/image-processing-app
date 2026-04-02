add_library(ui_model STATIC
    PipelineModel.cpp
)

target_include_directories(ui_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ui_model PUBLIC
    image_processing_lib
)

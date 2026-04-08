add_library(cli STATIC
    ParameterPrompter.cpp
    PipelineController.cpp
    Terminal.cpp
)

target_include_directories(cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cli PUBLIC
    ui_model
)

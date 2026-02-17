add_library(config INTERFACE)

target_include_directories(config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(config INTERFACE ENABLE_LOGGING=1)
else()
    target_compile_definitions(config INTERFACE ENABLE_LOGGING=0)
endif()

find_package(OpenCL CONFIG REQUIRED)

add_library(core INTERFACE)

target_include_directories(core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core INTERFACE
    utils
    OpenCL::OpenCL
    OpenCL::HeadersCpp
)

target_compile_definitions(core INTERFACE
    CL_HPP_ENABLE_EXCEPTIONS
    CL_HPP_TARGET_OPENCL_VERSION=200
)

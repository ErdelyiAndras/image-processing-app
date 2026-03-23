find_package(OpenCL CONFIG REQUIRED)

add_library(core STATIC
    Component.cpp
    GPUComponent.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core PUBLIC
    utils
    config
    OpenCL::OpenCL
    OpenCL::HeadersCpp
)

target_compile_definitions(core PUBLIC
    CL_HPP_ENABLE_EXCEPTIONS
    CL_HPP_TARGET_OPENCL_VERSION=200
)

get_target_property(OCL_INCLUDE_DIRS OpenCL::HeadersCpp INTERFACE_INCLUDE_DIRECTORIES)
foreach(OCL_INCLUDE_DIR ${OCL_INCLUDE_DIRS})
    target_compile_options(core PUBLIC
        /external:I${OCL_INCLUDE_DIR}
    )
endforeach()

target_precompile_headers(core PUBLIC
    <CL/opencl.hpp>
    <algorithm>
    <cmath>
    <cstdint>
    <memory>
    <stdexcept>
    <string>
    <vector>
)

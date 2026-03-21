find_package(Stb REQUIRED)

add_library(utils
    Image.cpp
    Color.cpp
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

foreach(STB_INCLUDE_DIR ${Stb_INCLUDE_DIR})
    target_include_directories(utils SYSTEM PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:${STB_INCLUDE_DIR}>
    )
    target_compile_options(utils PUBLIC
        $<$<CXX_COMPILER_ID:Clang>:-imsvc${STB_INCLUDE_DIR}>
    )
endforeach()

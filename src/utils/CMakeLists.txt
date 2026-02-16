find_package(Stb REQUIRED)

add_library(utils
    Image.cpp
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Stb_INCLUDE_DIR}
)

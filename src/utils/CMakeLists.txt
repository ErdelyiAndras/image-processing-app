find_package(Stb REQUIRED)

add_library(utils
    Image.cpp
    Color.cpp
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (MSVC)
    foreach(STB_INCLUDE_DIR ${Stb_INCLUDE_DIR})
        target_compile_options(utils PUBLIC
            "SHELL:/external:I ${STB_INCLUDE_DIR}"
        )
    endforeach()
else()
    foreach(STB_INCLUDE_DIR ${Stb_INCLUDE_DIR})
        target_include_directories(utils SYSTEM PUBLIC
            ${STB_INCLUDE_DIR}
        )
    endforeach()
endif()

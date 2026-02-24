cmake_minimum_required(VERSION 3.21)
project(image-processing-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL:NO>)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE image_processing_lib)

if (MSVC)
    get_filename_component(MSVC_TOOLS_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB ASAN_DLLS "${MSVC_TOOLS_BIN}/clang_rt.asan*dynamic*.dll")
    foreach(DLL ${ASAN_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E $<IF:$<CONFIG:Debug>,copy_if_different,true>
                "${DLL}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying ASan DLL: ${DLL}"
        )
    endforeach()
endif()

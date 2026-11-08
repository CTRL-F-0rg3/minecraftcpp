cmake_minimum_required(VERSION 3.16)
project(MinecraftCPP VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(COMPILED_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${COMPILED_SHADER_DIR})

function(compile_shader SHADER_NAME OUTPUT_NAME)
    add_custom_command(
        OUTPUT ${COMPILED_SHADER_DIR}/${OUTPUT_NAME}.spv
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER_DIR}/${SHADER_NAME} -o ${COMPILED_SHADER_DIR}/${OUTPUT_NAME}.spv
        DEPENDS ${SHADER_DIR}/${SHADER_NAME}
        COMMENT "Compiling shader ${SHADER_NAME}"
    )
    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER_DIR}/${OUTPUT_NAME}.spv)
    set(COMPILED_SHADERS ${COMPILED_SHADERS} PARENT_SCOPE)
endfunction()

compile_shader(shader.vert vert)
compile_shader(shader.frag frag)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/core/Engine.cpp
    src/renderer/Renderer.cpp
    src/world/World.cpp
    src/world/Chunk.cpp
)

add_custom_target(Shaders DEPENDS ${COMPILED_SHADERS})
add_dependencies(${PROJECT_NAME} Shaders)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw glm::glm)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()

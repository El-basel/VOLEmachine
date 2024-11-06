cmake_minimum_required(VERSION 3.26)
project(openGLinClion)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_DIR "${CMAKE_SOURCE_DIR}/glfw")

include_directories("${GLFW_DIR}/include")

add_subdirectory(${GLFW_DIR})

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui")
include_directories(${IMGUI_DIR})
set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

find_package(OpenGL REQUIRED)

add_executable(openGLinClion main.cpp ${IMGUI_SRC} vole.cpp vole.h)
target_link_libraries(openGLinClion glfw ${OPENGL_gl_LIBRARY})

if (WIN32)
    target_link_libraries(openGLinClion opengl32)
endif()


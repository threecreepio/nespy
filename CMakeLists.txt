cmake_minimum_required(VERSION 3.10)
enable_language(C)

project(NESpy)
include_directories(
    vendor/glad/include
    vendor/glfw/include
)
add_subdirectory(vendor/glfw)

file(COPY images DESTINATION .)
file(COPY NESpy.ini DESTINATION .)
file(COPY keycodes.txt DESTINATION .)

add_executable(NESpy
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ini.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inputs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keycodes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lodepng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NESpy.rc
)
target_link_libraries(NESpy glfw user32 dxguid dinput8 gdi32 opengl32 shell32 msvcrt)

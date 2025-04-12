cmake_minimum_required(VERSION 3.10)

project(NESpy)
enable_language(C)

include_directories(
    vendor/glad/include
    vendor/glfw/include
    vendor
)
add_subdirectory(vendor/glfw)

file(COPY images DESTINATION .)
file(COPY NESpy.ini DESTINATION .)
file(COPY keycodes.txt DESTINATION .)

add_executable(NESpy
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ini.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inputs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inputs_nes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inputs_kbd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inputs_joy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keycodes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/lodepng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NESpy.rc
)
target_link_libraries(NESpy glfw user32 dxguid dinput8 gdi32 opengl32 shell32 msvcrt )

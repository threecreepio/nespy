cmake_minimum_required(VERSION 3.10)

project(NESpy)
enable_language(C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

include_directories(
    vendor/glad/include
    vendor/glfw/include
    vendor
)
add_subdirectory(vendor/glfw)

file(COPY images DESTINATION ${CMAKE_BINARY_DIR}/out)
file(COPY NESpy.ini DESTINATION ${CMAKE_BINARY_DIR}/out)
file(COPY keycodes.txt DESTINATION ${CMAKE_BINARY_DIR}/out)
file(COPY firmware DESTINATION ${CMAKE_BINARY_DIR}/out)

add_executable(NESpy WIN32
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
target_link_libraries(NESpy glfw user32 dxguid dinput8 gdi32 opengl32 shell32 msvcrt)

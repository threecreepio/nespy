#define LODEPNG_NO_COMPILE_ENCODER
#define LODEPNG_NO_COMPILE_ERROR_TEXT
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <windows.h>
#include "lodepng.h"
#include "ini.h"
#include "inputs.h"
#include "nespy.h"

extern const uint8_t icon_bin[];
extern const uint32_t icon_bin_size;

uint32_t idlesleep = 1;

struct VTexData { GLfloat pos[2]; GLfloat uv[2]; };
static const struct VTexData vertices[] =
{
  { { -1.f, -1.f }, { 0.f, 1.f } },
  { { -1.f,  1.f }, { 0.f, 0.f } },
  { {  1.f,  1.f }, { 1.f, 0.f } },

  { { -1.f, -1.f }, { 0.f, 1.f } },
  { {  1.f,  1.f }, { 1.f, 0.f } },
  { {  1.f, -1.f }, { 1.f, 1.f } },
};

static const char* vertex_shader_text =
"#version 330\n"
"in vec2 vPos;\n"
"in vec2 vUV;\n"
"out vec2 uv;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"    uv = vUV;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"uniform int keys;\n"
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"
"uniform sampler2D tex2;\n"
"uniform sampler2D tex3;\n"
"uniform sampler2D tex4;\n"
"uniform sampler2D tex5;\n"
"uniform sampler2D tex6;\n"
"uniform sampler2D tex7;\n"
"uniform sampler2D tex8;\n"
"in vec2 uv;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = texture(tex0, uv);\n"
"    if (0x01 == (keys & 0x01) && texture(tex1, uv).a > 0) { fragment = (texture(tex1, uv));}"
"    if (0x02 == (keys & 0x02) && texture(tex2, uv).a > 0) { fragment = (texture(tex2, uv));}"
"    if (0x04 == (keys & 0x04) && texture(tex3, uv).a > 0) { fragment = (texture(tex3, uv));}"
"    if (0x08 == (keys & 0x08) && texture(tex4, uv).a > 0) { fragment = (texture(tex4, uv));}"
"    if (0x10 == (keys & 0x10) && texture(tex5, uv).a > 0) { fragment = (texture(tex5, uv));}"
"    if (0x20 == (keys & 0x20) && texture(tex6, uv).a > 0) { fragment = (texture(tex6, uv));}"
"    if (0x40 == (keys & 0x40) && texture(tex7, uv).a > 0) { fragment = (texture(tex7, uv));}"
"    if (0x80 == (keys & 0x80) && texture(tex8, uv).a > 0) { fragment = (texture(tex8, uv));}"
"}\n";

static void GlfwErrorCallback(int error, const char* description)
{
    fprintf(logfile, "glfw error %s\n", description);
}

static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

GLuint LoadTexture(const char *path) {
    uint32_t error;
    uint8_t* image;
    uint32_t width, height;
    error = lodepng_decode32_file(&image, &width, &height, path);
    if (error) {
        fprintf(logfile, "failed to load %s: %i\n", path, error);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);
    return textureID;
}

static int ReadSetting(void* user, const char* section, const char* name, const char* value)
{
    #define SETTING(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    InputReadSetting(user, section, name, value);
    if (SETTING("NESpy", "idlesleep")) {
        idlesleep = strtol(value, NULL, 10);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (fopen_s(&logfile, "NESpy.log", "w")) {
        logfile = stderr;
    }

    ini_parse("NESpy.ini", ReadSetting, 0);
    // ShowWindow(GetConsoleWindow(), SW_HIDE);

    glfwSetErrorCallback(GlfwErrorCallback);

    if (!glfwInit()) {
        fprintf(logfile, "glfw init failed\n");
        return -1;
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1, 1, "NESpy Controller", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(logfile, "failed to create window\n");
        fclose(logfile);
        return -1;
    }

    uint8_t *icondat;
    uint32_t width;
    uint32_t height;
    lodepng_decode32(&icondat, &width, &height, icon_bin, icon_bin_size);
    
    GLFWimage icons[1];
    icons[0].pixels = icondat;
    icons[0].width = width;
    icons[0].height = height;
    glfwSetWindowIcon(window, 1, icons);

    glfwSetKeyCallback(window, GlfwKeyCallback);

    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) {
        fprintf(logfile, "failed to init gl context\n");
        fclose(logfile);
        return -1;
    }

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vuv_location = glGetAttribLocation(program, "vUV");
    const GLint keys_location = glGetUniformLocation(program, "keys");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(struct VTexData), (void*) offsetof(struct VTexData, pos));
    glEnableVertexAttribArray(vuv_location);
    glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(struct VTexData), (void*) offsetof(struct VTexData, uv));

    GLuint textures[9];
    glUseProgram(program);
    for (int i=0; i<9; ++i) {
        char path[50];
        char attrib[50];
        sprintf(path, "images/%i.png", i);
        sprintf(attrib, "tex%i", i);

        glActiveTexture(GL_TEXTURE0 + i);
        textures[i] = LoadTexture(path);
        if (textures[i] == 0) {
            glfwTerminate();
            return -1;
        }

        if (i == 0) {
            int width, height;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glfwSetWindowSize(window, width, height);
            glViewport(0, 0, width, height);
        }
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glUniform1i(glGetUniformLocation(program, attrib), i);
    }
    
    fprintf(logfile, "waiting for input subsystem init\n");
    int errcount = 0;
    while (0 != InputStartup() && !glfwWindowShouldClose(window)) {
        errcount += 1;
        for (int i=0; i<10000; ++i) {
            if (!glfwWindowShouldClose(window)) {
                glClearColor(errcount % 2 > 0 ? 1.0f : 0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glFinish();
                glfwPollEvents();
            }
        }
    }

    uint8_t renderedControllerValue = 0xFF;
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window)) {
        if (currentInputs != renderedControllerValue) {
            renderedControllerValue = currentInputs;
            glUniform1i(keys_location, currentInputs);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glFinish();
        } else if (idlesleep) {
            Sleep(idlesleep);
        }
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
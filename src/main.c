#define LODEPNG_NO_COMPILE_ENCODER
#define LODEPNG_NO_COMPILE_ERROR_TEXT
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <windows.h>
#include "lodepng.h"
#include "ini.h"
#include "inputs.h"
#include "nespy.h"

#define INPUTS 12

FILE* logfile;
extern const uint8_t icon_bin[];
extern const uint32_t icon_bin_size;

int inputlogwindow = 0;
uint32_t idlesleep = 1;
char windowtitle[0x100] = "NESpy";
char imagepath[0x200] = "images";
char inputlogpath[0x200] = "";

static int ReadSetting(void* user, const char* section, const char* name, const char* value)
{
    InputReadSetting(user, section, name, value);
    if (SETTING("NESpy", "idlesleep"))
        idlesleep = strtol(value, NULL, 10);
    if (SETTING("NESpy", "imagepath"))
        snprintf(imagepath, sizeof(imagepath), "%s", value);
    if (SETTING("NESpy", "imagefolder"))
        snprintf(imagepath, sizeof(imagepath), "%s", value);
    if (SETTING("NESpy", "inputlog"))
        snprintf(inputlogpath, sizeof(inputlogpath), "%s", value);
    if (SETTING("NESpy", "windowtitle"))
        snprintf(windowtitle, sizeof(windowtitle), "%s", value);
    return 0;
}

struct VTexData
{
    GLfloat pos[2];
    GLfloat uv[2];
};
static const struct VTexData vertices[] =
{
    {{-1.f, -1.f}, {0.f, 1.f}},
    {{-1.f, 1.f}, {0.f, 0.f}},
    {{1.f, 1.f}, {1.f, 0.f}},
    {{-1.f, -1.f}, {0.f, 1.f}},
    {{1.f, 1.f}, {1.f, 0.f}},
    {{1.f, -1.f}, {1.f, 1.f}},
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
"uniform sampler2D tex9;\n"
"uniform sampler2D tex10;\n"
"uniform sampler2D tex11;\n"
"uniform sampler2D tex12;\n"
"in vec2 uv;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = texture(tex0, uv);\n"
"    if (0x001 == (keys & 0x001) && texture(tex1, uv).a > 0) { fragment = (texture(tex1, uv));}"
"    if (0x002 == (keys & 0x002) && texture(tex2, uv).a > 0) { fragment = (texture(tex2, uv));}"
"    if (0x004 == (keys & 0x004) && texture(tex3, uv).a > 0) { fragment = (texture(tex3, uv));}"
"    if (0x008 == (keys & 0x008) && texture(tex4, uv).a > 0) { fragment = (texture(tex4, uv));}"
"    if (0x010 == (keys & 0x010) && texture(tex5, uv).a > 0) { fragment = (texture(tex5, uv));}"
"    if (0x020 == (keys & 0x020) && texture(tex6, uv).a > 0) { fragment = (texture(tex6, uv));}"
"    if (0x040 == (keys & 0x040) && texture(tex7, uv).a > 0) { fragment = (texture(tex7, uv));}"
"    if (0x080 == (keys & 0x080) && texture(tex8, uv).a > 0) { fragment = (texture(tex8, uv));}"
"    if (0x100 == (keys & 0x100) && texture(tex9, uv).a > 0) { fragment = (texture(tex9, uv));}"
"    if (0x200 == (keys & 0x200) && texture(tex10, uv).a > 0) { fragment = (texture(tex10, uv));}"
"    if (0x400 == (keys & 0x400) && texture(tex11, uv).a > 0) { fragment = (texture(tex11, uv));}"
"    if (0x800 == (keys & 0x800) && texture(tex12, uv).a > 0) { fragment = (texture(tex12, uv));}"
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

GLuint LoadTexture(const char* path)
{
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

static void LoadIconResources(GLFWwindow* window)
{
    HGLOBAL res_handle = NULL;
    HRSRC res;

    // NOTE: providing g_hInstance is important, NULL might not work
    res = FindResource(GetModuleHandle(NULL), "icon_png", RT_RCDATA);
    if (!res) {
        fprintf(stderr, "no icon found\n");
        return;
    }
    res_handle = LoadResource(NULL, res);
    if (!res_handle) {
        fprintf(stderr, "could not load icon handle\n");
        return;
    }
    unsigned char* file = (unsigned char*)LockResource(res_handle);
    DWORD len = SizeofResource(NULL, res);

    if (file == 0) {
        fprintf(stderr, "could not lock resource\n");
        return;
    }

    uint8_t* icondat;
    uint32_t width;
    uint32_t height;
    lodepng_decode32(&icondat, &width, &height, file, len);

    GLFWimage icons[1];
    icons[0].pixels = icondat;
    icons[0].width = width;
    icons[0].height = height;
    glfwSetWindowIcon(window, 1, icons);
}

void showErrorFrame(GLFWwindow* window)
{
    int time = (int)glfwGetTime();
    glClearColor(time % 2 > 0 ? 1.0f : 0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFinish();
    glfwPollEvents();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    inputlog = stdout;
    if (0 != fopen_s(&logfile, "NESpy.log", "w")) {
        logfile = stderr;
    }

    ini_parse(lpCmdLine[0] != 0 ? lpCmdLine : "NESpy.ini", ReadSetting, 0);
    glfwSetErrorCallback(GlfwErrorCallback);

    // if we are logging to stdout, then a console window should be created
    if (strcmp(inputlogpath, "stdout") == 0) {
        AllocConsole();
        freopen_s(&inputlog, "CONOUT$", "w", stdout);
    // otherwise if we're logging to a file it should be opened
    } else if (inputlogpath[0]) {
        if (fopen_s(&inputlog, inputlogpath, "w")) {
            // fallback if we can't create the file.
            inputlog = stdout;
        }
    }

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
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1, 1, windowtitle, NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(logfile, "failed to create window\n");
        fclose(logfile);
        return -1;
    }

    LoadIconResources(window);

    glfwSetKeyCallback(window, GlfwKeyCallback);

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
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
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(struct VTexData), (void*)offsetof(struct VTexData, pos));
    glEnableVertexAttribArray(vuv_location);
    glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(struct VTexData), (void*)offsetof(struct VTexData, uv));

    glUseProgram(program);
    GLuint textures[INPUTS + 1];
    for (int i = 0; i < INPUTS + 1; ++i) {
        char path[0x200];
        char attrib[50];
        sprintf(path, "%s/%i.png", imagepath, i);
        sprintf(attrib, "tex%i", i);

        glActiveTexture(GL_TEXTURE0 + i);
        textures[i] = LoadTexture(path);
        if (textures[i] == 0) {
            fprintf(stderr, "failed to load texture %i\n", i);
            continue;
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

    glfwShowWindow(window);

    fprintf(logfile, "waiting for input subsystem init\n");
    int errcount = 0;
    while (0 != InputStartup() && !glfwWindowShouldClose(window)) {
        showErrorFrame(window);
    }

    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        if (inputErrorCode != 0) {
            showErrorFrame(window);
            Sleep(100);
            continue;
        }
        int current = currentInputs;
        glUniform1i(keys_location, currentInputs);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glFinish();
        if (idlesleep) {
            Sleep(idlesleep);
        }
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

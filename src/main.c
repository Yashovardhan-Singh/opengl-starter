// clang-format off
#include "cglm/mat4.h"
#include "cglm/quat.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define WIN_W 1280
#define WIN_H 720

static const float vertices[] = {
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f
};

static const unsigned int indices[] = {
    0, 1, 2, 1, 2, 3
};

// clang-format on

void resizeCallback(GLFWwindow *win, int w, int h) {
    fprintf(stdout, "Resize event triggered. New width: %d, New Height: %d\n",
            w, h);
    glViewport(0, 0, w, h);
}

GLuint compileShader(int type, const char *path) {
    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER)
        return 0;

    FILE *shader_source_file = fopen(path, "r");
    if (!shader_source_file) {
        fprintf(stderr, "File IO failed\n");
        return 0;
    }

    fseek(shader_source_file, 0, SEEK_END);
    int size = ftell(shader_source_file);
    fseek(shader_source_file, 0, SEEK_SET);

    char *shader_source = (char *) malloc(size + 1);  // Null char
    fread(shader_source, size, 1, shader_source_file);
    shader_source[size] = '\0';

    GLuint shader;
    shader = glCreateShader(type);

    // cast of doom and despair
    glShaderSource(shader, 1, (const GLchar *const *) &shader_source, NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "SHADER COMPILATION FAILED:\n %s\n", infoLog);
        free(shader_source);
        glDeleteShader(shader);
        return 0;
    }

    free(shader_source);
    return shader;
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "GLFW Failed to initialise\n");
        return 0;
    }

    // Let window know version of version and profile of opengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *win = glfwCreateWindow(1280, 720, "OpenGl", 0, 0);
    if (!win) {
        fprintf(stderr, "Window creation failed\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(win);  // window has opengl context

    // GPU Driver provides functions on runtime
    // so GLAD loads those function at runtime.
    // in code we can access through function pointers
    // provided by GLAD
    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) {
        fprintf(stderr, "Glad failed to load\n");
        glfwTerminate();
        return 0;
    }

    // Map opengl coordinates to window coordinates
    // set "viewing port" for the window
    glViewport(0, 0, WIN_W, WIN_H);

    // Everytime user resizes, this callback is triggered
    glfwSetFramebufferSizeCallback(win, resizeCallback);

    //----BUFFERS-------------------------
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (sizeof(float) * 2));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (sizeof(float) * 6));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    GLuint v_shader = compileShader(GL_VERTEX_SHADER, "shaders/vertex.glsl");
    GLuint f_shader =
        compileShader(GL_FRAGMENT_SHADER, "shaders/fragment.glsl");

    // -----SHADERS-------------------------------------
    GLuint shader_prog;
    shader_prog = glCreateProgram();
    glAttachShader(shader_prog, v_shader);
    glAttachShader(shader_prog, f_shader);
    glLinkProgram(shader_prog);

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    int  success;
    char infoLog[512];
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_prog, 512, NULL, infoLog);
        fprintf(stderr, "Shader link error: %s", infoLog);
        glDeleteBuffers(1, &vbo);
        glfwTerminate();
        return 0;
    }

    //----Textures----------------------
    stbi_set_flip_vertically_on_load(1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int            width, height, nrChan;
    unsigned char *img_data =
        stbi_load("assets/test.png", &width, &height, &nrChan, 0);

    if (!img_data) {
        fprintf(stderr, "KILL YOURSELF\n");
        return 1;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(img_data);

    int angle_counter = 0;

    // Main execution loop
    while (!glfwWindowShouldClose(win)) {

        if (angle_counter > 360)
            angle_counter = 0;

        float  angle = glm_rad(angle_counter++);
        versor q;
        glm_quatv(q, angle, (float[]) {0.0f, 0.0f, 1.0f});
        mat4 base_model = GLM_MAT4_IDENTITY_INIT, model;
        glm_quat_rotate(base_model, q, model);

        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);  // good ol' cornblue
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_prog);
        unsigned int transLoc = glGetUniformLocation(shader_prog, "transform");
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, (float *) model);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(win);  // swap draw buffer and present buffer
        glfwPollEvents();      // Key presses, close events etc.
    }

    return 0;
}

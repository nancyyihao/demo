#include "cosine_wave_renderer.h"
#include "render_common.h"
#include <cmath>

static const char *vertexShaderSourceCos =
    "#version 300 es\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform float uOffset;\n"
    "void main() {\n"
    "   // Using cos instead of sin\n"
    "   gl_Position = vec4(aPos.x, aPos.y + cos(aPos.x * 3.14 + uOffset) * 0.5, aPos.z, 1.0);\n"
    "}\0";

static const char *fragmentShaderSourceCos =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   // Different color (Green-ish) to distinguish\n"
    "   FragColor = vec4(0.2, 1.0, 0.5, 1.0);\n"
    "}\n\0";

CosineWaveRenderer::CosineWaveRenderer() {}

CosineWaveRenderer::~CosineWaveRenderer() {
    Destroy();
}

void CosineWaveRenderer::OnSurfaceCreated(EGLCore* eglCore) {
    LOGI("CosineWaveRenderer OnSurfaceCreated");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourceCos, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCos, nullptr);
    glCompileShader(fragmentShader);

    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertices_.clear();
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) {
        vertices_.push_back(x);
        vertices_.push_back(0.0f);
        vertices_.push_back(0.0f);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void CosineWaveRenderer::OnSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
}

void CosineWaveRenderer::Draw() {
    offset_ += 0.05f;

    // Different Background Color (Darker blue)
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_);
    glUniform1f(glGetUniformLocation(program_, "uOffset"), offset_);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, vertices_.size() / 3);
}

void CosineWaveRenderer::Destroy() {
    if (program_ != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(program_);
        program_ = 0;
    }
}

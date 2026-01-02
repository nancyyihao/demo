#include "render_thread.h"
#include "render_common.h"
#include <cmath>
#include <vector>

const char *vertexShaderSource =
    "#version 300 es\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform float uOffset;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y + sin(aPos.x * 3.14 + uOffset) * 0.5, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
    "}\n\0";

RenderThread::RenderThread() {}

RenderThread::~RenderThread() {
    Stop();
}

void RenderThread::Start(void* window, int width, int height) {
    LOGI("RenderThread Start");
    window_ = window;
    width_ = width;
    height_ = height;
    running_ = true;
    thread_ = std::thread(&RenderThread::RenderLoop, this);
}

void RenderThread::UpdateSize(int width, int height) {
    LOGI("RenderThread UpdateSize: %d %d", width, height);
    std::lock_guard<std::mutex> lock(mtx_);
    width_ = width;
    height_ = height;
    glViewport(0, 0, width_, height_);
}

void RenderThread::TogglePause() {
    bool expected = paused_.load();
    paused_.store(!expected);
    LOGI("RenderThread TogglePause: %d", paused_.load());
}

void RenderThread::Stop() {
    LOGI("RenderThread Stop");
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

bool RenderThread::InitEGL() {
    LOGI("InitEGL");
    eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(eglDisplay_, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }

    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLint numConfigs;
    if (!eglChooseConfig(eglDisplay_, attribs, &eglConfig_, 1, &numConfigs)) {
        LOGE("eglChooseConfig failed");
        return false;
    }

    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    eglContext_ = eglCreateContext(eglDisplay_, eglConfig_, EGL_NO_CONTEXT, contextAttribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }

    eglSurface_ = eglCreateWindowSurface(eglDisplay_, eglConfig_, reinterpret_cast<EGLNativeWindowType>(window_), nullptr);
    if (eglSurface_ == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }

    if (!eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }

    return true;
}

void RenderThread::DestroyEGL() {
    LOGI("DestroyEGL");
    if (eglDisplay_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglContext_ != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay_, eglContext_);
        }
        if (eglSurface_ != EGL_NO_SURFACE) {
            eglDestroySurface(eglDisplay_, eglSurface_);
        }
        eglTerminate(eglDisplay_);
    }
    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
    eglSurface_ = EGL_NO_SURFACE;
}

void RenderThread::RenderLoop() {
    if (!InitEGL()) {
        LOGE("Failed to init EGL");
        return;
    }

    // Init Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Prepare Vertex Data (Simple Line Strip)
    std::vector<float> vertices;
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) {
        vertices.push_back(x);
        vertices.push_back(0.0f); // y calculated in shader
        vertices.push_back(0.0f);
    }

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (running_) {
        if (!paused_) {
            offset_ += 0.05f;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program_);
        glUniform1f(glGetUniformLocation(program_, "uOffset"), offset_);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);

        if (eglDisplay_ != EGL_NO_DISPLAY && eglSurface_ != EGL_NO_SURFACE) {
            eglSwapBuffers(eglDisplay_, eglSurface_);
        }
        
        // 16ms sleep for ~60fps
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(program_);

    DestroyEGL();
}

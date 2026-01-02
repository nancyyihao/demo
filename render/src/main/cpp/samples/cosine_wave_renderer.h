#ifndef COSINE_WAVE_RENDERER_H
#define COSINE_WAVE_RENDERER_H

#include "renderer_interface.h"
#include <vector>

class CosineWaveRenderer : public IRenderer {
public:
    CosineWaveRenderer();
    ~CosineWaveRenderer();

    void OnSurfaceCreated(EGLCore* eglCore) override;
    void OnSurfaceChanged(int width, int height) override;
    void Draw() override;
    void Destroy() override;

private:
    GLuint program_ = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
    float offset_ = 0.0f;
    std::vector<float> vertices_;
};

#endif // COSINE_WAVE_RENDERER_H

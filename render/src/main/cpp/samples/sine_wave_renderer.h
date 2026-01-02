#ifndef SINE_WAVE_RENDERER_H
#define SINE_WAVE_RENDERER_H

#include "renderer_interface.h"
#include <vector>

class SineWaveRenderer : public IRenderer {
public:
    SineWaveRenderer();
    ~SineWaveRenderer();

    void OnSurfaceCreated(EGLCore* eglCore) override;
    void OnSurfaceChanged(int width, int height) override;
    void Draw() override;
    void Destroy() override;

    // Additional method for logic control if needed, but for now specific to this renderer
    void UpdateOffset(float delta);

private:
    GLuint program_ = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
    float offset_ = 0.0f;
    std::vector<float> vertices_;
};

#endif // SINE_WAVE_RENDERER_H

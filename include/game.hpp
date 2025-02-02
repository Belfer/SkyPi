#pragma once

#include <engine/application.hpp>

class SkyPiGame final : public Application
{
public:
    SkyPiGame();

    bool CanAddScene() override;

    void Configure() override;
    bool Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;
    
private:
    friend class SkyPiEditor;

    SceneHandle m_gameScene{ SCENE_INVALID_HANDLE };
};
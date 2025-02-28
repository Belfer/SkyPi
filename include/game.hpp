#pragma once

#include <engine/application.hpp>

class SkyPiGame final : public Application
{
public:
    SkyPiGame();

    void Configure() override;
    bool Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;
    
private:
    friend class SkyPiEditor;
};
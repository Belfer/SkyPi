#include <editor/game_editor.hpp>

#include <engine/engine.hpp>

void SkyPiEditor::Configure()
{
    m_game.Configure();
}

bool SkyPiEditor::Initialize()
{
    return m_game.Initialize();
}

void SkyPiEditor::Update()
{
    m_game.Update();
}

void SkyPiEditor::Render()
{
    m_game.Render();
}

void SkyPiEditor::Shutdown()
{
    m_game.Shutdown();
}

void SkyPiEditor::OnMainMenuBarGui()
{
}
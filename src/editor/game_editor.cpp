#include <editor/game_editor.hpp>

#include <engine/engine.hpp>

bool SkyPiEditor::CanAddScene()
{
    return true;
}

void SkyPiEditor::Configure()
{
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

void SkyPiEditor::OnMainMenuBar()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open")) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))
            Engine::Get().Close();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo")) {}
        if (ImGui::MenuItem("Redo")) {}
        ImGui::EndMenu();
    }
}
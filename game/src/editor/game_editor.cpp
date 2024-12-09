#include <editor/game_editor.hpp>
#include <editor/terrain_editor.hpp>
#include <engine/time.hpp>

SkyPiEditor::SkyPiEditor()
{
    SetTitle("SkyPi");
    SetPresistent(true);
    SetExclusive(true);
}

void SkyPiEditor::Configure()
{
    EditorManager::Get().AddMenuBar(
        [this]()
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Terrain"))
                    EditorManager::Get().AddEditor(meta::make_unique<Inspector<Terrain>>(m_game.m_terrain));

                ImGui::EndMenu();
            }
        });

    EditorManager::Get().AddEditor(meta::make_unique<SkyPiEditor>(*this));
    EditorManager::Get().AddEditor(meta::make_unique<Inspector<Terrain>>(m_game.m_terrain));
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

void SkyPiEditor::OnGui()
{
    m_frames++;
    m_timer += Time::Get().DeltaTime();
    if (m_timer >= 1.f)
    {
        m_fps = (i32)(m_frames / m_timer);
        m_frames = 0;
        m_timer = Math::FMod(m_timer, 1.f);
    }

    CString<64> fps;
    fps.format("FPS: {}", m_fps);
    ImGui::Text(fps);
}
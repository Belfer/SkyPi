#include <editor/game_editor.hpp>
#include <editor/terrain_view.hpp>
#include <engine/time.hpp>

class SkyPiEditorView final : public Editor
{
public:
    SkyPiEditorView();
    void OnGui() override;

private:
    f32 m_timer{ 1.f };
    i32 m_frames{ 0 };
    i32 m_fps{ 0 };
};

SkyPiEditorView::SkyPiEditorView()
{
    SetTitle("SkyPi");
    SetPresistent(true);
    SetExclusive(true);
}

void SkyPiEditorView::OnGui()
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

void SkyPiEditor::Configure()
{
    EditorManager::Get().AddMenuBar(
        [this]()
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Open")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo")) {}
                if (ImGui::MenuItem("Redo")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Terrain"))
                    EditorManager::Get().AddEditor(meta::make_unique<Inspector<Terrain>>(m_game.m_terrain));

                ImGui::EndMenu();
            }
        });

    EditorManager::Get().AddEditor(meta::make_unique<SkyPiEditorView>());
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
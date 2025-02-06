#include <editor/game_editor.hpp>

#include <engine/engine.hpp>
#include <engine/version.hpp>

#include <imgui_internal.h>
#include <IconsFontAwesome5.h>

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
    if (ImGui::Button(ICON_FA_ADJUST))
    {
        //theme = !theme;
        //SelectTheme();
    }
    //Tooltip("Theme");

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    ImGui::Text("| Engine v%s |", BX_VERSION_STR);
    //Tooltip("Version");
    
    if (ImGui::Button(ICON_FA_QUESTION_CIRCLE))
    {
        // TODO: About dialog
    }
    //Tooltip("About");
    
    ImGui::PopStyleColor(); // Pop text color
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    if (ImGui::Button(ICON_FA_BUG))
    {
        //show_settings = !show_settings;
    }
    //Tooltip("Debug");

    if (ImGui::Button(ICON_FA_CHART_PIE))
    {
        //show_profiler = !show_profiler;
    }
    //Tooltip("Profiler");

    if (ImGui::Button(ICON_FA_DATABASE)) // ICON_FA_COG alternative
    {
        //show_data = !show_data;
    }
    //Tooltip("Data");

    if (ImGui::Button(ICON_FA_TERMINAL))
    {
        //show_console = !show_console;
    }
    //Tooltip("Console");

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
}
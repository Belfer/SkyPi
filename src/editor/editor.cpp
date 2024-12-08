#include <editor/editor.hpp>
#include <engine/window.hpp>
#include <engine/graphics.hpp>

EditorManager& EditorManager::Get()
{
	static EditorManager instance;
	return instance;
}

bool EditorManager::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = GAME_PATH"/settings/imgui.ini";

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    if (!Window::Get().InitializeImGui())
    {
        LOGE(Editor, "Failed to initialize window imgui!");
        return false;
    }

    if (!Graphics::Get().InitializeImGui())
    {
        LOGE(Editor, "Failed to initialize graphics imgui!");
        return false;
    }

    return true;
}

void EditorManager::Shutdown()
{
    Graphics::Get().ShutdownImGui();
    Window::Get().ShutdownImGui();
    ImGui::DestroyContext();
}

void EditorManager::AddMenuBar(std::function<void()> callback)
{
    m_menuBars.emplace_back(callback);
}

void EditorManager::AddEditor(UniquePtr<Editor> editor)
{
    if (editor->m_isExclusive)
    {
        for (const auto& e : m_editors)
        {
            if (editor->m_title == e->m_title)
                return;
        }
    }
	m_editors.emplace_back(std::move(editor));
}

void EditorManager::OnGui()
{
    Graphics::Get().NewFrameImGui();
    Window::Get().NewFrameImGui();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        for (const auto& menuBar : m_menuBars)
            menuBar();

        ImGui::EndMainMenuBar();
    }

    for (auto it = m_editors.begin(); it != m_editors.end();)
    {
        Editor& editor = **it;

        bool* open = nullptr;
        if (!editor.m_isPersistent)
        {
            open = &editor.m_isOpen;
        }

        if (ImGui::Begin(editor.m_title, open, editor.m_flags))
        {
            editor.OnGui();
        }
        ImGui::End();

        if (!editor.IsOpen())
            it = m_editors.erase(it);
        else
            ++it;
    }
    Graphics::Get().EndFrameImGui();
    Window::Get().EndFrameImGui();
}

void EditorManager::Clear()
{
	m_editors.clear();
}
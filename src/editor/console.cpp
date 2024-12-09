#include <editor/console.hpp>

#include <engine/log.hpp>

#include <imgui.h>
#include <imgui_internal.h>
//#include <misc/cpp/imgui_stdlib.h>
//#include <IconsFontAwesome5.h>
#include <algorithm>

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    //ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
    //return console->TextEditCallback(data);
    return 0;
}

Console::Console()
{
    SetTitle("Console");
    SetExclusive(true);
}

void Console::OnGui()
{
	//ImGui::Begin(ICON_FA_TERMINAL"  Console", &show, ImGuiWindowFlags_NoCollapse);
	//ImGui::BeginListBox("##ConsoleLog");
	//for (const auto& entry : Log::GetLogs())
	//{
	//	ImGui::Text(entry.message.c_str());
	//}
	//ImGui::EndListBox();
	//ImGui::End();

    //ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    //ImGui::Begin(ICON_FA_TERMINAL"  Console", &show, ImGuiWindowFlags_NoCollapse);
    //if (!ImGui::Begin(title, p_open))
    //{
    //    ImGui::End();
    //    return;
    //}

    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
    // So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    //if (ImGui::BeginPopupContextItem())
    //{
    //    if (ImGui::MenuItem("Close Console"))
    //        *p_open = false;
    //    ImGui::EndPopup();
    //}

    //ImGui::TextWrapped(
    //    "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
    //    "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    //ImGui::TextWrapped("Enter 'HELP' for help.");

    // TODO: display items starting from the bottom

    //if (ImGui::SmallButton("Add Debug Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
    //ImGui::SameLine();
    //if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
    //ImGui::SameLine();
    if (ImGui::SmallButton("Clear")) { Log::Get().Clear(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu
    //if (ImGui::BeginPopup("Options"))
    //{
    //    ImGui::Checkbox("Auto-scroll", &AutoScroll);
    //    ImGui::EndPopup();
    //}

    // Options, Filter
    //if (ImGui::Button("Options"))
    //    ImGui::OpenPopup("Options");
    //ImGui::SameLine();
    //Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    //ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) Log::Get().Clear();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();

        SizeType entryCount = 0;
        LogEntry* entries = Log::Get().GetLogs(entryCount);
        for (SizeType i = 0; i < entryCount; ++i)
        {
            const auto& entry = entries[i];
            //if (!Filter.PassFilter(item))
            //    continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (entry.level == LogLevel::LOG_DEBUG) { color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f); has_color = true; }
            if (entry.level == LogLevel::LOG_INFO) { color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); has_color = true; }
            if (entry.level == LogLevel::LOG_WARNING) { color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); has_color = true; }
            if (entry.level == LogLevel::LOG_ERROR) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            //else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(entry.message.c_str());
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        //if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        //ScrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    static char s_inputBuffer[256];
    if (ImGui::InputText("Input", s_inputBuffer, BX_ARRAYSIZE(s_inputBuffer), input_text_flags, &TextEditCallbackStub, nullptr))
    {
        //char* s = s_inputBuffer;
        BX_LOGI(s_inputBuffer);
        //Strtrim(s);
        //if (s[0])
        //    ExecCommand(s);
        //strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    //ImGui::End();
}
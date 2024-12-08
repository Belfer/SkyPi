#include <editor/terrain_editor.hpp>
#include <engine/time.hpp>

Inspector<Terrain>::Inspector(Terrain& terrain)
    : m_terrain(terrain)
{
    SetTitle("Terrain");
    SetExclusive(true);
    m_path = "/assets/tamriel_cell.png";
}

void Inspector<Terrain>::OnGui()
{
    if (ImGui::CollapsingHeader("Inspector", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Inspect(m_terrain);
    }

    if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Path: ");
        ImGui::SameLine();
        ImGui::InputText("##Path", m_path.data(), m_path.length());
        
        CString<64> genTime;
        genTime.format("Gen Time (ms): {}", (i32)m_genTime);
        ImGui::Text(genTime);
        
        if (ImGui::Button("Generate"))
        {
            Stopwatch sw;
            sw.Start();
            m_terrain.Generate(m_path.data());
            m_genTime = sw.ElapsedMilliseconds();
        }

        //if (ImGui::Button("Reload"))
        //{
        //    m_terrain.Reload();
        //}
    }
}

void Inspector<Terrain>::Inspect(Terrain& terrain)
{
    i32 vs = terrain.m_vertexShader;

    ImGui::Text("Color: ");
    ImGui::SameLine();
    ImGui::ColorEdit4("##Color", &terrain.m_drawData.color[0]);

    ImGui::Text("Light Dir: ");
    ImGui::SameLine();
    if (ImGui::SliderFloat3("##LightDir", &terrain.m_drawData.lightDir[0], -1.f, 1.f))
        Vec3::Normalize(terrain.m_drawData.lightDir);

    ImGui::Text("Light I: ");
    ImGui::SameLine();
    ImGui::InputFloat("##LightI", &terrain.m_drawData.lightI);
}
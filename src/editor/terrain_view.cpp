#include <editor/terrain_view.hpp>
#include <engine/time.hpp>

EDITOR_MENU("Views/Terrain", []() { LOGI(Terrain, "HelloWorld!"); })

Editor<Terrain>::Editor(Terrain& terrain)
    : m_terrain(terrain)
{
    SetTitle("Terrain");
    SetExclusive(true);
    m_heightmapSrcPath = "/assets/tamriel_lowres.png";
    m_heightmapDstPath = "/assets/tamriel_lowres.bin";
}

void Editor<Terrain>::OnGui()
{
    if (ImGui::CollapsingHeader("Inspector", ImGuiTreeNodeFlags_DefaultOpen))
    {
        OnInspectGui(m_terrain);
    }

    if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SeparatorText("Processing");

        ImGui::Text("Heightmap Src Path: ");
        ImGui::SameLine();
        ImGui::InputText("##HeightmapSrcPath", m_heightmapSrcPath.data(), m_heightmapSrcPath.size());

        ImGui::Text("Heightmap Dst Path: ");
        ImGui::SameLine();
        ImGui::InputText("##HeightmapDstPath", m_heightmapDstPath.data(), m_heightmapDstPath.size());

        if (ImGui::Button("Import"))
        {
            m_terrain.Import(m_heightmapSrcPath, m_heightmapDstPath);
        }
        
        ImGui::SameLine();

        f32 time{ 0 };
        CString<64> genTime;
        genTime.format("Import Time (ms): {}", (i32)time);
        ImGui::Text(genTime);

        ImGui::SeparatorText("Streaming");

        if (ImGui::Button("Open Stream"))
        {
            m_terrain.OpenStream(m_heightmapDstPath);
        }
        
        ImGui::SameLine();

        if (ImGui::Button("Close Stream"))
        {
            m_terrain.CloseStream();
        }

        ImGui::SeparatorText("Graphics");

        ImGui::Text("Wireframe: ");
        ImGui::SameLine();
        if (ImGui::Checkbox("##Wireframe", &m_wireframe))
            Graphics::Get().SetWireframe(m_wireframe);
    }
}

void Editor<Terrain>::OnInspectGui(Terrain& terrain)
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

    ImGui::SeparatorText("Debug");
    
    ImGui::Text("Debug Draw: ");
    ImGui::SameLine();
    ImGui::Checkbox("##DebugDraw", &terrain.m_debugDraw);

    ImGui::Text("Update Frustum: ");
    ImGui::SameLine();
    ImGui::Checkbox("##UpdateFrustum", &terrain.m_updateFrustum);

    ImGui::Text("Update Camera: ");
    ImGui::SameLine();
    ImGui::Checkbox("##UpdateCamera", &terrain.m_updateCamera);

    ImGui::Text("LOD: ");
    ImGui::SameLine();
    ImGui::SliderInt("##LOD", &terrain.m_lod, -1, 7);
}
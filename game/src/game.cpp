#include <game.hpp>
#include <engine/window.hpp>
#include <engine/time.hpp>

SkyPiGame::SkyPiGame()
{
}

void SkyPiGame::Configure()
{
}

bool SkyPiGame::Initialize()
{
    // Create global buffer
    {
        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::UNIFORM_BUFFER;
        bufferInfo.usage = BufferUsage::DYNAMIC;
        bufferInfo.access = BufferAccess::WRITE;

        BufferData bufferData;
        bufferData.dataSize = 0;
        bufferData.pData = nullptr;

        m_constantBuffer = Graphics::Get().CreateBuffer(bufferInfo, bufferData);
    }

    m_terrain.Initialize();
    Graphics::Get().BindResource(m_terrain.GetResources(), "ConstantBuffer", m_constantBuffer);

    return true;
}

void SkyPiGame::Update()
{
    // Update camera rotation
    Vec2 mousePos{ Window::Get().GetMouseX(), Window::Get().GetMouseY() };
    Vec2 viewDelta = mousePos - m_mousePos;
    m_mousePos = mousePos;

    if (Window::Get().GetMouseButton(MouseButton::MOUSE_BUTTON_RIGHT))
    {
        m_cameraRot.x += viewDelta.y * m_lookSpeed;
        m_cameraRot.y -= viewDelta.x * m_lookSpeed;
    }

    // Update camera position
    Vec3 moveDelta{};
    if (Window::Get().GetKey(Key::W))
        moveDelta.z += 1;
    if (Window::Get().GetKey(Key::S))
        moveDelta.z -= 1;
    if (Window::Get().GetKey(Key::A))
        moveDelta.x += 1;
    if (Window::Get().GetKey(Key::D))
        moveDelta.x -= 1;
    if (Window::Get().GetKey(Key::E))
        moveDelta.y += 1;
    if (Window::Get().GetKey(Key::Q))
        moveDelta.y -= 1;

    Quat camRot = Quat::Euler(m_cameraRot.x, m_cameraRot.y, m_cameraRot.z);
    m_cameraPos += camRot * moveDelta * m_moveSpeed * Time::Get().DeltaTime();

    // Update view data
    int w, h;
    Window::Get().GetSize(&w, &h);
    m_camera.SetProjection(Mat4::Perspective(60.f, h > 0 ? (f32)w / h : 1.f, 0.1f, 5000.f));
    m_camera.SetView(Mat4::LookAt(m_cameraPos, m_cameraPos + camRot * Vec3::Forward(), Vec3::Up()));
    m_camera.Update();

    m_constantData.viewProjMtx = m_camera.GetViewProjection();
}

static void DrawXZGrid(const Vec3& cameraPos, float zfar)
{
    float cameraHeight = fabs(cameraPos.y);

    // Determine the current and next grid spacing
    float currentSpacing = pow(10.0f, floor(log10(cameraHeight))); // Current nearest 10th
    float nextSpacing = currentSpacing * 10.0f;                    // Next larger scale

    // Grid extent based on zfar
    float gridExtent = std::min(100.0f * nextSpacing, zfar);

    // Colors
    uint32_t darkGray = 0xFF444444;
    uint32_t lightGray = 0xFF888888;
    uint32_t nextGridGray = 0xFF666666; // Slightly lighter for the next grid
    uint32_t red = 0xFFFF0000;
    uint32_t green = 0xFF00FF00;
    uint32_t blue = 0xFF0000FF;

    // Draw the current grid spacing
    for (float x = -gridExtent; x <= gridExtent; x += currentSpacing)
    {
        if (fabs(x) < 0.0001f) continue; // Skip the center X line to avoid overlap
        if (fabs(x - cameraPos.x) > zfar) continue; // Skip lines outside view range
        uint32_t colorX = fmod(fabs(x / currentSpacing), 10.0f) == 0 ? darkGray : lightGray;
        DebugDraw::Get().Line(Vec3(x, 0, -gridExtent), Vec3(x, 0, gridExtent), colorX);
    }

    for (float z = -gridExtent; z <= gridExtent; z += currentSpacing)
    {
        if (fabs(z) < 0.0001f) continue; // Skip the center Z line to avoid overlap
        if (fabs(z - cameraPos.z) > zfar) continue; // Skip lines outside view range
        uint32_t colorZ = fmod(fabs(z / currentSpacing), 10.0f) == 0 ? darkGray : lightGray;
        DebugDraw::Get().Line(Vec3(-gridExtent, 0, z), Vec3(gridExtent, 0, z), colorZ);
    }

    // Draw the next grid spacing
    for (float x = -gridExtent; x <= gridExtent; x += nextSpacing)
    {
        if (fabs(x) < 0.0001f) continue; // Skip the center X line to avoid overlap
        if (fabs(x - cameraPos.x) > zfar) continue; // Skip lines outside view range
        DebugDraw::Get().Line(Vec3(x, 0, -gridExtent), Vec3(x, 0, gridExtent), nextGridGray);
    }

    for (float z = -gridExtent; z <= gridExtent; z += nextSpacing)
    {
        if (fabs(z) < 0.0001f) continue; // Skip the center Z line to avoid overlap
        if (fabs(z - cameraPos.z) > zfar) continue; // Skip lines outside view range
        DebugDraw::Get().Line(Vec3(-gridExtent, 0, z), Vec3(gridExtent, 0, z), nextGridGray);
    }

    // Draw center lines last to ensure they are visually on top
    float lineLength = std::min(gridExtent, zfar); // Center lines extend across the grid but are limited by zfar
    DebugDraw::Get().Line(Vec3(-lineLength, 0, 0), Vec3(lineLength, 0, 0), red);   // X-axis
    DebugDraw::Get().Line(Vec3(0, -lineLength, 0), Vec3(0, lineLength, 0), green); // Y-axis
    DebugDraw::Get().Line(Vec3(0, 0, -lineLength), Vec3(0, 0, lineLength), blue); // Z-axis
}


void SkyPiGame::Render()
{
    DrawXZGrid(m_cameraPos, 5000.f);

    static f32 clearColor[] = { .4f, .6f, .9f, 1.f };
    Graphics::Get().ClearRenderTarget(Graphics::Get().GetCurrentBackBufferRT(), clearColor);

    BufferData bufferData;
    bufferData.dataSize = sizeof(ConstantData);
    bufferData.pData = &m_constantData;
    Graphics::Get().UpdateBuffer(m_constantBuffer, bufferData);
    
    m_terrain.Render(m_camera);

#ifdef DEBUG_BUILD
    DebugDraw::Get().Render(m_camera.GetViewProjection());
#endif
}

void SkyPiGame::Shutdown()
{
    Graphics::Get().DestroyBuffer(m_constantBuffer);
    m_terrain.Shutdown();
}
#include <game.hpp>
#include <engine/window.hpp>
#include <engine/time.hpp>

#include <iostream>

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

    f32 moveSpeed = m_moveSpeed;
    if (Window::Get().GetKey(Key::SPACE))
        moveSpeed *= 20;

    Quat camRot = Quat::Euler(m_cameraRot.x, m_cameraRot.y, m_cameraRot.z);
    m_cameraPos += camRot * moveDelta * moveSpeed * Time::Get().DeltaTime();

    // Update view data
    int w, h;
    Window::Get().GetSize(&w, &h);
    m_camera.SetProjection(Mat4::Perspective(60.f, h > 0 ? (f32)w / h : 1.f, 0.1f, 5000.f));
    m_camera.SetView(Mat4::LookAt(m_cameraPos, m_cameraPos + camRot * Vec3::Forward(), Vec3::Up()));
    m_camera.Update();

    m_constantData.viewProjMtx = m_camera.GetViewProjection();

    m_terrain.Update(m_camera);
}

static void DrawXZGrid(const Vec3& cameraPos, f32 zfar)
{
    f32 cameraHeight = fabs(cameraPos.y);

    // Determine the current grid spacing based on camera height
    f32 currentSpacing = pow(10.f, floor(log10(cameraHeight)));
    f32 nextSpacing = currentSpacing * 10.f;

    // Grid extent based on zfar
    f32 gridExtent = std::min(100.0f * nextSpacing, zfar);

    // Colors
    static const u32 lightGray = 0xFF555555;
    static const u32 darkGray = 0xFF444444;
    static const u32 red = 0xFF0000FF;
    static const u32 green = 0xFF00FF00;
    static const u32 blue = 0xFFFF0000;

    // Round the camera's position to the nearest grid spacing
    f32 roundedX = currentSpacing * floor(cameraPos.x / currentSpacing);
    f32 roundedY = currentSpacing * floor(cameraPos.y / currentSpacing);
    f32 roundedZ = currentSpacing * floor(cameraPos.z / currentSpacing);

    // Draw the grid lines in X and Z directions
    for (f32 x = roundedX - gridExtent; x < roundedX + gridExtent; x += currentSpacing)
    {
        if (fabs(x - cameraPos.x) > gridExtent) continue; // Check if line is within view range
        if (fabs(x) < 0.0001f) continue; // Skip center line

        u32 colorX = fmod(fabs(x / currentSpacing), 10.0f) == 0 ? lightGray : darkGray;
        DebugDraw::Get().Line(Vec3(x, 0, roundedZ - gridExtent), Vec3(x, 0, roundedZ + gridExtent), colorX);
    }

    for (f32 z = roundedZ - gridExtent; z < roundedZ + gridExtent; z += currentSpacing)
    {
        if (fabs(z - cameraPos.z) > gridExtent) continue; // Check if line is within view range
        if (fabs(z) < 0.0001f) continue; // Skip center line

        u32 colorZ = fmod(fabs(z / currentSpacing), 10.0f) == 0 ? lightGray : darkGray;
        DebugDraw::Get().Line(Vec3(roundedX - gridExtent, 0, z), Vec3(roundedX + gridExtent, 0, z), colorZ);
    }

    // Center lines (Global origin, drawn at origin 0, 0, 0)
    if (fabs(cameraPos.y) <= gridExtent && fabs(cameraPos.z) <= gridExtent)
        DebugDraw::Get().Line(Vec3(roundedX - gridExtent, 0, 0), Vec3(roundedX + gridExtent, 0, 0), red); // X-axis
    if (fabs(cameraPos.x) <= gridExtent && fabs(cameraPos.z) <= gridExtent)
        DebugDraw::Get().Line(Vec3(0, roundedY - gridExtent, 0), Vec3(0, roundedY + gridExtent, 0), green); // Y-axis
    if (fabs(cameraPos.x) <= gridExtent && fabs(cameraPos.y) <= gridExtent)
        DebugDraw::Get().Line(Vec3(0, 0, roundedZ - gridExtent), Vec3(0, 0, roundedZ + gridExtent), blue); // Z-axis
}

void SkyPiGame::Render()
{
    static f32 clearColor[] = { .2f, .2f, .2f, 1.f };
    Graphics::Get().ClearRenderTarget(Graphics::Get().GetCurrentBackBufferRT(), clearColor);

    DrawXZGrid(m_cameraPos, 5000.f);
    DebugDraw::Get().Line(Vec3(0, 0, 0), Vec3(1, 0, 1), 0xFFFFFFFF); // Z-axis

    BufferData bufferData;
    bufferData.dataSize = sizeof(ConstantData);
    bufferData.pData = &m_constantData;
    Graphics::Get().UpdateBuffer(m_constantBuffer, bufferData);
    
    m_terrain.Render(m_camera);

#if defined(EDITOR_BUILD) || defined(DEBUG_BUILD)
    DebugDraw::Get().Render(m_camera.GetViewProjection());
#endif
}

void SkyPiGame::Shutdown()
{
    Graphics::Get().DestroyBuffer(m_constantBuffer);
    m_terrain.Shutdown();
}
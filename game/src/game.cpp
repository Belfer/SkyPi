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
    Mat4 camProj = Mat4::Perspective(60.f, h > 0 ? (f32)w / h : 1.f, 0.1f, 5000.f);
    Mat4 camView = Mat4::LookAt(m_cameraPos, m_cameraPos + camRot * Vec3::Forward(), Vec3::Up());
    m_constantData.viewProjMtx = camProj * camView;
}

void SkyPiGame::Render()
{
    static f32 clearColor[] = { .4f, .6f, .9f, 1.f };
    Graphics::Get().ClearRenderTarget(0, clearColor);

    BufferData bufferData;
    bufferData.dataSize = sizeof(ConstantData);
    bufferData.pData = &m_constantData;
    Graphics::Get().UpdateBuffer(m_constantBuffer, bufferData);
    
    m_terrain.Render();
}

void SkyPiGame::Shutdown()
{
    Graphics::Get().DestroyBuffer(m_constantBuffer);
    m_terrain.Shutdown();
}
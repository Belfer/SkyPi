#include <engine/application.hpp>
#include <engine/log.hpp>
#include <engine/time.hpp>
#include <engine/window.hpp>
#include <engine/graphics.hpp>

Application& Application::Get() noexcept
{
    static Application instance;
    return instance;
}

bool Application::IsRunning() const noexcept
{
    return m_isRunning && Window::Get().IsOpen();
}

void Application::Close() noexcept
{
    m_isRunning = false;
}

int Application::Run(int argc, char** args, Game& game)
{
    LOGD(Core, "Application starting...");

    game.Configure();
    if (!Initialize() || !game.Initialize())
    {
        LOGE(Core, "Failed to initialize application!");
        return EXIT_FAILURE;
    }

    Time::Get().Start();
    while (IsRunning())
    {
        Time::Get().Update();

        Window::Get().PollEvents();
        game.Update();

        Graphics::Get().NewFrame();
        game.Render();
        Graphics::Get().EndFrame();

        Window::Get().Display();
    }

    game.Shutdown();
    Shutdown();

    LOGD(Core, "Application exit.");
    return EXIT_SUCCESS;
}

bool Application::Initialize() noexcept
{
    LOGD(Core, "Initializing application...");

    if (!Window::Get().Initialize())
    {
        LOGE(Core, "Failed to initialize window!");
        return false;
    }

    if (!Graphics::Get().Initialize())
    {
        LOGE(Core, "Failed to initialize graphics!");
        return false;
    }

    LOGD(Core, "Application initialized successfully.");
    return true;
}

void Application::Shutdown() noexcept
{
    LOGD(Core, "Shutting down application...");

    Graphics::Get().Shutdown();
    Window::Get().Shutdown();

    LOGD(Core, "Application shutdown complete.");
}

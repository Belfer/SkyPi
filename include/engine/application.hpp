#pragma once

class Game
{
public:
    virtual ~Game() {}
    virtual void Configure() = 0;
    virtual bool Initialize() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;
};

class Application
{
public:
    static Application& Get() noexcept;

    bool IsRunning() const noexcept;
    void Close() noexcept;

    int Run(int argc, char** args, Game& game);

private:
    Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    bool Initialize() noexcept;
    void Shutdown() noexcept;

private:
    bool m_isRunning = true;
};
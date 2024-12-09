#pragma once

#include <game.hpp>
#include <editor/editor.hpp>

class SkyPiEditor final : public Game
{
public:
	SkyPiEditor();

	void Configure() override;
	bool Initialize() override;
	void Update() override;
	void Render() override;
	void Shutdown() override;

private:
	SkyPiGame m_game;

	//f32 m_timer{ 1.f };
	//i32 m_frames{ 0 };
	//i32 m_fps{ 0 };
};
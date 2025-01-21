#pragma once

#include <game.hpp>
#include <editor/editor.hpp>

class SkyPiEditor final : public Game
{
public:
	void Configure() override;
	bool Initialize() override;
	void Update() override;
	void Render() override;
	void Shutdown() override;

private:
	SkyPiGame m_game;
};
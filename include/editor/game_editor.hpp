#pragma once

#include <game.hpp>
#include <editor/editor.hpp>

class SkyPiEditor final : public EditorApplication
{
public:
	bool CanAddScene() override;

	void Configure() override;
	bool Initialize() override;
	void Update() override;
	void Render() override;
	void Shutdown() override;

	void OnMainMenuBarGui() override;

private:
	SkyPiGame m_game;
};
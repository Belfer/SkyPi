#pragma once

#include <terrain.hpp>
#include <editor/editor.hpp>

template <>
class Editor<Terrain> final : public EditorView
{
public:
	Editor(Terrain& terrain);
	void OnGui() override;

	static void OnInspectGui(Terrain& terrain);

private:
	bool m_wireframe{ false };
	Terrain& m_terrain;

	CString<512> m_heightmapSrcPath{};
	CString<512> m_heightmapDstPath{};
};
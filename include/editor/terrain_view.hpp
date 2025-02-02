#pragma once

#include <terrain.hpp>
#include <editor/editor.hpp>

template <>
class EditorInspector<Terrain> final : public EditorWindow
{
public:
	EditorInspector(Terrain& terrain);
	void OnGui(EditorApplication& app) override;

	static void OnInspectGui(Terrain& terrain);

private:
	bool m_wireframe{ false };
	Terrain& m_terrain;

	CString<512> m_heightmapSrcPath{};
	CString<512> m_heightmapDstPath{};
};
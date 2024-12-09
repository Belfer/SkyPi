#pragma once

#include <terrain.hpp>
#include <editor/editor.hpp>
#include <editor/inspector.hpp>

template <>
class Inspector<Terrain> final : public Editor
{
public:
	Inspector(Terrain& terrain);
	void OnGui() override;

	static void Inspect(Terrain& terrain);

private:
	Terrain& m_terrain;

	CString<512> m_heightmapSrcPath{};
	CString<512> m_heightmapDstPath{};
};
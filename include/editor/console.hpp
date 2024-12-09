#pragma once

#include <editor/editor.hpp>

class Console final : public Editor
{
public:
	Console();
	void OnGui() override;
};
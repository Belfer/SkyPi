#pragma once

#include <bx/editor/view.hpp>

class BX_API ProfilerView final : public View
{
	RTTR_ENABLE(View)

public:
	bool Initialize() override;
	void Reload() override;
	void Shutdown() override;

	const char* GetTitle() const override;
	void Present(const char* title, bool& isOpen) override;

private:
	int m_frames = 0;
	float m_time = 0;
	int m_fps = 0;

	int m_frame = 0;
};
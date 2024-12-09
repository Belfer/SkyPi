#pragma once

#include <engine/string.hpp>
#include <engine/memory.hpp>
#include <engine/list.hpp>

#include <imgui.h>
#include <functional>

enum struct EditorTheme
{
	DARK,
	LIGHT,
	GRAY,
	ACRYLIC
};

class Editor
{
public:
	virtual ~Editor() {}
	virtual void OnGui() = 0;

	bool IsOpen() const { return m_isOpen; }
	void SetPresistent(bool isPersistent) { m_isPersistent = isPersistent; }
	void SetExclusive(bool isExclusive) { m_isExclusive = isExclusive; }
	void Close() { if (!m_isPersistent) m_isOpen = false; }
	
	const char* GetTitle() const { return m_title; }
	void SetTitle(const char* title) { m_title = title; }

	ImGuiWindowFlags GetFlags() const { return m_flags; }
	void SetFlags(ImGuiWindowFlags flags) { m_flags = flags; }

private:
	friend class EditorManager;

	bool m_isOpen{ true };
	bool m_isPersistent{ false };
	bool m_isExclusive{ false };

	CString<64> m_title{ "Editor" };
	ImGuiWindowFlags m_flags{ ImGuiWindowFlags_None };
};

using MenuBarOnGui = void(*)();

class EditorManager
{
public:
	static EditorManager& Get();

	bool Initialize();
	void Shutdown();

	void AddMenuBar(std::function<void()> callback);
	void AddEditor(UniquePtr<Editor> editor);
	void OnGui();
	void Clear();

private:
	void ApplyTheme();
	void PushMenuTheme();
	void PopMenuTheme();

private:
	f32 m_uiScale = 1.0f;
	EditorTheme m_currentTheme{ EditorTheme::ACRYLIC };

	List<std::function<void()>> m_menuBars;
	List<UniquePtr<Editor>> m_editors;
};
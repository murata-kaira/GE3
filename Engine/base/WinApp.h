#pragma once
#include<Windows.h>
#include<cstdint>

class WinApp {

public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHinstance() const { return wc.hInstance; }


public:

	void Initialize();

	void Update();

	void Finalize();

	bool ProcessMessage();

private:

	HWND hwnd = nullptr;

	WNDCLASS wc{};

};
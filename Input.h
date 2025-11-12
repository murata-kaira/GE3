
#pragma once
#include<Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


class Input {

public:

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

	void Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();


private:

	ComPtr<IDirectInput8> directInput;

	ComPtr<IDirectInputDevice8> keyboard;


	BYTE keyPre[256] = {};

	BYTE key[256] = {};



};

#pragma once
#include<Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include"WinApp.h"


class Input {

public:

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

	void Initialize(WinApp* winApp);

	void Update();


private:

	ComPtr<IDirectInput8> directInput;

	ComPtr<IDirectInputDevice8> keyboard;


	BYTE keyPre[256] = {};

	BYTE key[256] = {};

	WinApp* winApp = nullptr;



};

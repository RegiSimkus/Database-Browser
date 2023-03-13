#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "ImGui.h"

namespace Window
{
	extern ID3D11Device* pD3DDevice;
	extern ID3D11DeviceContext* pD3DDeviceContext;
	extern IDXGISwapChain* pSwapChain;
	extern ID3D11RenderTargetView* pMainRenderTargetView;

	HWND CreateAppWindow();
	bool CreateDevice(HWND hWnd);
	void DestroyDevice();
	void DestroyRenderTarget();
	void CreateRenderTarget();

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
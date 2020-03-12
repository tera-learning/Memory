#include "DrawManager.h"
#include <atltypes.h>
#include <Windows.h>

DrawManager dx11Manager;


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		dx11Manager.UpdateSpriteList(wp);
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */, LPSTR /* lpCmdLine */, int nCmdShow)
{

	WNDCLASSEX winc;

	//構造体のサイズ
	winc.cbSize = sizeof(WNDCLASSEX);
	//ウィンドウクラスの基本スタイルを表す定数
	winc.style = CS_HREDRAW | CS_VREDRAW;
	//ウィンドウプロシージャ（ウィンドウで発生したイベントを処理する関数）のポイント型
	winc.lpfnWndProc = WndProc;
	//クラスの構造体の追加領域
	winc.cbClsExtra = 0;
	//ウィンドウ構造体の追加領域
	winc.cbWndExtra = 0;
	//インスタンスハンドル
	winc.hInstance = hInstance;
	//デスクトップ等に描画されるアイコンの情報を持つアイコンハンドル
	winc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	//ウィンドウのクライアントエリア上のマウスカーソル
	winc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//クライアントエリアの背景色
	winc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//クラスメニュー
	winc.lpszMenuName = nullptr;
	//ウィンドウクラスの名前
	winc.lpszClassName = TEXT("Polygon");
	//タスクバーやタイトルバーに表示される小さいアイコンの情報を持つアイコンハンドル
	winc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&winc)) return 0;


	//////////////////////////////////////////////////////////////////////////////////
	//ウィンドウ生成
	//////////////////////////////////////////////////////////////////////////////////
	int width = 600;
	int height = 600;

	HWND hwnd = CreateWindow(
		winc.lpszClassName,
		TEXT("Polygon"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,
		0,
		width,
		height,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (hwnd == NULL) return 0;


	//////////////////////////////////////////////////////////////////////////////////
	//クライアント領域基準で再生成
	//////////////////////////////////////////////////////////////////////////////////
	CRect crect, wrect;
	GetWindowRect(hwnd, &wrect);
	GetClientRect(hwnd, &crect);

	int newWidth = width + ((wrect.right - wrect.left) - (crect.right - crect.left));
	int newHeight = height + ((wrect.bottom - wrect.top) - (crect.bottom - crect.top));

	SetWindowPos(hwnd, nullptr, 0, 0, newWidth, newHeight, SWP_SHOWWINDOW);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	

	//////////////////////////////////////////////////////////////////////////////////
	//描画
	//////////////////////////////////////////////////////////////////////////////////
	dx11Manager.Create(hwnd);


	//////////////////////////////////////////////////////////////////////////////////
	//描画
	//////////////////////////////////////////////////////////////////////////////////
	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			dx11Manager.UpdateWindow();
			dx11Manager.Render(hwnd);
		}
	}

	return msg.wParam;
}
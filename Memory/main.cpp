#include "DrawManager.h"
#include <atltypes.h>
#include <Windows.h>

DrawManager dx11Manager;
unsigned int totalAllocatedNum = 0;
unsigned int currentAllocatedNum = 0;
std::size_t totalMemorySize = 0;
std::size_t peakMemorySize = 0;
const std::size_t memoryHeader = 16;

void* operator new(std::size_t n)
{
	//割り当てた合計数(Newされた数)
	totalAllocatedNum++;
	//現在割り当ててる数(現在確保されている数)
	currentAllocatedNum++;

	std::size_t allocateSize = n + memoryHeader;

	//合計使用メモリ量
	totalMemorySize += allocateSize;
	//ピーク時の使用メモリ量
	peakMemorySize = (totalMemorySize > peakMemorySize) ? totalMemorySize : peakMemorySize;
	
	//メモリ確保
	void* p = std::malloc(allocateSize);
	char* memory = nullptr;

	if (p != nullptr)
	{
		//メモリに確保した容量を保持しておく
		*(static_cast<std::size_t*>(p)) = n;
		//実際に使用する領域を返す
		memory = static_cast<char *>(p) + memoryHeader;
	}
	
	return memory;
}

void operator delete(void* ptr)
{
	if (ptr != nullptr)
	{
		void* ptrHeader = static_cast<char *>(ptr) - memoryHeader;
		char* memory = static_cast<char *>(ptrHeader);

		if (memory != nullptr)
		{
			//Newしたときに確保したサイズを取得
			std::size_t size = *(static_cast<std::size_t*>(ptrHeader));
			//合計使用メモリ量から差し引く
			totalMemorySize -= (size + memoryHeader);
			//現在割り当ててる数からひく
			currentAllocatedNum--;

			//メモリ領域解放
			std::free(memory);
		}
	}
}

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
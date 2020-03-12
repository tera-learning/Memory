#pragma once
#include "Sprite.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "DebugMenu.h"
#include "FrameRate.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>

struct ConstantBuffer {
	int centerWindow[2];
	int drawPos[2];
	int height;
	int width;
};

class DrawManager final
{
private:
	//deque コンテナ選ぶときにどれを選ぶか意識　メモリ配置
	std::vector<Sprite> m_SpriteList;
	std::shared_ptr<Texture> m_TextureImage;
	VertexBuffer m_Vertex;
	DebugMenu m_DebugMenu;
	FrameRate m_FrameRate;

public:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ConstantBuffer;
	D3D11_VIEWPORT m_ViewPort;

	DrawManager(){};
	DrawManager(const DrawManager& drawManager) = delete;
	DrawManager& operator=(const DrawManager& drawManager) = delete;
	/* virtual */ ~DrawManager() { m_Context->ClearState();  };

	HRESULT Create(HWND hwnd);
	void Render(HWND hwnd);
	void UpdateWindow();
	void UpdateSpriteList(WPARAM key);
};


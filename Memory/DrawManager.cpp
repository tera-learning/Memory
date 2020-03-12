#include "DrawManager.h"
#include "VertexBuffer.h"
#include "SpriteControllerKeyBinding.h"
#include "MoveSprite.h"
#include <atltypes.h>
#include <d3dcompiler.h>
#include <random>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

HRESULT DrawManager::Create(HWND hwnd)
{

	//////////////////////////////////////////////////////////////////////////////////
	//ウィンドウサイズ取得
	//////////////////////////////////////////////////////////////////////////////////
	CRect rect;
	GetClientRect(hwnd, &rect);


	//////////////////////////////////////////////////////////////////////////////////
	//デバイスとスワップチェインの生成
	//////////////////////////////////////////////////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferCount = 1;									// スワップチェインのバッファ数
	desc.BufferDesc.Width = rect.Width();					// スワップチェインのバッファサイズ
	desc.BufferDesc.Height = rect.Height();					// スワップチェインのバッファサイズ
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// スワップチェインのバッファフォーマット
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// バッファをレンダーターゲットとして使用
	desc.OutputWindow = hwnd;								// HWNDハンドル
	desc.SampleDesc.Count = 1;								// マルチサンプリングのピクセル単位の数
	desc.SampleDesc.Quality = 0;							// マルチサンプリングの品質
	desc.Windowed = TRUE;									// ウィンドウモード

	D3D_FEATURE_LEVEL level;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(
		nullptr,					//どのビデオアダプタを使用するかIDXGIAdapterのアドレスを渡す.既定ならばnullptr
		D3D_DRIVER_TYPE_HARDWARE,	//ドライバのタイプ
		nullptr,					//上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドル
		0,							//フラグ指定.D3D11_CREATE_DEVICE列挙型
		nullptr,					//作成を試みる機能レベルの順序を指定する配列
		0,							//作成を試みる機能レベルの順序を指定する配列の数
		D3D11_SDK_VERSION,			//SDKのバージョン
		&desc,						//スワップチェインの初期化パラメーター
		&m_Swapchain,				//作成されるスワップチェイン
		&m_Device,					//作成されるデバイス
		&level,						//作成されたデバイスの機能レベル
		&m_Context					//作成されるデバイスコンテキスト
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//バックバッファの取得
	//////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* backBuffer;
	hresult = m_Swapchain->GetBuffer(
		0,							//バッファのインデックス(基本は0)
		IID_PPV_ARGS(&backBuffer)	//バッファの取得先
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//レンダリングターゲットの生成
	//////////////////////////////////////////////////////////////////////////////////
	hresult = m_Device->CreateRenderTargetView(
		backBuffer,			//作成するバッファのリソース
		nullptr,			//作成するViewの設定内容データの指定(nullptrでデフォルト設定になる)
		&m_RenderTargetView	//作成されたRenderTargetView
	);

	backBuffer->Release();
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//定数バッファの設定
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.ByteWidth = ((sizeof(ConstantBuffer) - 1) / 16 + 1) * 16;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;									//バッファーで想定されている読み込みおよび書き込みの方法
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;						//バッファーをどのようにパイプラインにバインドするか
	constantBufferDesc.CPUAccessFlags = 0;											//CPU アクセスのフラグ

	hresult = m_Device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffer);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//頂点バッファの生成
	//////////////////////////////////////////////////////////////////////////////////
	m_Vertex.AddVertex({ {0.0f, 0.0f}, {0.0f, 0.0f} });
	m_Vertex.AddVertex({ {1.0f, 0.0f}, {1.0f, 0.0f} });
	m_Vertex.AddVertex({ {0.0f, 1.0f}, {0.0f, 1.0f} });
	m_Vertex.AddVertex({ {1.0f, 1.0f}, {1.0f, 1.0f} });

	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(Vertex) * m_Vertex.GetVertexNum();	//バッファーのサイズ (バイト単位)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;									//バッファーで想定されている読み込みおよび書き込みの方法
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						//バッファーをどのようにパイプラインにバインドするか
	bufferDesc.CPUAccessFlags = 0;											//CPU アクセスのフラグ
	bufferDesc.MiscFlags = 0;												//その他のフラグ
	bufferDesc.StructureByteStride = 0;										//構造体が構造化バッファーを表す場合、その構造体のサイズ (バイト単位)

	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
	subresource.pSysMem = m_Vertex.GetVertexList();	//初期化データへのポインタ
	subresource.SysMemPitch = 0;							//テクスチャーにある 1 本の線の先端から隣の線までの距離 (バイト単位) 
	subresource.SysMemSlicePitch = 0;						//1 つの深度レベルの先端から隣の深度レベルまでの距離 (バイト単位)

	hresult = m_Device->CreateBuffer(
		&bufferDesc,	//バッファーの記述へのポインタ
		&subresource,	//初期化データへのポインタ
		&m_VertexBuffer	//作成されるバッファーへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//VertexShaderの生成
	//////////////////////////////////////////////////////////////////////////////////
	ID3DBlob* pBlob;

	hresult = D3DCompileFromFile(
		L"VertexShader.vsh",			//ファイル名
		nullptr,							//D3D_SHADER_MACRO構造体の配列を指定.シェーダマクロを定義する際に設定する.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//コンパイラがインクルードファイルを取り扱うために使用するID3DIncludeインタフェースへのポインタ
		"vs_main",							//エントリーポイントのメソッド名
		"vs_5_0",							//コンパイルターゲットを指定
		0,									//シェーダのコンパイルオプション
		0,									//エフェクトファイルのコンパイルオプション
		&pBlob,								//コンパイルされたコードへアクセスするためのID3DBlobインタフェースのポインタ
		nullptr								//コンパイルエラーメッセージへアクセスするためのID3DBlobインタフェースのポインタ
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreateVertexShader(
		pBlob->GetBufferPointer(),	//コンパイル済みシェーダーへのポインタ
		pBlob->GetBufferSize(),		//コンパイル済み頂点シェーダーのサイズ
		nullptr,					//ID3D11ClassLinkage インターフェースへのポインタ
		&m_VertexShader				//ID3D11VertexShader インターフェイスへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//入力レイアウトの生成
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hresult = m_Device->CreateInputLayout(
		vertexDesc,					//入力アセンブラー ステージの入力データ型の配列
		ARRAYSIZE(vertexDesc),		//入力要素の配列内の入力データ型の数
		pBlob->GetBufferPointer(),	//コンパイル済みシェーダーへのポインタ
		pBlob->GetBufferSize(),		//コンパイル済みシェーダーのサイズ
		&m_InputLayout				//作成される入力レイアウト オブジェクトへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//PixelShaderの生成
	//////////////////////////////////////////////////////////////////////////////////
	hresult = D3DCompileFromFile(
		L"PixelShader.psh",			//ファイル名
		nullptr,							//D3D_SHADER_MACRO構造体の配列を指定.シェーダマクロを定義する際に設定する.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//コンパイラがインクルードファイルを取り扱うために使用するID3DIncludeインタフェースへのポインタ
		"ps_main",							//エントリーポイントのメソッド名
		"ps_5_0",							//コンパイルターゲットを指定
		0,									//シェーダのコンパイルオプション
		0,									//エフェクトファイルのコンパイルオプション
		&pBlob,								//コンパイルされたコードへアクセスするためのID3DBlobインタフェースのポインタ
		nullptr								//コンパイルエラーメッセージへアクセスするためのID3DBlobインタフェースのポインタ
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreatePixelShader(
		pBlob->GetBufferPointer(),	//コンパイル済みシェーダーへのポインタ
		pBlob->GetBufferSize(),		//コンパイル済み頂点シェーダーのサイズ
		nullptr,					//ID3D11ClassLinkage インターフェースへのポインタ
		&m_PixelShader				//ID3D11PixelShader インターフェイスへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//テクスチャの作成
	//////////////////////////////////////////////////////////////////////////////////
	m_TextureImage = std::make_shared<Texture>();
	m_TextureImage->LoadTexture("Image.nbmp");


	//////////////////////////////////////////////////////////////////////////////////
	//テクスチャの設定
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC texture2DDesc;
	ZeroMemory(&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2DDesc.Width = m_TextureImage->GetWidth();
	texture2DDesc.Height = m_TextureImage->GetHeight();
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.SampleDesc.Quality = 0;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2DDesc.CPUAccessFlags = 0;
	texture2DDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subTextureResource;
	ZeroMemory(&subTextureResource, sizeof(D3D11_SUBRESOURCE_DATA));
	subTextureResource.pSysMem = m_TextureImage->GetTextureBuffer();
	subTextureResource.SysMemPitch = m_TextureImage->GetWidth() * m_TextureImage->GetPixelByte();

	hresult = m_Device->CreateTexture2D(&texture2DDesc, &subTextureResource, &m_Texture);

	if (FAILED(hresult))
		return hresult;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = texture2DDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hresult = m_Device->CreateShaderResourceView(m_Texture.Get(), &shaderResourceViewDesc, &m_TextureView);

	if (FAILED(hresult))
		return hresult;
	
	
	//////////////////////////////////////////////////////////////////////////////////
	//サンプラーの作成
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	//拡大縮小時の色の取得方法
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;		//UV座標が範囲外の場合の色の取得方法
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;		//UV座標が範囲外の場合の色の取得方法
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;		//UV座標が範囲外の場合の色の取得方法

	hresult = m_Device->CreateSamplerState(&samplerDesc, &m_SamplerState);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//ビューポートの生成
	//////////////////////////////////////////////////////////////////////////////////
	m_ViewPort.TopLeftX = 0;					//ビューポートの左側の X 位置
	m_ViewPort.TopLeftY = 0;					//ビューポートの上部の Y 位置
	m_ViewPort.Width = (FLOAT)rect.Width();		//ビューポートの幅
	m_ViewPort.Height = (FLOAT)rect.Height();	//ビューポートの高さ
	m_ViewPort.MinDepth = 0.0f;					//ビューポートの最小深度
	m_ViewPort.MaxDepth = 1.0f;					//ビューポートの最大深度


	//////////////////////////////////////////////////////////////////////////////////
	//デバッグメニュー生成
	//////////////////////////////////////////////////////////////////////////////////
	m_DebugMenu.Create(m_Device);
	m_FrameRate.StartMeasureTime();


	return hresult;
}

void DrawManager::Render(HWND hwnd)
{

	//クライアント領域取得
	CRect rect;
	CPoint center;
	GetClientRect(hwnd, &rect);
	center = rect.CenterPoint();

	UINT strides = sizeof(Vertex);
	UINT offsets = 0;
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);						//レンダーターゲットをクリアする
	m_Context->IASetInputLayout(m_InputLayout.Get());										//インプットレイアウトの設定
	m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);	//頂点バッファの設定
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);				//頂点バッファがどの順番で三角形を作るか
	m_Context->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());					//定数バッファの設定	
	m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);								//頂点シェーダーの設定
	m_Context->RSSetViewports(1, &m_ViewPort);												//ビューポートの設定
	m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);								//ピクセルシェーダーの設定
	m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);			//レンダーターゲットの設定
	m_Context->PSSetShaderResources(0, 1, m_TextureView.GetAddressOf());					//テクスチャの設定
	m_Context->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());							//サンプラーの設定

	std::vector<Sprite>::iterator it;
	for (it = m_SpriteList.begin(); it != m_SpriteList.end(); it++)
	{
		int x = static_cast<int>(rect.Width() * it->m_DrawX);
		int y = static_cast<int>(rect.Height() * it->m_DrawY);

		//クライアント領域の中心をConstantBufferに設定
		ConstantBuffer constantBuffer;
		constantBuffer.centerWindow[0] = center.x;
		constantBuffer.centerWindow[1] = center.y;
		constantBuffer.drawPos[0] = x;
		constantBuffer.drawPos[1] = y;
		constantBuffer.height = m_TextureImage->GetHeight();
		constantBuffer.width = m_TextureImage->GetWidth();

		//////////////////////////////////////////////////////////////////////////////////
		//描画
		//////////////////////////////////////////////////////////////////////////////////
		m_Context->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, &constantBuffer, 0, 0);//定数バッファの更新
		m_Context->Draw(m_Vertex.GetVertexNum(), 0);										//描画する
	}

	// フレームレート計測
	m_FrameRate.IncrFrame();

	// デバッグメニュー表示
	DebugMenuLineup lineup{m_FrameRate.GetFrameRate(), m_SpriteList.size()};
	m_DebugMenu.ExecDisp(hwnd, m_Context, lineup);
	m_Swapchain->Present(0, 0);
}

void DrawManager::UpdateWindow()
{
	MoveSprite moveSprite;
	moveSprite.Exec(&m_SpriteList);
}

void DrawManager::UpdateSpriteList(WPARAM key)
{
	SpriteControllerKeyBinding spriteControllerKeyBinding;
	std::unique_ptr<ControlSprite> controlSprite = spriteControllerKeyBinding.CreateController(key);
	if (controlSprite != nullptr) {
		controlSprite->UpdateList(&m_SpriteList);
	}
}

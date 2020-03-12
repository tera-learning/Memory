#include "DebugMenu.h"
#include "DrawManager.h"

DebugMenu::DebugMenu()
{
	m_TextureImage.LoadTexture("AsciiCode.nbmp");
}


DebugMenu::~DebugMenu()
{
}

HRESULT DebugMenu::Create(const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{

	//////////////////////////////////////////////////////////////////////////////////
	//テクスチャの設定
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC texture2DDesc;
	ZeroMemory(&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2DDesc.Width = m_TextureImage.GetWidth();
	texture2DDesc.Height = m_TextureImage.GetHeight();
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
	subTextureResource.pSysMem = m_TextureImage.GetTextureBuffer();
	subTextureResource.SysMemPitch = m_TextureImage.GetWidth() * m_TextureImage.GetPixelByte();

	HRESULT hresult = device->CreateTexture2D(&texture2DDesc, &subTextureResource, &m_Texture);

	if (FAILED(hresult))
		return hresult;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = texture2DDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hresult = device->CreateShaderResourceView(m_Texture.Get(), &shaderResourceViewDesc, &m_TextureView);

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

	hresult = device->CreateSamplerState(&samplerDesc, &m_SamplerState);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//頂点バッファの生成
	//////////////////////////////////////////////////////////////////////////////////
	VertexBuffer v;
	v.AddVertex({ {0.0f, 0.0f}, {0.0f, 0.0f} });
	v.AddVertex({ {1.0f, 0.0f}, {1.0f, 0.0f} });
	v.AddVertex({ {0.0f, 1.0f}, {0.0f, 1.0f} });
	v.AddVertex({ {1.0f, 1.0f}, {1.0f, 1.0f} });

	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(Vertex) * v.GetVertexNum();	//バッファーのサイズ (バイト単位)
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;									//バッファーで想定されている読み込みおよび書き込みの方法
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						//バッファーをどのようにパイプラインにバインドするか
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;											//CPU アクセスのフラグ
	bufferDesc.MiscFlags = 0;												//その他のフラグ
	bufferDesc.StructureByteStride = 0;										//構造体が構造化バッファーを表す場合、その構造体のサイズ (バイト単位)

	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
	subresource.pSysMem = v.GetVertexList();	//初期化データへのポインタ
	subresource.SysMemPitch = 0;							//テクスチャーにある 1 本の線の先端から隣の線までの距離 (バイト単位) 
	subresource.SysMemSlicePitch = 0;						//1 つの深度レベルの先端から隣の深度レベルまでの距離 (バイト単位)

	hresult = device->CreateBuffer(
		&bufferDesc,	//バッファーの記述へのポインタ
		&subresource,	//初期化データへのポインタ
		&m_VertexBuffer	//作成されるバッファーへのポインタ
	);

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

	hresult = device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffer);

	if (FAILED(hresult))
		return hresult;

	return hresult;
}

void DebugMenu::ExecDisp(HWND hwnd, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dContext, const DebugMenuLineup& lineup) const
{
	std::string title = "< DebugDisp >";
	std::string strSpriteNum = "  SpriteNum : " + std::to_string(lineup.m_SpriteNum);
	std::string strFrameRate = "  FrameRate : " + std::to_string(lineup.m_FrameRate);

	ExecDispString(hwnd, dContext, title, 0);
	ExecDispString(hwnd, dContext, strSpriteNum, 1);
	ExecDispString(hwnd, dContext, strFrameRate, 2);
}

void DebugMenu::ExecDispString(HWND hwnd, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dContext, const std::string& str, int line) const
{

	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	//ウィンドウ中央座標を取得
	CRect rect;
	CPoint center;
	GetClientRect(hwnd, &rect);
	center = rect.CenterPoint();

	for (unsigned int i = 0; i < str.size(); i++)
	{

		//文字表示位置を取得
		CPoint charDispPoint = GetDispPointCharacterTexture(i, line);

		//表示する文字のテクスチャ上の位置を取得する
		CRect charTexRect = GetRectCharacterTexture(str[i]);

		float left = static_cast<float>(charTexRect.left) / m_TextureImage.GetWidth();
		float top = static_cast<float>(charTexRect.top) / m_TextureImage.GetHeight();
		float right = static_cast<float>(charTexRect.right) / m_TextureImage.GetWidth();
		float bottom = static_cast<float>(charTexRect.bottom) / m_TextureImage.GetHeight();


		//クライアント領域の中心をConstantBufferに設定
		ConstantBuffer constantBuffer;
		constantBuffer.centerWindow[0] = center.x;
		constantBuffer.centerWindow[1] = center.y;
		constantBuffer.drawPos[0] = charDispPoint.x;
		constantBuffer.drawPos[1] = charDispPoint.y;
		constantBuffer.height = m_TextureImage.GetHeight() / m_CharacterTextureVerticalNum;
		constantBuffer.width = m_TextureImage.GetWidth() / m_CharacterTextureHorizontalNum;


		VertexBuffer v;
		v.AddVertex({ {0.0f, 0.0f}, {left, top} });
		v.AddVertex({ {1.0f, 0.0f}, {right, top} });
		v.AddVertex({ {0.0f, 1.0f}, {left, bottom} });
		v.AddVertex({ {1.0f, 1.0f}, {right, bottom} });

		D3D11_MAPPED_SUBRESOURCE msr;
		dContext->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, v.GetVertexList(), sizeof(Vertex) * 4); // 3頂点分コピー
		dContext->Unmap(m_VertexBuffer.Get(), 0);

		//////////////////////////////////////////////////////////////////////////////////
		//描画
		//////////////////////////////////////////////////////////////////////////////////
		dContext->PSSetShaderResources(0, 1, m_TextureView.GetAddressOf());						//テクスチャの設定
		dContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);	//頂点バッファの設定
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);				//頂点バッファがどの順番で三角形を作るか
		dContext->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());							//サンプラーの設定
		dContext->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());					//定数バッファの設定	
		dContext->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, &constantBuffer, 0, 0);			//定数バッファの更新
		dContext->Draw(4, 0);																	//描画する
	}
}

CRect DebugMenu::GetRectCharacterTexture(int asciicode) const 
{

	int unitWidth = m_TextureImage.GetWidth() / m_CharacterTextureHorizontalNum;
	int unitHeight = m_TextureImage.GetHeight() / m_CharacterTextureVerticalNum;
	int charx = asciicode % m_CharacterTextureHorizontalNum;
	int chary = asciicode / m_CharacterTextureHorizontalNum;

	CRect rect;
	rect.left = charx * unitWidth;
	rect.top = chary * unitHeight;
	rect.right = rect.left + unitWidth;
	rect.bottom = rect.top + unitHeight;

	return rect;
}

CPoint DebugMenu::GetDispPointCharacterTexture(int x, int y) const
{
	CPoint point;

	float unitWidth = static_cast<float>(m_TextureImage.GetWidth()) / m_CharacterTextureHorizontalNum;
	float unitHeight = static_cast<float>(m_TextureImage.GetHeight()) / m_CharacterTextureVerticalNum;

	point.x = m_DispMenuOffsetX + static_cast<int>((x * unitWidth));
	point.y = m_DispMenuOffsetY + static_cast<int>((y * unitHeight));

	return point;
}

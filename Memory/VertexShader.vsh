///////////////////////////////////////////////////////
// グローバル
///////////////////////////////////////////////////////
cbuffer ConstBuffer : register(b0)
{
    int2 gCenterWindow;
    int2 gDrawPos;
    int gHeight;
    int gWidth;
}

///////////////////////////////////////////////////////
// 構造体宣言ブロック
///////////////////////////////////////////////////////
struct vertexInput
{
    float2 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct vertexOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

///////////////////////////////////////////////////////
// 頂点シェーダプログラムブロック
///////////////////////////////////////////////////////
vertexOutput vs_main(vertexInput inData)
{
    vertexOutput outData;
    
    int x = gDrawPos[0] - (gWidth / 2.0f) + (inData.pos[0] * gWidth);
    int y = gDrawPos[1] - (gHeight / 2.0f) + (inData.pos[1] * gHeight);
    
    float2 center = float2(gCenterWindow[0], gCenterWindow[1]);
    float2 vertex = float2(x, y);
    
    outData.pos[0] = (vertex[0] - center[0]) / center[0];
    outData.pos[1] = (center[1] - vertex[1]) / center[1];
    outData.pos[2] = 1.0f;
    outData.pos[3] = 1.0f;
    
    outData.tex = inData.tex;
    
    return outData;
}

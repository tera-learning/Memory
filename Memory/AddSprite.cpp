#define NOMINMAX
#include "AddSprite.h"
#include <random>

AddSprite::AddSprite()
{
}


AddSprite::~AddSprite()
{
}

void AddSprite::UpdateList(std::vector<Sprite>* spriteList)
{
	std::random_device rand;
	constexpr unsigned int randomMaxValue = std::random_device::max();

	//最初のスタート座標　0.0～1.0の間の乱数
	float x = static_cast<float>(rand()) / randomMaxValue;
	float y = static_cast<float>(rand()) / randomMaxValue;

	//移動方向のベクトル
	float mx = ( (static_cast<float>(rand()) / randomMaxValue) * 2 ) - 1.0f;
	float my = ( (static_cast<float>(rand()) / randomMaxValue) * 2 ) - 1.0f;
	float length = sqrt(mx * mx + my * my);
	mx /= length;
	my /= length;

	Sprite sprite{x, y, mx, my};

	spriteList->push_back(sprite);
}

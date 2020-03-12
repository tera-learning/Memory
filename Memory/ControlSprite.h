#pragma once
#include "Sprite.h"
#include <vector>

class ControlSprite
{
public:
	ControlSprite();
	virtual ~ControlSprite();
	virtual void UpdateList(std::vector<Sprite>* spriteList) = 0;
};


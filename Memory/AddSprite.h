#pragma once
#include "ControlSprite.h"
class AddSprite :
	public ControlSprite
{
public:
	AddSprite();
	virtual ~AddSprite();
	void UpdateList(std::vector<Sprite>* spriteList);
};


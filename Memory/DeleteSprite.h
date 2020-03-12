#pragma once
#include "ControlSprite.h"
class DeleteSprite :
	public ControlSprite
{
public:
	DeleteSprite();
	virtual ~DeleteSprite();
	void UpdateList(std::vector<Sprite>* spriteList);
};


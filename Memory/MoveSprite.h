#pragma once
#include "ControlSprite.h"
class MoveSprite 
{
public:
	MoveSprite();
	~MoveSprite();
	void Exec(std::vector<Sprite>* spriteList);
};


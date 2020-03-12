#pragma once
#include "ControlSprite.h"
#include <memory>

class SpriteControllerKeyBinding
{
public:
	SpriteControllerKeyBinding();
	~SpriteControllerKeyBinding();
	std::unique_ptr<ControlSprite> CreateController(int key);
};


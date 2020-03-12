#include "SpriteControllerKeyBinding.h"
#include "AddSprite.h"
#include "DeleteSprite.h"

SpriteControllerKeyBinding::SpriteControllerKeyBinding()
{
}


SpriteControllerKeyBinding::~SpriteControllerKeyBinding()
{
}

std::unique_ptr<ControlSprite> SpriteControllerKeyBinding::CreateController(int key)
{
	if (key == VK_OEM_PLUS)
	{
		return std::unique_ptr<AddSprite>(new AddSprite());
	}
	else if (key == VK_OEM_MINUS)
	{
		return std::unique_ptr<DeleteSprite>(new DeleteSprite());
	}

	return nullptr;
}

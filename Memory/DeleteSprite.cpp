#include "DeleteSprite.h"



DeleteSprite::DeleteSprite()
{
}


DeleteSprite::~DeleteSprite()
{
}

void DeleteSprite::UpdateList(std::vector<Sprite>* spriteList)
{
	if (spriteList->size() > 0) {
		spriteList->erase(spriteList->begin());
	}
}

#include "MoveSprite.h"



MoveSprite::MoveSprite()
{
}


MoveSprite::~MoveSprite()
{
}

void MoveSprite::Exec(std::vector<Sprite>* spriteList)
{
	float speed = 0.0001f;
	std::vector<Sprite>::iterator it;
	for (it = spriteList->begin(); it != spriteList->end(); it++)
	{
		it->m_DrawX += it->m_MoveX * speed;
		it->m_DrawY += it->m_MoveY * speed;

		if (it->m_DrawX < 0.0f || it->m_DrawX > 1.0f)
		{
			it->m_MoveX *= -1.0f;
		}
		if (it->m_DrawY < 0.0f || it->m_DrawY > 1.0f)
		{
			it->m_MoveY *= -1.0f;
		}
	}
}

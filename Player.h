#pragma once
#include "objects.h"

class Player : public Object
{
public:
	Player(GameEngine* engine);

	void shoot();

	~Player(void);

private:
	GLuint m_currentWeapon;
};

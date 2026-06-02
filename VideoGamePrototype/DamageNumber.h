#pragma once
#include "Vector2f.h"

struct DamageNumber
{
	Vector2f position;
	float    damage;
	float    lifetime;       // seconds remaining
	bool     dealtByPlayer;  // true = player hit enemy (white), false = enemy hit player (orange-red)

	static constexpr float k_MaxLifetime{ 1.2f };
};
#pragma once
#include "Character.h"
#include <vector>


class CharacterManager final
{
public:
	explicit CharacterManager(const Rectf& playfield);
	~CharacterManager();

	void Draw() const;
	
	void MouseDownEvent(float x, float y);
	void MouseMoveEvent(float x, float y);
	void MouseReleaseEvent();
	void CreateEnemy(int amount);

	void Update(float elapsedSec);

private:
	void DrawShotDirection() const;
	void ShootPlayer();

	Character m_Player;
	std::vector<Character*> m_pEnemies{};

	bool m_MouseDownOnPlayer{ false };
	Vector2f m_MousePosition{};

	const Rectf m_Playfield{};

	static const float m_MaxCastLength;
	static const float m_MaxShotPower;

	static const Color4f m_PlayerFillColor;
	static const Color4f m_PlayerOutlineColor;
	static const Color4f m_EnemyFillColor;
	static const Color4f m_EnemyOutlineColor;
};


#pragma once
#include "Character.h"
#include <vector>
#include <functional>

class CharacterManager final
{
public:
	explicit CharacterManager(const Rectf& playfield);
	~CharacterManager();

	void Draw() const;

	bool IsPlayerDead() const;
	bool IsGameOver() const;
	int GetWaveNumber() const;

	void MouseDownEvent(float x, float y);
	void MouseMoveEvent(float x, float y);
	void MouseReleaseEvent();
	void CreateEnemy(int amount);

	void Update(float elapsedSec);
	void Reset();

private:
	void DrawShotDirection() const;
	void ShootPlayer();
	void ShootEnemy(Character* pEnemy);

	Character m_Player;
	std::vector<Character*> m_pEnemies{};

	int m_CurrentWave{ 1 };
	bool m_HasPlayerShot{ false };
	bool m_HasGameStarted{ false };

	bool m_MouseDown{ false };
	Vector2f m_InitialMouseClickPos{};
	Vector2f m_CurrentMouseClickPos{};

	const Rectf m_Playfield{};

	static const float m_MaxCastLength;
	static const float m_MaxShotPower;
	static const float m_MaxEnemyShotPower;
	static const float m_MinimumEnemyShotPower;
	static const float m_EnemyShootCooldown;

	static const Color4f m_PlayerFillColor;
	static const Color4f m_PlayerOutlineColor;
	static const Color4f m_EnemyFillColor;
	static const Color4f m_EnemyOutlineColor;
};
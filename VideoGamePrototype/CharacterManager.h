// --- START OF FILE CharacterManager.h ---
#pragma once
#include "Character.h"
#include "Powerup.h"
#include <vector>
#include <functional>

class Texture;

class CharacterManager final
{
public:
	explicit CharacterManager(const Rectf& playfield);
	~CharacterManager();

	void Draw() const;

	bool IsPlayerDead() const;
	bool IsGameOver() const;
	int GetWaveNumber() const;
	int GetTotalWaves() const;

	void MouseDownEvent(float x, float y);
	void MouseMoveEvent(float x, float y);
	void MouseReleaseEvent();
	void CreateEnemy(int amount);

	void Update(float elapsedSec);
	void Reset();

	void ToggleGodMode();

private:
	void DrawShotDirection() const;
	void ShootPlayer();
	void ShootEnemy(Character* pEnemy);
	void SpawnPowerup();
	void UpdatePowerups();

	struct DamageText {
		Texture* pTexture;
		Vector2f position;
		float lifetime;
	};
	std::vector<DamageText> m_DamageTexts;
	void SpawnDamageText(float damage, const Vector2f& pos, const Color4f& color);
	void UpdateDamageTexts(float elapsedSec);
	void DrawDamageTexts() const;

	Character m_Player;
	std::vector<Character*> m_pEnemies{};
	std::vector<Powerup*> m_pPowerups{};

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
	static const float m_PowerupSpawnInterval;

	float m_PowerupSpawnTimer{ 0.f };

	static const Color4f m_PlayerFillColor;
	static const Color4f m_PlayerOutlineColor;
	static const Color4f m_EnemyFillColor;
	static const Color4f m_EnemyOutlineColor;
};
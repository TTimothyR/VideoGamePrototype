// --- START OF FILE Character.h ---
#pragma once
#include <functional>
#include "EnemyType.h"

class Character final
{
public:
	explicit Character(float health, bool isPlayer, const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor);
	explicit Character(EnemyType type, const Rectf& playfield);

	Character() = default;

	static float GetRadius();
	static float GetBounceFactor(); // Exposes bounce factor for trajectory prediction

	Vector2f    GetPosition() const;
	Vector2f    GetVelocity() const; // <-- Added this!
	bool        IsDead()      const;
	bool        IsEnabled()   const;
	EnemyType   GetEnemyType() const;

	void SetPosition(float x, float y);
	void SetPosition(const Vector2f& position);
	void SetVelocity(float x, float y);
	void SetVelocity(const Vector2f& velocity);

	void Update(float elapsedSec, const Rectf& playfield);

	void CollisionDetection(Character& other, float& outDamageToOther, float& outDamageToThis);
	float ApplyExplosionImpact(Character& target, float force, float damage);

	// Enemy cooldown system
	bool ShotTimer(float elapsedSec);

	// Player cooldown system
	bool IsReadyToShoot() const;
	void PlayerShoot();

	bool IsReadyToExplode() const;
	void MarkExploded();

	float GetShotSpread()   const;
	float GetMinShotPower() const;
	float GetMaxShotPower() const;
	float GetInstanceRadius() const;

	void TakeDamage(float amount);
	void Reset();
	void Enable();

	// Health specific
	void RestoreHealth();
	float GetCurrentHealth() const;
	float GetMaxHealth() const;

	// Powerup effects
	void ActivateShield();
	bool HasShield() const;
	void ActivateHealthRegen(float duration);
	void ActivateRapidFire(float duration);
	bool HasRapidFire() const;

	// God Mode
	void ToggleGodMode();
	bool IsGodMode() const;

	void Draw() const;

private:
	void  DrawHealthBar() const;
	void  InitFromEnemyType(EnemyType type);

	static Vector2f GenerateRandomPosition(const Rectf& playfield, float radius);
	static float    GenerateRandomShotCooldown(float minCd, float maxCd);

	static const float m_DefaultRadius;
	static const float m_BounceFactor;
	static const float m_AnimationTime;
	static const float m_RespawnCooldown;

	float m_InstanceRadius{ 40.f };
	float m_MinShotCooldown{ 2.f };
	float m_MaxShotCooldown{ 4.f };
	float m_ShotSpread{ 0.5f };
	float m_MinShotPower{ 150.f };
	float m_MaxShotPower{ 350.f };

	// Player specific adjustable cooldowns
	float m_BasePlayerCooldown{ 1.5f };
	float m_RapidFireCooldown{ 0.0f }; // Set to 0.0f for instant shots

	float m_MaxHealth{ 200.f };
	float m_CurrentHealth{};
	float m_EndHealth{};

	float m_ShotCooldown{};
	float m_ElapsedSinceLastShot{};
	float m_ElapsedSinceDeath{};

	bool m_Enabled{ false };
	bool m_IsPlayer{};
	bool m_GodMode{ false };

	EnemyType m_EnemyType{ EnemyType::Basic };
	bool      m_HasExploded{ false };

	Color4f  m_FillColor{};
	Color4f  m_OutlineColor{};
	Vector2f m_Position{};
	Vector2f m_Velocity{};

	const Rectf m_Playfield{};

	bool  m_HasShield{ false };
	float m_ShieldTimeRemaining{ 0.f };
	float m_RegenTimeRemaining{ 0.f };
	float m_RapidFireTimeRemaining{ 0.f };

	static const float m_RegenRate;
	static const float m_RegenDuration;
	static const float m_ShieldDuration;
	static const float m_RapidFireDuration;
};
#pragma once
#include <functional>

class Character final
{
public:
	explicit Character(float health, bool isPlayer, const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor);
	Character() = default;

	static float GetRadius();
	Vector2f GetPosition() const;
	bool IsDead() const;
	bool IsEnabled() const;

	void SetPosition(float x, float y);
	void SetPosition(const Vector2f& position);
	void SetVelocity(float x, float y);
	void SetVelocity(const Vector2f& velocity);

	void Update(float elapsedSec, const Rectf& playfield);
	void CollisionDetection(Character& other);

	bool ShotTimer(float elapsedSec);

	void Reset();
	void Enable();

	void Draw() const;

private:
	void DrawHealthBar() const;

	static Vector2f GenerateRandomPosition(const Rectf& playfield);
	static float GenerateRandomShotCooldown();

	const float m_MaxHealth{ 200.f };
	float m_CurrentHealth{ };
	float m_EndHealth{ };

	float m_ShotCooldown{};
	float m_ElapsedSinceLastShot{};
	float m_ElapsedSinceDeath{};

	bool m_Enabled{ false };

	bool m_IsPlayer{};
	
	Color4f m_FillColor{};
	Color4f m_OutlineColor{};

	Vector2f m_Position{};
	Vector2f m_Velocity{};

	const Rectf m_Playfield{};

	static const float m_Radius;
	static const float m_BounceFactor;
	static const float m_AnimationTime;
	static const float m_MinimumShotCooldown;
	static const float m_MaximumShotCooldown;
	static const float m_RespawnCooldown;
};


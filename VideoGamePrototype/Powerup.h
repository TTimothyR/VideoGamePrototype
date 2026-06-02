// --- START OF FILE Powerup.h ---
#pragma once

enum class PowerupType
{
	Shield,
	HealthRegen,
	RapidFire
};

class Powerup final
{
public:
	explicit Powerup(PowerupType type, float x, float y);
	Powerup() = default;

	PowerupType GetType() const;
	Vector2f GetPosition() const;
	bool IsCollected() const;
	static float GetRadius();

	void Collect();
	void Draw() const;

private:
	PowerupType m_Type{};
	Vector2f m_Position{};
	bool m_Collected{ false };

	static const float m_Radius;
};
#pragma once
class Character final
{
public:
	explicit Character(const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor);
	Character() = default;

	static float GetRadius();
	Vector2f GetPosition() const;

	void SetPosition(float x, float y);
	void SetPosition(const Vector2f& position);
	void SetVelocity(float x, float y);
	void SetVelocity(const Vector2f& velocity);

	void Update(float elapsedSec, const Rectf& playfield);
	void CollisionDetection(Character& other);

	void Draw() const;

private:
	void DrawHealthBar() const;

	static Vector2f GenerateRandomPosition(const Rectf& playfield);

	const float m_MaxHealth{ 200.f };
	float m_CurrentHealth{ m_MaxHealth };
	float m_EndHealth{ m_CurrentHealth };
	
	Color4f m_FillColor{};
	Color4f m_OutlineColor{};

	Vector2f m_Position{};
	Vector2f m_Velocity{};

	static const float m_Radius;
	static const float m_BounceFactor;
	static const float m_AnimationTime;
};


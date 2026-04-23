#include "pch.h"
#include "Character.h"
#include "utils.h"
#include <iostream>

const float Character::m_Radius{ 40.f };
const float Character::m_BounceFactor{ 0.75f };
const float Character::m_AnimationTime{ 0.5f };

Character::Character(bool isPlayer, const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor)
	: m_FillColor{fillColor}
	, m_IsPlayer{isPlayer}
	, m_OutlineColor{outlineColor}
	, m_Playfield{playfield}
{
	SetPosition(GenerateRandomPosition(playfield));
}

float Character::GetRadius()
{
	return m_Radius;
}

Vector2f Character::GetPosition() const
{
	return m_Position;
}

void Character::SetPosition(float x, float y)
{
	Character::SetPosition(Vector2f{ x, y });
}

void Character::SetPosition(const Vector2f& position)
{
	m_Position = position;
}

void Character::SetVelocity(float x, float y)
{
	Character::SetVelocity(Vector2f{ x, y });
}

void Character::SetVelocity(const Vector2f& velocity)
{
	m_Velocity = velocity;
}

void Character::Update(float elapsedSec, const Rectf& playfield) {
	if (m_CurrentHealth > 0.f) {
		const float friction{ 0.60f };

		m_Position += m_Velocity * elapsedSec;
		m_Velocity *= std::powf(friction, elapsedSec);

		if (m_CurrentHealth != m_EndHealth && m_CurrentHealth > m_EndHealth) {
			m_CurrentHealth -= elapsedSec * 100.f;
		}
		if (m_CurrentHealth <= m_EndHealth) {
			m_CurrentHealth = m_EndHealth;
		}

		if (m_Velocity.Length() < 0.1f) {
			m_Velocity = Vector2f{ 0.f, 0.f };
		}

		if (m_Position.x - m_Radius <= playfield.left) {
			m_Position.x = playfield.left + m_Radius;
			m_Velocity.x = std::fabsf(m_Velocity.x) * m_BounceFactor;
		}
		else if (m_Position.x + m_Radius >= playfield.left + playfield.width) {
			m_Position.x = (playfield.left + playfield.width) - m_Radius;
			m_Velocity.x = -std::fabsf(m_Velocity.x) * m_BounceFactor;
		}

		if (m_Position.y - m_Radius <= playfield.bottom) {
			m_Position.y = playfield.bottom + m_Radius;
			m_Velocity.y = std::fabsf(m_Velocity.y) * m_BounceFactor;
		}
		else if (m_Position.y + m_Radius >= playfield.bottom + playfield.height) {
			m_Position.y = (playfield.bottom + playfield.height) - m_Radius;
			m_Velocity.y = -std::fabsf(m_Velocity.y) * m_BounceFactor;
		}
	}
	else {
		if (!m_IsPlayer) {
			Reset();
		}
	}
}

void Character::CollisionDetection(Character& other)
{
	if (utils::IsOverlapping(Circlef{ m_Position, m_Radius }, Circlef{ other.m_Position, m_Radius })) {
		const Vector2f collisionVector{ other.m_Position - m_Position };
		const float distance{ collisionVector.Length() };

		const Vector2f normal{ collisionVector / distance };

		const Vector2f relativeVelocity{ m_Velocity - other.m_Velocity };
		const float speedAlongNormal{ relativeVelocity.x * normal.x + relativeVelocity.y * normal.y };

		if (speedAlongNormal <= 0.f) {
			return;
		}

		const float impulseStrength{ -(1 + m_BounceFactor) * speedAlongNormal / 2.f };
		
		float damageSomething{};
		if (this->m_Velocity.Length() > other.m_Velocity.Length()) {
			if (this->m_IsPlayer) {
				damageSomething = 10.f;
			}
			else {
				damageSomething = 1.f;
			}
		}
		else {
			if (other.m_IsPlayer) {
				damageSomething = 10.f;
			}
			else {
				damageSomething = 1.f;
			}
		}

		const float damage{ relativeVelocity.Length() / (m_MaxHealth/damageSomething) };

		if (this->m_Velocity.Length() > other.m_Velocity.Length()) {
			other.m_EndHealth -= damage;
		}
		else {
			this->m_EndHealth -= damage;
		}

		m_Velocity += impulseStrength * normal;
		other.m_Velocity -= impulseStrength * normal;

		const float overlap = m_Radius + m_Radius - distance;
		m_Position -= (overlap / 2.f) * normal;
		other.m_Position += (overlap / 2.f) * normal;
	}
}

void Character::Draw() const {
	if (m_CurrentHealth > 0.f) {
		utils::SetColor(m_FillColor);
		utils::FillEllipse(m_Position, m_Radius, m_Radius);
		DrawHealthBar();
		utils::SetColor(m_OutlineColor);
		utils::DrawEllipse(m_Position, m_Radius, m_Radius, 5.f);
	}
}

void Character::DrawHealthBar() const
{
	const float pi{ static_cast<float>(M_PI) };
	const float healthPercentage{ m_CurrentHealth / m_MaxHealth };
	const float healthBarAngle{ 2 * pi * healthPercentage };
	
	utils::SetColor(Color4f{ 1.f,1.f,1.f,0.25f });
	utils::FillArc(m_Position, m_Radius, m_Radius, 0.f+pi/2.f, healthBarAngle+pi/2.f);
}

void Character::Reset()
{
	m_Position = GenerateRandomPosition(m_Playfield);
	m_Velocity = Vector2f{ 0.f,0.f };
	m_CurrentHealth = m_MaxHealth;
	m_EndHealth = m_MaxHealth;
}

Vector2f Character::GenerateRandomPosition(const Rectf& playfield)
{
	const int minX{ static_cast<int>(playfield.left + m_Radius) };
	const int maxX{ static_cast<int>(playfield.left + playfield.width - m_Radius) };
	const int minY{ static_cast<int>(playfield.bottom + m_Radius) };
	const int maxY{ static_cast<int>(playfield.bottom + playfield.height - m_Radius) };

	const float randomX{ static_cast<float>(rand() % (maxX - minX + 1) + minX) };
	const float randomY{ static_cast<float>(rand() % (maxY - minY + 1) + minY) };

	return Vector2f{ randomX, randomY };
}

#include "pch.h"
#include "Character.h"
#include "utils.h"
#include <iostream>

const float Character::m_Radius{ 40.f };
const float Character::m_BounceFactor{ 0.75f };
const float Character::m_AnimationTime{ 0.5f };
const float Character::m_MinimumShotCooldown{ 2.f };
const float Character::m_MaximumShotCooldown{ 4.f };
const float Character::m_RespawnCooldown{ 1.5f };

Character::Character(float health, bool isPlayer, const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor)
	: m_FillColor{ fillColor }
	, m_IsPlayer{ isPlayer }
	, m_MaxHealth{ health }
	, m_CurrentHealth{ health }
	, m_EndHealth{ health }
	, m_OutlineColor{ outlineColor }
	, m_Playfield{ playfield }
	, m_ShotCooldown{ GenerateRandomShotCooldown() }
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

bool Character::IsDead() const
{
	return (m_CurrentHealth <= 0);
}

bool Character::IsEnabled() const
{
	return m_Enabled;
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
	if (!m_Enabled) {
		return;
	}
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
		if (m_IsPlayer) {
			m_ElapsedSinceDeath += elapsedSec;
		}
	}
}

void Character::CollisionDetection(Character& other)
{
	if (!m_Enabled || !other.m_Enabled) {
		return;
	}
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

		if (this->m_Velocity.Length() > other.m_Velocity.Length()) {
			// 'this' is faster/attacker, 'other' takes damage
			if (this->m_IsPlayer || other.m_IsPlayer) {
				const float baseDamage{ std::fabsf(relativeVelocity.Length()) / other.m_MaxHealth };
				const float damage{ this->m_IsPlayer ? baseDamage * 2.f : baseDamage };
				other.m_EndHealth -= damage;
			}
			if (other.m_EndHealth <= 0.f && !other.m_IsPlayer) {
				other.m_Enabled = false;
			}
		}
		else {
			// 'other' is faster/attacker, 'this' takes damage
			if (this->m_IsPlayer || other.m_IsPlayer) {
				const float baseDamage{ std::fabsf(relativeVelocity.Length()) / this->m_MaxHealth };
				const float damage{ other.m_IsPlayer ? baseDamage * 2.f : baseDamage };
				this->m_EndHealth -= damage;
			}
			if (this->m_EndHealth <= 0.f && !this->m_IsPlayer) {
				this->m_Enabled = false;
			}
		}

		m_Velocity += impulseStrength * normal;
		other.m_Velocity -= impulseStrength * normal;

		const float overlap = m_Radius + m_Radius - distance;
		m_Position -= (overlap / 2.f) * normal;
		other.m_Position += (overlap / 2.f) * normal;
	}
}

bool Character::ShotTimer(float elapsedSec)
{
	m_ElapsedSinceLastShot += elapsedSec;
	if (m_ElapsedSinceLastShot >= m_ShotCooldown) {
		m_ElapsedSinceLastShot -= m_ShotCooldown;
		m_ShotCooldown = GenerateRandomShotCooldown();
		return true;
	}
	return false;
}

void Character::Reset() {
	m_Enabled = false;
	m_Position = GenerateRandomPosition(m_Playfield);
	m_CurrentHealth = m_MaxHealth;
	m_EndHealth = m_MaxHealth;
	m_Velocity = Vector2f{ 0.f,0.f };
	m_ElapsedSinceDeath = 0.f;
	m_ElapsedSinceLastShot = 0.f;
}

void Character::Enable()
{
	m_Enabled = true;
	m_ElapsedSinceLastShot = 0.f;
	m_ShotCooldown = GenerateRandomShotCooldown();
}

void Character::Draw() const {
	if (m_Enabled) {
		if (m_CurrentHealth > 0.f) {
			utils::SetColor(m_FillColor);
			utils::FillEllipse(m_Position, m_Radius, m_Radius);
			DrawHealthBar();
			utils::SetColor(m_OutlineColor);
			utils::DrawEllipse(m_Position, m_Radius, m_Radius, 5.f);
		}
	}
}

void Character::DrawHealthBar() const
{
	const float pi{ static_cast<float>(M_PI) };
	const float healthPercentage{ m_CurrentHealth / m_MaxHealth };
	const float healthBarAngle{ 2 * pi * healthPercentage };

	utils::SetColor(Color4f{ 1.f,1.f,1.f,0.25f });
	utils::FillArc(m_Position, m_Radius, m_Radius, 0.f + pi / 2.f, healthBarAngle + pi / 2.f);
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

float Character::GenerateRandomShotCooldown()
{
	const int minimum{ static_cast<int>(m_MinimumShotCooldown * 100) };
	const int maximum{ static_cast<int>(m_MaximumShotCooldown * 100) };
	const float random{ static_cast<float>(rand() % (maximum - minimum + 1) + minimum) / 100.f };

	return random;
}
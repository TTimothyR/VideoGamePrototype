#include "pch.h"
#include "Character.h"
#include "utils.h"
#include <iostream>

const float Character::m_Radius{ 40.f };

Character::Character(const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor)
	: m_FillColor{fillColor}
	, m_OutlineColor{outlineColor}
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
	const float friction{ 0.70f };

	m_Position += m_Velocity * elapsedSec;
	m_Velocity *= std::powf(friction, elapsedSec);

	if (m_Velocity.Length() < 0.01f) {
		m_Velocity = Vector2f{ 0.f, 0.f };
	}

	if (m_Position.x - m_Radius <= playfield.left) {
		m_Velocity.x *= -1;
	}
	else if (m_Position.x + m_Radius >= playfield.left + playfield.width) {
		m_Velocity.x *= -1;
	}
	if (m_Position.y - m_Radius <= playfield.bottom) {
		m_Velocity.y *= -1;
	}
	else if (m_Position.y + m_Radius >= playfield.bottom + playfield.height) {
		m_Velocity.y *= -1;
	}
}

void Character::Draw() const {
	utils::SetColor(m_FillColor);
	utils::FillEllipse(m_Position, m_Radius, m_Radius);
	utils::SetColor(m_OutlineColor);
	utils::DrawEllipse(m_Position, m_Radius, m_Radius, 5.f);
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

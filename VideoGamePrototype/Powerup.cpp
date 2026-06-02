// --- START OF FILE Powerup.cpp ---
#include "pch.h"
#include "Powerup.h"
#include "utils.h"

const float Powerup::m_Radius{ 20.f };

Powerup::Powerup(PowerupType type, float x, float y)
	: m_Type{ type }
	, m_Position{ x, y }
{
}

PowerupType Powerup::GetType() const
{
	return m_Type;
}

Vector2f Powerup::GetPosition() const
{
	return m_Position;
}

bool Powerup::IsCollected() const
{
	return m_Collected;
}

float Powerup::GetRadius()
{
	return m_Radius;
}

void Powerup::Collect()
{
	m_Collected = true;
}

void Powerup::Draw() const
{
	if (m_Collected) return;

	if (m_Type == PowerupType::Shield)
	{
		// Blue hexagon-ish shield icon
		utils::SetColor(Color4f{ 0.2f, 0.8f, 1.f, 0.85f });
		utils::FillEllipse(m_Position, m_Radius, m_Radius);
		utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
		utils::DrawEllipse(m_Position, m_Radius, m_Radius, 3.f);
		utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
		utils::DrawLine(Vector2f{ m_Position.x - m_Radius * 0.4f, m_Position.y + m_Radius * 0.5f }, Vector2f{ m_Position.x, m_Position.y - m_Radius * 0.5f }, 3.f);
		utils::DrawLine(Vector2f{ m_Position.x, m_Position.y - m_Radius * 0.5f }, Vector2f{ m_Position.x + m_Radius * 0.4f, m_Position.y + m_Radius * 0.5f }, 3.f);
		utils::DrawLine(Vector2f{ m_Position.x - m_Radius * 0.4f, m_Position.y + m_Radius * 0.5f }, Vector2f{ m_Position.x + m_Radius * 0.4f, m_Position.y + m_Radius * 0.5f }, 3.f);
	}
	else if (m_Type == PowerupType::HealthRegen)
	{
		// Green cross (health) icon
		utils::SetColor(Color4f{ 0.2f, 0.9f, 0.3f, 0.85f });
		utils::FillEllipse(m_Position, m_Radius, m_Radius);
		utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
		utils::DrawEllipse(m_Position, m_Radius, m_Radius, 3.f);
		const float arm{ m_Radius * 0.55f };
		const float thick{ m_Radius * 0.22f };
		utils::FillRect(Rectf{ m_Position.x - thick, m_Position.y - arm, thick * 2.f, arm * 2.f });
		utils::FillRect(Rectf{ m_Position.x - arm,   m_Position.y - thick, arm * 2.f, thick * 2.f });
	}
	else if (m_Type == PowerupType::RapidFire)
	{
		// Orange Rapid Fire / Lightning icon
		utils::SetColor(Color4f{ 1.f, 0.6f, 0.f, 0.85f });
		utils::FillEllipse(m_Position, m_Radius, m_Radius);
		utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
		utils::DrawEllipse(m_Position, m_Radius, m_Radius, 3.f);
		utils::DrawLine(Vector2f{ m_Position.x + 5.f, m_Position.y + 10.f }, Vector2f{ m_Position.x - 2.f, m_Position.y + 2.f }, 3.f);
		utils::DrawLine(Vector2f{ m_Position.x - 2.f, m_Position.y + 2.f }, Vector2f{ m_Position.x + 4.f, m_Position.y }, 3.f);
		utils::DrawLine(Vector2f{ m_Position.x + 4.f, m_Position.y }, Vector2f{ m_Position.x - 5.f, m_Position.y - 10.f }, 3.f);
	}
}
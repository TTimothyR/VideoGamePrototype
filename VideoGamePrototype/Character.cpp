// --- START OF FILE Character.cpp ---
#include "pch.h"
#include "Character.h"
#include "utils.h"
#include <iostream>

const float Character::m_DefaultRadius{ 40.f };
const float Character::m_BounceFactor{ 0.75f };
const float Character::m_AnimationTime{ 0.5f };
const float Character::m_RespawnCooldown{ 1.5f };
const float Character::m_RegenRate{ 7.f };
const float Character::m_RegenDuration{ 5.f };
const float Character::m_ShieldDuration{ 6.f };
const float Character::m_RapidFireDuration{ 5.f };

Character::Character(float health, bool isPlayer, const Rectf& playfield, const Color4f& fillColor, const Color4f& outlineColor)
	: m_FillColor{ fillColor }
	, m_IsPlayer{ isPlayer }
	, m_MaxHealth{ health }
	, m_CurrentHealth{ health }
	, m_EndHealth{ health }
	, m_OutlineColor{ outlineColor }
	, m_Playfield{ playfield }
	, m_InstanceRadius{ m_DefaultRadius }
{
	m_ElapsedSinceLastShot = 999.f; // Player ready immediately
	SetPosition(GenerateRandomPosition(playfield, m_InstanceRadius));
}

Character::Character(EnemyType type, const Rectf& playfield)
	: m_IsPlayer{ false }
	, m_Playfield{ playfield }
{
	InitFromEnemyType(type);
	m_ShotCooldown = GenerateRandomShotCooldown(m_MinShotCooldown, m_MaxShotCooldown);
	SetPosition(GenerateRandomPosition(playfield, m_InstanceRadius));
}

void Character::InitFromEnemyType(EnemyType type)
{
	m_EnemyType = type;
	switch (type)
	{
	case EnemyType::Basic:
		m_MaxHealth = 75.f; m_InstanceRadius = 40.f; m_MinShotCooldown = 2.f; m_MaxShotCooldown = 4.f;
		m_FillColor = Color4f{ 1.f, 0.5f, 0.5f, 1.f }; m_OutlineColor = Color4f{ 196.f / 255.f, 35.f / 255.f, 35.f / 255.f, 1.f };
		m_MinShotPower = 150.f; m_MaxShotPower = 350.f; m_ShotSpread = 0.5f; break;
	case EnemyType::Tank:
		m_MaxHealth = 150.f; m_InstanceRadius = 55.f; m_MinShotCooldown = 3.f; m_MaxShotCooldown = 6.f;
		m_FillColor = Color4f{ 0.7f, 0.3f, 0.8f, 1.f }; m_OutlineColor = Color4f{ 0.4f, 0.1f, 0.5f, 1.f };
		m_MinShotPower = 100.f; m_MaxShotPower = 250.f; m_ShotSpread = 0.8f; break;
	case EnemyType::Sniper:
		m_MaxHealth = 40.f; m_InstanceRadius = 30.f; m_MinShotCooldown = 4.f; m_MaxShotCooldown = 7.f;
		m_FillColor = Color4f{ 1.f, 0.9f, 0.2f, 1.f }; m_OutlineColor = Color4f{ 0.8f, 0.7f, 0.1f, 1.f };
		m_MinShotPower = 400.f; m_MaxShotPower = 600.f; m_ShotSpread = 0.05f; break;
	case EnemyType::Bomber:
		m_MaxHealth = 50.f; m_InstanceRadius = 35.f; m_MinShotCooldown = 1.5f; m_MaxShotCooldown = 3.f;
		m_FillColor = Color4f{ 1.f, 0.4f, 0.0f, 1.f }; m_OutlineColor = Color4f{ 0.8f, 0.2f, 0.0f, 1.f };
		m_MinShotPower = 200.f; m_MaxShotPower = 400.f; m_ShotSpread = 0.4f; break;
	case EnemyType::Boss:
		m_MaxHealth = 2000.f; m_InstanceRadius = 80.f; m_MinShotCooldown = 1.0f; m_MaxShotCooldown = 2.0f;
		m_FillColor = Color4f{ 0.1f, 0.1f, 0.1f, 1.f }; m_OutlineColor = Color4f{ 1.f, 0.2f, 0.2f, 1.f };
		m_MinShotPower = 400.f; m_MaxShotPower = 900.f; m_ShotSpread = 0.0f; break;
	}
	m_CurrentHealth = m_MaxHealth; m_EndHealth = m_MaxHealth;
}

float Character::GetRadius() { return m_DefaultRadius; }
float Character::GetBounceFactor() { return m_BounceFactor; }
float Character::GetInstanceRadius() const { return m_InstanceRadius; }
Vector2f Character::GetPosition() const { return m_Position; }
Vector2f Character::GetVelocity() const { return m_Velocity; } // <-- Added implementation here!
bool Character::IsEnabled() const { return m_Enabled; }
EnemyType Character::GetEnemyType() const { return m_EnemyType; }
float Character::GetShotSpread() const { return m_ShotSpread; }
float Character::GetMinShotPower() const { return m_MinShotPower; }
float Character::GetMaxShotPower() const { return m_MaxShotPower; }

float Character::GetCurrentHealth() const { return m_CurrentHealth; }
float Character::GetMaxHealth() const { return m_MaxHealth; }

bool Character::IsDead() const { return (m_EndHealth <= 0.f); }
bool Character::IsReadyToExplode() const { return m_EnemyType == EnemyType::Bomber && IsDead() && !m_HasExploded; }
void Character::MarkExploded() { m_HasExploded = true; }

void Character::SetPosition(float x, float y) { Character::SetPosition(Vector2f{ x, y }); }
void Character::SetPosition(const Vector2f& position) { m_Position = position; }
void Character::SetVelocity(float x, float y) { Character::SetVelocity(Vector2f{ x, y }); }
void Character::SetVelocity(const Vector2f& velocity) { m_Velocity = velocity; }

void Character::Update(float elapsedSec, const Rectf& playfield) {
	if (!m_Enabled) return;

	if (m_IsPlayer) m_ElapsedSinceLastShot += elapsedSec;

	if (m_CurrentHealth > 0.f) {
		const float friction{ 0.60f };
		m_Position += m_Velocity * elapsedSec;
		m_Velocity *= std::powf(friction, elapsedSec);

		if (m_RegenTimeRemaining > 0.f) {
			m_RegenTimeRemaining -= elapsedSec;
			if (m_RegenTimeRemaining < 0.f) m_RegenTimeRemaining = 0.f;
			m_EndHealth = std::fminf(m_EndHealth + m_RegenRate * elapsedSec, m_MaxHealth);
			m_CurrentHealth = std::fminf(m_CurrentHealth + m_RegenRate * elapsedSec, m_MaxHealth);
		}

		if (m_ShieldTimeRemaining > 0.f) {
			m_ShieldTimeRemaining -= elapsedSec;
			if (m_ShieldTimeRemaining <= 0.f) m_HasShield = false;
		}

		if (m_RapidFireTimeRemaining > 0.f) {
			m_RapidFireTimeRemaining -= elapsedSec;
		}

		if (m_CurrentHealth != m_EndHealth && m_CurrentHealth > m_EndHealth) {
			m_CurrentHealth -= elapsedSec * 100.f;
		}
		if (m_CurrentHealth <= m_EndHealth) {
			m_CurrentHealth = m_EndHealth;
		}

		if (m_Velocity.Length() < 0.1f) m_Velocity = Vector2f{ 0.f, 0.f };

		if (m_Position.x - m_InstanceRadius <= playfield.left) {
			m_Position.x = playfield.left + m_InstanceRadius;
			m_Velocity.x = std::fabsf(m_Velocity.x) * m_BounceFactor;
		}
		else if (m_Position.x + m_InstanceRadius >= playfield.left + playfield.width) {
			m_Position.x = (playfield.left + playfield.width) - m_InstanceRadius;
			m_Velocity.x = -std::fabsf(m_Velocity.x) * m_BounceFactor;
		}

		if (m_Position.y - m_InstanceRadius <= playfield.bottom) {
			m_Position.y = playfield.bottom + m_InstanceRadius;
			m_Velocity.y = std::fabsf(m_Velocity.y) * m_BounceFactor;
		}
		else if (m_Position.y + m_InstanceRadius >= playfield.bottom + playfield.height) {
			m_Position.y = (playfield.bottom + playfield.height) - m_InstanceRadius;
			m_Velocity.y = -std::fabsf(m_Velocity.y) * m_BounceFactor;
		}
	}
	else if (m_IsPlayer) {
		m_ElapsedSinceDeath += elapsedSec;
	}
}

void Character::CollisionDetection(Character& other, float& outDamageToOther, float& outDamageToThis)
{
	outDamageToOther = 0.f; outDamageToThis = 0.f;
	if (!m_Enabled || !other.m_Enabled) return;

	if (utils::IsOverlapping(Circlef{ m_Position, m_InstanceRadius }, Circlef{ other.m_Position, other.m_InstanceRadius })) {
		const Vector2f collisionVector{ other.m_Position - m_Position };
		const float distance{ collisionVector.Length() };
		const Vector2f normal{ collisionVector / distance };
		const Vector2f relativeVelocity{ m_Velocity - other.m_Velocity };
		const float speedAlongNormal{ relativeVelocity.x * normal.x + relativeVelocity.y * normal.y };

		if (speedAlongNormal <= 0.f) return;

		const float impulseStrength{ -(1 + m_BounceFactor) * speedAlongNormal / 2.f };

		// UNIVERSAL DAMAGE FORMULA: Decreased to 0.025 to make the game harder.
		const float damageCoefficient = 0.025f;
		const float impactSpeed = std::fabsf(relativeVelocity.Length());

		if (this->m_Velocity.Length() > other.m_Velocity.Length()) {
			// 'this' is the attacker
			if (this->m_IsPlayer || other.m_IsPlayer) {
				if (!other.m_HasShield && !other.m_GodMode) {
					float damage = impactSpeed * damageCoefficient;

					// Player gets a 2x damage multiplier to feel powerful
					if (this->m_IsPlayer) damage *= 2.0f;

					// CRITICAL HIT: Extra 50% damage if the impact is incredibly fast
					if (impactSpeed >= 500.f) damage *= 1.5f;

					other.m_EndHealth -= damage;
					outDamageToOther = damage;
				}
			}
			if (other.m_EndHealth <= 0.f && !other.m_IsPlayer) other.m_Enabled = false;
		}
		else {
			// 'other' is the attacker
			if (this->m_IsPlayer || other.m_IsPlayer) {
				if (!this->m_HasShield && !this->m_GodMode) {
					float damage = impactSpeed * damageCoefficient;

					// Player gets a 2x damage multiplier if they are the one attacking
					if (other.m_IsPlayer) damage *= 2.0f;

					// CRITICAL HIT
					if (impactSpeed >= 500.f) damage *= 1.5f;

					this->m_EndHealth -= damage;
					outDamageToThis = damage;
				}
			}
			if (this->m_EndHealth <= 0.f && !this->m_IsPlayer) this->m_Enabled = false;
		}

		m_Velocity += impulseStrength * normal;
		other.m_Velocity -= impulseStrength * normal;
		const float overlap = m_InstanceRadius + other.m_InstanceRadius - distance;
		m_Position -= (overlap / 2.f) * normal;
		other.m_Position += (overlap / 2.f) * normal;
	}
}

float Character::ApplyExplosionImpact(Character& target, float force, float damage)
{
	if (!target.m_Enabled) return 0.f;

	Vector2f diff = target.GetPosition() - m_Position;
	float dist = diff.Length();
	if (dist < 0.1f) return 0.f;

	float actualDamage = 0.f;
	float explosionRadius = m_InstanceRadius * 4.f;
	if (dist < explosionRadius) {
		float falloff = 1.f - (dist / explosionRadius);
		target.m_Velocity += (diff / dist) * force * falloff;

		if (damage > 0.f && !target.m_HasShield && !target.m_GodMode) {
			actualDamage = damage * falloff;
			target.m_EndHealth -= actualDamage;
			if (target.m_EndHealth <= 0.f && !target.m_IsPlayer) target.m_Enabled = false;
		}
	}
	return actualDamage;
}

void Character::TakeDamage(float amount) {
	if (m_HasShield || m_GodMode) return;
	m_EndHealth -= amount;
	if (m_EndHealth <= 0.f && !m_IsPlayer) m_Enabled = false;
}

void Character::RestoreHealth() {
	m_CurrentHealth = m_MaxHealth;
	m_EndHealth = m_MaxHealth;
}

bool Character::ShotTimer(float elapsedSec) {
	if (m_IsPlayer) return false;
	m_ElapsedSinceLastShot += elapsedSec;
	if (m_ElapsedSinceLastShot >= m_ShotCooldown) {
		m_ElapsedSinceLastShot -= m_ShotCooldown;
		m_ShotCooldown = GenerateRandomShotCooldown(m_MinShotCooldown, m_MaxShotCooldown);
		return true;
	}
	return false;
}

bool Character::IsReadyToShoot() const {
	float cd = (m_RapidFireTimeRemaining > 0.f) ? m_RapidFireCooldown : m_BasePlayerCooldown;
	return m_ElapsedSinceLastShot >= cd;
}

void Character::PlayerShoot() {
	m_ElapsedSinceLastShot = 0.f;
}

void Character::Reset() {
	m_Enabled = false;
	m_Position = GenerateRandomPosition(m_Playfield, m_InstanceRadius);
	m_CurrentHealth = m_MaxHealth; m_EndHealth = m_MaxHealth;
	m_Velocity = Vector2f{ 0.f,0.f };
	m_ElapsedSinceDeath = 0.f;
	m_ElapsedSinceLastShot = m_IsPlayer ? 999.f : 0.f;
	m_HasShield = false; m_ShieldTimeRemaining = 0.f;
	m_RegenTimeRemaining = 0.f; m_RapidFireTimeRemaining = 0.f;
	m_HasExploded = false;
	// Deliberately not resetting m_GodMode so it stays toggled across deaths/resets
}

void Character::Enable() {
	m_Enabled = true;
	m_ElapsedSinceLastShot = m_IsPlayer ? 999.f : 0.f;
	m_ShotCooldown = GenerateRandomShotCooldown(m_MinShotCooldown, m_MaxShotCooldown);
}

void Character::ActivateShield() { m_HasShield = true; m_ShieldTimeRemaining = m_ShieldDuration; }
bool Character::HasShield() const { return m_HasShield; }
void Character::ActivateHealthRegen(float duration) { m_RegenTimeRemaining = duration; }
void Character::ActivateRapidFire(float duration) { m_RapidFireTimeRemaining = duration; }
bool Character::HasRapidFire() const { return m_RapidFireTimeRemaining > 0.f; }

void Character::ToggleGodMode() { m_GodMode = !m_GodMode; }
bool Character::IsGodMode() const { return m_GodMode; }

void Character::Draw() const {
	if (!m_Enabled || m_CurrentHealth <= 0.f) return;

	// Background slice for player cooldown
	if (m_IsPlayer) {
		float cd = (m_RapidFireTimeRemaining > 0.f) ? m_RapidFireCooldown : m_BasePlayerCooldown;
		// Prevent division by zero if cd is 0.0f
		float ratio = (cd > 0.f) ? std::fminf(m_ElapsedSinceLastShot / cd, 1.f) : 1.f;
		if (ratio < 1.f) {
			utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 0.3f });
			const float pi = static_cast<float>(M_PI);
			utils::FillArc(m_Position, m_InstanceRadius + 6.f, m_InstanceRadius + 6.f, pi / 2.f, pi / 2.f + (2 * pi * ratio));
		}
	}

	utils::SetColor(m_FillColor);
	utils::FillEllipse(m_Position, m_InstanceRadius, m_InstanceRadius);

	// Dont draw healthbar overhead if it's a boss, Boss has its own big UI bar
	if (m_EnemyType != EnemyType::Boss) {
		DrawHealthBar();
	}

	utils::SetColor(m_OutlineColor);
	utils::DrawEllipse(m_Position, m_InstanceRadius, m_InstanceRadius, 5.f);

	if (m_HasShield) {
		utils::SetColor(Color4f{ 0.2f, 0.8f, 1.f, 0.7f });
		utils::DrawEllipse(m_Position, m_InstanceRadius + 8.f, m_InstanceRadius + 8.f, 4.f);
	}
	if (m_RegenTimeRemaining > 0.f) {
		utils::SetColor(Color4f{ 0.2f, 0.9f, 0.3f, 0.4f });
		utils::DrawEllipse(m_Position, m_InstanceRadius + 14.f, m_InstanceRadius + 14.f, 3.f);
	}
	if (m_RapidFireTimeRemaining > 0.f) {
		utils::SetColor(Color4f{ 1.f, 0.6f, 0.f, 0.4f });
		utils::DrawEllipse(m_Position, m_InstanceRadius + 20.f, m_InstanceRadius + 20.f, 3.f);
	}
	if (m_GodMode) {
		utils::SetColor(Color4f{ 1.f, 0.8f, 0.f, 1.f });
		utils::DrawEllipse(m_Position, m_InstanceRadius - 6.f, m_InstanceRadius - 6.f, 3.f); // Inner gold ring for GodMode
	}
}

void Character::DrawHealthBar() const {
	const float pi{ static_cast<float>(M_PI) };
	const float healthPercentage{ m_CurrentHealth / m_MaxHealth };
	const float healthBarAngle{ 2 * pi * healthPercentage };
	utils::SetColor(Color4f{ 1.f,1.f,1.f,0.25f });
	utils::FillArc(m_Position, m_InstanceRadius, m_InstanceRadius, pi / 2.f, healthBarAngle + pi / 2.f);
}

Vector2f Character::GenerateRandomPosition(const Rectf& playfield, float radius) {
	const int minX{ static_cast<int>(playfield.left + radius) };
	const int maxX{ static_cast<int>(playfield.left + playfield.width - radius) };
	const int minY{ static_cast<int>(playfield.bottom + radius) };
	const int maxY{ static_cast<int>(playfield.bottom + playfield.height - radius) };
	return Vector2f{ static_cast<float>(rand() % (maxX - minX + 1) + minX), static_cast<float>(rand() % (maxY - minY + 1) + minY) };
}

float Character::GenerateRandomShotCooldown(float minCd, float maxCd) {
	const int minimum{ static_cast<int>(minCd * 100) };
	const int maximum{ static_cast<int>(maxCd * 100) };
	return static_cast<float>(rand() % (maximum - minimum + 1) + minimum) / 100.f;
}
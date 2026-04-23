#include "pch.h"
#include "CharacterManager.h"
#include "Character.h"
#include "utils.h"
#include <vector>
#include <iostream>

const float CharacterManager::m_MaxCastLength{ 200.f };
const float CharacterManager::m_MaxShotPower{ 500.f };
const float CharacterManager::m_MinimumEnemyShotPower{ 150.f };
const float CharacterManager::m_EnemyShootCooldown{ 2.f };

const Color4f CharacterManager::m_PlayerFillColor{ 0.25f,0.5f,1.f,1.f };
const Color4f CharacterManager::m_PlayerOutlineColor{ 25.f / 255,75.f / 255,156.f / 255,1.f };
const Color4f CharacterManager::m_EnemyFillColor{ 1.f, 0.5f, 0.5f, 1.f };
const Color4f CharacterManager::m_EnemyOutlineColor{ 196.f / 255, 35.f / 255, 35.f / 255, 1.f };

CharacterManager::CharacterManager(const Rectf& playfield)
	: m_Playfield{ playfield }
	, m_Player{ Character{true, m_Playfield, m_PlayerFillColor, m_PlayerOutlineColor} }
{
	m_Player.SetPosition(playfield.width/2, playfield.height/2);
	CreateEnemy(5);
}

CharacterManager::~CharacterManager()
{
	for (Character* pEnemy : m_pEnemies) {
		delete pEnemy;
	}
	m_pEnemies.clear();
}

void CharacterManager::CreateEnemy(int amount)
{
	for (int index{ 0 }; index < amount; ++index) {
		m_pEnemies.emplace_back(new Character{ false, m_Playfield, m_EnemyFillColor, m_EnemyOutlineColor });
	}
}

void CharacterManager::Update(float elapsedSec) {
	m_ElapsedSinceLastShot += elapsedSec;
	if (m_ElapsedSinceLastShot >= m_EnemyShootCooldown) {
		ShootEnemies();
		m_ElapsedSinceLastShot -= m_EnemyShootCooldown;
	}
	m_Player.Update(elapsedSec, m_Playfield);
	for (Character* pEnemy : m_pEnemies) {
		pEnemy->Update(elapsedSec, m_Playfield);
		m_Player.CollisionDetection(*pEnemy);
	}
	for (size_t index1{ 0 }; index1 < m_pEnemies.size(); ++index1) {
		for (size_t index2{ 0 }; index2 < m_pEnemies.size(); ++index2) {
			if (m_pEnemies.at(index1) != m_pEnemies.at(index2)) {
				m_pEnemies.at(index1)->CollisionDetection(*m_pEnemies.at(index2));
			}
		}
	}
}

void CharacterManager::DrawShotDirection() const
{
	const Vector2f playerPosition{ m_Player.GetPosition() };
	Vector2f difference{ playerPosition - m_MousePosition };

	if (difference.Length() > m_MaxCastLength) {
		const float scale{ m_MaxCastLength / difference.Length() };
		difference *= scale;
	}


	utils::SetColor(Color4f{ 1.f,1.f,1.f,1.f });
	utils::DrawLine(playerPosition, playerPosition + difference, 7.f);
}

void CharacterManager::ShootPlayer()
{
	const Vector2f playerPosition{ m_Player.GetPosition() };
	const Vector2f difference{ playerPosition - m_MousePosition };

	const float power{ std::fminf(difference.Length(), m_MaxCastLength) / m_MaxCastLength };

	const Vector2f direction{ difference / difference.Length() };

	m_Player.SetVelocity(direction * power * m_MaxShotPower);
}

void CharacterManager::ShootEnemies()
{
	const Vector2f playerPosition{ m_Player.GetPosition() };
	
	for (Character* pEnemy : m_pEnemies) {
		const Vector2f enemyPosition{ pEnemy->GetPosition() };

		const Vector2f difference{ playerPosition - enemyPosition };

		//const float power{ std::fminf(difference.Length(), m_MaxCastLength) / m_MaxCastLength };
		const float power{ static_cast<float>(rand() % (static_cast<int>(m_MaxShotPower - m_MinimumEnemyShotPower + 1)) + m_MinimumEnemyShotPower)/m_MaxCastLength };
		const Vector2f direction{ difference / difference.Length() };

		pEnemy->SetVelocity(direction * power * m_MaxShotPower);
	}
}

void CharacterManager::MouseDownEvent(float x, float y)
{
	if (utils::IsPointInCircle(Vector2f{ x, y }, Circlef{ m_Player.GetPosition(), m_Player.GetRadius() })) {
		m_MouseDownOnPlayer = true;
		m_MousePosition = Vector2f{ x, y };
	}
}

void CharacterManager::MouseMoveEvent(float x, float y)
{
	if (m_MouseDownOnPlayer) {
		m_MousePosition = Vector2f{ x, y };
	}
}

void CharacterManager::MouseReleaseEvent()
{
	if (m_MouseDownOnPlayer) {
		m_MouseDownOnPlayer = false;
		ShootPlayer();
	}
}

void CharacterManager::Draw() const {
	m_Player.Draw();
	if (m_MouseDownOnPlayer) {
		DrawShotDirection();
	}

	for (const Character* pEnemy : m_pEnemies) {
		pEnemy->Draw();
	}
}
#include "pch.h"
#include "CharacterManager.h"
#include "Character.h"
#include "utils.h"
#include <vector>
#include <iostream>

const float CharacterManager::m_MaxCastLength{ 200.f };
const float CharacterManager::m_MaxShotPower{ 600.f };
const float CharacterManager::m_MaxEnemyShotPower{ 350.f };
const float CharacterManager::m_MinimumEnemyShotPower{ 150.f };
const float CharacterManager::m_EnemyShootCooldown{ 2.f };

const Color4f CharacterManager::m_PlayerFillColor{ 0.25f,0.5f,1.f,1.f };
const Color4f CharacterManager::m_PlayerOutlineColor{ 25.f / 255,75.f / 255,156.f / 255,1.f };
const Color4f CharacterManager::m_EnemyFillColor{ 1.f, 0.5f, 0.5f, 1.f };
const Color4f CharacterManager::m_EnemyOutlineColor{ 196.f / 255, 35.f / 255, 35.f / 255, 1.f };

CharacterManager::CharacterManager(const Rectf& playfield)
	: m_Playfield{ playfield }
	, m_Player{ Character{150.f, true, m_Playfield, m_PlayerFillColor, m_PlayerOutlineColor} }
{
	m_Player.Enable();
	m_Player.SetPosition(playfield.width / 2, playfield.height / 2);
	CreateEnemy(9);
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
		m_pEnemies.emplace_back(new Character{ 75.f, false, m_Playfield, m_EnemyFillColor, m_EnemyOutlineColor });
	}
	for (int index{ 0 }; index < m_CurrentWave; ++index) {
		m_pEnemies.at(index)->Enable();
	}
	m_HasGameStarted = true;
}

void CharacterManager::Update(float elapsedSec) {
	if (m_HasGameStarted) {
		bool anyEnemyAlive{ false };
		for (Character* pEnemy : m_pEnemies) {
			if (pEnemy->IsEnabled() && !pEnemy->IsDead()) {
				anyEnemyAlive = true;
				break;
			}
		}
		if (!anyEnemyAlive && m_HasPlayerShot) {
			if (m_CurrentWave >= static_cast<int>(m_pEnemies.size())) {
				// Beat the final wave - signal game over
				m_HasGameStarted = false;
				return;
			}
			m_CurrentWave += 1;
			m_HasPlayerShot = false;
			for (Character* pEnemy : m_pEnemies) {
				pEnemy->Reset();
			}
			for (int index{ 0 }; index < m_CurrentWave && index < static_cast<int>(m_pEnemies.size()); ++index) {
				m_pEnemies.at(index)->Enable();
			}
		}
	}
	if (m_HasPlayerShot) {
		for (Character* pEnemy : m_pEnemies) {
			bool isReadyToShoot{ pEnemy->ShotTimer(elapsedSec) };
			if (isReadyToShoot) {
				ShootEnemy(pEnemy);
			}
		}
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

void CharacterManager::Reset() {
	m_CurrentWave = 1;
	m_HasPlayerShot = false;
	m_HasGameStarted = false;
	m_MouseDown = false;

	m_Player.Reset();
	m_Player.Enable();
	m_Player.SetPosition(m_Playfield.width / 2, m_Playfield.height / 2);

	for (Character* pEnemy : m_pEnemies) {
		pEnemy->Reset();
	}
	m_pEnemies.at(0)->Enable();
	m_HasGameStarted = true;
}

void CharacterManager::DrawShotDirection() const
{
	const Vector2f playerPosition{ m_Player.GetPosition() };
	Vector2f difference{ m_InitialMouseClickPos - m_CurrentMouseClickPos };

	if (difference.Length() > m_MaxCastLength) {
		const float scale{ m_MaxCastLength / difference.Length() };
		difference *= scale;
	}


	utils::SetColor(Color4f{ 1.f,1.f,1.f,1.f });
	utils::DrawLine(playerPosition, playerPosition + difference, 7.f);
}

void CharacterManager::ShootPlayer()
{
	m_HasPlayerShot = true;
	const Vector2f playerPosition{ m_Player.GetPosition() };
	const Vector2f difference{ m_InitialMouseClickPos - m_CurrentMouseClickPos };

	const float power{ std::fminf(difference.Length(), m_MaxCastLength) / m_MaxCastLength };

	const Vector2f direction{ difference / difference.Length() };

	m_Player.SetVelocity(direction * power * m_MaxShotPower);
}

void CharacterManager::ShootEnemy(Character* pEnemy)
{
	const Vector2f playerPosition{ m_Player.GetPosition() };

	const Vector2f enemyPosition{ pEnemy->GetPosition() };

	const Vector2f difference{ playerPosition - enemyPosition };

	//const float power{ std::fminf(difference.Length(), m_MaxCastLength) / m_MaxCastLength };
	const float power{ static_cast<float>(rand() % (static_cast<int>(m_MaxEnemyShotPower - m_MinimumEnemyShotPower + 1)) + m_MinimumEnemyShotPower) / m_MaxCastLength };

	const float maxSpreadAngle{ 0.5f };
	const float randomAngle{ ((rand() % 1000) / 1000.0f - 0.5f) * 2.0f * maxSpreadAngle };

	const float baseAngle{ std::atan2f(difference.y, difference.x) };
	const float finalAngle{ baseAngle + randomAngle };
	const Vector2f direction{ std::cosf(finalAngle), std::sinf(finalAngle) };

	pEnemy->SetVelocity(direction * power * m_MaxEnemyShotPower);
}

void CharacterManager::MouseDownEvent(float x, float y)
{
	m_MouseDown = true;
	m_InitialMouseClickPos = Vector2f{ x, y };
	m_CurrentMouseClickPos = Vector2f{ x, y };
}

void CharacterManager::MouseMoveEvent(float x, float y)
{
	if (m_MouseDown) {
		m_CurrentMouseClickPos = Vector2f{ x, y };
	}
}

void CharacterManager::MouseReleaseEvent()
{
	if (m_MouseDown) {
		m_MouseDown = false;
		ShootPlayer();
	}
}

void CharacterManager::Draw() const {
	m_Player.Draw();
	if (m_MouseDown) {
		DrawShotDirection();
	}

	for (const Character* pEnemy : m_pEnemies) {
		pEnemy->Draw();
	}
}

bool CharacterManager::IsPlayerDead() const
{
	return (m_Player.IsDead());
}

bool CharacterManager::IsGameOver() const
{
	return !m_HasGameStarted;
}

int CharacterManager::GetWaveNumber() const
{
	return m_CurrentWave;
}
// --- START OF FILE CharacterManager.cpp ---
#include "pch.h"
#include "CharacterManager.h"
#include "Character.h"
#include "Powerup.h"
#include "Texture.h"
#include "utils.h"
#include <vector>
#include <iostream>
#include <cmath>

const float CharacterManager::m_MaxCastLength{ 200.f };
const float CharacterManager::m_MaxShotPower{ 600.f };
const float CharacterManager::m_MaxEnemyShotPower{ 350.f };
const float CharacterManager::m_MinimumEnemyShotPower{ 150.f };
const float CharacterManager::m_EnemyShootCooldown{ 2.f };
const float CharacterManager::m_PowerupSpawnInterval{ 8.f };

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
	CreateEnemy(9); // Will create 9 normal enemies, plus 1 boss wave = 10 waves total
}

CharacterManager::~CharacterManager()
{
	for (Character* pEnemy : m_pEnemies) delete pEnemy;
	m_pEnemies.clear();
	for (Powerup* pPowerup : m_pPowerups) delete pPowerup;
	m_pPowerups.clear();
	for (auto& dt : m_DamageTexts) delete dt.pTexture;
	m_DamageTexts.clear();
}

void CharacterManager::CreateEnemy(int amount)
{
	// Regular Waves
	for (int index{ 0 }; index < amount; ++index) {
		EnemyType type = static_cast<EnemyType>(rand() % 4);
		m_pEnemies.emplace_back(new Character{ type, m_Playfield });
	}

	// Final Boss Wave!
	m_pEnemies.emplace_back(new Character{ EnemyType::Boss, m_Playfield });

	// Enable wave 1
	for (int index{ 0 }; index < m_CurrentWave && index < amount; ++index) m_pEnemies.at(index)->Enable();
	m_HasGameStarted = true;
}

void CharacterManager::Update(float elapsedSec) {
	if (m_HasGameStarted) {
		bool anyEnemyAlive{ false };
		for (Character* pEnemy : m_pEnemies) {
			if (pEnemy->IsEnabled() && !pEnemy->IsDead()) {
				anyEnemyAlive = true; break;
			}
		}
		if (!anyEnemyAlive && m_HasPlayerShot) {
			if (m_CurrentWave >= static_cast<int>(m_pEnemies.size())) { // Defeated Boss (Wave 10)
				m_HasGameStarted = false; return;
			}
			m_CurrentWave += 1;
			m_HasPlayerShot = false;

			// Despawn all powerups between waves
			for (Powerup* pPowerup : m_pPowerups) delete pPowerup;
			m_pPowerups.clear();

			for (Character* pEnemy : m_pEnemies) pEnemy->Reset();

			// Setup new wave
			if (m_CurrentWave == static_cast<int>(m_pEnemies.size())) {
				// Boss Wave specifics
				m_Player.RestoreHealth();
				m_pEnemies.back()->Enable(); // Enable ONLY the boss
			}
			else {
				// Normal waves
				for (int index{ 0 }; index < m_CurrentWave && index < static_cast<int>(m_pEnemies.size()) - 1; ++index) {
					m_pEnemies.at(index)->Enable();
				}
			}
		}
	}

	if (m_HasPlayerShot) {
		for (Character* pEnemy : m_pEnemies) {
			// Only let enemies shoot if they are active in the current wave and alive!
			if (pEnemy->IsEnabled() && !pEnemy->IsDead()) {
				if (pEnemy->ShotTimer(elapsedSec)) ShootEnemy(pEnemy);
			}
		}
	}

	m_Player.Update(elapsedSec, m_Playfield);

	for (Character* pEnemy : m_pEnemies) {
		pEnemy->Update(elapsedSec, m_Playfield);

		float dmgToEnemy = 0.f, dmgToPlayer = 0.f;
		m_Player.CollisionDetection(*pEnemy, dmgToEnemy, dmgToPlayer);

		if (dmgToEnemy > 0.f) SpawnDamageText(dmgToEnemy, pEnemy->GetPosition(), Color4f{ 1.f, 1.f, 0.2f, 1.f });
		if (dmgToPlayer > 0.f) SpawnDamageText(dmgToPlayer, m_Player.GetPosition(), Color4f{ 1.f, 0.2f, 0.2f, 1.f });

		if (pEnemy->GetEnemyType() == EnemyType::Bomber && (dmgToEnemy > 0.f || dmgToPlayer > 0.f)) {
			pEnemy->TakeDamage(9999.f);
		}

		if (pEnemy->IsReadyToExplode()) {
			pEnemy->MarkExploded();
			float dmg = pEnemy->ApplyExplosionImpact(m_Player, 1000.f, 30.f);
			if (dmg > 0.f) SpawnDamageText(dmg, m_Player.GetPosition(), Color4f{ 1.f, 0.2f, 0.2f, 1.f });

			for (Character* pOther : m_pEnemies) {
				if (pOther != pEnemy && pOther->IsEnabled() && !pOther->IsDead()) {
					pEnemy->ApplyExplosionImpact(*pOther, 800.f, 0.f);
				}
			}
		}
	}

	for (size_t index1{ 0 }; index1 < m_pEnemies.size(); ++index1) {
		for (size_t index2{ 0 }; index2 < m_pEnemies.size(); ++index2) {
			if (m_pEnemies.at(index1) != m_pEnemies.at(index2)) {
				float d1 = 0.f, d2 = 0.f;
				m_pEnemies.at(index1)->CollisionDetection(*m_pEnemies.at(index2), d1, d2);
			}
		}
	}

	UpdateDamageTexts(elapsedSec);

	if (m_HasPlayerShot) {
		m_PowerupSpawnTimer += elapsedSec;
		if (m_PowerupSpawnTimer >= m_PowerupSpawnInterval) {
			m_PowerupSpawnTimer -= m_PowerupSpawnInterval;
			SpawnPowerup();
		}
	}
	UpdatePowerups();
}

void CharacterManager::Reset() {
	m_CurrentWave = 1; m_HasPlayerShot = false; m_HasGameStarted = false; m_MouseDown = false; m_PowerupSpawnTimer = 0.f;

	for (Powerup* pPowerup : m_pPowerups) delete pPowerup;
	m_pPowerups.clear();
	for (auto& dt : m_DamageTexts) delete dt.pTexture;
	m_DamageTexts.clear();

	m_Player.Reset(); m_Player.Enable(); m_Player.SetPosition(m_Playfield.width / 2, m_Playfield.height / 2);
	for (Character* pEnemy : m_pEnemies) pEnemy->Reset();
	m_pEnemies.at(0)->Enable(); m_HasGameStarted = true;
}

void CharacterManager::ToggleGodMode()
{
	m_Player.ToggleGodMode();
}

void CharacterManager::DrawShotDirection() const
{
	const Vector2f playerPosition{ m_Player.GetPosition() };
	Vector2f difference{ m_InitialMouseClickPos - m_CurrentMouseClickPos };

	// Draw physical drag line (faded)
	utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 0.3f });
	utils::DrawLine(playerPosition, playerPosition + difference, 4.f);

	const float diffLength{ difference.Length() };
	if (diffLength < 1.f) return;

	// Calculate simulation variables
	const float power{ std::fminf(diffLength, m_MaxCastLength) / m_MaxCastLength };
	const Vector2f direction{ difference / diffLength };
	Vector2f simVel = direction * power * m_MaxShotPower;
	Vector2f simPos = playerPosition;

	const float simDt = 0.016f;
	const float simDuration = 1.2f;
	const float friction = 0.60f;
	const float bounce = Character::GetBounceFactor();
	const float radius = m_Player.GetInstanceRadius();

	// Store path points to draw the line and animate the ghost
	std::vector<Vector2f> path;
	path.reserve(static_cast<size_t>(simDuration / simDt) + 1);
	path.push_back(simPos);

	for (float t = 0; t < simDuration; t += simDt) {
		simPos += simVel * simDt;
		simVel *= std::powf(friction, simDt);

		// Playfield Wall Collisions
		if (simPos.x - radius <= m_Playfield.left) {
			simPos.x = m_Playfield.left + radius; simVel.x = std::fabsf(simVel.x) * bounce;
		}
		else if (simPos.x + radius >= m_Playfield.left + m_Playfield.width) {
			simPos.x = (m_Playfield.left + m_Playfield.width) - radius; simVel.x = -std::fabsf(simVel.x) * bounce;
		}

		if (simPos.y - radius <= m_Playfield.bottom) {
			simPos.y = m_Playfield.bottom + radius; simVel.y = std::fabsf(simVel.y) * bounce;
		}
		else if (simPos.y + radius >= m_Playfield.bottom + m_Playfield.height) {
			simPos.y = (m_Playfield.bottom + m_Playfield.height) - radius; simVel.y = -std::fabsf(simVel.y) * bounce;
		}

		// Enemy Collisions (Predictive)
		for (const Character* pEnemy : m_pEnemies) {
			if (!pEnemy->IsEnabled() || pEnemy->IsDead()) continue;

			Vector2f ePos = pEnemy->GetPosition();
			float eRad = pEnemy->GetInstanceRadius();
			Vector2f diffVector = simPos - ePos;
			float dist = diffVector.Length();
			float minDist = radius + eRad;

			if (dist < minDist && dist > 0.001f) {
				Vector2f normal = diffVector / dist;
				float speedAlongNormal = simVel.x * normal.x + simVel.y * normal.y;

				// Only bounce if moving towards the enemy
				if (speedAlongNormal < 0.f) {
					// We use the same math as the actual game logic, assuming enemy is static during prediction
					float impulseStrength = -(1.f + bounce) * speedAlongNormal / 2.f;
					simVel += impulseStrength * normal;
				}

				// Resolve the overlap so the ghost doesn't get stuck inside the enemy
				float overlap = minDist - dist;
				simPos += normal * (overlap / 2.f);
			}
		}

		path.push_back(simPos);
	}

	// Draw predicted trajectory path
	utils::SetColor(Color4f{ 1.f, 1.f, 0.2f, 0.7f });
	for (size_t i = 0; i < path.size() - 1; ++i) {
		utils::DrawLine(path[i], path[i + 1], 2.f);
	}

	// Draw animated ghost traversing the path
	if (!path.empty()) {
		// Use SDL_GetTicks to loop the animation over time
		float timeSec = SDL_GetTicks() / 1000.f;
		float cycleDuration = 1.0f; // The ghost takes 1 second to travel the line
		float progress = std::fmodf(timeSec, cycleDuration) / cycleDuration;

		size_t ghostIdx = static_cast<size_t>(progress * path.size());
		if (ghostIdx >= path.size()) ghostIdx = path.size() - 1;

		Vector2f ghostPos = path[ghostIdx];

		// Fade out the ghost as it reaches the end of the line
		float alpha = 0.65f * (1.f - progress);

		// Draw Ghost (Matches player colors)
		utils::SetColor(Color4f{ m_PlayerFillColor.r, m_PlayerFillColor.g, m_PlayerFillColor.b, alpha });
		utils::FillEllipse(ghostPos, radius, radius);
		utils::SetColor(Color4f{ m_PlayerOutlineColor.r, m_PlayerOutlineColor.g, m_PlayerOutlineColor.b, alpha });
		utils::DrawEllipse(ghostPos, radius, radius, 3.f);
	}
}

void CharacterManager::ShootPlayer()
{
	m_HasPlayerShot = true;
	const Vector2f difference{ m_InitialMouseClickPos - m_CurrentMouseClickPos };
	const float diffLength{ difference.Length() };

	if (diffLength < 1.f) return;

	const float power{ std::fminf(diffLength, m_MaxCastLength) / m_MaxCastLength };
	const Vector2f direction{ difference / diffLength };
	m_Player.SetVelocity(direction * power * m_MaxShotPower);
}

void CharacterManager::ShootEnemy(Character* pEnemy)
{
	const Vector2f playerPosition{ m_Player.GetPosition() };
	const Vector2f enemyPosition{ pEnemy->GetPosition() };
	const Vector2f difference{ playerPosition - enemyPosition };

	if (pEnemy->GetEnemyType() == EnemyType::Boss) {
		// Boss special attacks
		int attackType = rand() % 3;
		const float maxPower = pEnemy->GetMaxShotPower();

		if (attackType == 0) {
			// Attack 1: Gravity Slam (Pulls player in, then dashes)
			Vector2f pullDir = (enemyPosition - playerPosition);
			float dist = pullDir.Length();
			if (dist > 0.1f) {
				m_Player.SetVelocity(m_Player.GetVelocity() + (pullDir / dist) * 600.f);
			}
			const float baseAngle{ std::atan2f(difference.y, difference.x) };
			const Vector2f direction{ std::cosf(baseAngle), std::sinf(baseAngle) };
			pEnemy->SetVelocity(direction * maxPower * 0.8f);
		}
		else if (attackType == 1) {
			// Attack 2: Frenzy Bounce (Shoots off in a random direction at extreme speed)
			float randAng = (rand() % 360) * static_cast<float>(M_PI) / 180.f;
			const Vector2f direction{ std::cosf(randAng), std::sinf(randAng) };
			pEnemy->SetVelocity(direction * maxPower * 1.5f);
		}
		else {
			// Attack 3: Sniper Dash (Highly accurate, extremely fast direct attack)
			const float baseAngle{ std::atan2f(difference.y, difference.x) };
			const Vector2f direction{ std::cosf(baseAngle), std::sinf(baseAngle) };
			pEnemy->SetVelocity(direction * maxPower * 1.2f);
		}
		return;
	}

	const float minPower = pEnemy->GetMinShotPower();
	const float maxPower = pEnemy->GetMaxShotPower();
	const float power{ static_cast<float>(rand() % (static_cast<int>(maxPower - minPower + 1)) + minPower) / m_MaxCastLength };

	const float maxSpreadAngle{ pEnemy->GetShotSpread() };
	const float randomAngle{ ((rand() % 1000) / 1000.0f - 0.5f) * 2.0f * maxSpreadAngle };
	const float baseAngle{ std::atan2f(difference.y, difference.x) };
	const float finalAngle{ baseAngle + randomAngle };

	const Vector2f direction{ std::cosf(finalAngle), std::sinf(finalAngle) };
	pEnemy->SetVelocity(direction * power * maxPower);
}

void CharacterManager::MouseDownEvent(float x, float y)
{
	// Only allow dragging if the player is ready to shoot
	if (!m_Player.IsReadyToShoot()) return;

	m_MouseDown = true;
	m_InitialMouseClickPos = Vector2f{ x, y };
	m_CurrentMouseClickPos = Vector2f{ x, y };
}

void CharacterManager::MouseMoveEvent(float x, float y)
{
	if (m_MouseDown) m_CurrentMouseClickPos = Vector2f{ x, y };
}

void CharacterManager::MouseReleaseEvent()
{
	if (m_MouseDown) {
		m_MouseDown = false;
		ShootPlayer();
		m_Player.PlayerShoot(); // Triggers the cooldown
	}
}

void CharacterManager::SpawnPowerup()
{
	const int margin{ static_cast<int>(Powerup::GetRadius() + 10.f) };
	const int minX{ static_cast<int>(m_Playfield.left) + margin };
	const int maxX{ static_cast<int>(m_Playfield.left + m_Playfield.width) - margin };
	const int minY{ static_cast<int>(m_Playfield.bottom) + margin };
	const int maxY{ static_cast<int>(m_Playfield.bottom + m_Playfield.height) - margin };

	const float x{ static_cast<float>(rand() % (maxX - minX + 1) + minX) };
	const float y{ static_cast<float>(rand() % (maxY - minY + 1) + minY) };

	const PowerupType type{ static_cast<PowerupType>(rand() % 3) };
	m_pPowerups.emplace_back(new Powerup{ type, x, y });
}

void CharacterManager::UpdatePowerups()
{
	const Vector2f playerPos{ m_Player.GetPosition() };
	const float collectRadius{ m_Player.GetInstanceRadius() + Powerup::GetRadius() };

	for (Powerup* pPowerup : m_pPowerups) {
		if (pPowerup->IsCollected()) continue;

		const Vector2f diff{ playerPos - pPowerup->GetPosition() };
		if (diff.Length() <= collectRadius) {
			pPowerup->Collect();
			if (pPowerup->GetType() == PowerupType::Shield) m_Player.ActivateShield();
			else if (pPowerup->GetType() == PowerupType::HealthRegen) m_Player.ActivateHealthRegen(5.f);
			else if (pPowerup->GetType() == PowerupType::RapidFire) m_Player.ActivateRapidFire(5.f);
		}
	}

	for (size_t i{ 0 }; i < m_pPowerups.size(); ) {
		if (m_pPowerups.at(i)->IsCollected()) {
			delete m_pPowerups.at(i);
			m_pPowerups.erase(m_pPowerups.begin() + static_cast<int>(i));
		}
		else {
			++i;
		}
	}
}

void CharacterManager::SpawnDamageText(float damage, const Vector2f& pos, const Color4f& color)
{
	if (damage < 0.1f) return;
	int dmgInt = static_cast<int>(std::ceilf(damage));
	DamageText dt;
	dt.pTexture = new Texture(std::to_string(dmgInt), "ARLRDBD.TTF", 20, color);

	float offsetX = (rand() % 40) - 20.f;
	float offsetY = (rand() % 40) - 20.f;
	dt.position = Vector2f{ pos.x + offsetX, pos.y + offsetY };
	dt.lifetime = 1.2f;

	m_DamageTexts.push_back(dt);
}

void CharacterManager::UpdateDamageTexts(float elapsedSec)
{
	for (size_t i = 0; i < m_DamageTexts.size(); ) {
		m_DamageTexts[i].lifetime -= elapsedSec;
		m_DamageTexts[i].position.y += 30.f * elapsedSec;

		if (m_DamageTexts[i].lifetime <= 0.f) {
			delete m_DamageTexts[i].pTexture;
			m_DamageTexts.erase(m_DamageTexts.begin() + i);
		}
		else {
			++i;
		}
	}
}

void CharacterManager::DrawDamageTexts() const
{
	for (const auto& dt : m_DamageTexts) dt.pTexture->Draw(dt.position);
}

void CharacterManager::Draw() const {
	for (const Powerup* pPowerup : m_pPowerups) pPowerup->Draw();
	m_Player.Draw();
	if (m_MouseDown) DrawShotDirection();
	for (const Character* pEnemy : m_pEnemies) pEnemy->Draw();
	DrawDamageTexts();

	// Draw Boss Health Bar if Boss Wave
	if (m_CurrentWave == static_cast<int>(m_pEnemies.size())) {
		const Character* boss = m_pEnemies.back();
		if (boss->IsEnabled() && !boss->IsDead()) {
			float hpPercent = boss->GetCurrentHealth() / boss->GetMaxHealth();
			float barWidth = m_Playfield.width * 0.6f;
			float barHeight = 20.f;
			Rectf bgRect{ m_Playfield.left + (m_Playfield.width - barWidth) / 2.f,
						  m_Playfield.bottom + m_Playfield.height - 40.f,
						  barWidth, barHeight };

			// Dark Red Background
			utils::SetColor(Color4f{ 0.15f, 0.0f, 0.0f, 0.8f });
			utils::FillRect(bgRect);

			// Bright Red Foreground
			Rectf fgRect{ bgRect.left, bgRect.bottom, barWidth * hpPercent, barHeight };
			utils::SetColor(Color4f{ 1.f, 0.1f, 0.1f, 0.9f });
			utils::FillRect(fgRect);

			// Gold Outline
			utils::SetColor(Color4f{ 1.f, 0.8f, 0.0f, 1.f });
			utils::DrawRect(bgRect, 3.f);
		}
	}
}

bool CharacterManager::IsPlayerDead() const { return (m_Player.IsDead()); }
bool CharacterManager::IsGameOver() const { return !m_HasGameStarted; }
int CharacterManager::GetWaveNumber() const { return m_CurrentWave; }
int CharacterManager::GetTotalWaves() const { return static_cast<int>(m_pEnemies.size()); }
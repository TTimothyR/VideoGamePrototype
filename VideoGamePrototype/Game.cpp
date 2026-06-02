// --- START OF FILE Game.cpp ---
#include "pch.h"
#include "Game.h"
#include "utils.h"
#include "Texture.h"
#include <iostream>

Game::Game(const Window& window)
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	m_pStartText = new Texture{ "Start Game", "ARLRDBD.TTF", 28,  Color4f{0.1f, 0.4f, 0.1f, 1.f} };
	m_pHelpBtnText = new Texture{ "Help", "ARLRDBD.TTF", 28,  Color4f{0.1f, 0.1f, 0.4f, 1.f} };
	m_pQuitBtnText = new Texture{ "Quit", "ARLRDBD.TTF", 28,  Color4f{0.4f, 0.1f, 0.1f, 1.f} };
	m_pResumeText = new Texture{ "Resume", "ARLRDBD.TTF", 28,  Color4f{0.1f, 0.1f, 0.1f, 1.f} };
	m_pBackText = new Texture{ "Back", "ARLRDBD.TTF", 28,  Color4f{0.1f, 0.1f, 0.1f, 1.f} };

	// Win/Loss Screen Textures
	m_pWinTitleText = new Texture{ "VICTORY!", "ARLRDBD.TTF", 72, Color4f{1.f, 0.8f, 0.f, 1.f} };
	m_pLossTitleText = new Texture{ "GAME OVER", "ARLRDBD.TTF", 72, Color4f{1.f, 0.2f, 0.2f, 1.f} };
	m_pMainMenuBtnText = new Texture{ "Main Menu", "ARLRDBD.TTF", 28, Color4f{0.1f, 0.1f, 0.1f, 1.f} };

	for (int index{ 0 }; index < 10; ++index) {
		m_Digits.emplace_back(new Texture{ std::to_string(index), "ARLRDBD.TTF", 240, Color4f{1.f, 1.f, 1.f, 0.25f} });
	}

	// Help text lines
	Color4f white{ 1.f, 1.f, 1.f, 1.f };
	m_HelpLines.push_back(new Texture("HOW TO PLAY", "ARLRDBD.TTF", 36, Color4f{ 1.f, 1.f, 0.2f, 1.f }));
	m_HelpLines.push_back(new Texture(" ", "ARLRDBD.TTF", 20, white));
	m_HelpLines.push_back(new Texture("Click and drag your character to aim and shoot.", "ARLRDBD.TTF", 24, white));
	m_HelpLines.push_back(new Texture("The yellow dotted line predicts your trajectory.", "ARLRDBD.TTF", 24, white));
	m_HelpLines.push_back(new Texture("Enemies attack automatically, outmaneuver them!", "ARLRDBD.TTF", 24, white));
	m_HelpLines.push_back(new Texture("There is a short cooldown between your shots.", "ARLRDBD.TTF", 24, white));
	m_HelpLines.push_back(new Texture(" ", "ARLRDBD.TTF", 20, white));
	m_HelpLines.push_back(new Texture("POWERUPS:", "ARLRDBD.TTF", 28, Color4f{ 0.5f, 1.f, 0.5f, 1.f }));
	m_HelpLines.push_back(new Texture("Blue Shield: Prevents incoming damage.", "ARLRDBD.TTF", 24, white));
	m_HelpLines.push_back(new Texture("Green Cross: Regenerates your health.", "ARLRDBD.TTF", 24, white));
	m_HelpLines.push_back(new Texture("Orange Bolt: Drastically reduces your shoot cooldown.", "ARLRDBD.TTF", 24, white));
}

void Game::Cleanup()
{
	delete m_pStartText;
	delete m_pHelpBtnText;
	delete m_pQuitBtnText;
	delete m_pResumeText;
	delete m_pBackText;
	delete m_pWinTitleText;
	delete m_pLossTitleText;
	delete m_pMainMenuBtnText;

	for (Texture* pTexture : m_Digits) delete pTexture;
	m_Digits.clear();
	for (Texture* pText : m_HelpLines) delete pText;
	m_HelpLines.clear();
}

void Game::Update(float elapsedSec)
{
	if (m_HasGameStarted && !m_IsPaused) {
		m_CharacterManager.Update(elapsedSec);

		// Check for Game Over Conditions
		if (m_CharacterManager.IsPlayerDead()) {
			m_ShowLossScreen = true;
			m_HasGameStarted = false;
			m_IsPaused = false;
		}
		else if (m_CharacterManager.IsGameOver()) { // Triggered when Boss is defeated
			m_ShowWinScreen = true;
			m_HasGameStarted = false;
			m_IsPaused = false;
		}
	}
}

void Game::Draw() const
{
	ClearBackground();
	if (!m_HasGameStarted) {
		if (m_ShowWinScreen || m_ShowLossScreen) {
			// Draw Title (Win or Loss)
			Texture* pTitle = m_ShowWinScreen ? m_pWinTitleText : m_pLossTitleText;
			Vector2f titlePos{
				Game::GetViewPort().width / 2 - pTitle->GetWidth() / 2.f,
				Game::GetViewPort().height / 2 + 100.f
			};
			pTitle->Draw(titlePos);

			// Draw Main Menu Button
			utils::SetColor(Color4f{ 0.8f, 0.8f, 0.8f, 1.f });
			utils::FillRect(m_MenuBtnBounds);
			utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
			utils::DrawRect(m_MenuBtnBounds, 2.f);

			Vector2f btnTextPos{
				m_MenuBtnBounds.left + (m_MenuBtnBounds.width - m_pMainMenuBtnText->GetWidth()) / 2.f,
				m_MenuBtnBounds.bottom + (m_MenuBtnBounds.height - m_pMainMenuBtnText->GetHeight()) / 2.f
			};
			m_pMainMenuBtnText->Draw(btnTextPos);
		}
		else if (m_ShowingHelp) {
			// Draw Help Text
			float startY = Game::GetViewPort().height - 100.f;
			for (size_t i = 0; i < m_HelpLines.size(); ++i) {
				Vector2f pos{ Game::GetViewPort().width / 2 - m_HelpLines[i]->GetWidth() / 2, startY };
				m_HelpLines[i]->Draw(pos);
				startY -= m_HelpLines[i]->GetHeight() + 10.f;
			}

			// Back Button
			utils::SetColor(Color4f{ 0.7f, 0.7f, 0.7f, 1.f });
			utils::FillRect(m_HelpBackBounds);
			utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
			utils::DrawRect(m_HelpBackBounds, 2.f);
			Vector2f textPos{
				m_HelpBackBounds.left + (m_HelpBackBounds.width - m_pBackText->GetWidth()) / 2.f,
				m_HelpBackBounds.bottom + (m_HelpBackBounds.height - m_pBackText->GetHeight()) / 2.f
			};
			m_pBackText->Draw(textPos);
		}
		else {
			// Main Menu Screen

			// Start Button (Greenish)
			utils::SetColor(Color4f{ 185.f / 255, 1.f, 148.f / 255, 1.f });
			utils::FillRect(m_MainStartBounds);
			utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
			utils::DrawRect(m_MainStartBounds, 2.f);
			m_pStartText->Draw(Vector2f{
				m_MainStartBounds.left + (m_MainStartBounds.width - m_pStartText->GetWidth()) / 2.f,
				m_MainStartBounds.bottom + (m_MainStartBounds.height - m_pStartText->GetHeight()) / 2.f });

			// Help Button (Bluish)
			utils::SetColor(Color4f{ 148.f / 255, 200.f / 255, 1.f, 1.f });
			utils::FillRect(m_MainHelpBounds);
			utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
			utils::DrawRect(m_MainHelpBounds, 2.f);
			m_pHelpBtnText->Draw(Vector2f{
				m_MainHelpBounds.left + (m_MainHelpBounds.width - m_pHelpBtnText->GetWidth()) / 2.f,
				m_MainHelpBounds.bottom + (m_MainHelpBounds.height - m_pHelpBtnText->GetHeight()) / 2.f });

			// Quit Button (Reddish)
			utils::SetColor(Color4f{ 1.f, 0.35f, 0.35f, 1.f });
			utils::FillRect(m_MainQuitBounds);
			utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
			utils::DrawRect(m_MainQuitBounds, 2.f);
			m_pQuitBtnText->Draw(Vector2f{
				m_MainQuitBounds.left + (m_MainQuitBounds.width - m_pQuitBtnText->GetWidth()) / 2.f,
				m_MainQuitBounds.bottom + (m_MainQuitBounds.height - m_pQuitBtnText->GetHeight()) / 2.f });
		}
	}
	else {
		// Playing State
		DrawWaveNumber();
		m_CharacterManager.Draw();
		if (m_IsPaused) DrawPauseMenu();
	}
	utils::SetColor(Color4f{ 68.f / 255, 139.f / 255, 252.f / 255, 1.0f });
	utils::DrawRect(Rectf{ 0.f, 0.f, Game::GetViewPort().width, Game::GetViewPort().height }, 5.f);
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	if (m_HasGameStarted) {
		if (e.keysym.sym == SDLK_ESCAPE) m_IsPaused = !m_IsPaused;
		if (e.keysym.sym == SDLK_g) m_CharacterManager.ToggleGodMode(); // 'G' triggers God Mode
	}
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e) {}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	if (m_HasGameStarted && !m_IsPaused) m_CharacterManager.MouseMoveEvent(static_cast<float>(e.x), static_cast<float>(e.y));
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	if (m_HasGameStarted && !m_IsPaused && e.button == SDL_BUTTON_LEFT) {
		m_CharacterManager.MouseDownEvent(static_cast<float>(e.x), static_cast<float>(e.y));
	}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	if (m_HasGameStarted && m_IsPaused) {
		if (e.button == SDL_BUTTON_LEFT) {
			const Vector2f mousePos{ static_cast<float>(e.x), static_cast<float>(e.y) };
			if (utils::IsPointInRect(mousePos, m_PauseResumeBounds)) m_IsPaused = false;
			else if (utils::IsPointInRect(mousePos, m_PauseQuitBounds)) {
				m_HasGameStarted = false; m_IsPaused = false; m_CharacterManager.Reset();
			}
		}
		return;
	}

	if (m_HasGameStarted && !m_IsPaused) {
		if (e.button == SDL_BUTTON_LEFT) m_CharacterManager.MouseReleaseEvent();
	}
	else if (!m_HasGameStarted && e.button == SDL_BUTTON_LEFT) {
		const Vector2f mousePos{ static_cast<float>(e.x), static_cast<float>(e.y) };

		if (m_ShowWinScreen || m_ShowLossScreen) {
			if (utils::IsPointInRect(mousePos, m_MenuBtnBounds)) {
				m_ShowWinScreen = false;
				m_ShowLossScreen = false;
				m_CharacterManager.Reset(); // Wipe the game board cleanly for the next run
			}
		}
		else if (m_ShowingHelp) {
			if (utils::IsPointInRect(mousePos, m_HelpBackBounds)) m_ShowingHelp = false;
		}
		else {
			if (utils::IsPointInRect(mousePos, m_MainStartBounds)) m_HasGameStarted = true;
			else if (utils::IsPointInRect(mousePos, m_MainHelpBounds)) m_ShowingHelp = true;
			else if (utils::IsPointInRect(mousePos, m_MainQuitBounds)) {
				SDL_Event quitEvent; quitEvent.type = SDL_QUIT; SDL_PushEvent(&quitEvent);
			}
		}
	}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::DrawWaveNumber() const
{
	Rectf gameViewPort{ Game::GetViewPort() };

	const float multi{ 2.5f };
	const int wave{ m_CharacterManager.GetWaveNumber() };
	if (wave < 10) {
		const Texture* pDigitToDraw{ m_Digits.at(wave) };
		pDigitToDraw->Draw(Rectf{
			gameViewPort.width / 2 - pDigitToDraw->GetWidth() * multi / 2,
			gameViewPort.height / 2 - pDigitToDraw->GetHeight() * multi / 2,
			pDigitToDraw->GetWidth() * multi,
			pDigitToDraw->GetHeight() * multi
			});
	}

	const int totalWaves{ m_CharacterManager.GetTotalWaves() };
	const std::string waveLabel{ "Wave " + std::to_string(wave) + " / " + std::to_string(totalWaves) };
	Texture waveLabelTex{ waveLabel, "ARLRDBD.TTF", 22, Color4f{1.f, 1.f, 1.f, 0.85f} };
	waveLabelTex.Draw(Vector2f{ 16.f, gameViewPort.height - 16.f - waveLabelTex.GetHeight() });
}

void Game::DrawPauseMenu() const
{
	const Rectf viewport{ Game::GetViewPort() };

	utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.55f });
	utils::FillRect(Rectf{ 0.f, 0.f, viewport.width, viewport.height });

	const float panelW{ 320.f }; const float panelH{ 240.f };
	const Rectf panel{ viewport.width / 2 - panelW / 2, viewport.height / 2 - panelH / 2, panelW, panelH };
	utils::SetColor(Color4f{ 0.15f, 0.15f, 0.25f, 1.f });
	utils::FillRect(panel);
	utils::SetColor(Color4f{ 68.f / 255, 139.f / 255, 252.f / 255, 1.f });
	utils::DrawRect(panel, 3.f);

	utils::SetColor(Color4f{ 185.f / 255, 1.f, 148.f / 255, 1.f });
	utils::FillRect(m_PauseResumeBounds);
	utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
	utils::DrawRect(m_PauseResumeBounds, 2.f);
	m_pResumeText->Draw(Vector2f{
		m_PauseResumeBounds.left + (m_PauseResumeBounds.width - m_pResumeText->GetWidth()) / 2.f,
		m_PauseResumeBounds.bottom + (m_PauseResumeBounds.height - m_pResumeText->GetHeight()) / 2.f });

	utils::SetColor(Color4f{ 1.f, 0.35f, 0.35f, 1.f });
	utils::FillRect(m_PauseQuitBounds);
	utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 0.3f });
	utils::DrawRect(m_PauseQuitBounds, 2.f);
	m_pQuitBtnText->Draw(Vector2f{
		m_PauseQuitBounds.left + (m_PauseQuitBounds.width - m_pQuitBtnText->GetWidth()) / 2.f,
		m_PauseQuitBounds.bottom + (m_PauseQuitBounds.height - m_pQuitBtnText->GetHeight()) / 2.f });
}
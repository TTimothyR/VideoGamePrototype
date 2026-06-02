// --- START OF FILE Game.h ---
#pragma once
#include "BaseGame.h"
#include "CharacterManager.h"

class Texture;

class Game : public BaseGame
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update(float elapsedSec) override;
	void Draw() const override;

	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:
	void Initialize();
	void Cleanup();
	void ClearBackground() const;
	void DrawWaveNumber() const;
	void DrawPauseMenu() const;

	Texture* m_pStartText{};
	Texture* m_pHelpBtnText{};
	Texture* m_pQuitBtnText{};
	Texture* m_pResumeText{};
	Texture* m_pBackText{};

	// Win/Loss Screen Textures
	Texture* m_pWinTitleText{};
	Texture* m_pLossTitleText{};
	Texture* m_pMainMenuBtnText{};

	std::vector<Texture*> m_Digits{};
	std::vector<Texture*> m_HelpLines{};

	bool m_HasGameStarted{ false };
	bool m_IsPaused{ false };
	bool m_ShowingHelp{ false };

	// Win/Loss State Bools
	bool m_ShowWinScreen{ false };
	bool m_ShowLossScreen{ false };

	const float m_BtnW{ 260.f };
	const float m_BtnH{ 65.f };
	const float m_Spacing{ 20.f };

	// Main Menu Bounds
	const Rectf m_MainStartBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2 + m_BtnH + m_Spacing,
		m_BtnW, m_BtnH
	};
	const Rectf m_MainHelpBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2,
		m_BtnW, m_BtnH
	};
	const Rectf m_MainQuitBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2 - m_BtnH - m_Spacing,
		m_BtnW, m_BtnH
	};

	// Pause Menu Bounds
	const Rectf m_PauseResumeBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2 + 10.f,
		m_BtnW, m_BtnH
	};
	const Rectf m_PauseQuitBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2 - m_BtnH - 10.f,
		m_BtnW, m_BtnH
	};

	// Help Screen Bounds
	const Rectf m_HelpBackBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2 - 200.f,
		m_BtnW, m_BtnH
	};

	// Win/Loss Menu Button Bounds
	const Rectf m_MenuBtnBounds{
		Game::GetViewPort().width / 2 - m_BtnW / 2,
		Game::GetViewPort().height / 2 - m_BtnH,
		m_BtnW, m_BtnH
	};

	CharacterManager m_CharacterManager{ Rectf{0.f, 0.f, Game::GetViewPort().width, Game::GetViewPort().height} };
};
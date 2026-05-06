#pragma once
#include "BaseGame.h"
#include "CharacterManager.h"

class Texture;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
	void DrawWaveNumber() const;

	Texture* m_pGameFont{};
	std::vector<Texture*> m_Digits{};

	bool m_HasGameStarted{ false };

	const float m_StartButtonWidth{ 300.f };
	const float m_StartButtonHeight{ 100.f };
	const Rectf m_StartButtonBounds
	{
		Game::GetViewPort().width / 2 - m_StartButtonWidth / 2,
		Game::GetViewPort().height / 2 - m_StartButtonHeight / 2,
		m_StartButtonWidth,
		m_StartButtonHeight
	};


	const Color4f m_StartButtonColor{ 185.f / 255, 1.f, 148.f / 255, 1.f };

	CharacterManager m_CharacterManager{ Rectf{0.f, 0.f, Game::GetViewPort().width, Game::GetViewPort().height} };
};
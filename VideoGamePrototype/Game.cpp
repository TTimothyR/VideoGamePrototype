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
	m_pGameFont = new Texture{ "Press To Start", "ARLRDBD.TTF", 24, Color4f{77.f / 255,128.f / 255,71.f / 255,1.f} };
	for (int index{ 0 }; index < 10; ++index) {
		m_Digits.emplace_back(new Texture{ std::to_string(index), "ARLRDBD.TTF", 240, Color4f{1.f,1.f,1.f,0.25f} });
	}
}

void Game::Cleanup()
{
	delete m_pGameFont;
	for (Texture* pTexture : m_Digits) {
		delete pTexture;
	}
	m_Digits.clear();
}

void Game::Update(float elapsedSec)
{
	if (m_HasGameStarted) {
		m_CharacterManager.Update(elapsedSec);
		if (m_CharacterManager.IsPlayerDead() || m_CharacterManager.IsGameOver()) {
			m_HasGameStarted = false;
			m_CharacterManager.Reset();
		}
	}
	// Check keyboard state
	//const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	//if ( pStates[SDL_SCANCODE_RIGHT] )
	//{
	//	std::cout << "Right arrow key is down\n";
	//}
	//if ( pStates[SDL_SCANCODE_LEFT] && pStates[SDL_SCANCODE_UP])
	//{
	//	std::cout << "Left and up arrow keys are down\n";
	//}
}

void Game::Draw() const
{
	ClearBackground();
	if (!m_HasGameStarted) {
		utils::SetColor(m_StartButtonColor);
		utils::FillRect(m_StartButtonBounds);
		m_pGameFont->Draw(m_StartButtonBounds);
	}
	else {
		DrawWaveNumber();
		m_CharacterManager.Draw();
	}
	utils::SetColor(Color4f{ 68.f / 255, 139.f / 255, 252.f / 255, 1.0f });
	utils::DrawRect(Rectf{ 0.f, 0.f, Game::GetViewPort().width, Game::GetViewPort().height }, 5.f);
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	if (m_HasGameStarted) {
		m_CharacterManager.MouseMoveEvent(static_cast<float>(e.x), static_cast<float>(e.y));
	}
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	if (m_HasGameStarted) {
		switch (e.button)
		{
		case SDL_BUTTON_LEFT:
			m_CharacterManager.MouseDownEvent(static_cast<float>(e.x), static_cast<float>(e.y));
			break;
		}
	}
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}

}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONUP event: ";
	if (m_HasGameStarted) {
		switch (e.button)
		{
		case SDL_BUTTON_LEFT:
			m_CharacterManager.MouseReleaseEvent();
			break;
		}
	}
	else {
		switch (e.button)
		{
		case SDL_BUTTON_LEFT:
			if (utils::IsPointInRect(Vector2f{ static_cast<float>(e.x), static_cast<float>(e.y) }, m_StartButtonBounds)) {
				m_HasGameStarted = true;
			}
		}
	}
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
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
}
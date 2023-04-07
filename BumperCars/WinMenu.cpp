/////////////////////////////
// Class created by Jackson Wise, Spring 2023
// Created to add a screen when the player wins the game
/////////////////////////////

#include "WinMenu.h"
#include "Game.h"
#include "DialogBox.h"
#include <SDL/SDL.h>
#include "AudioSystem.h"
WinMenu::WinMenu(Game* game)
	:UIScreen(game)
{
	mGame->SetState(Game::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("WinTitle");

	AudioSystem* mAudio = mGame->GetAudioSystem();
	mAudio->PlayEvent("event:/MenuMusic");
	AddButton("RestartButton", [this]() {
		mGame->SetState(Game::EGameplay);
		mGame->Shutdown();
		Game newGame;
		newGame.Initialize();
		newGame.RunLoop();//Caleb bellisle restarts the game by creating a new instance of the game;
		//Close();
		});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
		[this]() {
				mGame->SetState(Game::EQuit);
				mGame->Shutdown();
			});
		});
	
}

WinMenu::~WinMenu()
{
	SetRelativeMouseMode(true);
	mGame->SetState(Game::EGameplay);
}

void WinMenu::HandleKeyPress(int key)
{
	UIScreen::HandleKeyPress(key);

	if (key == SDLK_ESCAPE)
	{
		Close();
	}
}
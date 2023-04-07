/////////////////////////////
// Class created by Jackson Wise, Spring 2023
// Created to add a screen when the player wins the game
/////////////////////////////

#pragma once
#include "UIScreen.h"

class WinMenu : public UIScreen
{
public:
	WinMenu(class Game* game);
	~WinMenu();

	void HandleKeyPress(int key) override;
};
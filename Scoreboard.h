#pragma once
#include "UIScreen.h"
class Scoreboard : public UIScreen
{
public:
	Scoreboard(class Game* game);
	~Scoreboard();
	void Scoreboard::SetScore(int numTagged);
private:
};
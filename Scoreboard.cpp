#include "Scoreboard.h"
#include "Game.h"
#include "DialogBox.h"
#include <SDL/SDL.h>
#include <rapidjson/document.h>
#include <string>
#include "UIScreen.h"

Scoreboard::Scoreboard(Game* game)
	:UIScreen(game, 0.0f, 200.0f)
{

}

Scoreboard::~Scoreboard()
{
}

void Scoreboard::SetScore(int numTagged)
{
	SetTitle(std::to_string(numTagged)+"pts");
}
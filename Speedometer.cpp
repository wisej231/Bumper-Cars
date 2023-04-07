//edited by caleb bellisle

#include "Speedometer.h"
#include "Game.h"
#include "DialogBox.h"
#include <SDL/SDL.h>
#include <rapidjson/document.h>
#include <string>
#include "UIScreen.h"
Speedometer::Speedometer(Game* game)
	:UIScreen(game, 300.0f, 0.0f)
{

	

}



Speedometer::~Speedometer()
{
}


void Speedometer::CalcSpeed(CarActor* car)
{
	int speed1 = std::int64_t(Math::Abs(car->getForwardSpeed() / 26.5));
	SetTitle(std::to_string(speed1));

	
}

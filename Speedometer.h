#pragma once
//edited by caleb bellisle

#pragma once
#include "UIScreen.h"
#include "CarActor.h"
class Speedometer : public UIScreen
{
public:
	Speedometer(class Game* game);
	~Speedometer();

	
	void CalcSpeed(CarActor* car);

private:
	//std::unordered_map<std::string, std::string> mText;

};
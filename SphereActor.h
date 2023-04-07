//////////////////////////
// Class created by Jackson Wise, Spring 2023
// Class was created in order to access collisions between the player and the obstacles
//////////////////////////
#pragma once

#include "Actor.h"
#include "Math.h"

class SphereActor : public Actor
{
public:
	SphereActor(class Game* game, float x, float y, float z, float scale);
	~SphereActor();
	class CircleComponent* GetCircle() { return mCircle; } //Carl McAninch
	void SetVisible(bool visible);
private:
	class MeshComponent* mMeshComp;
	class CircleComponent* mCircle;		//Carl McAninch
};
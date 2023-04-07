// Ai car h file
// Michael Ruff 
// Spring 2023

#pragma once
#include "Actor.h"

class AiCar : public Actor
{
public:
	AiCar(class Game* game);

	//void ActorInput(const uint8_t* keys) override;

	void AiCarMovement();

	void AiSeedSet(int setSeed);
	void Update(float delataTime);
	void SetVisible(bool visible);
private:
	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	const float mAcceleration = 20.0f;
	float mForwardSpeed;
	float mDeathTimer = 0.5f;
	class Mesh* mExplosion;

	// seed decides spawn location and movement of the car
	int seed;
	Vector3 spawn[3] = { Vector3(1000.0f, 1000.0f, -100.0f), Vector3(10000,2000,-100), Vector3(4000,9000,-100) };
	float targetX[3][10] = { {1000,2000,8000,7000,4500,4000,3000,2000,2000,1000},
						   {5000,4000,6000,5000,9000,3000,8000,9000,4000,10000},
						   {500,1000,2000,2500,1000,2000,1000,1000,1000,4000} };

	float targetY[3][10] = { {8000,10000,9000,2000,2000,1000,1500,2000,500,1000},
						   {8000,5000,4000,1000,500,8000,9000,7500,5000,2000},
						   {8000,1000,500,2000,500,1000,1000,1000,1000,9000} };

	Vector3 UnitZ = Vector3(0.0f, 0.0f, 1.0f);

	int currentTarget = 0;

	class CircleComponent* mCircle;	// Carl McANinch
	bool firstHit = true; // Jackson Wise - Checking if the car has been hit
};

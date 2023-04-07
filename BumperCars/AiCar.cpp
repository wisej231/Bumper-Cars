// Ai car cpp file
// Michael Ruff 
// Spring 2023

#include "AiCar.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "MoveComponent.h"
#include "CircleComponent.h"
#include <math.h>
AiCar::AiCar(Game* game)
	:Actor(game)
{
	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/aiCar.gpmesh"));
	SetPosition(Vector3(0,0,0));

	mCircle = new CircleComponent(this);	//carl Mcaninch
	mCircle->SetRadius(100.0f);

	mMoveComp = new MoveComponent(this);
	mMoveComp->SetForwardSpeed(2000);

	mExplosion = game->GetRenderer()->GetMesh("Assets/Explosion.gpmesh");
}

void AiCar::AiCarMovement()
{
	float targetYtemp = targetY[seed][currentTarget];
	float currentY = GetPosition().y;
	float targetXtemp = targetX[seed][currentTarget];
	float currentX = GetPosition().x;

	if (currentY > targetYtemp - 100 && currentY <  targetYtemp + 100 && currentX > targetXtemp - 100 && currentX < targetXtemp + 100)
	{
		if (currentTarget == 9)
			currentTarget = 0;
		else
			currentTarget++;
		targetYtemp = targetY[seed][currentTarget];
		targetXtemp = targetX[seed][currentTarget];
	}
	float angle = Math::Atan2(targetYtemp - currentY, targetXtemp - currentX);
	Quaternion q(UnitZ, angle);
	SetRotation(q);
	
}

void AiCar::Update(float deltaTime) //Carl McAninch
{
	if (Intersect(*(GetGame()->GetPlayerCar().GetCircle()), *mCircle))
	{
		Vector3 pos = GetPosition();
		mMoveComp->SetAngularSpeed(0);
		mMoveComp->SetForwardSpeed(0);
		mMoveComp->SetStrafeSpeed(0);

		mMeshComp->SetMesh(mExplosion);
		SetScale(20);

		mDeathTimer -= deltaTime;
		if (mDeathTimer <= 0)
		{
			mMeshComp->SetVisible(false);
		}


		//Jackson Wise - Adding only one point if the target is destroyed
		if (firstHit)
		{
			GetGame()->AddPoint();
			firstHit = false;
		}

		//mMeshComp->SetVisible(false);
	}
}

// Sets the seed and spawn location of the car
void AiCar::AiSeedSet(int setSeed)
{
	seed = setSeed;
	SetPosition(spawn[seed]);
}

/*
void AiCar::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;

	float maxSpeed = 1600.0f;

	// wasd movement
	if (keys[SDL_SCANCODE_W])
	{
		if (mForwardSpeed < maxSpeed)
			mForwardSpeed += mAcceleration;
	}
	if (keys[SDL_SCANCODE_S])
	{
		if (mForwardSpeed > -maxSpeed / 2)
			mForwardSpeed -= mAcceleration;
	}
	if (keys[SDL_SCANCODE_A])
	{
		angularSpeed -= Math::Pi;
		if (keys[SDL_SCANCODE_LSHIFT])
			angularSpeed *= 1.5;
	}
	if (keys[SDL_SCANCODE_D])
	{
		angularSpeed += Math::Pi;
		if (keys[SDL_SCANCODE_LSHIFT])
			angularSpeed *= 1.5;
	}

	// JCW - Gradually slows down if nothing is being done
	else
	{
		if (mForwardSpeed > 0)
			mForwardSpeed -= mAcceleration / 2;
	}

	mMoveComp->SetForwardSpeed(mForwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);

}
*/

void AiCar::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

////////////////////
// Edited by Jackson Wise, Spring 2023
// Edited to fit the parameters of the game X-TREME BUMPER CARS
// Added drift mechanics, boost mechanics, and more realistic car physics
////////////////////

#include "CarActor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "FollowCamera.h"
#include "MoveComponent.h"
#include "AiCar.h"
#include "CircleComponent.h"
#include "SphereActor.h"

CarActor::CarActor(Game* game)
	:Actor(game)
{
	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/playerCar.gpmesh"));
	SetPosition(Vector3(0.0f, 0.0f, -100.0f));
	
	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(100.0f);
}

void CarActor::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;

	// Jackson Wise - Different speed settings
	const float slowAccel = 1600.0f;
	const float maxSpeed = 2140.0f;
	const float maxBoost = 2650.0f;

	// wasd movement
	if (keys[SDL_SCANCODE_W])
	{
		// Jackson Wise - Setting up max speed
		if (mForwardSpeed < slowAccel)
			mForwardSpeed += mAcceleration;
		else if (mForwardSpeed <= maxSpeed)
			mForwardSpeed += mAcceleration / 10.0f;

		// Jackson Wise - Boost
		if (keys[SDL_SCANCODE_SPACE] && mForwardSpeed <= maxBoost && mBoostCooldown <= 0.0f)
		{
			// Jackson Wise - If there is boost to use
			if (mBoostDuration > 0.0f)
			{
				mForwardSpeed += mAcceleration * 2;
				mBoostDuration -= 0.1f;
			}

			// Jackson Wise - Once the boost is used up
			else if (mBoostDuration <= 0.0f)
			{
				mBoostCooldown = 2.0f; // JCW - Time until next boost (2 seconds)
			}
		}
		else if (mForwardSpeed >= maxSpeed)
		{
			mForwardSpeed -= mAcceleration;
		}
	}
	else if (keys[SDL_SCANCODE_S])
	{
		// Jackson Wise
		if (mForwardSpeed > -maxSpeed/2)
			mForwardSpeed -= mAcceleration;
	}
	// Jackson Wise - Gradually slows down if nothing is being done
	else 
	{
		if (mForwardSpeed > 0)
			mForwardSpeed -= mAcceleration / 2;
		else if (mForwardSpeed < 0)
			mForwardSpeed += mAcceleration / 2;
	}

	if (keys[SDL_SCANCODE_A] && mForwardSpeed != 0.0f)
	{
		angularSpeed -= Math::Pi;

		// Jackson Wise - Drift Mechanics
		if (keys[SDL_SCANCODE_LSHIFT])
		{
			angularSpeed *= 1.5;
			mForwardSpeed -= mAcceleration;
		}
	}
	if (keys[SDL_SCANCODE_D] && mForwardSpeed != 0.0f)
	{
		angularSpeed += Math::Pi;

		// Jackson Wise - Drift Mechanics
		if (keys[SDL_SCANCODE_LSHIFT])
		{
			angularSpeed *= 1.5;
			mForwardSpeed -= mAcceleration;
		}
	} 


	mMoveComp->SetForwardSpeed(mForwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);

	// Adjust horizontal distance of camera based on speed
	if (!Math::NearZero(forwardSpeed))
	{
		mCameraComp->SetHorzDist(500.0f);
	}
	else
	{
		mCameraComp->SetHorzDist(350.0f);
	}

	
}

void CarActor::UpdateActor(float deltaTime)
{
	// Jackson Wise - Updating boost duration and boost cooldown as time progresses
	if (mBoostDuration <= 6.0f)
		mBoostDuration += deltaTime; 
	mBoostCooldown -= deltaTime;
	
	// Jackson Wise - Collisions with the obstacles
	for (auto sphere : GetGame()->GetSpheres())
	{
		if (Intersect(*mCircle, *sphere->GetCircle()))
		{
			mForwardSpeed = 0.0f;
		}
	}

}

void CarActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

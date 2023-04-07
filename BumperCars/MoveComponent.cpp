// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
:Component(owner, updateOrder)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mOwner->GetRotation();
		float angle = mAngularSpeed * deltaTime;
		// Create quaternion for incremental rotation
		// (Rotate about up axis)
		Quaternion inc(Vector3::UnitZ, angle);
		// Concatenate old and new quaternion
		rot = Quaternion::Concatenate(rot, inc);
		mOwner->SetRotation(rot);
	}
	
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
		mOwner->SetPosition(pos);
	}




	float start = -1250.0f;
	float size = 500.0f;
	const int ySize = 25;
	const int xSize = 25;
	int roomSize = size * 19;

	Vector3 pos = mOwner->GetPosition();		//Carl McAninch wall collisions
	for (int i = 0; i < 25; i++)
	{
		if (pos.y < start - size + 100.0f)
		{
			mOwner->SetPosition(Vector3(pos.x, start - size + 100.0f, pos.z));
		}
		if (pos.y > -start + size + roomSize - 100.0f)
		{
			mOwner->SetPosition(Vector3(pos.x, -start + size + roomSize - 100.0f, pos.z));
		}
	}

	for (int i = 0; i < 25; i++)
	{
		if (pos.x > -start + size + roomSize - 100.0f)
		{
			mOwner->SetPosition(Vector3(-start + size + roomSize - 100.0f, pos.y, pos.z));
		}
		if (pos.x < start - size + 100.0f)
		{
			mOwner->SetPosition(Vector3(start - size + 100.0f, pos.y, pos.z));
		}
	}
}

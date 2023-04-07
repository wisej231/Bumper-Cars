//////////////////////////
// Class created by Jackson Wise, Spring 2023
// Class was created in order to access collisions between the player and the obstacles
//////////////////////////

#include "SphereActor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "CircleComponent.h"

SphereActor::SphereActor(Game* game, float x, float y, float z, float scale)
	:Actor(game)
{	
	this->SetPosition(Vector3(x, y, z));
	this->SetScale(scale);

	mCircle = new CircleComponent(this);
	mCircle->SetRadius(scale+1.0f);

	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));

	game->AddSphere(this);
}


void SphereActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

SphereActor::~SphereActor()
{
	GetGame()->AddSphere(this);
}
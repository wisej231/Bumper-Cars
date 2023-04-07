// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <algorithm>
#include "Renderer.h"
#include "AudioSystem.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "WallActor.h"	// Jackson Wise
#include "FloorActor.h"	// Jackson Wise
#include "AiCar.h" //Michael Ruff
#include "SkyBox.h"
#include "AudioComponent.h"
#include "CarActor.h"
#include "OrbitActor.h"
#include "SplineActor.h"
#include "UIScreen.h" //CB		//Caleb Bellisle
#include "Font.h"//CB		//Caleb Bellisle
#include "PauseMenu.h"//CB		//Caleb Bellisle
#include "StartMenu.h"//CB		//Caleb Bellisle
#include "WinMenu.h" // Jackson Wise
#include "SphereActor.h" // Jackson Wise
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <fstream>
#include <sstream>		//Caleb Bellisle
#include <rapidjson/document.h>		//Caleb Bellisle
#include "Speedometer.h"		//Caleb Bellisle
#include "Scoreboard.h" // Jackson Wise
Game::Game()
:mRenderer(nullptr)
,mAudioSystem(nullptr)
,mGameState(EStart)
,mUpdatingActors(false)
{
	
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create the renderer
	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(960.0f, 540.0f))
	{
		SDL_Log("Failed to initialize renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	// Create the audio system
	mAudioSystem = new AudioSystem(this);
	if (!mAudioSystem->Initialize())
	{
		SDL_Log("Failed to initialize audio system");
		mAudioSystem->Shutdown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
		return false;
	}

	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_TTF");
		return false;
	}

	LoadData();
	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::RunLoop()
{
	static bool openMenu = false;
	new StartMenu(this);		//Caleb Bellisle
	while (mGameState != EQuit)
	{

		ProcessInput();
		UpdateGame();
		GenerateOutput();
		if (mGameState == EWin && !openMenu)
		{
			new WinMenu(this);
			openMenu = true;
		}

		


	}
	new WinMenu(this);

}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mGameState = EQuit;		//Caleb Bellisle
			break;
			// This fires when a key's initially pressed
		case SDL_KEYDOWN:
			if (!event.key.repeat)
			{
				if (mGameState == EGameplay)
				{
					HandleKeyPress(event.key.keysym.sym);
				}
			/*	else if (mGameState == EWin)
				{
					HandleKeyPress(event.key.keysym.sym);
				}*/
				else if (!mUIStack.empty())
				{
					mUIStack.back()->
						HandleKeyPress(event.key.keysym.sym);
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (mGameState == EGameplay)
			{
				HandleKeyPress(event.button.button);
			}
			else if (!mUIStack.empty())
			{
				mUIStack.back()->
					HandleKeyPress(event.button.button);
			}
			break;
		default:
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (mGameState == EGameplay)
	{
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EActive)
			{
				actor->ProcessInput(state);
			}
		}
	}
	else if (!mUIStack.empty())
	{
		mUIStack.back()->ProcessInput(state);
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::HandleKeyPress(int key)
{
	switch (key)
	{
	case SDLK_ESCAPE:
		// Create pause menu
		//new PauseMenu(this);
		new PauseMenu(this);		//Caleb Bellisle
		break;
	case SDLK_q:
		this->SetState(EQuit);
		break;
	case '-':
	{
		// Reduce master volume
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Max(0.0f, volume - 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case '=':
	{
		// Increase master volume
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Min(1.0f, volume + 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	default:
		break;
	}
}

void Game::UpdateGame()
{
	// Michael Ruff Make Car Move
	mAiCarOne->AiCarMovement();
	mAiCarTwo->AiCarMovement();
	mAiCarThree->AiCarMovement();

	//mMusicEvent = mAudioSystem->PlayEvent("event:/MenuMusic");

	float speed = mCarActor->getForwardSpeed()/ 26.5;		//Caleb Bellisle
	// Jackson Wise - Update if you won
	if (mScore == numAiCars)
	{
		mMusicEvent.SetPitch(1);

		mMusicEvent.SetPaused(true);
		if(mMusicEvent.GetPaused() && mScore == numAiCars)
			mMusicEvent = mAudioSystem->PlayEvent("event:/WinEvent");
		mScore++;
		mGameState = EWin;
		

	}

	//mMusicEvent = mAudioSystem->PlayEvent("event:/Idlesound");
	//mMusicEvent.SetVolume(0.75);

	if(speed == 0.0f && mGameState == EGameplay)	//IDLE CAR MUSIC/
		if (mMusicEvent.GetPaused())
			mMusicEvent = mAudioSystem->PlayEvent("event:/IdleSound");

		//mMusicEvent.SetPaused(true);
	if(speed >= 0.0001f && speed <= 2.5f)
		mMusicEvent.SetPaused(true);

	if (abs(speed) > 2.5f)// we know we are moving less than 60
	{
		mMusicEvent.SetVolume(1);

		mMusicEvent.SetPitch(speed / 100 +1.3);
		if (mMusicEvent.GetPaused())
		{
			mMusicEvent = mAudioSystem->PlayEvent("event:/IdleSound");
			mMusicEvent = mAudioSystem->PlayEvent("event:/Explosion");

			//mMusicEvent = mAudioSystem->PlayEvent("event:/40mph");

		}
	}

	
	//if (abs(speed) >= 59.0f)
	//{

	//	//mMusicEvent.SetPaused(true);
	//	
	//	
	//}
	//if(abs(speed) > 61.0f)
	//{
	//	if (mMusicEvent.GetPaused())
	//	{
	//		//mMusicEvent = mAudioSystem->PlayEvent("event:/IdleSound");

	//		//mMusicEvent = mAudioSystem->PlayEvent("event:/Idlesound");

	//	}
	//}




	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;


	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	mAiCarOne->Update(deltaTime);//Carl McAninch
	mAiCarTwo->Update(deltaTime);
	mAiCarThree->Update(deltaTime);

	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	if (mGameState == EGameplay)
	{
		//mAudioSystem->Shutdown();
		//mMusicEvent.SetPaused(true);

		mSpeedometer->CalcSpeed(mCarActor);
		mScoreboard->SetScore(mScore); // Jackson Wise - setting the scoreboard to the score

		static int tempScore = 0;
		if (mScore > tempScore)
		{
			mMusicEvent = mAudioSystem->PlayEvent("event:/Explosion");
			mMusicEvent.SetPaused(true);
		}

		tempScore = mScore;

		


		// Update all actors
		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			actor->Update(deltaTime);
		}
		mUpdatingActors = false;

		// Move any pending actors to mActors
		for (auto pending : mPendingActors)
		{
			pending->ComputeWorldTransform();
			mActors.emplace_back(pending);
		}
		mPendingActors.clear();

		// Add any dead actors to a temp vector
		std::vector<Actor*> deadActors;
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EDead)
			{
				deadActors.emplace_back(actor);
			}
		}
		// Delete dead actors (which removes them from mActors)
		for (auto actor : deadActors)
		{
			delete actor;
		}
	}


	// Update UI screens		//Caleb Bellisle
	for (auto ui : mUIStack)
	{
		if (ui->GetState() == UIScreen::EActive)
		{
			ui->Update(deltaTime);
		}
	}
	// Delete any UIScreens that are closed		//Caleb Bellisle
	auto iter = mUIStack.begin();
	while (iter != mUIStack.end())
	{
		if ((*iter)->GetState() == UIScreen::EClosing)
		{
			delete* iter;
			iter = mUIStack.erase(iter);
		}
		else
		{
			++iter;
		}
	} 
	// Update audio system
	mAudioSystem->Update(deltaTime);

	//mAudioSystem->SetBusPaused("event:/MenuMusic", true);
	// Update Skybox
	mySkyBox->SetPosition(mCarActor->GetPosition());
	

}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	
	LoadText("Assets/English.gptext");		//Caleb Bellisle

	// Loads obstacles into game			//Julian Powell
	Actor* a = new Actor(this);		
	Quaternion q(Vector3::UnitY, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.0f));
	MeshComponent* mc = new MeshComponent(a);
	a->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	a->SetScale(0.0f);
	a->SetRotation(q);
	mc->SetMesh(mRenderer->GetMesh("Assets/Cube.gpmesh"));

	float boxCoordsX[] = { 3000,800,1700,2200,800, 2000,3000, 4000, 200, 2500 };
	float boxCoordsY[] = { 4000, 2000, 100, 2500, 3500, 1800, 3000, 4000, 600, 3400 };
	float boxCoordsZ = -50.0f;
	float boxSizes[] = { 200.0f, 300.0f, 250.0f, 300.0f, 180.0f, 200.0f, 150.0f, 300.0f, 280.0f, 160.0f };

	/*
	for (int i = 0; i < 10; ++i) {
		a = new SphereActor(this);
		a->SetPosition(Vector3(boxCoordsX[i], boxCoordsY[i], boxCoordsZ));
		a->SetScale(boxSizes[i]);
		mc = new MeshComponent(a);
		mc->SetMesh(mRenderer->GetMesh("Assets/Cube.gpmesh"));
	}
	*/

	float rockCoordsX[] = { 4000,10000,7000,2000,2500,1800,3000, 4000, 600, 3400, 7000, 9000,
		10000, 2000, 4000, 2000, 3000, 11000, 7000, 12000,30000, 2000, 10000,4000 };
	float rockCoordsY[] = { 2000,4000,1200,2700,1000, 1700,2000, 4000, 1000, 2000,5000,7000,
		9000,6000,10000,11000,3000, 2000, 10000,4000, 6000, 3400, 7000 };
	float rockCoordsZ = -100.0f;
	float rockSizes[] = { 8.0f, 15.0f, 10.0f, 15.0f, 8.0f, 14.0f, 9.0f, 13.0f, 8.0f, 6.0f, 
		8.0f, 5.0f, 10.0f, 13.0f, 8.0f, 14.0f, 9.0f, 13.0f, 8.0f, 6.0f, 15.0f, 8.0f, 14.0f, 9.0f };

	// Adding the obstacles
	for (int i = 0; i < 20; ++i) {
		new SphereActor(this, rockCoordsX[i], rockCoordsY[i], rockCoordsZ, rockSizes[i]);
	}

	// Setup floor
	float start = -1250.0f;
	float size = 500.0f;
	const int ySize = 25;
	const int xSize = 25;

	for (int i = 0; i < ySize; i++)
	{
		for (int j = 0; j < xSize; j++)
		{
			a = new FloorActor(this);
			a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	int roomSize = size*19;

	// Left/right walls
	q = Quaternion(Vector3::UnitX, Math::PiOver2);
	for (int i = 0; i < 25; i++)
	{
		a = new WallActor(this); // Jackson Wise
		a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
		a->SetRotation(q);
		
		a = new WallActor(this); // Jackson Wise
		a->SetPosition(Vector3(start + i * size, -start + size + roomSize, 0.0f));
		a->SetRotation(q);
	}

	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	// Forward/back walls
	for (int i = 0; i < 25; i++)
	{
		a = new WallActor(this); // Jackson Wise
		a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
		a->SetRotation(q);

		a = new WallActor(this); // Jackson Wise
		a->SetPosition(Vector3(-start + size + roomSize, start + i * size, 0.0f));
		a->SetRotation(q);
	}

	// Setup lights
	mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
	DirectionalLight& dir = mRenderer->GetDirectionalLight();
	dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
	dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
	dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

	// UI elements CAleb bellisle
	static bool firstrun = false;

	if (firstrun)
	{
		a = new Actor(this);
		a->SetPosition(Vector3(-740.0f, -500.0f, 0.0f));
		a->SetScale(1.5f);
		SpriteComponent* sc = new SpriteComponent(a);
		//sc->SetTexture(mRenderer->GetTexture("Assets/HealthBar.png"));
		firstrun = true;

	}
	mSpeedometer = new Speedometer(this);

	// Jackson Wise - Making the scoreboard
	mScoreboard = new Scoreboard(this);

	// Start music
	mMusicEvent = mAudioSystem->PlayEvent("event:/StartMusic");

	// Enable relative mouse mode for camera look
	SDL_SetRelativeMouseMode(SDL_TRUE);
	// Make an initial call to get relative to clear out
	SDL_GetRelativeMouseState(nullptr, nullptr);

	// Different camera actors
	mCarActor = new CarActor(this);
	mOrbitActor = new OrbitActor(this);
	mSplineActor = new SplineActor(this);

	// create aicar Michael Ruff
	mAiCarOne = new AiCar(this);
	mAiCarTwo = new AiCar(this);
	mAiCarThree = new AiCar(this);
	mAiCarOne->AiSeedSet(0);
	mAiCarTwo->AiSeedSet(1);
	mAiCarThree->AiSeedSet(2);

	//if(mGameState != EStart)
	// create skybox
	mySkyBox = new SkyBox(this);

	// Michae Ruff Set Camera 
	mCarActor->SetState(Actor::EPaused);
	mCarActor->SetVisible(false);
	mOrbitActor->SetState(Actor::EPaused);
	mOrbitActor->SetVisible(false);
	mSplineActor->SetState(Actor::EPaused);

	mCarActor->SetState(Actor::EActive);
	mCarActor->SetVisible(true);



	// Spheres for demonstrating unprojection
	mStartSphere = new Actor(this);
	mStartSphere->SetPosition(Vector3(10000.0f, 0.0f, 0.0f));
	mStartSphere->SetScale(0.25f);
	mc = new MeshComponent(mStartSphere);
	mc->SetMesh(mRenderer->GetMesh("Assets/Sphere.gpmesh"));
	mEndSphere = new Actor(this);
	mEndSphere->SetPosition(Vector3(10000.0f, 0.0f, 0.0f));
	mEndSphere->SetScale(0.25f);
	mc = new MeshComponent(mEndSphere);
	mc->SetMesh(mRenderer->GetMesh("Assets/Sphere.gpmesh"));
	mc->SetTextureIndex(1);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	while (!mUIStack.empty())
	{
		delete mUIStack.back();
		mUIStack.pop_back();
	}

	if (mRenderer)
	{
		mRenderer->UnloadData();
	}
}

void Game::Shutdown()
{
	UnloadData();
	if (mRenderer)
	{
		mRenderer->Shutdown();
	}
	if (mAudioSystem)
	{
		mAudioSystem->Shutdown();
	}
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}


void Game::PushUI(UIScreen* screen)
{
	mUIStack.emplace_back(screen);
}
Font* Game::GetFont(const std::string& fileName)
{
	auto iter = mFonts.find(fileName);
	if (iter != mFonts.end())
	{
		return iter->second;
	}
	else
	{
		Font* font = new Font(this);
		if (font->Load(fileName))
		{
			mFonts.emplace(fileName, font);
		}
		else
		{
			font->Unload();
			delete font;
			font = nullptr;
		}
		return font;
	}
}

void Game::LoadText(const std::string& fileName)
{
	// Clear the existing map, if already loaded
	mText.clear();
	// Try to open the file
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Text file %s not found", fileName.c_str());
		return;
	}
	// Read the entire file to a string stream
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	// Open this file in rapidJSON
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);
	if (!doc.IsObject())
	{
		SDL_Log("Text file %s is not valid JSON", fileName.c_str());
		return;
	}
	// Parse the text map
	const rapidjson::Value& actions = doc["TextMap"];
	for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
		itr != actions.MemberEnd(); ++itr)
	{
		if (itr->name.IsString() && itr->value.IsString())
		{
			mText.emplace(itr->name.GetString(),
				itr->value.GetString());
		}
	}
}

const std::string& Game::GetText(const std::string& key)
{
	static std::string errorMsg("**KEY NOT FOUND**");
	// Find this text in the map, if it exists
	auto iter = mText.find(key);
	if (iter != mText.end())
	{
		return iter->second;
	}
	else
	{
		return errorMsg;
	}
}

// Jackson Wise
void Game::AddSphere(SphereActor* sphere)
{
	mSphereActors.emplace_back(sphere);
}
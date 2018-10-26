#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleInput.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball = NULL;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	LOG("Loading ball");

	ball = App->textures->Load("pinball/Sprites.png");
	circle = App->physics->CreateCircle(800, 200, 12);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	
	
	//All draw functions
	SDL_Rect ball_rect = { 245, 165, 43, 42 };

	int x, y;
	circle->GetPosition(x, y);

	App->renderer->Blit(ball, x, y, &ball_rect);

	return UPDATE_CONTINUE;
}




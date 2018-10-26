#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
//#include "ModuleUI.h"



ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball_tex = flipper_tex = NULL;
	lifes = 5;
	init_position.x = PIXEL_TO_METERS(1062);
	init_position.y = PIXEL_TO_METERS(1000);
	impulse_force = 0.0f;
	launcher_init_pos.x = 1062.f;
	launcher_init_pos.y = 1050.f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	// Textures ======================================================
	ball_tex = App->textures->Load("textures/Sprites.png");
	flipper_tex = App->textures->Load("textures/Sprites.png");
	//flipper_tex = App->textures->Load("textures/Sprites.png");
	flipper_rect = { 0,86,92,42 };
	flipper_rect_r = { 115,86,92,42 };
	// PhysBodies ====================================================
	int left_flipper[8]
	{
		0,  12,
		0 ,-12, 
		70,  7, 
		70 ,-7
	};

	int right_flipper[8]
	{
		0,  12,
		0 ,-12,
		-70,  7,
		-70 ,-7
	};
	
	ball = App->physics->CreateCircle(1062, 1000, 11);
	ball->body->SetBullet(true);
	ball->listener = this;

	flipper_l = App->physics->CreateFlipper(b2Vec2(165+400, 918+110), left_flipper, 8, b2Vec2(133+400, 918+110), -30 , 40 , flipper_l_joint);
	flipper_r = App->physics->CreateFlipper(b2Vec2(293+353+80, 918+120), right_flipper, 8, b2Vec2(325+353+80, 918+120), -40, 30, flipper_r_joint);
 	launcher = App->physics->CreateLauncher(launcher_init_pos.x, launcher_init_pos.y, 33, 33, launcher_joint);

	flipper_fx = App->audio->LoadFx("sfx/flipper.wav");
	kicker_fx = App->audio->LoadFx("sfx/kicker.wav");
	lose_fx = App->audio->LoadFx("sfx/negative_beeps.wav");
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	App->textures->Unload(ball_tex);
	App->textures->Unload(flipper_tex);
	return true;
}

// Update
update_status ModulePlayer::Update()
{
	// Prepare for raycast =====================================================

	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();

	if (reset_pos)
	{
		Reset();
		reset_pos = false;
	}

	if (reset_all)
	{
		Reset();
		lifes = 5;
		//App->ui->current_score = 0;
		//App->ui->multiplier = 1;
		reset_all = false;
	}
	// Game ============================================================
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->player->is_dead == true)
	{
		reset_all = true;
		App->scene_intro->reset = true;
		App->scene_intro->reset_combo = true;
		is_dead = false;
	}


	// Ball =============================================================
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN )
	{
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY())), 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && ball)
	{
		Reset();
	}

	// Flippers =============================================================
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		engageFlipper(flipper_l, -10.0f);
		//App->audio->PlayFx(flipper_fx);
	}


	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		engageFlipper(flipper_r, 10.0f);
	//	App->audio->PlayFx(flipper_fx);
	}

	// Launcher ==============================================================
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
	{
		launcher_joint->SetMotorSpeed(-2);
		impulse_force += 1.2f;

		if (impulse_force > 60)
		{
			impulse_force = 60;
		}
	//	App->audio->PlayFx(kicker_fx);
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) 
	{
		launcher_joint->SetMotorSpeed(impulse_force);
		impulse_force = 0; //Sfx
	}

	//--------Launcher----------------------------------------------
	int x, y;
	launcher->GetPosition(x, y);
	//SDL_Rect rect = { x ,  y, 33, 33 };
	//App->renderer->DrawQuad(rect, 255, 255, 255);

	// -------Flippers----------------------------------------------
	flipper_l->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x , y - 12, &flipper_rect, 1.0f, flipper_l->GetRotation(), 8,14);
	flipper_r->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x + 8-100, y , &flipper_rect_r, 1.0f, flipper_r->GetRotation(), 30, 0);
	//App->renderer->Blit(flipper_tex, x, y, &rect);

	//--------Ball--------------------------------------------------
	if (ball != nullptr)
	{
		int x, y;
		ball->GetPosition(x, y);
		SDL_Rect rect = { 245, 165, 43, 42 };
		App->renderer->Blit(ball_tex, x, y, &rect);
	}



	return UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact)
{
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	// Slingshots ======================================================
	if (ball == bodyA && App->scene_intro->slingshots.find(bodyB) != -1)
	{
		bodyB->activated = true;
		// Change bonus ==================================
		//App->scene_intro->current_bonus = (Bonuses)((int)App->scene_intro->current_bonus + 1);
	//	if (App->scene_intro->current_bonus == Bonuses::max) {
		//	App->scene_intro->current_bonus = Bonuses::extra_ball;
		//}
		// Add points ====================================
		//App->ui->AddPoints(1000);
		// Sfx ===========================================
		//App->audio->PlayFx(App->scene_intro->bonus_fx);
		// Aplly impulse =================================
		b2Vec2 normal = worldManifold.normal;
		normal.x *= 1.5f;
		normal.y *= 0.2f;
		ball->body->SetLinearVelocity({ 0.0f ,ball->body->GetLinearVelocity().y * 0.2f });
		ball->body->ApplyLinearImpulse(normal, ball->body->GetWorldCenter(), true);
	}
	//  Bumpers ======================================================
	else if (ball == bodyA && App->scene_intro->bumpers.find(bodyB) != -1)
	{
		bodyB->activated = true;
		// Add points ================================
		//App->ui->AddPoints(500);
		// Sfx =======================================
		//App->audio->PlayFx(App->scene_intro->bonus_fx);
		// Aplly impulse =============================
		int random = rand() % 3;
		b2Vec2 normal = worldManifold.normal;
		switch (random)
		{
		case 0:
			normal.y *= 0.8f;
			break;
		case 1:
			normal.y *= 1.5f;
			break;
		case 2:
			normal.y *= 1.2f;
			break;
		}
		normal.x *= 1.8f;
		ball->body->SetLinearVelocity({ 0.0f, 0.0f });
		ball->body->ApplyLinearImpulse(normal, ball->body->GetWorldCenter(), true);
	}
	// Death sensor ======================================================
	else if (ball == bodyA && App->scene_intro->sensor_death == bodyB)
	{
		// Lifes logic ===============================
		--lifes;
		if (lifes <= 0) {
			lifes = 0;
			is_dead = true;
			App->audio->PlayFx(lose_fx);
		}
		else
		// Set ball ==================================
		{
			// Sfx =======================================
			App->audio->PlayFx(App->scene_intro->bonus_fx); //lose_fx
			reset_pos = true;
		}
	}
}

void ModulePlayer::engageFlipper(PhysBody *flipper, float impulse)
{
	if (flipper)
	{
		flipper->body->ApplyAngularImpulse(impulse, true);
	}
}

void ModulePlayer::Reset ()
{
	if (ball)
	{
		ball->body->SetLinearVelocity({ 0,0 });
		ball->body->SetTransform( init_position, 0);
		App->scene_intro->reset = true;
	}
}

#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	Bg = App->textures->Load("pinball/Pinball_Start.png");
	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50, this);


	int Cruz[18] = {
		362, 439,
		340, 383,
		335, 340,
		337, 259,
		367, 197,
		413, 156,
		472, 134,
		524, 251,
		524, 380
	};

	App->physics->CreateChain(0, 0, Cruz, 18);

	int fireh[10] = {
		531, 438,
		540, 446,
		540, 469,
		409, 525,
		390, 487
	};

	int firev[14] = {
		917, 200,
		917, 315,
		921, 324,
		928, 324,
		940, 274,
		940, 229,
		925, 198
	};

	App->physics->CreateChain(0, 0, firev, 14);


	int Triangle_left[6] = {
		397, 744,
		396, 867,
		528, 954
	};

	App->physics->CreateChain(0, 0, Triangle_left, 6);

	int Triangle_right[8] = {
		760, 950,
		884, 742,
		892, 743,
		892, 872
	};

	App->physics->CreateChain(0, 0, Triangle_right, 8);

	int carril_I[12] = {
		344, 744,
		348, 895,
		533, 1020,
		528, 1028,
		336, 902,
		336, 742
	};

	App->physics->CreateChain(0, 0, carril_I, 12);

	int carril_D[12] = {
		945, 743,
		942, 895,
		753, 1022,
		762, 1030,
		955, 905,
		954, 743
	};

	App->physics->CreateChain(0, 0, carril_D, 12);

	int Barra_1[8] = {
		703, 156,
		702, 252,
		717, 251,
		717, 156
	};

	App->physics->CreateChain(0, 0, Barra_1, 8);

	int Barra_2[8] = {
		767, 169,
		770, 265,
		782, 263,
		782, 170
	};

	App->physics->CreateChain(0, 0, Barra_2, 8);

	int Barra_3[8] = {
		833, 181,
		834, 277,
		847, 277,
		848, 182
	};

	App->physics->CreateChain(0, 0, Barra_3, 8);

	int Barra_4[8] = {
		913, 200,
		896, 201,
		896, 297,
		912, 298
	};

	App->physics->CreateChain(0, 0, Barra_4, 8);


	int Borde[180] = {
		1034, 1039,
		1084, 1039,
		1083, 213,
		1071, 170,
		1041, 125,
		1004, 95,
		939, 74,
		496, 73,
		428, 86,
		389, 104,
		353, 131,
		326, 160,
		300, 200,
		281, 237,
		273, 284,
		271, 331,
		282, 388,
		294, 438,
		333, 537,
		328, 540,
		297, 534,
		279, 546,
		273, 562,
		278, 586,
		300, 602,
		314, 617,
		316, 659,
		308, 674,
		295, 686,
		289, 702,
		283, 726,
		277, 740,
		288, 743,
		288, 757,
		288, 856,
		287, 951,
		279, 957,
		282, 968,
		298, 975,
		300, 988,
		328, 990,
		345, 1000,
		407, 1040,
		517, 1115,
		520, 1119,
		520, 1143,
		770, 1143,
		770, 1128,
		770, 1116,
		963, 986,
		992, 984,
		993, 975,
		1002, 968,
		1002, 951,
		1002, 745,
		1006, 734,
		1001, 711,
		990, 691,
		975, 670,
		972, 653,
		978, 641,
		985, 632,
		985, 503,
		978, 491,
		980, 478,
		991, 463,
		987, 446,
		977, 433,
		970, 422,
		974, 408,
		985, 383,
		996, 355,
		1001, 333,
		1004, 308,
		1006, 283,
		1007, 251,
		1004, 221,
		998, 199,
		988, 179,
		977, 161,
		966, 151,
		963, 138,
		973, 139,
		991, 145,
		1006, 160,
		1020, 177,
		1029, 197,
		1033, 221,
		1035, 268,
		1034, 1011
	};

	App->physics->CreateChain(0, 0, Borde, 180);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 25));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();
	SDL_Rect y = { 0,0,1920,1200 };
	//App->renderer->Blit(Bg, 0, 0, &y);

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}

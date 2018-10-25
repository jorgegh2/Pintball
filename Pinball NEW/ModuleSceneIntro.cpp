#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleUI.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	//Textures===================================
	background_tex =  circle_tex = combo_letters_tex = multiplier_tex = bonus_tex= target_tex= slingshots_tex= bumper_tex = NULL;
	
	// Fx =====================================================
	int width = 36, height = 36;
	for (int i = 0; i < 9; ++i)
	{
		combo_letters[i].actived_rect = { i*width , 0, width, height };
	}
	combo_letters[0].position = { 111,606 };
	combo_letters[1].position = { 163,606 };
	combo_letters[2].position = { 215,606 };
	combo_letters[3].position = { 267,606 };
	combo_letters[4].position = { 319,606 };
	combo_letters[5].position = { 140,678 };
	combo_letters[6].position = { 192,678 };
	combo_letters[7].position = { 244,678 };
	combo_letters[8].position = { 296,678 };

	pos_multiplier_bonus[0] = { 251, 825 };
	pos_multiplier_bonus[1] = { 251, 799 };
	pos_multiplier_bonus[2] = { 251, 773 };
	pos_multiplier_bonus[3] = { 251, 747 };

	pos_magnet_bonus[0] = { 366,561 };
	pos_magnet_bonus[1] = { 382,536 };
	pos_magnet_bonus[2] = { 394,511 };
	
	pinball_rect.x = 0;
	pinball_rect.y = 0;
	pinball_rect.w = 1920;
	pinball_rect.h = 1200;

	target_rect = { 0, 0, 38,7 };
	targets_angles[0] = 140;
	targets_angles[1] = -40;
	targets_angles[2] = 40;
	targets_angles[3] = 40;
	targets_angles[4] = -45;

}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	// Textures ===============================================
	background_tex = App->textures->Load("textures/Pinball_Start.png");
	combo_letters_tex = App->textures->Load("textures/chocolate.png");
	multiplier_tex = App->textures->Load("textures/multiplayer_bonus.png");
	bonus_tex = App->textures->Load("textures/magnet_bonus.png");
	bumper_tex = App->textures->Load("textures/bumper.png");
	slingshots_tex = App->textures->Load("textures/slingshot.png");
	target_tex = App->textures->Load("textures/target.png");
	points_rect = { 0,0,19,19 };
	
	// Animation ==============================================
	// Bumpers --------------------------------
	for (uint i = 0; i < 3; ++i)
	{
		animations[i].rect = {0, 0 ,55,55};
		animations[i].tex = bumper_tex;
	}
	animations[0].position = { 259, 290 };
	animations[1].position = {147, 290};
	animations[2].position = {201, 376};
	// Slingshots --------------------------------
	animations[3].position = { 78, 716};
	animations[4].position = { 320,716 };
	animations[3].rect = animations[4].rect = {0, 0, 66, 142};
	animations[3].tex = animations[4].tex = slingshots_tex;

	// PhyBodies ==============================================
	AddStaticBodies();
	sensor_death = App->physics->CreateRectangleSensor(205 + 56 / 2, 1046 + 6 / 2, 56, 6, 0); //the x and y take pos from the center
	top_hole = App->physics->CreateCircleIsSensor(237, 168, 8);
	top_hole->listener = this;
	magnet_hole = App->physics->CreateCircleIsSensor(438, 280, 8);
	magnet_hole->listener = this;

	// Barriers ---------------------------------------
	sensor_barrier_right = App->physics->CreateRectangleSensor(370, 134, 60, 6, 90);
	sensor_barrier_right->listener = this;
	sensor_initial_barrier_left = App->physics->CreateRectangleSensor(78, 188, 36, 10, 50);
	sensor_initial_barrier_left->listener = this;
	sensor_final_barrier_left = App->physics->CreateRectangleSensor(140, 147, 80, 6, 90);
	sensor_final_barrier_left->listener = this;

	// Music & Fx-------------------------------------------
	App->audio->PlayMusic("music/house_music.ogg");
	bonus_fx = App->audio->LoadFx("sfx/bonus.wav");
	chocolate_combo_sfx = App->audio->LoadFx("sfx/chocolate_combo.wav");
	targets_fx = App->audio->LoadFx("sfx/target.wav");
	advance_letter_fx = App->audio->LoadFx("sfx/advance_letter.wav");
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
	// Logic ==================================================================
	if (reset)
	{
		// Bodies ---------------------------------
		if (left_barrier == NULL) create_left_barrier = true;
		if (right_barrier) destroy_right_barrier = true;

		// Bonuses --------------------------------
		keep_inside_frames = 0;
		keep_player_magnet;
		keep_player_top;

		reset = false;
	}
	if (reset_combo)
	{
		// Targets --------------------------------
		for (uint i = 0; i < 4; ++i) {
			if (targets[i]) {
				App->physics->DestroyPhysBody(targets[i]);
			}
		}
		combo_delay_frames = 0;
		create_targets = true;
		combo_delay = false;
		activate_final_target = false;
		// Combo ----------------------------------
		combo_delay_frames = 0;
		combo_letters_amount = 0;

		reset_combo = false;
	}
	//--------Barriers----------------------------------------------
	if (destroy_left_barrier == true)
	{
		App->physics->DestroyPhysBody(left_barrier);
		left_barrier = nullptr;
		destroy_left_barrier = false;
	}
	if (destroy_right_barrier == true)
	{
		App->physics->DestroyPhysBody(right_barrier);
		right_barrier = nullptr;
		destroy_right_barrier = false;
	}
	if (create_right_barrier == true)
	{
		right_barrier = App->physics->CreateRectangle(397, 139, 33, 6, -60, false);
		create_right_barrier = false;
	}
	if (create_left_barrier == true)
	{
		left_barrier = App->physics->CreateRectangle(101, 159, 33, 6, 57, false);
		create_left_barrier = false;
	}
	// --------Combo------------------------------------------- 
	if (combo_delay)
	{
		if (combo_delay_frames == COMBO_DELAY_FRAMES) {

			combo_delay_frames = 0;
			create_targets = true;
			combo_delay = false;
			activate_final_target = false;
		}
		else
			++combo_delay_frames;
	}

	if (final_target && final_target->to_delete == true)
	{
		App->physics->DestroyPhysBody(final_target);
		final_target = nullptr;
	}

	if (create_targets)
	{
		final_target = App->physics->CreateRectangleSensor(59, 533, 10, 39, 45);
		final_target->listener = this;
		targets[0] = App->physics->CreateRectangleSensor(106, 279, 10, 39, 50);//left-bottom
		targets[0]->listener = this;
		targets[1] = App->physics->CreateRectangleSensor(142, 250, 9, 39, 50);//left-upper
		targets[1]->listener = this;
		targets[2] = App->physics->CreateRectangleSensor(318, 242, 9, 39, -50);//right-upper
		targets[2]->listener = this;
		targets[3] = App->physics->CreateRectangleSensor(355, 271, 10, 39, -50);//right-bottom
		targets[3]->listener = this;

		create_targets = false;
	}

	int count = 0;
	for (uint i = 0; (i < 4) && (activate_final_target == false); ++i)
	{
		if (targets[i] && targets[i]->to_delete == true)
		{
			App->physics->DestroyPhysBody(targets[i]);
			targets[i] = nullptr;
		}
		if (targets[i] == nullptr)
			++count;
	}
	if (count == 4)
	{
		activate_final_target = true;
	}
	// --------Bonuses--------------------------------------------

	if (keep_player_magnet)
	{
		if (keep_inside_frames == KEEP_INSIDE_FRAMES)
		{
			keep_inside_frames = 0;
			keep_player_magnet = false;
		}
		else
		{
			App->player->ball->body->SetLinearVelocity({0,0});
			App->player->ball->body->SetTransform(magnet_hole->body->GetPosition(), 0);
			++keep_inside_frames;
		}
	}

	if (keep_player_top)
	{
		if (keep_inside_frames == KEEP_INSIDE_FRAMES)
		{
			keep_inside_frames = 0;
			keep_player_top = false;
		}
		else
		{
			App->player->ball->body->SetLinearVelocity({ 0,0 });
			App->player->ball->body->SetTransform(top_hole->body->GetPosition(), 0);
			++keep_inside_frames;
		}
	}

	// All draw functions ======================================================

	// --------Background---------------------------------------
	App->renderer->Blit(background_tex, SCREEN_WIDTH / 2 - pinball_rect.w / 2, 0, &pinball_rect);
	// -------Combo letters ------------------------------------
	for (uint i = 0; i < combo_letters_amount; ++i)
	{
		App->renderer->Blit(combo_letters_tex, combo_letters[i].position.x, combo_letters[i].position.y, &combo_letters[i].actived_rect);
	}
	// ------- Targets -----------------------------------------
	for (uint i = 0; (i < 4) && (activate_final_target == false); ++i)
	{
		if (targets[i])
		{
			int x, y;
			targets[i]->GetPosition(x, y);
			App->renderer->Blit(target_tex, x - 7, y + 36, &target_rect, 1.0f, targets_angles[i]);
		}
	}
	if (final_target) 
	{
		int x, y;
		final_target->GetPosition(x, y);
		App->renderer->Blit(target_tex, x - 7, y + 36, &target_rect, 1.0f, targets_angles[4]);
	}
	// -------Animations----------------------------------------
	for (uint i = 0; i < 3; ++i)
	{
		PhysBody *data = nullptr;
		bumpers.at(i, data);

		if (animations[i].frames == LIGHT_FRAMES || data->activated == false)
		{
			animations[i].frames = 0;
			data->activated = false;
			continue;
		}
		++animations[i].frames;
		App->renderer->Blit(animations[i].tex, animations[i].position.x , animations[i].position.y, &animations[i].rect);
	}

	for (uint i = 3; i < 5; ++i)
	{
		PhysBody *data = nullptr;
		slingshots.at(i - 3, data);

		if (animations[i].frames == LIGHT_FRAMES || data->activated == false)
		{
			animations[i].frames = 0;
			data->activated = false;
			continue;
		}
		++animations[i].frames;
		if (i == 3)
			App->renderer->Blit(animations[i].tex, animations[i].position.x, animations[i].position.y, &animations[i].rect);
		else
			App->renderer->Blit(animations[i].tex, animations[i].position.x, animations[i].position.y, &animations[i].rect, 1.0f, 0,0,0,true);
	}
	//--------Bonus Points--------------------------------------
	App->renderer->Blit(multiplier_tex, pos_multiplier_bonus[ App->ui->multiplier -1].x, pos_multiplier_bonus[App->ui->multiplier - 1].y, &points_rect);
	App->renderer->Blit(bonus_tex, pos_magnet_bonus[(int)current_bonus].x, pos_magnet_bonus[(int)current_bonus].y, &points_rect);

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact)
{
	// Sensors ========================================================================
	if (sensor_barrier_right == bodyA && App->player->ball == bodyB && right_barrier == nullptr)
	{
		create_right_barrier = true;
		return;
	}
	else if (sensor_final_barrier_left == bodyA && App->player->ball == bodyB && left_barrier == nullptr)
	{
		create_left_barrier = true;
		return;
	}
	else if (sensor_initial_barrier_left == bodyA && App->player->ball == bodyB && left_barrier != nullptr)
	{
		destroy_left_barrier = true;
		return;
	}
	else if (magnet_hole == bodyA && App->player->ball == bodyB && left_barrier)
	{
		// Bonuses ==================================
		switch ((Bonuses)current_bonus) {
		case Bonuses::extra_ball:
			++App->player->lifes;
			if (App->player->lifes > 9)
			{
				App->player->lifes = 9;
			}
			break;
		case Bonuses::add_50k:
			App->ui->AddPoints(50000);
			break;
		case Bonuses::advance_multiplier:
			++App->ui->multiplier;
			if (App->ui->multiplier == 5)
			{
				App->ui->multiplier = 1;
			}
			break;
		}

		keep_player_magnet = true;
		return;
	}
	else if (top_hole == bodyA && App->player->ball == bodyB && left_barrier)
	{
		keep_player_top = true;
		// Add points ================================
		App->ui->AddPoints(5000);
		return;
	}
	// Targets ========================================================================
	else if (final_target && final_target == bodyA && App->player->ball == bodyB && activate_final_target)
	{
		// Add points ================================
		App->ui->AddPoints(10000);
		// Logic =====================================
		final_target->to_delete = true;
		if (combo_letters_amount < 8) 
		{
			++combo_letters_amount;
			App->audio->PlayFx(advance_letter_fx);
		}
		else 
		{
			App->audio->PlayFx(chocolate_combo_sfx);
			combo_letters_amount = 0;
			// Add points ================================
			App->ui->AddPoints(1000000);
		}
		combo_delay = true;
	}

	for (uint i = 0; (i < 4) && (combo_delay == false); ++i)
	{
		if (targets[i] &&  targets[i] == bodyA && App->player->ball == bodyB )
		{
			targets[i]->to_delete = true;
			// Add points ================================
			App->ui->AddPoints(2000);
			App->audio->PlayFx(targets_fx);
			break;
		}
	}
}

void ModuleSceneIntro::AddStaticBodies()
{
	//Bumpers============================================
	bumpers.add(App->physics->CreateCircle(SCREEN_WIDTH / 2 + 29, SCREEN_HEIGHT / 2 - 222, 27, false));
	bumpers.add(App->physics->CreateCircle(SCREEN_WIDTH / 2 - 85, SCREEN_HEIGHT / 2 - 222, 27, false));
	bumpers.add(App->physics->CreateCircle(SCREEN_WIDTH / 2 - 29, SCREEN_HEIGHT / 2 - 135, 27, false));

	//Slingshots============================================
	int LeftStruct[26] = {
	80, 727,
	80, 821,
	80, 827,
	85, 831,
	130, 856,
	138, 857,
	142, 854,
	145, 850,
	144, 844,
	100, 720,
	96, 716,
	84, 717,
	80, 721
	};

	slingshots.add(App->physics->CreateChain(0, 0, LeftStruct, 26, false));

	int RightStruct[26] = {
		321, 846,
		322, 852,
		328, 856,
		334, 857,
		340, 855,
		383, 829,
		386, 825,
		386, 722,
		382, 716,
		374, 715,
		367, 717,
		364, 722,
		323, 842
	};

	slingshots.add(App->physics->CreateChain(0, 0, RightStruct, 26, false));

	//Board parts=============================================================
	int BigStruct[52] = {
		98, 402,
		102, 397,
		103, 389,
		101, 376,
		83, 290,
		154, 230,
		195, 232,
		198, 228,
		200, 223,
		200, 217,
		197, 212,
		110, 176,
		103, 180,
		95, 187,
		78, 209,
		66, 232,
		61, 248,
		56, 266,
		55, 289,
		55, 313,
		59, 338,
		64, 360,
		72, 381,
		81, 395,
		88, 402,
		94, 403
	};
	
	board_parts.add(App->physics->CreateChain(0, 0, BigStruct, 52, false));

	int LeftLine[16] = {
		49, 739,
		50, 847,
		143, 901,
		143, 904,
		140, 904,
		45, 848,
		45, 739,
		47, 737
	};

	board_parts.add(App->physics->CreateChain(0, 0, LeftLine, 16, false));

	

	//OUR PINBALL

	int Pinball[180] = {
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



	board_parts.add(App->physics->CreateChain(0,0, Pinball, 180, false));


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

}

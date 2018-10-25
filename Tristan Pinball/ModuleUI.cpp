#include <string>
#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleFonts.h"
#include "ModuleUI.h"
#include "ModulePlayer.h"
#include <stdio.h>

using namespace std;

ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled) {}

ModuleUI::~ModuleUI() {}

bool ModuleUI::Start()
{
	LOG("Loading UI");

	font_1 = App->fonts->Load("textures/font_1.png", "0123456789", 1);
	font_2 = App->fonts->Load("textures/font_2.png", "0123456789", 1);

	gameover_tex = App->textures->Load("textures/gameover.png");

	gameover_rect.x = 0;
	gameover_rect.y = 0;
	gameover_rect.w = 287;
	gameover_rect.h = 44;

	current_score = 0;

	return true;
}

bool ModuleUI::CleanUp()
{
	LOG("Unloading user interface")

	App->fonts->UnLoad(font_1);
	App->fonts->UnLoad(font_2);

	return true;
}
update_status ModuleUI::Update()
{
	//highscore logic
	if (current_score > high_score) {
		high_score = current_score;
	}

	//scores
	char const* str_score = nullptr;
	string score = to_string(current_score);
	str_score = score.c_str();
	App->fonts->BlitText(20, 21, font_1, str_score);

	char const* str_HighScore = nullptr;
	string HighScore = to_string(high_score);
	str_HighScore = HighScore.c_str();
	App->fonts->BlitText(224, 21, font_2, str_HighScore);

	//lifes
	string lifes = to_string(App->player->lifes);
	char const* str_lifes = lifes.c_str();;
	App->fonts->BlitText(436, 21, font_1, str_lifes);

	//Gameover
	if (App->player->is_dead == true) {
		App->renderer->Blit(gameover_tex, SCREEN_WIDTH / 2 - gameover_rect.w / 2 - 11, SCREEN_HEIGHT / 2 - gameover_rect.h / 2, &gameover_rect, 0.0f);
	}

	return UPDATE_CONTINUE;
}

void ModuleUI::AddPoints(int points)
{
	current_score += points * multiplier;
}

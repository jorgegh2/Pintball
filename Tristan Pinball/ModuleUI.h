#ifndef __MODULEUI_H__
#define __MODULEUI_H__

#include "Module.h"
#include "p2Point.h"

class ModuleUI : public Module
{

public:

	ModuleUI(Application* app, bool start_enabled = true);
	~ModuleUI();


	bool Start();
	update_status Update();
	bool CleanUp();

	void AddPoints(int points);

public:
	//GameOver =======================================
	SDL_Texture* gameover_tex;
	SDL_Rect gameover_rect;

	//Fonts ==========================================
	int font_1 = 0;
	int font_2 = 1;

	// Player info ===================================
	int current_score = 0;
	int multiplier = 1;
	int high_score = 0;

};

#endif //__MODULEUI_H__


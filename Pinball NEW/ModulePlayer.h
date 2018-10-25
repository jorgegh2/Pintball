#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"
#include "SDL/include/SDL.h"

class PhysBody;
class b2RevoluteJoint;
class b2PrismaticJoint;
struct SDL_Texture;
class b2Contact;

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact);
	void Reset();

	//Flipper actions ===============================
	void engageFlipper(PhysBody *flipper, float impulse);

public:
	// State =======================================
	b2Vec2 init_position;
	bool reset_pos = false;
	bool reset_all = false;
	bool is_dead = false;
	int lifes;
	uint points;
	uint multiplier;

	// PhysBodies ==================================
	PhysBody* ball = NULL;
	PhysBody* flipper_r = NULL;
	PhysBody* flipper_l = NULL;
	PhysBody* right_barrier = NULL;
	PhysBody* left_barrier = NULL;
	PhysBody* launcher = NULL;

	//Launcher
	b2Vec2 launcher_init_pos;
	float impulse_force;

	// Joints =======================================
	b2RevoluteJoint* flipper_r_joint = NULL;
	b2RevoluteJoint* flipper_l_joint = NULL;
	b2PrismaticJoint* launcher_joint = NULL;

	// Textures ====================================
	SDL_Texture* ball_tex;
	SDL_Texture* flipper_tex;
	SDL_Rect flipper_rect;

	// Sfx==========================================
	uint flipper_fx;
	uint lose_fx;
	uint kicker_fx;

};
#pragma once
#include "Module.h"
#include "Globals.h"

class b2World;
class b2BodyDef;

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

private:
	b2World* world;
	bool debug;

};
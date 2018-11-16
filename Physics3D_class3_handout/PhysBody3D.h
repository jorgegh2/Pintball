#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"

class btRigidBody;
class Module;

struct PhysBody3D
{
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	btTransform GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);

private:
	btRigidBody* body;

public:
	p2List<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__
#include "PhysBody3D.h"
#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// TODO 4: Fill in PhysBody methods. Remember to use 
// get/setOpenGLMatrix methods

// ---------------------------------------------------------
PhysBody3D::PhysBody3D(btRigidBody* body)
{}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{}

// ---------------------------------------------------------
btTransform PhysBody3D::GetTransform(float* matrix) const
{
	return body->getWorldTransform();
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
}

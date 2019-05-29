#include<iostream>
#define BT_USE_DOUBLE_PRECISION 1
#include <btBulletDynamicsCommon.h>

using namespace std;

int main(int, char**)
{
    // Build the broadphase
    btBroadphaseInterface* pBroadphase = new btDbvtBroadphase();

    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* pCollisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* pDispatcher = new btCollisionDispatcher(pCollisionConfiguration);

    // The actual physics solver
    btSequentialImpulseConstraintSolver* pSolver = new btSequentialImpulseConstraintSolver();

    // The world
    btDiscreteDynamicsWorld* pDynamicsWorld = new btDiscreteDynamicsWorld(pDispatcher, pBroadphase, pSolver, pCollisionConfiguration);
    pDynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    // Create Collision Models
    btCollisionShape* pGroundShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 1);
    btCollisionShape* pFallShape = new btSphereShape(1.0f);

    // Create Rigid body
    btDefaultMotionState* pGroundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, -1.0f, 0.0f)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0.0f, pGroundMotionState, pGroundShape, btVector3(0.0f, 0.0f, 0.0f));
    btRigidBody* pGroundRigidBody = new btRigidBody(groundRigidBodyCI);
    pDynamicsWorld->addRigidBody(pGroundRigidBody);

    btDefaultMotionState* pFallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 50.0f, 0.0f)));
    btScalar mass = 1.0f;
    btVector3 fallInertia(0.0f, 0.0f, 0.0f);
    pFallShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, pFallMotionState, pFallShape, fallInertia);
    btRigidBody* pFallRigidBody = new btRigidBody(fallRigidBodyCI);
    pDynamicsWorld->addRigidBody(pFallRigidBody);

    for (int i = 0 ; i < 300; ++i)
    {
        pDynamicsWorld->stepSimulation(1.0f / 60.0f, 10.0f);

        btTransform trans;
        pFallRigidBody->getMotionState()->getWorldTransform(trans);
        
        cout << "Sphere Height: " << trans.getOrigin().getY() << std::endl;
    }

    pDynamicsWorld->removeRigidBody(pFallRigidBody);
    delete pFallRigidBody->getMotionState();
    delete pFallRigidBody;

    pDynamicsWorld->removeRigidBody(pGroundRigidBody);
    delete pGroundRigidBody->getMotionState();
    delete pGroundRigidBody;

    delete pFallShape;
    delete pGroundShape;

    // Clean up
    delete pDynamicsWorld;
    delete pSolver;
    delete pDispatcher;
    delete pCollisionConfiguration;
    delete pBroadphase;
    
    return 0;
}
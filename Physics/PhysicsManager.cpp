#include <iostream>
#include "PhysicsManager.hpp"

namespace Panda
{
    int PhysicsManager::Initialize()
    {
        // Build the broadphase
        m_pBroadphase = new btDbvtBroadphase();

        // set up the collision configuration and dispatcher
        m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
        m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

        // The actual physics solver
        m_pSolver = new btSequentialImpulseConstraintSolver();

        // The world
        m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);
        m_pDynamicsWorld->setGravity(btVector3(0.0f, 0.0f, -9.8f));

        return 0;
    }

    void PhysicsManager::Finalize()
    {
        // Clean up
        ClearRigidBodies();

        delete m_pDynamicsWorld;
        delete m_pSolver;
        delete m_pDispatcher;
        delete m_pCollisionConfiguration;
        delete m_pBroadphase;
    }

    void PhysicsManager::Tick()
    {
        if (g_pSceneManager->IsSceneChanged())
        {
            g_pPhysicsManager->ClearRigidBodies();
            g_pPhysicsManager->CreateRigidBodies();
            g_pSceneManager->NotifySceneIsPhysicalSimulationQueued();
        }

        m_pDynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
    }

    void PhysicsManager::CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry)
    {
        btRigidBody* pRigidBody = nullptr;

        const float* param = geometry.CollisionParameters();

        switch(geometry.CollisionType())
        {
            case SceneObjectCollisionType::kSceneObjectCollisionTypeSphere:
            {
                btSphereShape* sphere = new btSphereShape(param[0]);
                m_pCollisionShapes.push_back(sphere);

                const auto trans = node.GetCalculatedTransform();
                btTransform startTransform;
                startTransform.setIdentity();
                startTransform.setOrigin(btVector3(trans->m[3][0], trans->m[3][1], trans->m[3][2]));
                startTransform.setBasis(btMatrix3x3(trans->m[0][0], trans->m[1][0], trans->m[2][0],
                                            trans->m[0][1], trans->m[1][1], trans->m[2][1],
                                            trans->m[0][2], trans->m[1][2], trans->m[2][2]));
                btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
                btScalar mass = 1.0f;
                btVector3 fallInertia(0.0f, 0.0f, 0.0f);
                sphere->calculateLocalInertia(mass, fallInertia);
                btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphere, fallInertia);
                pRigidBody = new btRigidBody(rigidBodyCI);

                m_pDynamicsWorld->addRigidBody(pRigidBody);
                break;
            }
            case SceneObjectCollisionType::kSceneObjectCollisionTypeBox:
            {
                btBoxShape* box = new btBoxShape(btVector3(param[0], param[1], param[2]));
                m_pCollisionShapes.push_back(box);

                const auto trans = node.GetCalculatedTransform();
                btTransform startTransform;
                startTransform.setIdentity();
                startTransform.setOrigin(btVector3(trans->m[3][0], trans->m[3][1], trans->m[3][2]));
                startTransform.setBasis(btMatrix3x3(trans->m[0][0], trans->m[1][0], trans->m[2][0],
                                            trans->m[0][1], trans->m[1][1], trans->m[2][1],
                                            trans->m[0][2], trans->m[1][2], trans->m[2][2]));
                btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
                btScalar mass = 0.0f;
                btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, box, btVector3(0.0f, 0.0f, 0.0f));
                pRigidBody = new btRigidBody(rigidBodyCI);
                m_pDynamicsWorld->addRigidBody(pRigidBody);

                break;
            }
            case SceneObjectCollisionType::kSceneObjectCollisionTypePlane:
                {
                    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(param[0], param[1], param[2]), param[3]);
                    m_pCollisionShapes.push_back(plane);

                    const auto trans = node.GetCalculatedTransform();
                    btTransform startTransform;
                    startTransform.setIdentity();
                    startTransform.setOrigin(btVector3(trans->m[3][0], trans->m[3][1], trans->m[3][2]));
                    startTransform.setBasis(btMatrix3x3(trans->m[0][0], trans->m[1][0], trans->m[2][0],
                                                trans->m[0][1], trans->m[1][1], trans->m[2][1],
                                                trans->m[0][2], trans->m[1][2], trans->m[2][2]));
                    btDefaultMotionState* motionState = 
                        new btDefaultMotionState(
                                    startTransform
                                );
                    btScalar mass = 0.0f;
                    btRigidBody::btRigidBodyConstructionInfo
                        rigidBodyCI(mass, motionState, plane, btVector3(0.0f, 0.0f, 0.0f));
                    pRigidBody = new btRigidBody(rigidBodyCI);
                    m_pDynamicsWorld->addRigidBody(pRigidBody);
                }
                break;
            default:
                break;
        }

        node.LinkRigidBody(pRigidBody);
    }
    void PhysicsManager::UpdateRigidBodyTransform(SceneGeometryNode& node)
    {
        const auto trans = node.GetCalculatedTransform();
        auto rigidBody = node.RigidBody();
        auto motionState = reinterpret_cast<btRigidBody*>(rigidBody)->getMotionState();
        btTransform _trans;
        _trans.setIdentity();
        _trans.setOrigin(btVector3(trans->m[3][0], trans->m[3][1], trans->m[3][2]));
        _trans.setBasis(btMatrix3x3(trans->m[0][0], trans->m[1][0], trans->m[2][0],
                                    trans->m[0][1], trans->m[1][1], trans->m[2][1],
                                    trans->m[0][2], trans->m[1][2], trans->m[2][2]));
        motionState->setWorldTransform(_trans);
    }

    void PhysicsManager::DeleteRigidBody(SceneGeometryNode& node)
    {
        btRigidBody* pRigidBody = reinterpret_cast<btRigidBody*>(node.UnlinkRigidBody());
        if (pRigidBody)
        {
            m_pDynamicsWorld->removeRigidBody(pRigidBody);
            if (auto motionState = pRigidBody->getMotionState())
                delete motionState;
            delete pRigidBody;
            //m_pDynamicsWorld->removeCollisionObject(rigidBody);
        }
    }

    int PhysicsManager::CreateRigidBodies()
    {
        auto& scene = g_pSceneManager->GetScene();

        // Geometries
        for (auto _it : scene.GeometryNodes)
        {
            auto pGeometryNode = _it.second;
            auto pGeometry = scene.GetGeometry(pGeometryNode->GetSceneObjectRef());
            assert(pGeometry);

            CreateRigidBody(*pGeometryNode, *pGeometry);
        }

        return 0;
    }

    void PhysicsManager::ClearRigidBodies()
    {
        auto& scene = g_pSceneManager->GetScene();

        // Geometries
        for (auto _it : scene.GeometryNodes)
        {
            auto pGeometryNode = _it.second;
            DeleteRigidBody(*pGeometryNode);
        }

        for (auto shape : m_pCollisionShapes)
        {
            delete shape;
        }

        m_pCollisionShapes.clear();
    }

    Matrix4f PhysicsManager::GetRigidBodyTransform(void* rigidBody)
    {
        Matrix4f result;
        btTransform trans;
        reinterpret_cast<btRigidBody*>(rigidBody)->getMotionState()->getWorldTransform(trans);
        auto basis = trans.getBasis();
        auto origin = trans.getOrigin();
        result.SetIdentity();
        result.m[0][0] = basis[0][0];
        result.m[1][0] = basis[0][1];
        result.m[2][0] = basis[0][2];
        result.m[0][1] = basis[1][0];
        result.m[1][1] = basis[1][1];
        result.m[2][1] = basis[1][2];
        result.m[0][2] = basis[2][0];
        result.m[1][2] = basis[2][1];
        result.m[2][2] = basis[2][2];
        result.m[3][0] = origin.getX();
        result.m[3][1] = origin.getY();
        result.m[3][2] = origin.getZ();

        return result;
    }
    
    void PhysicsManager::ApplyCentralForce(void* rigidBody, Vector3Df force)
    {
        btRigidBody* _rigidBody = reinterpret_cast<btRigidBody*>(rigidBody);
        btVector3 _force(force.x, force.y, force.z);
        _rigidBody->activate(true);
        _rigidBody->applyCentralForce(_force);
    }
}
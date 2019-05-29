#pragma once

#include <vector>
#define BT_USE_DOUBLE_PRECISION 1
#include <btBulletDynamicsCommon.h>
#include "IRuntimeModule.hpp"
#include "SceneManager.hpp"

namespace Panda
{
    class PhysicsManager : implements IRuntimeModule
    {
        public:
            virtual int Initialize();
            virtual void Finalize();
            virtual void Tick();

            virtual void CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry);
            virtual void DeleteRigidBody(SceneGeometryNode& node);

            virtual int CreateRigidBodies();
            virtual void ClearRigidBodies();

            Matrix4f GetRigidBodyTransform(void* rigidBody);

        protected:

        protected:
            btBroadphaseInterface*                  m_pBroadphase;
            btDefaultCollisionConfiguration*        m_pCollisionConfiguration;
            btCollisionDispatcher*                  m_pDispatcher;
            btSequentialImpulseConstraintSolver*    m_pSolver;
            btDiscreteDynamicsWorld*                m_pDynamicWorld;

            std::vector<btCollisionShape*>          m_pCollisionShapes;
    };

    extern PhysicsManager* g_pPhysicsManager;
}
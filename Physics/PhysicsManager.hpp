#pragma once

#include <vector>
#include "IPhysicsManager.hpp"
#define BT_USE_DOUBLE_PRECISION 1
#include <btBulletDynamicsCommon.h>


namespace Panda
{
    class PhysicsManager : implements IPhysicsManager
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
            void UpdateRigidBodyTransform(SceneGeometryNode& node);

            void ApplyCentralForce(void* rigidBody, Vector3Df force);

        protected:

        protected:
            btBroadphaseInterface*                  m_pBroadphase;
            btDefaultCollisionConfiguration*        m_pCollisionConfiguration;
            btCollisionDispatcher*                  m_pDispatcher;
            btSequentialImpulseConstraintSolver*    m_pSolver;
            btDiscreteDynamicsWorld*                m_pDynamicsWorld;

            std::vector<btCollisionShape*>          m_pCollisionShapes;
    };
}
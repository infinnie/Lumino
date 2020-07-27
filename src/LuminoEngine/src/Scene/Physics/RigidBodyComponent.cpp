﻿
#include "../Internal.hpp""
#include <LuminoEngine/Physics/PhysicsWorld.hpp>
#include <LuminoEngine/Scene/Physics/RigidBodyComponent.hpp>
#include <LuminoEngine/Scene/WorldObject.hpp>
#include "Engine/EngineManager.hpp"

namespace ln {

//=============================================================================
// Collision

Collision::Collision()
    : m_worldObject(nullptr)
    , m_physicsObject(nullptr)
{
}

void Collision::init(WorldObject* worldObject/*, RigidBodyComponent* component*/, PhysicsObject* physicsObject)
{
    Object::init();
    m_worldObject = worldObject;
    m_physicsObject = physicsObject;
}

//=============================================================================
// RigidBodyComponent

Ref<RigidBodyComponent> RigidBodyComponent::create()
{
	return makeObject<RigidBodyComponent>();
}

RigidBodyComponent::RigidBodyComponent()
{
}

void RigidBodyComponent::init()
{
	Component::init();
	m_body = makeObject<RigidBody>();
    m_body->setEventListener(this);
    m_body->setOwnerData(this);
    detail::EngineDomain::engineManager()->mainPhysicsWorld()->addPhysicsObject(m_body);
}

void RigidBodyComponent::onDispose(bool explicitDisposing)
{
	if (m_body) {
		m_body->setEventListener(nullptr);
		m_body->removeFromPhysicsWorld();
		m_body = nullptr;
	}

	Component::onDispose(explicitDisposing);
}

void RigidBodyComponent::setMass(float value)
{
    m_body->setMass(value);
}

void RigidBodyComponent::addCollisionShape(CollisionShape* shape)
{
    m_body->addCollisionShape(shape);
}

void RigidBodyComponent::onAttachedScene(Level* newOwner)
{
}

void RigidBodyComponent::onDetachedScene(Level* oldOwner)
{
}

void RigidBodyComponent::onBeforeStepSimulation()
{
    // worldObject -> body へ姿勢を同期する
    m_body->setTransform(worldObject()->worldMatrix());
}

void RigidBodyComponent::onAfterStepSimulation()
{
    if (m_body->isDynamic()) {
        // body -> worldObject へ姿勢を同期する。
        // 拡大率は持っていないことを前提とする。
        worldObject()->setRotation(Quaternion::makeFromRotationMatrix(m_body->transform()));
        worldObject()->setPosition(m_body->transform().position());
    }
}

RigidBody* RigidBodyComponent::rigidBody() const
{
    return m_body;
}

Ref<EventConnection> RigidBodyComponent::connectOnCollisionEnter(Ref<CollisionEventHandler> handler)
{
    return m_onCollisionEnter.connect(handler);
}

Ref<EventConnection> RigidBodyComponent::connectOnCollisionLeave(Ref<CollisionEventHandler> handler)
{
    return m_onCollisionLeave.connect(handler);
}

Ref<EventConnection> RigidBodyComponent::connectOnCollisionStay(Ref<CollisionEventHandler> handler)
{
    return m_onCollisionStay.connect(handler);
}

void RigidBodyComponent::onCollisionEnter(PhysicsObject* otherObject, ContactPoint* contact)
{
    auto* ownerComponent = reinterpret_cast<RigidBodyComponent*>(otherObject->ownerData());
    auto* worldObject = (ownerComponent) ? ownerComponent->worldObject() : nullptr;

	// TODO: Cache
	auto c = makeObject<Collision>(worldObject, otherObject);
	m_onCollisionEnter.raise(c);
}

void RigidBodyComponent::onCollisionLeave(PhysicsObject* otherObject, ContactPoint* contact)
{
    auto* ownerComponent = reinterpret_cast<RigidBodyComponent*>(otherObject->ownerData());
    auto* worldObject = (ownerComponent) ? ownerComponent->worldObject() : nullptr;

	// TODO: Cache
	auto c = makeObject<Collision>(worldObject, otherObject);
	m_onCollisionLeave.raise(c);
}

void RigidBodyComponent::onCollisionStay(PhysicsObject* otherObject, ContactPoint* contact)
{
    auto* ownerComponent = reinterpret_cast<RigidBodyComponent*>(otherObject->ownerData());
    auto* worldObject = (ownerComponent) ? ownerComponent->worldObject() : nullptr;

    // TODO: Cache
    auto c = makeObject<Collision>(worldObject, otherObject);
    m_onCollisionStay.raise(c);
}




//=============================================================================
// TriggerBodyComponent

Ref<TriggerBodyComponent> TriggerBodyComponent::create()
{
    return makeObject<TriggerBodyComponent>();
}

TriggerBodyComponent::TriggerBodyComponent()
{
}

void TriggerBodyComponent::init()
{
    Component::init();
    m_body = makeObject<TriggerBody>();
    m_body->setEventListener(this);
    m_body->setOwnerData(this);
    detail::EngineDomain::engineManager()->mainPhysicsWorld()->addPhysicsObject(m_body);
}

void TriggerBodyComponent::onDispose(bool explicitDisposing)
{
    if (m_body) {
        m_body->setEventListener(nullptr);
        m_body->removeFromPhysicsWorld();
        m_body = nullptr;
    }

    Component::onDispose(explicitDisposing);
}

void TriggerBodyComponent::addCollisionShape(CollisionShape* shape)
{
    m_body->addCollisionShape(shape);
}

void TriggerBodyComponent::onAttachedScene(Level* newOwner)
{
}

void TriggerBodyComponent::onDetachedScene(Level* oldOwner)
{
}

void TriggerBodyComponent::onBeforeStepSimulation()
{
    m_body->setPosition(worldObject()->position());
}

void TriggerBodyComponent::onAfterStepSimulation()
{
}

Ref<EventConnection> TriggerBodyComponent::connectOnCollisionEnter(Ref<CollisionEventHandler> handler)
{
    return m_onCollisionEnter.connect(handler);
}

Ref<EventConnection> TriggerBodyComponent::connectOnCollisionLeave(Ref<CollisionEventHandler> handler)
{
    return m_onCollisionLeave.connect(handler);
}

Ref<EventConnection> TriggerBodyComponent::connectOnCollisionStay(Ref<CollisionEventHandler> handler)
{
    return m_onCollisionStay.connect(handler);
}

void TriggerBodyComponent::onCollisionEnter(PhysicsObject* otherObject, ContactPoint* contact)
{
    auto* ownerComponent = reinterpret_cast<TriggerBodyComponent*>(otherObject->ownerData());
    auto* worldObject = (ownerComponent) ? ownerComponent->worldObject() : nullptr;

    // TODO: Cache
    auto c = makeObject<Collision>(worldObject, otherObject);
    m_onCollisionEnter.raise(c);
}

void TriggerBodyComponent::onCollisionLeave(PhysicsObject* otherObject, ContactPoint* contact)
{
    auto* ownerComponent = reinterpret_cast<TriggerBodyComponent*>(otherObject->ownerData());
    auto* worldObject = (ownerComponent) ? ownerComponent->worldObject() : nullptr;

    // TODO: Cache
    auto c = makeObject<Collision>(worldObject, otherObject);
    m_onCollisionLeave.raise(c);
}

void TriggerBodyComponent::onCollisionStay(PhysicsObject* otherObject, ContactPoint* contact)
{
    auto* ownerComponent = reinterpret_cast<TriggerBodyComponent*>(otherObject->ownerData());
    auto* worldObject = (ownerComponent) ? ownerComponent->worldObject() : nullptr;

    // TODO: Cache
    auto c = makeObject<Collision>(worldObject, otherObject);
    m_onCollisionStay.raise(c);
}

} // namespace ln

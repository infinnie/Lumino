﻿
#pragma once
#include "Component.hpp"
#include "../Physics/PhysicsWorld2D.hpp"

namespace ln {
class Collision;
class RigidBody2DComponent;

using CollisionEventHandler = std::function<void(Collision*)>;

class Collision
    : public Object
{
public:
    WorldObject* worldObject() const { return m_worldObject; }

LN_CONSTRUCT_ACCESS:
    Collision();
    virtual ~Collision() = default;
    void init(WorldObject* worldObject/*, RigidBody2DComponent* component*/);

private:
    WorldObject* m_worldObject;
};

class RigidBody2DComponent
	: public Component
	, protected detail::IPhysicsObjectEventListener
{
public:
    void setVelocity(const Vector2& value) { m_body->setVelocity(value); }

    const Vector2& velocity() const { return m_body->velocity(); }

    void setMass(float value);
    void addCollisionShape(CollisionShape2D* shape);

    void setFixedRotation(bool value) { m_body->setFixedRotation(value); }

    /** 重心に力を加えます。 */
    void applyForce(const Vector2& force) { m_body->applyForce(force); }

    /** 指定したローカル位置に力を加えます。 */
    void applyForce(const Vector2& force, const Vector2& localPosition) { m_body->applyForce(force, localPosition); }

    /** 重心に衝撃を与えます。 */
    void applyImpulse(const Vector2& impulse) { m_body->applyImpulse(impulse); }

    /** 指定したローカル位置に衝撃を与えます。 */
    void applyImpulse(const Vector2& impulse, const Vector2& localPosition) { m_body->applyImpulse(impulse, localPosition); }

    /** トルクをかけます。 */
    void applyTorque(float torque) { m_body->applyTorque(torque); }

    /** トルク衝撃を与えます。 */
    void applyTorqueImpulse(float torque) { m_body->applyTorqueImpulse(torque); }
    

    /** onTriggerEnter イベントの通知を受け取るコールバックを登録します。*/
    LN_METHOD(Event)
    EventConnection connectOnCollisionEnter(CollisionEventHandler handler);

    /** onTriggerLeave イベントの通知を受け取るコールバックを登録します。*/
    LN_METHOD(Event)
    EventConnection connectOnCollisionLeave(CollisionEventHandler handler);

    /** onTriggerStay イベントの通知を受け取るコールバックを登録します。*/
    LN_METHOD(Event)
    EventConnection connectOnCollisionStay(CollisionEventHandler handler);

LN_CONSTRUCT_ACCESS:
	RigidBody2DComponent();
	virtual ~RigidBody2DComponent() = default;
	void init();

protected:
	virtual void onAttachedWorld(World* newOwner) override;
	virtual void onDetachedWorld(World* oldOwner) override;
	virtual void onBeforeStepSimulation() override;
	virtual void onAfterStepSimulation() override;
    virtual void onCollisionEnter(PhysicsObject2D* otherObject, ContactPoint2D* contact) override;
    virtual void onCollisionLeave(PhysicsObject2D* otherObject, ContactPoint2D* contact) override;
    virtual void onCollisionStay(PhysicsObject2D* otherObject, ContactPoint2D* contact) override;

private:
	Ref<RigidBody2D> m_body;
    Event<CollisionEventHandler> m_onCollisionEnter;
    Event<CollisionEventHandler> m_onCollisionLeave;
    Event<CollisionEventHandler> m_onCollisionStay;
};

} // namespace ln

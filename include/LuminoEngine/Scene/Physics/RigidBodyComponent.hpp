﻿
#pragma once
#include "../Component.hpp"
#include "../../Physics/RigidBody.hpp"
#include "../../Physics/TriggerBody.hpp"

namespace ln {
class Collision;
class RigidBodyComponent;

using CollisionEventHandler = Delegate<void(Collision*)>;

class Collision
    : public Object
{
public:
	/** 自分自身と衝突している他の WorldObject */
    WorldObject* worldObject() const { return m_worldObject; }

    /** 自分自身と衝突している他の PhysicsObject */
    PhysicsObject* physicsObject() const { return m_physicsObject; }

LN_CONSTRUCT_ACCESS:
    Collision();
    virtual ~Collision() = default;
    void init(WorldObject* worldObject, PhysicsObject* physicsObject);

private:
    WorldObject* m_worldObject;
    PhysicsObject* m_physicsObject;
};

class RigidBodyComponent
	: public Component
	, protected detail::IPhysicsObjectEventListener
{
public:
	static Ref<RigidBodyComponent> create();

    void setVelocity(const Vector3& value) { m_body->setVelocity(value); }

    const Vector3& velocity() const { return m_body->velocity(); }

    void setMass(float value);

    /** キネマティックモードを設定します。 キネマティックモードでは、剛体に力はかかりません。 */
    void setKinematic(bool value) { m_body->setKinematic(value); }

    /** 摩擦係数を設定します。 */
    void setFriction(float value) { m_body->setFriction(value); }

    /** 反発係数を設定します。 */
    void setRestitution(float value) { m_body->setRestitution(value); }

    void addCollisionShape(CollisionShape* shape);

    /** 衝突グループを設定します。デフォルトは 0x00000001 で、0番のグループに属することを示します。 */
    void setCollisionGroup(uint32_t value) { m_body->setCollisionGroup(value); }

    /** 衝突グループマスクを設定します。デフォルトは 0x0000FFFF で、0～15番のグループと衝突することを示します。 */
    void setCollisionGroupMask(uint32_t value) { m_body->setCollisionGroupMask(value); }

    /** 重心に力を加えます。 */
    void applyForce(const Vector3& force) { m_body->applyForce(force); }

    /** 指定したローカル位置に力を加えます。 */
    void applyForce(const Vector3& force, const Vector3& localPosition) { m_body->applyForce(force, localPosition); }

    /** 重心に衝撃を与えます。 */
    void applyImpulse(const Vector3& impulse) { m_body->applyImpulse(impulse); }

    /** 指定したローカル位置に衝撃を与えます。 */
    void applyImpulse(const Vector3& impulse, const Vector3& localPosition) { m_body->applyImpulse(impulse, localPosition); }

    /** トルクをかけます。 */
    void applyTorque(Vector3 torque) { m_body->applyTorque(torque); }

    /** トルク衝撃を与えます。 */
    void applyTorqueImpulse(Vector3 torque) { m_body->applyTorqueImpulse(torque); }

    /** このコンポーネントと関連づいている RigidBody を取得します。 */
    RigidBody* rigidBody() const;
    

    /** onTriggerEnter イベントの通知を受け取るコールバックを登録します。*/
    //LN_METHOD(Event)
	Ref<EventConnection> connectOnCollisionEnter(Ref<CollisionEventHandler> handler);

    /** onTriggerLeave イベントの通知を受け取るコールバックを登録します。*/
    //LN_METHOD(Event)
	Ref<EventConnection> connectOnCollisionLeave(Ref<CollisionEventHandler> handler);

    /** onTriggerStay イベントの通知を受け取るコールバックを登録します。*/
    //LN_METHOD(Event)
	Ref<EventConnection> connectOnCollisionStay(Ref<CollisionEventHandler> handler);

LN_CONSTRUCT_ACCESS:
	RigidBodyComponent();
	virtual ~RigidBodyComponent() = default;
	void init();
	virtual void onDispose(bool explicitDisposing) override;

protected:
	virtual void onAttachedScene(Level* newOwner) override;
	virtual void onDetachedScene(Level* oldOwner) override;
	virtual void onBeforeStepSimulation() override;
	virtual void onAfterStepSimulation() override;
    virtual void onCollisionEnter(PhysicsObject* otherObject, ContactPoint* contact) override;
    virtual void onCollisionLeave(PhysicsObject* otherObject, ContactPoint* contact) override;
    virtual void onCollisionStay(PhysicsObject* otherObject, ContactPoint* contact) override;

private:
	Ref<RigidBody> m_body;
    Event<CollisionEventHandler> m_onCollisionEnter;
    Event<CollisionEventHandler> m_onCollisionLeave;
    Event<CollisionEventHandler> m_onCollisionStay;
};

class TriggerBodyComponent
    : public Component
    , protected detail::IPhysicsObjectEventListener
{
public:
    static Ref<TriggerBodyComponent> create();

    void addCollisionShape(CollisionShape* shape);

    /** 衝突グループマスクを設定します。デフォルトは 0x0000FFFF で、0～15番のグループと衝突することを示します。 */
    void setCollisionGroupMask(uint32_t value) { m_body->setCollisionGroupMask(value); }



    /** onTriggerEnter イベントの通知を受け取るコールバックを登録します。*/
    //LN_METHOD(Event)
	Ref<EventConnection> connectOnCollisionEnter(Ref<CollisionEventHandler> handler);

    /** onTriggerLeave イベントの通知を受け取るコールバックを登録します。*/
    //LN_METHOD(Event)
	Ref<EventConnection> connectOnCollisionLeave(Ref<CollisionEventHandler> handler);

    /** onTriggerStay イベントの通知を受け取るコールバックを登録します。*/
    //LN_METHOD(Event)
	Ref<EventConnection> connectOnCollisionStay(Ref<CollisionEventHandler> handler);

LN_CONSTRUCT_ACCESS:
    TriggerBodyComponent();
    virtual ~TriggerBodyComponent() = default;
    void init();
    virtual void onDispose(bool explicitDisposing) override;

protected:
    virtual void onAttachedScene(Level* newOwner) override;
    virtual void onDetachedScene(Level* oldOwner) override;
    virtual void onBeforeStepSimulation() override;
    virtual void onAfterStepSimulation() override;
    virtual void onCollisionEnter(PhysicsObject* otherObject, ContactPoint* contact) override;
    virtual void onCollisionLeave(PhysicsObject* otherObject, ContactPoint* contact) override;
    virtual void onCollisionStay(PhysicsObject* otherObject, ContactPoint* contact) override;

private:
    Ref<TriggerBody> m_body;
    Event<CollisionEventHandler> m_onCollisionEnter;
    Event<CollisionEventHandler> m_onCollisionLeave;
    Event<CollisionEventHandler> m_onCollisionStay;
};

} // namespace ln
﻿
#include "Internal.hpp"
#include "../Rendering/RenderStage.hpp"
#include <LuminoEngine/Animation/AnimationContext.hpp>
#include <LuminoEngine/Physics/PhysicsWorld.hpp>
#include <LuminoEngine/Physics/PhysicsWorld2D.hpp>
#include <LuminoEngine/Effect/EffectContext.hpp>
#include <LuminoEngine/ImageEffect/TransitionImageEffect.hpp>
#include <LuminoEngine/Scene/Component.hpp>
#include <LuminoEngine/Scene/Scene.hpp>
#include <LuminoEngine/Scene/SceneConductor.hpp>
#include <LuminoEngine/Scene/WorldObject.hpp>
#include <LuminoEngine/Scene/Light.hpp>
#include <LuminoEngine/Scene/World.hpp>

namespace ln {

//==============================================================================
// World

LN_OBJECT_IMPLEMENT(World, Object) {}

World::World()
	: m_masterScene(makeObject<Level>())
    , m_sceneList(makeList<Ref<Level>>())
    , m_timeScale(1.0f)
{
    m_masterScene->m_ownerWorld = this;
    m_masterScene->m_initialUpdate = true;
}

World::~World()
{
}

void World::init()
{
    Object::init();
    m_animationContext = makeObject<AnimationContext>();
    m_physicsWorld = makeObject<PhysicsWorld>();
	m_physicsWorld2D = makeObject<PhysicsWorld2D>();
    m_effectContext = makeObject<EffectContext>();
    m_renderingContext = makeRef<detail::WorldSceneGraphRenderingContext>();

    m_sceneConductor = makeRef<detail::SceneConductor>();

	m_mainDirectionalLight = makeObject<DirectionalLight>();
	add(m_mainDirectionalLight);
	m_mainAmbientLight = makeObject<AmbientLight>();
	add(m_mainAmbientLight);
}

void World::onDispose(bool explicitDisposing)
{
    removeAllObjects();

    if (m_sceneConductor) {
        m_sceneConductor->releaseAndTerminateAllRunningScenes();
    }

    if (m_mainAmbientLight) {
        //m_mainAmbientLight->dispose();
        m_mainAmbientLight = nullptr;
    }

    if (m_mainDirectionalLight) {
        //m_mainDirectionalLight->dispose();
        m_mainDirectionalLight = nullptr;
    }

    if (m_effectContext) {
        m_effectContext->dispose();
        m_effectContext = nullptr;
    }

    m_renderingContext.reset();
    m_physicsWorld.reset();
	m_physicsWorld2D.reset();
    m_animationContext.reset();
    Object::onDispose(explicitDisposing);
}

void World::add(WorldObject* obj)
{
    masterScene()->addObject(obj);
}

// TODO: テスト用。置き場考えておく。
void World::removeAllObjects()
{
    masterScene()->removeAllObjects();
}

ReadOnlyList<Ref<WorldObject>>* World::rootObjects() const
{
	return masterScene()->m_rootWorldObjectList;
}

void World::setMainAmbientLight(AmbientLight* value)
{
	m_mainAmbientLight = value;
}

void World::setMainDirectionalLight(DirectionalLight* value)
{
	m_mainDirectionalLight = value;
}

WorldObject* World::findObjectByComponentType(const TypeInfo* type) const
{
    for (auto& scene : m_sceneList) {
        auto obj = scene->findObjectByComponentType(type);
        if (obj) {
            return obj;
        }
    }

    return nullptr;
}

Level* World::masterScene() const
{
    return m_masterScene;
}

void World::addScene(Level* scene)
{
	if (LN_REQUIRE(scene)) return;
	if (LN_REQUIRE(!scene->m_ownerWorld)) return;
	m_sceneList->add(scene);
	scene->m_ownerWorld = this;
	scene->m_initialUpdate = true;
}

void World::gotoScene(Level* scene)
{
    m_sceneConductor->gotoScene(scene);
}

void World::callScene(Level* scene)
{
    m_sceneConductor->callScene(scene);
}

void World::returnScene()
{
    m_sceneConductor->returnScene();
}

Level* World::activeScene() const
{
    return m_sceneConductor->activeScene();
}

void World::updateObjectsWorldMatrix()
{
    m_masterScene->updateObjectsWorldMatrix();
    for (auto& scene : m_sceneList) {
        scene->updateObjectsWorldMatrix();
    }

    if (auto* scene = m_sceneConductor->activeScene()) {
        scene->updateObjectsWorldMatrix();
    }
}

void World::updateFrame(float elapsedSeconds)
{
    m_sceneConductor->executeCommands();

    float t = elapsedSeconds * m_timeScale;
    onPreUpdate(t);
    onInternalPhysicsUpdate(t);
    onUpdate(t);
    onInternalAnimationUpdate(t);
    onPostUpdate(t);
}

void World::onPreUpdate(float elapsedSeconds)
{
    m_effectContext->update(elapsedSeconds);

    m_masterScene->onPreUpdate(elapsedSeconds);
    for (auto& scene : m_sceneList) {
        scene->onPreUpdate(elapsedSeconds);
    }
    if (auto* scene = m_sceneConductor->activeScene()) {
        scene->onPreUpdate(elapsedSeconds);
    }
}

void World::onInternalPhysicsUpdate(float elapsedSeconds)
{
    // Physics モジュールの Component が、WorldObject の WorldMatrix を元にシミュレーション前準備を行うことがあるので
    // ここで WorldMatrix を更新しておく。
    updateObjectsWorldMatrix();

    if (m_physicsWorld) {
        m_physicsWorld->stepSimulation(elapsedSeconds);
    }
	if (m_physicsWorld2D) {
		m_physicsWorld2D->stepSimulation(elapsedSeconds);
	}
}

void World::onUpdate(float elapsedSeconds)
{
    m_masterScene->update(elapsedSeconds);
    for (auto& scene : m_sceneList) {
        scene->update(elapsedSeconds);
    }
    if (auto* scene = m_sceneConductor->activeScene()) {
        scene->update(elapsedSeconds);
    }
}

void World::onInternalAnimationUpdate(float elapsedSeconds)
{
}

void World::onPostUpdate(float elapsedSeconds)
{
    m_masterScene->onPostUpdate(elapsedSeconds);
    for (auto& scene : m_sceneList) {
        scene->onPostUpdate(elapsedSeconds);
    }
    if (auto* scene = m_sceneConductor->activeScene()) {
        scene->onPostUpdate(elapsedSeconds);
    }
}

//void World::serialize(Archive& ar)
//{
//    Object::serialize(ar);
//
//    ar & ln::makeNVP(u"Children", *m_rootWorldObjectList);
//
//    if (ar.isLoading()) {
//        for (auto& obj : m_rootWorldObjectList) {
//            obj->attachWorld(this);
//        }
//    }
//}

detail::WorldSceneGraphRenderingContext* World::prepareRender(RenderViewPoint* viewPoint)
{
	m_renderingContext->resetForBeginRendering();
	m_renderingContext->setViewPoint(viewPoint);
	return m_renderingContext;
}

void World::renderObjects()
{
	m_renderingContext->world = this;
    m_masterScene->renderObjects(m_renderingContext);
    for (auto& scene : m_sceneList) {
        scene->renderObjects(m_renderingContext);
    }
    if (auto* scene = m_sceneConductor->activeScene()) {
        scene->renderObjects(m_renderingContext);
    }

	m_renderingContext->pushState(true);
    m_effectContext->render(m_renderingContext);
    m_renderingContext->popState();

    m_renderingContext->collectImageEffect(m_sceneConductor->transitionEffect());
}

void World::renderGizmos(RenderingContext* context)
{
    m_masterScene->renderGizmos(context);
    for (auto& scene : m_sceneList) {
        scene->renderGizmos(context);
    }
    if (auto* scene = m_sceneConductor->activeScene()) {
        scene->renderGizmos(context);
    }
}

//==============================================================================
// WorldSceneGraphRenderingContext

namespace detail {

WorldSceneGraphRenderingContext::WorldSceneGraphRenderingContext()
	: m_elementList(makeRef<detail::DrawElementList>(detail::EngineDomain::renderingManager()))
{
	setDrawElementList(m_elementList);
}

void WorldSceneGraphRenderingContext::resetForBeginRendering()
{
	RenderingContext::resetForBeginRendering();
	m_elementList->clear();
}

} // namespace detail

} // namespace ln


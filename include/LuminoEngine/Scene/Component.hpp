﻿
#pragma once
#include "Common.hpp"

namespace ln {
class World;
class WorldObject;
class RenderingContext;
namespace detail {
    class WorldObjectTransform;
}

class Component
	: public Object
{
    LN_OBJECT;
protected:
    // アタッチされた WorldObject の transform へのポインタ
    detail::WorldObjectTransform* transrom() const;

    WorldObject* worldObject() const { return m_object; }


    // 以下、すべて空実装
    virtual void onAttached(WorldObject* owner);
    virtual void onDetaching(WorldObject* owner);
    virtual void onUpdate(float elapsedSeconds);
    virtual void onPrepareRender(RenderingContext* context);
    virtual void onRender(RenderingContext* context);

LN_CONSTRUCT_ACCESS:
	Component();
	virtual ~Component();
	void initialize();

public:
    WorldObject* m_object;

private:
    virtual void render(RenderingContext* context);

    friend class World;
    friend class WorldObject;
};

} // namespace ln

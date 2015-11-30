
#pragma once
#include "Common.h"
#include "../Variant.h"

LN_NAMESPACE_BEGIN

/**
	@brief	グラフィックスモジュールの基本的なリソースオブジェクトのベースクラスです。デバイスの状態変化を通知する機能を実装します。
*/
class GraphicsResourceObject
	: public tr::ReflectionObject
{
protected:
	friend class GraphicsManager;

	void Initialize(GraphicsManager* manager);

	// デバイスが変更される場合、まずは NULL が渡されて呼ばれる。このとき、必要なリソースを保存する。
	// 次に新しいデバイスが渡されて呼ばれる。このとき、保存したリソースをデバイスオブジェクトにロードする。
	virtual void OnChangeDevice(Driver::IGraphicsDevice* device) = 0;
	
	virtual ~GraphicsResourceObject();


	GraphicsManager*	m_manager;
};

LN_NAMESPACE_END

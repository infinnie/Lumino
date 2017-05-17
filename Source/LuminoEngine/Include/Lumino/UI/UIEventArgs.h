﻿
#pragma once
#include "Common.h"
#include <Lumino/Reflection/ReflectionEventArgs.h>

LN_NAMESPACE_BEGIN
class UIEventArgs;
using UIEventArgsPtr = RefPtr<UIEventArgs>;
class UIEventInfo;
using UIEventType = const UIEventInfo*;

/**
	@brief		
*/
class UIEventArgs
	: public tr::ReflectionEventArgs
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	static UIEventArgsPtr Create(UIEventType type, Object* sender, bool caching = true);

	UIEventType GetType() const { return m_type; }

	UIEventArgs();
	virtual ~UIEventArgs();
	void Initialize(UIEventType type);

public:
	UIElement*	sender;

private:
	UIEventType m_type;

	friend class UIElement;
};

/**
	@brief	マウスイベントの引数です。
*/
class UIMouseEventArgs
	: public UIEventArgs
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:

	/** UIMouseEventArgs のインスタンスを作成します。*/
	static RefPtr<UIMouseEventArgs> Create(UIEventType type, MouseButtons button, float x, float y, int clickCount, bool caching = true);

	/** ボタンの種類を取得します。*/
	MouseButtons GetMouseButtons() const { return m_button; }

	/** マウスイベント生成時のマウスの座標を取得します。*/
	const PointF& GetPosition() const { return m_position; }

	/** ボタンがクリックされた回数を取得します。(ダブルクリックやトリプルクリックを区別するために使用する。最大3まで) */
	int GetClickCount() const { return m_clickCount; }

	/** 指定した要素から見た相対的なマウス ポインターの位置を返します。*/
	PointF GetPosition(UIElement* relativeTo) const;

LN_CONSTRUCT_ACCESS:
	UIMouseEventArgs();
	virtual ~UIMouseEventArgs();
	void Initialize(UIEventType type, MouseButtons button, float x, float y, int clickCount);

private:
	MouseButtons	m_button;
	PointF			m_position;
	int				m_clickCount;
};

/**
	@brief	キーボードイベントの引数です。
*/
class UIKeyEventArgs
	: public UIEventArgs
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:

	/** UIKeyEventArgs のインスタンスを作成します。 */
	static RefPtr<UIKeyEventArgs> Create(UIEventType type, Keys keyCode, ModifierKeys modifierKeys, TCHAR charCode, bool caching = true);

	/** キーコードを取得します。 */
	Keys GetKey() const { return m_keyCode; }

	/** 修飾キーを取得します。 */
	ModifierKeys GetModifierKeys() const { return m_modifierKeys; }

	/** TextInput イベントにより通知された場合、その文字コードを取得します。 */
	TCHAR GetCharCode() const { return m_charCode; }

LN_CONSTRUCT_ACCESS:
	UIKeyEventArgs();
	virtual ~UIKeyEventArgs();
	void Initialize(UIEventType type, Keys keyCode, ModifierKeys modifierKeys, TCHAR charCode);

private:
	Keys			m_keyCode;
	ModifierKeys	m_modifierKeys;
	TCHAR			m_charCode;
};

/**
	@brief	マウスホイールの移動イベントの引数です。
*/
class UIMouseWheelEventArgs
	: public UIEventArgs
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:

	/** UIMouseWheelEventArgs のインスタンスを作成します。 */
	static RefPtr<UIMouseWheelEventArgs> Create(UIEventType type, int delta, bool caching = true);

	/** マウスホイールの回転回数を取得します。 (正または負の回数) */
	int GetDelta() const { return m_delta; }

LN_CONSTRUCT_ACCESS:
	UIMouseWheelEventArgs();
	virtual ~UIMouseWheelEventArgs();
	void Initialize(UIEventType type, int delta);

public:
	int		m_delta;
};

LN_NAMESPACE_END

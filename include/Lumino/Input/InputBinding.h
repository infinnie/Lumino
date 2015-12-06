
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class InputBinding
	: public tr::ReflectionObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:

	/**
		@brief		InputBinding �I�u�W�F�N�g���쐬���܂��B
		@details	�쐬���ꂽ�I�u�W�F�N�g�͎g���I������ Release() ���Ăяo���ĎQ�Ƃ��������K�v������܂��B
	*/
	static InputBinding* Create(const String& bindingName, Key key, int joyNumber);

public:

	const String& GetName() const { return m_bindingName; }

	float GetMinValidMThreshold() const { return m_minValidMThreshold; }

	bool IsNegativeValue() const { return m_negativeValue; }

LN_INTERNAL_ACCESS:
	InputBinding();
	InputBinding(const String& bindingName, Key key);
	virtual ~InputBinding();
	void Initialize(detail::InputManager* manager);
	const detail::DeviceInputSource& GetDeviceInputSource() const { return m_source; }

private:
	String						m_bindingName;
	detail::DeviceInputSource	m_source;
	float						m_minValidMThreshold;
	bool						m_negativeValue;
};

LN_NAMESPACE_END
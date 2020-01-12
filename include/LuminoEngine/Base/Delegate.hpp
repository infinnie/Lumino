﻿
#pragma once
#include <functional>
#include "../Engine/Object.hpp"

namespace ln {

// base for specialization
template<typename>
class Delegate;

/**
 */
template<class TReturn, class... TArgs>
class Delegate<TReturn(TArgs...)>
	: public Object
{
public:
	Delegate(const std::function<TReturn(TArgs...)>& function)
		: m_function(function)
	{}

	bool isEmpty() const
	{
		return !m_function;
	}

	TReturn call(TArgs... args)
	{
		return m_function(std::forward<TArgs>(args)...);
	}

private:
	std::function<TReturn(TArgs...)> m_function;
};

} // namespace ln
﻿
#pragma once
#include "../CoreObject.h"
#include "Inline.h"

namespace Lumino
{
namespace Documents
{

/**
	@brief
*/
class Span
	: public Inline
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
public:
	Span(DocumentsManager* manager);
	virtual ~Span();

private:
	GenericCoreList<Inline*>	m_inlineList;
};


} // namespace Documents
} // namespace Lumino

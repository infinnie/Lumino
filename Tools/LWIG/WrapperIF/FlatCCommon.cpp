
#include "../SymbolDatabase.h"
#include "FlatCCommon.h"

String FlatCCommon::MakeCppTypeName(Ref<TypeSymbol> typeInfo)
{
	if (typeInfo->IsClass())
	{
		return typeInfo->shortName() + _T("*");
	}

	return typeInfo->shortName();
}

String FlatCCommon::MakeCApiParamTypeName(Ref<MethodSymbol> methodInfo, Ref<ParameterSymbol> paramInfo)
{
	auto typeInfo = paramInfo->type;

	if (typeInfo->isStruct)
	{
		String modifer;
		if (paramInfo->isThis && methodInfo->isConst)
			modifer = "const ";
		if (!paramInfo->isThis && !paramInfo->isOut)
			modifer = "const ";
		return String::format("{0}LN{1}*", modifer, typeInfo->shortName());
	}

	String name;
	if (typeInfo == PredefinedTypes::stringType)
		name = "const LNChar*";
	else if (typeInfo->IsClass())
		name = "LNHandle";
	else
		name = typeInfo->shortName();

	if (paramInfo->isOut || paramInfo->isReturn)
		name += "*";

	//if (typeInfo->isStruct && paramInfo->isThis)
	//{
	//	if (methodInfo->isConst)
	//		name = "const " + name;
	//	name += "*";
	//}

	return name;
	//	String name;
	//
	//	if (isOut)
	//	{
	//		name = typeInfo->name;
	//		name += "*";
	//	}
	//	else
	//	{
	//		if (typeInfo == PredefinedTypes::stringType)
	//			name = "const LNChar*";
	//		else if (typeInfo->IsClass())
	//			name = "LNHandle";
	//		else
	//			name = typeInfo->name;
	//	}
	//	return name;
}

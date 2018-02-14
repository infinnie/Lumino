
#pragma once
#include "../Generator.h"
class SymbolDatabase;

/** C言語の用の API を出力する */
class WrapperIFGenerator
	: public Generator
{
public:
	WrapperIFGenerator();
	void generate();


private:
	String GenerateOverrideCallerFuncPtrs();

	String MakeInstanceParamName(Ref<TypeInfo> info);
	//String MakeMethods(Ref<TypeInfo> typeInfo);
	String MakeMethodHeader(Ref<MethodInfo> methodInfo);
	String MakeMethod(Ref<TypeInfo> typeInfo, Ref<MethodInfo> methodInfo);

	String MakeDocumentComment(Ref<DocumentInfo> doc);


};


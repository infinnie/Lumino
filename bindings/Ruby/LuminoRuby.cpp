#include "LuminoRuby.h"

VALUE g_luminoModule;

// 関数名は create_makefile で指定した名前と合わせておかなければならない
extern "C" void Init_Lumino()
{
	g_luminoModule = rb_define_module("Lumino");
	InitEnums();
	InitStructs();
	InitClasses();
}



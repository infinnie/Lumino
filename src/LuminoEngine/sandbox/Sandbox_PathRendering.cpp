﻿
#include <LuminoEngine.hpp>
using namespace ln;

class TestElement : public UIElement
{
public:
    void onRender(UIRenderingContext* context) override
    {

    }

private:
};

class App_Sandbox_PathRendering : public Application
{

    void onInit() override
    {
        auto e = makeObject<TestElement>();
        UI::add(e);
    }

    void onUpdate() override
    {
    }
};

void Sandbox_PathRendering()
{
    App_Sandbox_PathRendering app;
	detail::ApplicationHelper::run(&app);
}





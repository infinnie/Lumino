﻿
#include <LuminoEngine.hpp>
#include <LuminoEngine/UI/UIComboBox.hpp>
using namespace ln;

class App_Example_GameAudio : public Application
{
	int m_count = 0;

    virtual void onInit() override
    {
		auto sprite = UISprite::load(u"picture1.jpg");
		sprite->setAlignments(HAlignment::Left, VAlignment::Top);
		sprite->setPosition(100, 50);
		Engine::ui()->add(sprite);



		auto addButton = ln::UIButton::create(u"Play BGM");
		addButton->connectOnClicked([this]() {
			GameAudio::playBGM(u"D:/Music/momentum/02 - momentum.wav", 0.5, 1.0/*, 3.0*/);
			//GameAudio::playBGM(u"C:/Proj/LN/Lumino/src/LuminoEngine/test/Assets/Audio/sin_440_3s_S16L_48000_2ch.wav", 0.1);
		});
		//Engine::mainUIView()->addChild(addButton);

		addButton->setAlignments(HAlignment::Left, VAlignment::Top);
		addButton->setPosition(0, 0);
    }

    virtual void onUpdate() override
    {
		m_count++;


		if (m_count > 60) {
			//GameAudio::playSE(u"C:/Proj/LN/Lumino/src/LuminoEngine/sandbox/Assets/Audio/ln_cursor_1.wav", 0.1);
			m_count = 0;
		}

    }
};

void Example_GameAudio()
{
	App_Example_GameAudio app;
	detail::ApplicationHelper::run(&app);
}





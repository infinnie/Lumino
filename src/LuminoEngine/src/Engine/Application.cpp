﻿
#include "Internal.hpp"
#include <yaml-cpp/yaml.h>
#include <optional>
#include "EngineManager.hpp"
#include <LuminoEngine/Scene/World.hpp>
#include <LuminoEngine/UI/UICommand.hpp>
#include <LuminoEngine/UI/UIElement.hpp>
#include <LuminoEngine/Engine/Application.hpp>
#include "UI/UIManager.hpp"

namespace ln {

//==============================================================================
// Application

LN_OBJECT_IMPLEMENT(Application, Object) {}

Application::Application()
	: m_manager(nullptr)
	, m_commands()
	, m_actions()
	, m_initialized(false)
{
    detail::EngineManager::s_settings.application = this;
}

Application::~Application()
{
}

void Application::init()
{
    Object::init();
}

void Application::setupMainWindow(ln::UIMainWindow* window, bool createBasicObjects)
{
	return ln::detail::EngineDomain::engineManager()->setupMainWindow(window, createBasicObjects);
}

void Application::onInit()
{
}

void Application::onStart()
{
}

void Application::onUpdate()
{
}

void Application::onStop()
{
}

void Application::onDestroy()
{
}

void Application::addApplicationCommand(UICommand* command)
{
    m_commands.add(command);
}

void Application::addAction(UIAction* action)
{
    m_actions.add(action);
}

World* Application::world() const
{
	return m_manager->mainWorld();
}

void Application::onRoutedEvent(UIEventArgs* e)
{
    // ショートカットキーの実装
    for (auto& c : m_commands) {
        if (c->testInputEvent(e)) {
            e->handled = true;
            UICommandEventArgs::raiseExecute(detail::EngineDomain::uiManager()->forcusedElement(), c);
            return;
        }
    }

	if (detail::EngineDomain::uiManager()->handleCommonInputCommands(e)) {
		return;
	}


    if (detail::UICommandInternal::handleCommandRoutedEvent(e, m_actions)) {
        return;
    }
}

void Application::initInternal2()
{
	//EngineContext::current()->initializeEngineManager();
	//EngineContext::current()->engineManager()->initializeAllManagers();
	m_manager = detail::EngineDomain::engineManager();
	onInit();
	onStart();
}

void Application::updateInertnal2()
{
	if (!m_initialized) {
		initInternal2();
		m_initialized = true;
	}

	onUpdate();
	//return !detail::EngineDomain::engineManager()->isExitRequested();
}

void Application::finalizeInternal2()
{
	onStop();
	onDestroy();
}

void Application::run()
{
#ifdef __EMSCRIPTEN__
	LN_UNREACHABLE();
#endif
	//initInternal();
	EngineContext::current()->initializeEngineManager();
	EngineContext::current()->engineManager()->initializeAllManagers();

	do {

		detail::EngineDomain::engineManager()->updateFrame();
		detail::EngineDomain::engineManager()->presentFrame();
		//detail::EngineDomain::engineManager()->renderFrame();
	} while (!detail::EngineDomain::engineManager()->isExitRequested());

	detail::EngineDomain::release();
	//finalizeInternal();
}

//==============================================================================
// AppData

void AppData::setValue(const StringRef& key, Ref<Variant> value)
{
	detail::EngineDomain::engineManager()->appData()->setValue(key, value);
}

Ref<Variant> AppData::getValue(const StringRef& key)
{
	return detail::EngineDomain::engineManager()->appData()->getValue(key);
}


//==============================================================================
// ApplicationHelper

namespace detail {

void ApplicationHelper::init(Application* app)
{
	//app->initInternal();
}

bool ApplicationHelper::processTick(Application* app)
{
	detail::EngineDomain::engineManager()->updateFrame();
	return !detail::EngineDomain::engineManager()->isExitRequested();
	//return app->updateInertnal();
}

void ApplicationHelper::finalize(Application* app)
{
	detail::EngineDomain::release();
	//app->finalizeInternal();
}

void ApplicationHelper::run(Application* app)
{
	app->run();
}

//==============================================================================
// AppDataInternal

void AppDataInternal::setValue(const StringRef& key, Ref<Variant> value)
{
	m_values[key] = value;
}

Ref<Variant> AppDataInternal::getValue(const StringRef& key) const
{
	auto itr = m_values.find(key);
	if (itr != m_values.end())
		return itr->second;
	else
		return nullptr;
}

void AppDataInternal::attemptSave()
{
	if (!m_values.empty()) {
		save(makeFilePath());
	}
}

void AppDataInternal::attemptLoad()
{
	const auto path = makeFilePath();
	if (FileSystem::existsFile(path)) {
		load(path);
	}
}

Path AppDataInternal::makeFilePath() const
{
	return Path::combine(Environment::specialFolderPath(SpecialFolder::ApplicationData), u"Lumino", u"CommonAppData.yml");
}

void AppDataInternal::save(const Path& filePath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	for (const auto& pair : m_values) {
		out << YAML::Key;
		out << pair.first.toStdString();

		if (pair.second->type() == VariantType::Int) {
			out << pair.second->get<int>();
		}
		else if (pair.second->type() == VariantType::Float) {
			out << pair.second->get<float>();
		}
		else if (pair.second->type() == VariantType::String) {
			out << pair.second->get<String>().toStdString();
		}
	}
	out << YAML::EndMap;
	
	FileSystem::writeAllBytes(filePath, out.c_str(), strlen(out.c_str()));
}

// TODO: cpp-yaml は値の型を読み取ることができない。
// try で頑張る必要があるが、これも暗黙の型変換が働いたりするため float, int, string 以上に増やすと対応できなくなる可能性が高い。
// JSON にしたほうがよさそう。
void AppDataInternal::load(const Path& filePath)
{
	const auto buffer = FileSystem::readAllBytes(filePath);
	const auto text = std::string(reinterpret_cast<const char*>(buffer.data()), buffer.size());
	YAML::Node doc = YAML::Load(text);

	for (auto itr = doc.begin(); itr != doc.end(); ++itr) {
		std::string key;
		key = itr->first.as<std::string>();

		const YAML::Node node = itr->second;
		Ref<Variant> value;
		{
			try {
				value = makeVariant(node.as<float>());
			}
			catch (const YAML::BadConversion& e) {
			}
		}
		if (!value) {
			try {
				value = makeVariant(node.as<int>());
			}
			catch (const YAML::BadConversion& e) {
			}
		}
		if (!value) {
			try {
				value = makeVariant(String::fromStdString(node.as<std::string>()));
			}
			catch (const YAML::BadConversion& e) {
			}
		}

		m_values[String::fromStdString(key)] = value;
	}

}

} // namespace detail
} // namespace ln


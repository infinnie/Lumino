﻿
#include "../../LuminoEngine/src/Engine/EngineManager.hpp"
#include "../../LuminoEngine/src/Engine/EngineDomain.hpp"
#include "../../LuminoEngine/src/Platform/PlatformManager.hpp"
#include "../../LuminoEngine/src/UI/UIManager.hpp"
#include <AppData.hpp>
#include <EnvironmentSettings.hpp>
#include <Workspace.hpp>
#include <Project.hpp>
#include <AssetDatabase.hpp>
#include <PluginManager.hpp>
#include "NavigatorManager.hpp"
#include "DocumentManager.hpp"
#include "MainWindow.hpp"
#include "Application.hpp"
#include "NewProjectDialog.hpp"

Ref<ln::UICommand> EditorApplication::NewCommand;
Ref<ln::UICommand> EditorApplication::OpenCommand;

static EditorApplication* s_app = nullptr;

EditorApplication* EditorApplication::instance()
{
    assert(s_app);
    return s_app;
}

EditorApplication::EditorApplication()
{
    assert(!s_app);
    s_app = this;

    NewCommand = ln::makeObject<ln::UICommand>();
    OpenCommand = ln::makeObject<ln::UICommand>();

    addAction(ln::makeObject<ln::UIAction>(NewCommand, [this](ln::UICommandEventArgs* x){ handleNewProject(x); }));
    addAction(ln::makeObject<ln::UIAction>(OpenCommand, [this](ln::UICommandEventArgs* x) { handleOpenProject(x); }));
}

EditorApplication::~EditorApplication()
{
    s_app = nullptr;
}

ln::Result EditorApplication::init()
{
    lna::AppData::instance()->load();

	ln::EngineSettings::setMainWindowSize(1600, 800);
	ln::EngineSettings::setMainBackBufferSize(1600, 800);
    ln::EngineSettings::setAssetStorageAccessPriority(ln::AssetStorageAccessPriority::AllowLocalDirectory);
    ln::EngineSettings::setGraphicsAPI(ln::GraphicsAPI::Vulkan);
    ln::detail::EngineDomain::engineManager()->settings().defaultObjectsCreation = false;
    ln::detail::EngineDomain::engineManager()->init();

	auto root = lna::BuildEnvironment::findRepositoryRootForTesting();
    ln::Font::registerFontFromFile(ln::Path(root, u"tools/mplus-font/mplus-1c-regular.ttf"));

    m_workspace = ln::makeObject<lna::Workspace>();
    onInit();

    m_editorContext = ln::makeObject<lna::EditorContext>();
    m_editorContext->m_application = this;
    m_editorContext->m_mainWindow = mainWindow();

    // TODO: test
    openProject(u"D:/Proj/LN/PrivateProjects/HC0/HC0.lnproj");

    return true;
}

void EditorApplication::dispose()
{
    ln::detail::EngineDomain::release();
    lna::AppData::instance()->save();
}

void EditorApplication::run()
{
	uint64_t lastTime = ln::Environment::getTickCount();

    while (!ln::detail::EngineDomain::engineManager()->isExitRequested())
    {
        ln::detail::EngineDomain::engineManager()->platformManager()->windowManager()->processSystemEventQueue(ln::detail::EventProcessingMode::Polling);
        ln::detail::EngineDomain::engineManager()->uiManager()->dispatchPostedEvents();

		float elapsedSeconds = static_cast<float>(ln::Environment::getTickCount() - lastTime) / 1000.0f;
		ln::detail::EngineDomain::engineManager()->uiManager()->tickGlobal(elapsedSeconds);

        ln::Thread::sleep(1);
    }
}

MainWindow* EditorApplication::mainWindow() const
{
    auto ptr = ln::static_pointer_cast<MainWindow>(ln::detail::EngineDomain::engineManager()->mainWindow());
    return ptr;
}

void EditorApplication::setMainWindow(ln::UIMainWindow* window)
{
	return ln::detail::EngineDomain::engineManager()->setMainWindow(window);
}

lna::Project* EditorApplication::mainProject() const
{
    return m_workspace->mainProject();
}

// deprecated
void EditorApplication::importFile(const ln::Path& filePath)
{
    assert(0);
    if (m_workspace->mainProject())
    {
        auto exts = m_workspace->mainPluginManager()->getAssetImporterExtensions(filePath);
        if (exts.size() != 1) {
            LN_NOTIMPLEMENTED();
            return;
        }

        exts[0].second->import(filePath);
    }
}

void EditorApplication::openAssetFile(const ln::Path& filePath)
{
    if (m_workspace->mainProject())
    {
        auto asset = m_workspace->mainAssetDatabase()->openAsset(filePath);
        auto proxies = m_workspace->mainPluginManager()->geAssetEditorPloxy(asset->assetType());
        if (proxies.size() != 1) {
            LN_NOTIMPLEMENTED();
            return;
        }

        auto editor = proxies[0]->createEditor();
        editor->m_editorContext = m_editorContext;
        mainWindow()->documentManager()->addDocument(ln::makeObject<AssetEditorDocument>(asset, editor));
    }
}

void EditorApplication::onInit()
{
	setMainWindow(ln::makeObject<MainWindow>());

    auto sheet = ln::makeObject<ln::UIStyleSheet>();
	if (auto s = sheet->obtainStyle(u"NavigationBarItem")) {
		s->borderThickness = ln::Thickness(4, 0, 0, 0);
		s->borderInset = true;
	}
    if (auto s = sheet->obtainStyle(u"NavigationBarItem:Unselected")) {
        s->setBorderColor(ln::Color::Transparency);
        s->textColor = ln::UIColors::get(ln::UIColorHues::Grey, 4);
    }
    if (auto s = sheet->obtainStyle(u"NavigationBarItem:Selected")) {
        s->setBorderColor(ln::UIColors::get(ln::UIColorHues::LightGreen));
        s->textColor = ln::Color::White;
    }


    ln::Engine::mainUIContext()->styleContext()->addStyleSheet(sheet);
}

void EditorApplication::newProject(const ln::Path& dirPath, const ln::String& projectName)
{
	if (!closeProject()) {
		return;
	}

	m_workspace->newMainProject(ln::Path(dirPath, projectName), projectName);
	postProjectLoaded();
}

void EditorApplication::openProject(const ln::Path& filePath)
{
    //closeProject();

    lna::AppData::instance()->addRecentProjectFile(filePath);

    m_workspace->openMainProject(filePath);
	postProjectLoaded();
    //mainWindow()->mainHSplitter()->resetCellSize(0);

    //m_contentsViewManager->setup(m_workspace->project(), "ARPG-HC0");

    //m_assetBrowserContentsViewProvider = new AssetBrowserContentsViewProvider(m_workspace->project(), this);
    //m_contentsViewManager->addContentsViewProvider(m_assetBrowserContentsViewProvider);

    ////m_spritesetContentsViewProvider = new SpritesetContentsViewProvider(m_workspace->project(), this);
    ////m_contentsViewManager->addContentsViewProvider(m_spritesetContentsViewProvider);

    //m_audioContentsViewProvider->view()->setRootDir(
    //    LnToQt(ln::Path(m_workspace->project()->assetsDir(), u"Audio")));
}

bool EditorApplication::closeProject()
{
	bool result = mainWindow()->documentManager()->closeAllTabs();
	if (!result) {
		return false;
	}

	mainWindow()->navigatorManager()->unloadAdditionalNavigators();
    m_workspace->mainPluginManager()->deactivateAllExtensions(m_editorContext);
	m_workspace->closeMainProject();
	return true;
}

void EditorApplication::postProjectLoaded()
{

	mainWindow()->navigatorManager()->resetNavigators();

    m_workspace->mainPluginManager()->activateAllExtensions(m_editorContext);
}

void EditorApplication::handleNewProject(ln::UICommandEventArgs* e)
{
	auto dlg = ln::makeObject<NewProjectDialog>();
	mainWindow()->addElement(dlg);
	dlg->open();

#if 0
	auto popupContent = ln::makeObject<ln::UITextBlock>();
	popupContent->setText(u"POP");

    m_dialog = ln::makeObject<ln::UIDialog>();
	m_dialog->addElement(popupContent);
    m_dialog->setWidth(100);
    m_dialog->setHeight(200);
    m_dialog->setBackgroundColor(ln::Color::Green);
	mainWindow()->addElement(m_dialog);
	m_dialog->open();
    //mainWindow()->m_renderView->setDialog(m_dialog);

    //auto dlg = ln::PlatformSelectFolderDialog::create();
    //if (dlg->showDialog(mainWindow()->platformWindow())) {
    //}
#endif
}

void EditorApplication::handleOpenProject(ln::UICommandEventArgs* e)
{
    auto dlg = ln::PlatformOpenFileDialog::create();
    if (dlg->showDialog(mainWindow()->platformWindow())) {
        auto filePath = dlg->getPath();
        if (!filePath.isEmpty()) {
            openProject(filePath);
        }
    }
}

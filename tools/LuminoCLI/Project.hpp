﻿#pragma once

class LanguageContext;
class Project;
class Workspace;

class ProjectProperties
	: public ln::RefObject
{
public:
	ln::String language;	// "cpp", "cs" ...

	LN_SERIALIZE_CLASS_VERSION(1)
	void serialize(ln::Archive& ar)
	{
		ar & LN_NVP(language);
	}
};

class Project
	: public ln::RefObject
{
public:
	static const ln::String ProjectFileExt;

	Project(Workspace* owner);
	virtual ~Project();

	Result newProject(const ln::Path& projectDir, const ln::String& projectName);
	Result openProject(const ln::Path& dir);
	Result saveProject();

	Workspace* workspace() const { return m_workspace; }
	const ln::Path& engineDirPath() const { return m_engineDir; }
	const ln::Path& rootDirPath() const { return m_rootDir; }
	const ln::Path& buildDir() const { return m_buildDir; }

	ln::Path nativeProjectsDir() const { return ln::Path(rootDirPath(), u"NativeProjects"); }
	ln::Path emscriptenProjectDir() const { return ln::Path(nativeProjectsDir(), u"LuminoApp.Emscripten"); }
	ln::Path androidProjectDir() const { return ln::Path(nativeProjectsDir(), u"LuminoApp.Android"); }
	ln::Path macOSProjectDir() const { return ln::Path(nativeProjectsDir(), u"LuminoApp.macOS");  }

	/** 指定フォルダにプロジェクトファイルが含まれているかを確認する */
	static bool existsProjectFile(const ln::Path& dir);

private:
	void setupPathes();

	Workspace* m_workspace;
	Ref<ProjectProperties> m_properties;
	Ref<LanguageContext> m_context;

	ln::String m_projectName;
	ln::Path m_rootDir;
	ln::Path m_engineDir;
	ln::Path m_sourcesDir;
	ln::Path m_assetsDir;
	ln::Path m_buildDir;
};

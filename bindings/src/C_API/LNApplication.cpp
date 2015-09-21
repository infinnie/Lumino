﻿
#include "LNInternal.h"
#include "LNApplication.h"

extern "C" {

//=============================================================================
// LNApplication
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetApplicationLogEnabled(LNBool enabled)
{
	LFManager::ConfigData.ApplicationLogEnabled = LNC_TO_BOOL(enabled);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetConsoleEnabled(LNBool enabled)
{
	LFManager::ConfigData.ConsoleEnabled = LNC_TO_BOOL(enabled);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_RegisterArchive(const LNChar* filePath, const LNChar* password)
{
	ApplicationSettings::ArchiveFileEntry e;
	e.FilePath = filePath;
	e.Password = password;
	LFManager::ConfigData.ArchiveFileEntryList.Add(e);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetFileAccessPriority(LNFileAccessPriority priority)
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetUserWindowHandle(void* windowHandle)
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetSoundCacheSize(int count, int memorySize)
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetDirectMusicInitializeMode(LNDirectMusicMode mode)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNConfig_SetDirectMusicReverbLevel(int level)
{

}

//=============================================================================
// LNApplication
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LNResult LNApplication_Initialize()
{
	LN_FUNC_TRY_BEGIN;
	LFManager::PreInitialize();
	LFManager::Application->Initialize();
	LFManager::PostInitialize();
	LN_FUNC_TRY_END_RETURN;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LNResult LNApplication_InitializeAudio()
{
	LN_FUNC_TRY_BEGIN;
	LFManager::PreInitialize();
	LFManager::Application->InitialzeAudioManager();
	LFManager::PostInitialize();
	LN_FUNC_TRY_END_RETURN;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LNResult LNApplication_Update()
{
	LN_FUNC_TRY_BEGIN;
	LFManager::Application->UpdateFrame();
	LN_FUNC_TRY_END_RETURN;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LNResult LNApplication_ResetFrameDelay()
{
	LFManager::Application->ResetFrameDelay();
	return ::LN_OK;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LNResult LNApplication_IsEndRequested(LNBool* requested)
{
	*requested = LNOTE_BOOL_TO_LNBOOL(LFManager::Application->IsEndRequested());
	return ::LN_OK;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void LNApplication_Finalize()
{
	LFManager::Finalize();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void* LNApplication_GetInternalObject()
//{
//	return LFManager::CoreManager;
//}

} // extern "C"

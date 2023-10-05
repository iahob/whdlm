#include "StdAfx.h"
#include ".\sound.h"
#include ".\GameOption.h"


// 变量声明
CMusic			CMusic::m_Music;

// 构造函数
CMusic::CMusic()
{
	m_nMusicIndex = 0;
	m_bPlaying = false;
}

// 析构函数
CMusic::~CMusic()
{

}

// 创建环境
bool CMusic::CreateD3DSound( hwnd hWnd )
{
	return m_DexterSound.CreateD3DSound( hWnd, CMusic::SoundCallBack );
}

// 设置音量
void CMusic::SetVolume( int nVolume )
{
	return m_DexterSound.SetVolume( nVolume );
}

// 播放中
bool CMusic::IsPlaying()
{
	return m_bPlaying;
}

// 添加声音
void CMusic::AppendMusic(  const tchar * pszFilePath  )
{
	// 获取程序路径
	tchar szExePath[MAX_PATH + 1] = { _T("") };
	GetModuleFileName(NULL, szExePath, MAX_PATH);   
	PathRemoveFileSpec( szExePath );

	// 生成路径
	tchar szFile[MAX_PATH] = {0};
	_sntprintf(szFile, CountArray(szFile), TEXT("%s\\WaterMargin\\Sound\\%s"), szExePath, pszFilePath);

	// 添加路径
	m_ArrayMusic.Add( szFile );
}

// 清空声音
void CMusic::ClearMusic()
{
	m_nMusicIndex = 0;
	m_bPlaying = false;
	m_ArrayMusic.RemoveAll();
	m_DexterSound.StopSound();
}

// 停止声音
bool CMusic::StopMusic()
{
	m_bPlaying = false;
	return m_DexterSound.StopSound();
}

// 播放声音
bool CMusic::PlayMusic()
{
	// 无内容
	if ( m_ArrayMusic.GetCount() == 0 )
		return false;

	// 判断大小
	if ( m_nMusicIndex >= m_ArrayMusic.GetCount() )
		m_nMusicIndex = 0;

	// 设置变量
	m_bPlaying = true;

	// 播放
	return m_DexterSound.PlaySoundEx( m_ArrayMusic[m_nMusicIndex] );
}

// 下一个声音
bool CMusic::NextMusic()
{
	// 无内容
	if ( m_ArrayMusic.GetCount() == 0 )
		return false; 

	// 继续播放
	m_nMusicIndex = (int)((m_nMusicIndex + 1)%m_ArrayMusic.GetCount());

	// 播放
	return m_DexterSound.PlaySoundEx( m_ArrayMusic[m_nMusicIndex] );
}

// 回调函数
void CMusic::SoundCallBack( hwnd hWnd, guid nID, void * pData, uint nTime )
{
	// 换声音
	CMusic::_Object()->NextMusic();
}



// 变量声明
CSound			CSound::m_Sound;

// 构造函数
CSound::CSound()
{

}

// 析构函数
CSound::~CSound()
{

}

// 创建环境
bool CSound::CreateD3DSound( hwnd hWnd )
{	
	return m_DexterSound.CreateD3DSound( hWnd, NULL );
}

// 设置音量
void CSound::SetVolume( int nVolume )
{
	return m_DexterSound.SetVolume( nVolume );
}

// 停止声音
bool CSound::StopSound()
{
	return m_DexterSound.StopSound();
}

// 播放声音
bool CSound::PlaySound( const tchar * pszFileName )
{
	// 判断声音
	if ( !g_GameOption.bSound )
		return true;
	//return true;
	// 获取程序路径
	tchar szExePath[MAX_PATH + 1] = { _T("") };
	GetModuleFileName(NULL, szExePath, MAX_PATH);   
	PathRemoveFileSpec( szExePath );

	// 生成路径
	tchar szFile[MAX_PATH] = {0};
	_sntprintf(szFile, CountArray(szFile), TEXT("%s\\WaterMargin\\Sound\\%s"), szExePath, pszFileName);

	// 播放声音
	return m_DexterSound.PlaySoundEx(szFile);
}	

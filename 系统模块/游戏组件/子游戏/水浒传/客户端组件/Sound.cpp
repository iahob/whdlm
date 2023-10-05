#include "StdAfx.h"
#include ".\sound.h"
#include ".\GameOption.h"


// ��������
CMusic			CMusic::m_Music;

// ���캯��
CMusic::CMusic()
{
	m_nMusicIndex = 0;
	m_bPlaying = false;
}

// ��������
CMusic::~CMusic()
{

}

// ��������
bool CMusic::CreateD3DSound( hwnd hWnd )
{
	return m_DexterSound.CreateD3DSound( hWnd, CMusic::SoundCallBack );
}

// ��������
void CMusic::SetVolume( int nVolume )
{
	return m_DexterSound.SetVolume( nVolume );
}

// ������
bool CMusic::IsPlaying()
{
	return m_bPlaying;
}

// �������
void CMusic::AppendMusic(  const tchar * pszFilePath  )
{
	// ��ȡ����·��
	tchar szExePath[MAX_PATH + 1] = { _T("") };
	GetModuleFileName(NULL, szExePath, MAX_PATH);   
	PathRemoveFileSpec( szExePath );

	// ����·��
	tchar szFile[MAX_PATH] = {0};
	_sntprintf(szFile, CountArray(szFile), TEXT("%s\\WaterMargin\\Sound\\%s"), szExePath, pszFilePath);

	// ���·��
	m_ArrayMusic.Add( szFile );
}

// �������
void CMusic::ClearMusic()
{
	m_nMusicIndex = 0;
	m_bPlaying = false;
	m_ArrayMusic.RemoveAll();
	m_DexterSound.StopSound();
}

// ֹͣ����
bool CMusic::StopMusic()
{
	m_bPlaying = false;
	return m_DexterSound.StopSound();
}

// ��������
bool CMusic::PlayMusic()
{
	// ������
	if ( m_ArrayMusic.GetCount() == 0 )
		return false;

	// �жϴ�С
	if ( m_nMusicIndex >= m_ArrayMusic.GetCount() )
		m_nMusicIndex = 0;

	// ���ñ���
	m_bPlaying = true;

	// ����
	return m_DexterSound.PlaySoundEx( m_ArrayMusic[m_nMusicIndex] );
}

// ��һ������
bool CMusic::NextMusic()
{
	// ������
	if ( m_ArrayMusic.GetCount() == 0 )
		return false; 

	// ��������
	m_nMusicIndex = (int)((m_nMusicIndex + 1)%m_ArrayMusic.GetCount());

	// ����
	return m_DexterSound.PlaySoundEx( m_ArrayMusic[m_nMusicIndex] );
}

// �ص�����
void CMusic::SoundCallBack( hwnd hWnd, guid nID, void * pData, uint nTime )
{
	// ������
	CMusic::_Object()->NextMusic();
}



// ��������
CSound			CSound::m_Sound;

// ���캯��
CSound::CSound()
{

}

// ��������
CSound::~CSound()
{

}

// ��������
bool CSound::CreateD3DSound( hwnd hWnd )
{	
	return m_DexterSound.CreateD3DSound( hWnd, NULL );
}

// ��������
void CSound::SetVolume( int nVolume )
{
	return m_DexterSound.SetVolume( nVolume );
}

// ֹͣ����
bool CSound::StopSound()
{
	return m_DexterSound.StopSound();
}

// ��������
bool CSound::PlaySound( const tchar * pszFileName )
{
	// �ж�����
	if ( !g_GameOption.bSound )
		return true;
	//return true;
	// ��ȡ����·��
	tchar szExePath[MAX_PATH + 1] = { _T("") };
	GetModuleFileName(NULL, szExePath, MAX_PATH);   
	PathRemoveFileSpec( szExePath );

	// ����·��
	tchar szFile[MAX_PATH] = {0};
	_sntprintf(szFile, CountArray(szFile), TEXT("%s\\WaterMargin\\Sound\\%s"), szExePath, pszFileName);

	// ��������
	return m_DexterSound.PlaySoundEx(szFile);
}	

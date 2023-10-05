#pragma once

class CMusic
{

	// ��������
public:
	int						m_nMusicIndex;
	bool					m_bPlaying;
	CStringArray			m_ArrayMusic;
	CDexterSound			m_DexterSound;

	// ��̬����
public:
	Static CMusic			m_Music;

	// �ຯ��
protected:
	// ���캯��
	CMusic();
	// ��������
	~CMusic();

	// ��̬����
public:
	// ���ض���
	inline Static CMusic * _Object() { return &m_Music; }

	// ���ܺ���
public:
	// ��������
	bool CreateD3DSound( hwnd hWnd );
	// ��������
	void SetVolume( int nVolume );
	// ������
	bool IsPlaying();

	// ��������
public:
	// �������
	void AppendMusic( const tchar * pszFilePath );
	// �������
	void ClearMusic();
	// ֹͣ����
	bool StopMusic();
	// ��������
	bool PlayMusic();

	// �ڲ�����
protected:
	// ��һ������
	bool NextMusic();

	// ��̬����
public:
	// �ص�����
	Static void SoundCallBack( hwnd hWnd, guid nID, void * pData, uint nTime ); 
};


class CSound
{
	// ��������
public:
	CDexterSound			m_DexterSound;

	// ��̬����
public:
	Static CSound			m_Sound;

	// �ຯ��
protected:
	// ���캯��
	CSound();
	// ��������
	~CSound();

	// ��̬����
public:
	// ���ض���
	inline Static CSound * _Object() { return &m_Sound; }

	// ���ܺ���
public:
	// ��������
	bool CreateD3DSound( hwnd hWnd );
	// ��������
	void SetVolume( int nVolume );

	// ��������
public:
	// ֹͣ����
	bool StopSound();
	// ��������
	bool PlaySound( const tchar * pszFileName );
};

#pragma once

// ��������
#ifndef DEXTER_SOUNE_CLASS
	#define DEXTER_SOUNE_CLASS
#endif

// ��˵��
class CDexterSoundBuffer;
typedef CArray< CDexterSoundBuffer * >	CArraySoundBuffer;

// ����ָ��
typedef void (*SoundCallBack)( hwnd hWnd, guid nID, void * pData, uint nTime ); 

// ����������
class DEXTER_SOUNE_CLASS CDexterSound
{
	// �ӿڱ���
protected:
	IUnknown *						m_pIDexterSound;					// �豸����

	// �̱߳���
protected:
	bool							m_bSoundThread;						// �߳�����
	handle							m_hSoundThread;						// �߳̾��
	CCriticalSection				m_CriticalSection;					// ��������

	// ���Ʊ���
protected:
	hwnd							m_hSoundWnd;						// ��������
	int								m_nVolume;							// ����
	SoundCallBack					m_pSoundCallBack;					// �ص�����

	// �������
protected:
	CArraySoundBuffer				m_ArraySoundBufferActive;			// ��������
	CArraySoundBuffer				m_ArraySoundBufferBuffer;			// ��������

	// ��������
public:
	// ���캯��
	CDexterSound();
	// ��������
	virtual ~CDexterSound();

	// ���ܺ���
public:
	// ��������
	bool CreateD3DSound( hwnd hWnd, SoundCallBack pSoundCallBack );
	// ��������
	void SetVolume( int nVolume );

	// ��������
public:
	// ֹͣ����
	bool StopSound();
	// ֹͣ����
	bool StopSound( guid * pSoundID );
	// ��������
	bool PlaySoundEx( const tchar * pszFileName, guid * pSoundID = NULL, void * pSoundData = NULL, bool bRepetition = false );
	// ��������
	bool PlaySoundEx( hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, guid * pSoundID = NULL, void * pSoundData = NULL, bool bRepetition = false );

	// �ڲ�����
protected:
	// ��ȡ����
	bool CreateSoundBuffer( CDexterSoundBuffer * & pD3DSoundBuffer );
	// ɾ������
	bool DeleteSoundBuffer( CDexterSoundBuffer * & pD3DSoundBuffer );

	// �̺߳���
public:
	// �����߳�
	static void SoundThread( void * pThreadData );
};
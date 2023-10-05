#pragma once

// 导出定义
#ifndef DEXTER_SOUNE_CLASS
	#define DEXTER_SOUNE_CLASS
#endif

// 类说明
class CDexterSoundBuffer;
typedef CArray< CDexterSoundBuffer * >	CArraySoundBuffer;

// 函数指针
typedef void (*SoundCallBack)( hwnd hWnd, guid nID, void * pData, uint nTime ); 

// 声音播放类
class DEXTER_SOUNE_CLASS CDexterSound
{
	// 接口变量
protected:
	IUnknown *						m_pIDexterSound;					// 设备对象

	// 线程变量
protected:
	bool							m_bSoundThread;						// 线程启动
	handle							m_hSoundThread;						// 线程句柄
	CCriticalSection				m_CriticalSection;					// 锁定对象

	// 控制变量
protected:
	hwnd							m_hSoundWnd;						// 声音窗口
	int								m_nVolume;							// 音量
	SoundCallBack					m_pSoundCallBack;					// 回调函数

	// 组件变量
protected:
	CArraySoundBuffer				m_ArraySoundBufferActive;			// 声音缓冲
	CArraySoundBuffer				m_ArraySoundBufferBuffer;			// 声音缓冲

	// 函数定义
public:
	// 构造函数
	CDexterSound();
	// 析构函数
	virtual ~CDexterSound();

	// 功能函数
public:
	// 创建环境
	bool CreateD3DSound( hwnd hWnd, SoundCallBack pSoundCallBack );
	// 设置音量
	void SetVolume( int nVolume );

	// 声音函数
public:
	// 停止声音
	bool StopSound();
	// 停止声音
	bool StopSound( guid * pSoundID );
	// 播放声音
	bool PlaySoundEx( const tchar * pszFileName, guid * pSoundID = NULL, void * pSoundData = NULL, bool bRepetition = false );
	// 播放声音
	bool PlaySoundEx( hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, guid * pSoundID = NULL, void * pSoundData = NULL, bool bRepetition = false );

	// 内部函数
protected:
	// 获取对象
	bool CreateSoundBuffer( CDexterSoundBuffer * & pD3DSoundBuffer );
	// 删除对象
	bool DeleteSoundBuffer( CDexterSoundBuffer * & pD3DSoundBuffer );

	// 线程函数
public:
	// 声音线程
	static void SoundThread( void * pThreadData );
};
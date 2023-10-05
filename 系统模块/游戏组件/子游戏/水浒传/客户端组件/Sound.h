#pragma once

class CMusic
{

	// 变量定义
public:
	int						m_nMusicIndex;
	bool					m_bPlaying;
	CStringArray			m_ArrayMusic;
	CDexterSound			m_DexterSound;

	// 静态变量
public:
	Static CMusic			m_Music;

	// 类函数
protected:
	// 构造函数
	CMusic();
	// 析构函数
	~CMusic();

	// 静态函数
public:
	// 返回对象
	inline Static CMusic * _Object() { return &m_Music; }

	// 功能函数
public:
	// 创建环境
	bool CreateD3DSound( hwnd hWnd );
	// 设置音量
	void SetVolume( int nVolume );
	// 播放中
	bool IsPlaying();

	// 声音函数
public:
	// 添加声音
	void AppendMusic( const tchar * pszFilePath );
	// 清空声音
	void ClearMusic();
	// 停止声音
	bool StopMusic();
	// 播放声音
	bool PlayMusic();

	// 内部函数
protected:
	// 下一个声音
	bool NextMusic();

	// 静态函数
public:
	// 回调函数
	Static void SoundCallBack( hwnd hWnd, guid nID, void * pData, uint nTime ); 
};


class CSound
{
	// 变量定义
public:
	CDexterSound			m_DexterSound;

	// 静态变量
public:
	Static CSound			m_Sound;

	// 类函数
protected:
	// 构造函数
	CSound();
	// 析构函数
	~CSound();

	// 静态函数
public:
	// 返回对象
	inline Static CSound * _Object() { return &m_Sound; }

	// 功能函数
public:
	// 创建环境
	bool CreateD3DSound( hwnd hWnd );
	// 设置音量
	void SetVolume( int nVolume );

	// 声音函数
public:
	// 停止声音
	bool StopSound();
	// 播放声音
	bool PlaySound( const tchar * pszFileName );
};

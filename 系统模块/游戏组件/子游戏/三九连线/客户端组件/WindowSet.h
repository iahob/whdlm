#pragma once
#include "ObjectMove.h"
#include "D3DTextureIndex.h"
#include "EditKey.h"

// 音量选项
enum enumSetType
{
	ST_Music,
	ST_Sound,
	ST_MAX,
};

// 游戏设置
class CWindowSet : public CVirtualWindow
{
	// 设置变量
protected:
	INT						m_nCurrentVolume[ST_MAX];		// 音量大小
	INT						m_nLastVolume[ST_MAX];			// 上次音量大小
	BOOL					m_bCurrentVoice[ST_MAX];		// 声音
	BOOL					m_bLastVoice[ST_MAX];			// 上次声音

	// 位置信息
protected:
	BOOL					m_bMouseDown[ST_MAX];			// 鼠标点击
	BOOL					m_bMouseMove[ST_MAX];			// 鼠标悬停
	CPoint					m_ptLastSite[ST_MAX];			// 上次保存位置
	CPoint					m_ptLbuttonDown[ST_MAX];		// 鼠标点击位置
	CRect					m_rcMoveSelect[ST_MAX];			// 选择位置

	// 移动信息
protected:	
	CObjectMove				m_stuMoveFish[ST_MAX];			// 动画移动

	// 资源变量
protected:	
	CD3DTexture				m_ImageBack;					// 背景
	CD3DTexture				m_ImageSetBars;					// 拉杆
	CD3DTextureIndex		m_ImageSetMove;					// 移动
	CVirtualButton			m_btNoVoice[ST_MAX];			// 静音按钮
	CVirtualButton			m_btVoice[ST_MAX];				// 非静音按钮
	CVirtualButton			m_btClose;						// 关闭按钮
	CVirtualButton			m_btOk;							// 关闭按钮
	CVirtualButton			m_btDefaultKey;					// 默认按钮

	// 自定义按钮设置
public:
	UINT					m_nLastStartKey;				// 上次保存
	UINT					m_nLastLineCountUpKey;			// 上次保存
	UINT					m_nLastLineCountDownKey;		// 上次保存
	UINT					m_nLastLineNumberUpKey;			// 上次保存
	UINT					m_nLastLineNumberDownKey;		// 上次保存

	CEditKey				m_EditStart;
	CEditKey				m_EditLineCountUp;
	CEditKey				m_EditLineCountDown;
	CEditKey				m_EditLineNumberUp;
	CEditKey				m_EditLineNumberDown;

public:
	CWindowSet(void);
	~CWindowSet(void);


	// 系统事件
protected:
	// 动画消息
	virtual VOID OnWindowMovie();
	// 创建消息
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	// 位置消息
	virtual VOID OnWindowPosition(CD3DDevice * pD3DDevice);

	// 重载函数
protected:
	// 鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	// 按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	// 绘画窗口
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);

	// 窗口控制
public:
	// 显示窗口
	virtual VOID ShowWindow(bool bVisible);

	// 功能函数
public:
	// 返回音量
	int GetVolume( enumSetType emST ) { return m_nCurrentVolume[emST]; }
	// 返回启用
	BOOL GetVoice( enumSetType emST ) { return m_bCurrentVoice[emST]; }
	// 设置音量
	VOID SetVolume( int nVolume, enumSetType emST ) { m_nCurrentVolume[emST] = nVolume; }
	// 设置启用
	VOID SetVoice( BOOL bMute, enumSetType emST ) { m_bCurrentVoice[emST] = bMute; }
};

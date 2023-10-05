#pragma once
#include "GameClientView.h"

// 游戏视图
class CGameFrameViewDexter : public CGameFrameView, public dex::ID3DDeviceSink
{
	// 状态变量
protected:
	bool							m_bInitD3D;							// 创建标志
	HMODULE							m_hModule;							// 资源DLL

	// 资源变量
public:
	CGameClientView					m_GameClientView;					// 游戏窗口

	// 函数定义
public:
	// 构造函数
	CGameFrameViewDexter();
	// 析构函数
	virtual ~CGameFrameViewDexter();

	// 重载函数
public:
	// 重置界面
	virtual VOID ResetGameView();
	// 调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	// 界面更新
	virtual VOID InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight);

	// 银行函数
public:
	// 银行消息
	virtual bool OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize);

	// 设备接口
protected:
	// 配置设备
	virtual void OnInitDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 丢失设备
	virtual void OnLostDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 重置设备
	virtual void OnResetDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 控制灯光
	virtual void OnControlLight(dex::CD3DDevice * pD3DDevice);

	// 消息映射
protected:
	// 大小消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	// 定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	// 标题消息
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);
	// 右键谈起
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	// 重载函数
protected:
	// 消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	// 默认回调
	virtual LRESULT DefWindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};

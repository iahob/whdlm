#pragma once

class CWindowGameOver : public CVirtualWindow
{
	// 变量信息
public:
	LONGLONG						m_lPlayScore;							// 玩家积分
	LONGLONG						m_lPlayWinLose;							// 玩家输赢分

	// 资源信息
public:
	CD3DTexture						m_ImageBack;							// 背景
	CD3DTexture						m_ImageNumer;							// 数字
	CVirtualButton					m_btClosed;								// 关闭按钮

	// 构造函数
public:
	CWindowGameOver(void);
	~CWindowGameOver(void);

	// 系统事件
protected:
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

	// 界面函数
private:
	// 绘画数字
	void DrawNumber(CD3DDevice * pGLDevice , CD3DTexture* ImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT, BYTE cbAlpha = 255);	

	// 设置函数
public:
	// 设置积分
	void SetOverInfo( LONGLONG lPlayScore, LONGLONG lPlayWinLose ) { m_lPlayScore = lPlayScore; m_lPlayWinLose = lPlayWinLose; }

};

#ifndef GUID_8E861A52_58CD_4048_8978_187B0BE17989
#define GUID_8E861A52_58CD_4048_8978_187B0BE17989

#pragma once

#include "DirectUIWindow.h"

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_begin

//////////////////////////////////////////////////////////////////////////////////

// 文字结构
struct EditText
{
	TCHAR				Text[2];
	CSize				Size;
};

// 文字输入框
class DEXTER_D3D_CLASS CDirectUIEdit : public CDirectUIWindow
{

	// 显示变量
protected:
	LOGFONT											m_LogFont;					// 字体信息
	CD3DFont										m_EditFont;					// 输入字体
	CArray<EditText*>								m_ArrayText;				// 文字链表
	CArray<EditText*>								m_ArrayBuffer;				// 文字缓冲
	int												m_nBeginShow;				// 开始显示位置
	int												m_nEndShow;					// 结束显示位置
	int												m_nBeginSelect;				// 开始选中位置
	int												m_nEndSelect;				// 结束选中位置
	int												m_nStaySite;				// 停留职位

	// 设置变量
protected:
	bool											m_bOnlyNumber;				// 只能输入数字
	bool											m_bNumberPoint;				// 可以输入小叔点
	bool											m_bPassword;				// 密码
	int												m_nPasswordOffset;			// 密码偏移量

	// 鼠标动作
protected:
	bool											m_bMouseDown;				// 鼠标点击

	// 鼠标停留
protected:
	int												m_nStayInterval;			// 停留时间
	int												m_nStayTime;				// 停留时间
	CSize											m_SizeStay;					// 停留大小

	// 选择移动
protected:
	bool											m_bLeftSelect;				// 左选择
	bool											m_bRightSelect;				// 右选择
	int												m_nMoveSelectTime;			// 移动选择时间
	CLapseCount										m_SelectLapseCount;			// 选择计时

	// 颜色变量
protected:
	D3DCOLOR										m_dwBackColor;				// 背景颜色
	D3DCOLOR										m_dwFontColor;				// 字体颜色
	D3DCOLOR										m_dwSelectBackColor;		// 选中背景颜色
	D3DCOLOR										m_dwSelectFontColor;		// 选中字体颜色
	D3DCOLOR										m_dwStayColor;				// 停留颜色

	// 函数定义
public:
	// 构造函数
	CDirectUIEdit(void);
	// 析构函数
	virtual ~CDirectUIEdit(void);

	// 系统事件
protected:
	// 动画消息
	void OnWindowMovie();
	// 创建消息
	void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// 销毁消息
	void OnWindowDestory(CD3DDevice * pD3DDevice);
	// 位置消息
	void OnWindowPosition(CD3DDevice * pD3DDevice, int nPosX, int nPosY);
	// 绘画窗口
	void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// 消息重载函数
protected:
	// 消息处理
	virtual bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// 消息解释
	virtual bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

	// 鼠标消息
protected:
	// 鼠标移动 
	void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键抬起 
	void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键点下 
	void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键双击 
	void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// 键盘消息
protected:
	// 按键点下 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// 按键弹起 
	void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);
	// 字符消息 
	void OnChar(uint nChar, uint nRepCnt, uint nFlags);	

	// 操作函数
public:
	// 创建字体
	void SetFont( CD3DFont& font );
	// 设置只能输入数字
	void SetOnlyNumber( bool bOnlyNumber, bool bNumberPoint = false ) { m_bOnlyNumber = bOnlyNumber; m_bNumberPoint = bNumberPoint; }
	// 设置密码状态
	void SetPassword( bool bPassword ) { m_bPassword = bPassword; }
	// 设置背景颜色
	void SetBackColor( D3DCOLOR dwBackColor ) { m_dwBackColor = dwBackColor; }
	// 设置字体颜色
	void SetFontColor( D3DCOLOR dwFontColor ) { m_dwFontColor = dwFontColor; }
	// 设置选中背景颜色
	void SetSelectBackColor( D3DCOLOR dwSelectBackColor ) { m_dwSelectBackColor = dwSelectBackColor; }
	// 设置选中字体颜色
	void SetSelectFontColor( D3DCOLOR dwSelectFontColor ) { m_dwSelectFontColor = dwSelectFontColor; }
	// 设置停留颜色
	void SetStayColor( D3DCOLOR dwStayColor ) { m_dwStayColor = dwStayColor; }
	// 设置文本
	void SetEditText( CString StrText );
	// 获取文本
	void GetEditText( CString& StrText );

	// 内部函数
protected:
	// 获取新缓冲
	EditText* AccessBuffer( TCHAR szText );
	// 删除缓冲
	void DeleteBuffer( EditText* pEditText );
	// 获取选取索引
	int	GetSelectIndex( CPoint ptMouse, bool bSeparationOrSo );
	// 拷贝到剪贴板
	void CopyToClipboard();
	// 剪贴板中有数量
	int ClipboardCount();
	// 粘贴剪贴板
	void PasteFromClipboard();
	// 删除选中字符
	void DeleteSelectText();
	// 更新显示位置
	void UpdateShowSite();
	// 重置显示位置
	void ResetShowSite();

	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif
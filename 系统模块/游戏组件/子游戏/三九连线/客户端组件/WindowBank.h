#pragma once

// 银行窗口
class CWindowBank : public dex::CDirectUIWindow
{

	// 信息变量
public:
	bool									m_bPlaySave;				// 存款操作
	longlong								m_lUserScore;				// 用户分数
	longlong								m_lUserInsure;				// 银行分数

	// 窗口变量
public:
	dex::CD3DTexture						m_TextureBack;				// 银行背景
	dex::CD3DTexture						m_TextureNumber;			// 银行数字
	dex::CD3DTexture						m_TextureMask;				// 银行蒙板
	dex::CDirectUIEdit						m_EditOperating;			// 操作窗口
	dex::CDirectUIEdit						m_EditPassword;				// 密码窗口
	dex::CDirectUICheck						m_CheckSave;				// 存款按钮
	dex::CDirectUICheck						m_CheckTake;				// 取款按钮
	dex::CDirectUIButton					m_ButtonOK;					// 确定按钮
	dex::CDirectUIButton					m_ButtonClose;				// 关闭按钮


	// 类函数
public:
	// 构造函数
	CWindowBank();
	// 析构函数
	virtual ~CWindowBank();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口显示 
	void OnWindowShow( dex::CD3DDevice * pD3DDevice, bool bVisible );

	// 消息函数
public:
	// 确定 
	void OnClickedOK();
	// 关闭
	void OnClickedClose();
	// 单选按钮
	void OnClickedCheck();
	// 数据修改
	void OnEditChangeOperating();

	// 功能函数
public:
	// 设置银行分数
	void SetBankScore( longlong lUserScore, longlong lUserInsure );

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};

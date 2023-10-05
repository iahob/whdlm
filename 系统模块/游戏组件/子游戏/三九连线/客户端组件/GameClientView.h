#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "WindowTurntable.h"
#include "WindowWin.h"
#include "WindowBank.h"
#include "WindowAward.h"
#include "WindowRoad.h"
#include "WindowMessage.h"
#include "WindowHelp.h"
#include "WindowLine.h"
#include "WindowEgg.h"

//////////////////////////////////////////////////////////////////////////////////

// 视图位置
#define MYSELF_VIEW_ID				0									// 视图位置

// 游戏视图
class CGameClientView : public dex::CDirectUIWindow
{
	// 游戏信息
protected:
	CString							m_StrGameText;							// 游戏名字
	longlong						m_lRoomLotteryPool;						// 房间彩池
	EnumMultipleType				m_nEnumLineShow;						// 线数显示

	// 倍数信息
protected:
	int								m_nThreeLineMultiple[MultipleThree_Max];// 3连倍数
	int								m_nNineLineMultiple[MultipleNine_Max];	// 9连倍数

	// 任务信息
protected:
	int								m_nTaskProgress[TASK_INDEX_MAX];		// 任务进度
	int								m_nTaskTarget[TASK_INDEX_MAX];			// 任务目标
	int								m_nTaskCount[TASK_INDEX_MAX];			// 任务数量
	int								m_nTaskScore[TASK_INDEX_MAX];			// 任务金币

	// 下注信息
protected:
	int								m_nBetScore;							// 下注分数
	int								m_nBetLineCount;						// 线数数量
	bool							m_bBetLine[LINE_INDEX_MAX];				// 下注线数

	// 赢信息
protected:
	longlong						m_lPlayWinScore;						// 玩家赢分数
	bool							m_bPlayWinLine[LINE_INDEX_MAX];			// 玩家赢线数
	bool							m_bPlayWinMultiple[MultipleThree_Max + MultipleNine_Max];	// 赢的倍数

	// 背景光电
public:
	uint							m_nLightBackTime;						// 背景光电时间
	dex::CLapseCount				m_LCLightBack;							// 背景光电时间
	CWHArray< CLightMove * >		m_ArrayLightBack;						// 背景光电对象

	// 背景闪光
public:
	CWHArray< CLightMove * >		m_ArrayFlashBack;						// 背景闪光对象

	// 动画变量
public:
	dex::CLapseCount				m_LCLightMove;							// 光电移动延时
	CWHArray< CLightMove * >		m_ArrayLightMove;						// 光电移动对象

	// 字体变量
public:
	dex::CD3DFont					m_D3DFontWidth12;						// 字体信息
	dex::CD3DFont					m_D3DFontNormal12;						// 字体信息

	// 资源变量
public:
	dex::CD3DTexture				m_TextureBack1_1;						// 背景图片
	dex::CD3DTexture				m_TextureBack1_2;						// 背景图片
	dex::CD3DTexture				m_TextureBackFlash;						// 背景图片
	dex::CD3DTexture				m_TextureBack2;							// 背景图片
	dex::CD3DTexture				m_TextureLightLeft;						// 左光图片
	dex::CD3DTexture				m_TextureLightRight;					// 右光图片
	dex::CD3DTexture				m_TextureLightPoint;					// 灯点图片
	dex::CD3DTexture				m_TextureBetScore;						// 下注分数
	dex::CD3DTexture				m_TextureLotteryPoolNumber;				// 彩池数字	
	dex::CD3DTexture				m_TextureThreeBack;						// 三连背景	
	dex::CD3DTexture				m_TextureNineBack;						// 九连背景	
	dex::CD3DTexture				m_TextureNineAll;						// 九连全部
	dex::CD3DTexture				m_TextureMultipleNumber;				// 倍数数字
	dex::CD3DTexture				m_TextureMultipleSign;					// 倍数乘号
	dex::CD3DTexture				m_TextureAwardBox;						// 获奖框

	// 任务界面
public:
	dex::CD3DTexture				m_TextureTaskNumber;					// 任务数字
	dex::CD3DTexture				m_TextureTaskReward;					// 任务奖金
	dex::CD3DTexture				m_TextureTaskGrand;						// 任务累计
	dex::CD3DTexture				m_TextureTaskGold;						// 任务金

	// 界面变量
public:
	dex::CDirectUIButton			m_ButtonStartGame;						// 开始游戏
	dex::CDirectUIButton			m_ButtonStopGame;						// 停止游戏
	dex::CDirectUIButton			m_ButtonStartHosted;					// 托管开始
	dex::CDirectUIButton			m_ButtonStopHosted;						// 托管停止
	dex::CDirectUIButton			m_ButtonScoreLess;						// 分数减少
	dex::CDirectUIButton			m_ButtonScoreAdd;						// 分数增加
	dex::CDirectUIButton			m_ButtonScoreMax;						// 分数最大
	dex::CDirectUIButton			m_ButtonRoad;							// 路单显示
	dex::CDirectUIButton			m_ButtonBank;							// 银行显示
	dex::CDirectUIButton			m_ButtonLineCheck[LINE_INDEX_MAX];		// 线选择
	dex::CDirectUIButton			m_ButtonLineUnCheck[LINE_INDEX_MAX];	// 选未选择
	dex::CDirectUIButton			m_ButtonThreeLine;						// 三连按钮
	dex::CDirectUIButton			m_ButtonNineLine;						// 九连按钮

	// 界面变量
public:
	dex::CDirectUIButton			m_ButtonHelp;							// 帮助
	dex::CDirectUIButton			m_ButtonSoundOn;						// 声音开
	dex::CDirectUIButton			m_ButtonSoundOff;						// 声音关
	dex::CDirectUIButton			m_ButtonWinMin;							// 窗口最小化
	dex::CDirectUIButton			m_ButtonWinMax;							// 窗口最大化
	dex::CDirectUIButton			m_ButtonWinRevert;						// 窗口最还原
	dex::CDirectUIButton			m_ButtonWinClose;						// 窗口最关闭
	dex::CDirectUIButton			m_ButtonControl;						// 控制按钮

	// 界面变量
public:
	CWindowEgg						m_WindowEgg;							// 癞子蛋界面
	CWindowWin						m_WindowWin;							// 赢界面
	CWindowLine						m_WindowLine;							// 线界面
	CWindowTurntable				m_WindowTurntable;						// 转盘界面
	CWindowBank						m_WindowBank;							// 银行
	CWindowRoad						m_WindowRoad;							// 路单窗口
	CWindowHelp						m_WindowHelp;							// 帮助
	CWHArray< dex::CDirectUIWindow * > m_ArrayWindowUnique;					// 唯一界面
	CWHArray< CWindowAward * >		m_ArrayAward;							// 中奖窗口

	// 用户信息
public:
	IClientUserItem *				m_pIClientOneSelf;						// 自己的指针
	dex::CD3DTexture				m_TextureAvatar;						// 头像纹理

	// 原始界面
public:
	CGameFrameView *				m_pGameFrameView;						// 界面框架

	// 窗口变量
protected:
	bool							m_bWinMax;								// 最大化
	DEVMODE							m_DefaultDevmode;						// 窗口信息


	// 函数定义
public:
	// 构造函数
	CGameClientView( CGameFrameView * pGameFrameView );
	// 析构函数
	virtual ~CGameClientView();

	// 调用函数
public:
	// 重置界面
	void ResetGameView();

	// 银行函数
public:
	// 银行消息
	bool OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize);

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画 
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// 绘画函数
public:
	// 绘画闪光背景
	void DrawFlashBack( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 绘画线信息
	void DrawLineInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 绘任务信息
	void DrawTaskInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 绘画光电
	void DrawLightMove( dex::CD3DDevice * pD3DDevice, CWHArray< CLightMove * > & pArrayLightMove );

	// 消息函数
public:
	// 左键点下 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 右键点下 
	void OnRButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 按键点下 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// 按键谈起 
	void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);

	// 功能函数
public:
	// 设置游戏名次
	void SetGameText( CString StrGameText ) { m_StrGameText = StrGameText; }
	// 设置自己指针
	void SetClientOneSelf( IClientUserItem * pIClientUserItem );
	// 设置彩池
	void SetRoomLotteryPool( longlong lRoomLotteryPool ); 
	// 设置下注数
	void SetBetScore( int nBetScore );
	// 设置下注线
	void SetBetLine( bool nBetLine[LINE_INDEX_MAX] );
	// 设置玩家赢分数
	void SetPlayWinScore( longlong lPlayWinScore );
	// 设置玩家赢线数
	void SetPlayWinLine( bool bPlayWinLine[LINE_INDEX_MAX] );
	// 设置线显示
	void SetLineShow( EnumMultipleType nEnumLineShow );
	// 设置倍数
	void SetThreeLineMultiple( int nThreeLineMultiple[MultipleThree_Max] );
	// 设置倍数
	void SetNineLineMultiple( int nNineLineMultiple[MultipleNine_Max] );
	// 设置任务进度
	void SetTaskProgress( int nTaskProgress[TASK_INDEX_MAX] );
	// 设置任务目标
	void SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] );
	// 设置任务数量
	void SetTaskCount( int nTaskCount[TASK_INDEX_MAX] );
	// 设置任务金币
	void SetTaskScore( int nTaskScore[TASK_INDEX_MAX] );

	// 中奖函数
public:
	// 清空中奖
	void ClearMineAward();
	// 添加中奖
	void AppendMineAward( int nMultipleIndex );

	// 动画函数
public:
	// 添加光电背景闪光
	void AppendLightBack();
	// 添加光电移动
	void AppendLightMove();
	// 添加中奖
	void AppendAward( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore );

	// 功能函数
public:
	// 关闭窗口
	void CloseAllWindow( dex::CDirectUIWindow * pExcludeWindow );

	// 按钮消息
public:
	// 开始按钮 
	void OnClickedStartGame();
	// 停止按钮 
	void OnClickedStopGame();
	// 托管开始 
	void OnClickedStartHosted();
	// 托管停止 
	void OnClickedStopHosted();
	// 分数减少 
	void OnClickedScoreLess();
	// 分数增加 
	void OnClickedScoreAdd();
	// 分数最大
	void OnClickedScoreMax();
	// 路单 
	void OnClickedRoad();
	// 银行
	void OnClickedBack();
	// 线选择
	void OnClickedLineCheck1();
	// 线选择
	void OnClickedLineCheck2();
	// 线选择
	void OnClickedLineCheck3();
	// 线选择
	void OnClickedLineCheck4();
	// 线选择
	void OnClickedLineCheck5();
	// 线选择
	void OnClickedLineCheck6();
	// 线选择
	void OnClickedLineCheck7();
	// 线选择
	void OnClickedLineCheck8();
	// 线选择
	void OnClickedLineUnCheck1();
	// 线选择
	void OnClickedLineUnCheck2();
	// 线选择
	void OnClickedLineUnCheck3();
	// 线选择
	void OnClickedLineUnCheck4();
	// 线选择
	void OnClickedLineUnCheck5();
	// 线选择
	void OnClickedLineUnCheck6();
	// 线选择
	void OnClickedLineUnCheck7();
	// 线选择
	void OnClickedLineUnCheck8();
	// 三线奖励
	void OnClickedThreeLine();
	// 九线奖励
	void OnClickedNineLine();
	// 帮助
	void OnClickedHelp();
	// 声音开
	void OnClickedSoundOn();
	// 声音关
	void  OnClickedSoundOff();
	// 窗口最小化
	void OnClickedWinMin();
	// 窗口最大化
	void OnClickedWinMax();
	// 窗口最还原
	void OnClickedWinRevert();
	// 窗口最关闭
	void OnClickedWinClose();
	// 打开控制
	void OnBnClickedOpenControl();

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif
#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DirectUIButtonEx.h"
#include "ClientControl.h"
#include "MyUIThread.h"
#include "Animation.h"
#include "Quicken.h"
#include "MyUIThread.h"
#include "AnimationScroll.h"
#include "D3DTextureex.h"
#include ".\directuibuttonexx.h"
////////////////////////////////////////////////////////////////////////////////// 
#define IDM_GAME_OPERATE			(WM_USER+208)						//按键消息
#define IDM_GAME_OPTION				(WM_USER+209)						//游戏设置
#define IDM_GAME_SOUND				(WM_USER+210)						//游戏声音
#define IDM_GAME_CLOSE				(WM_USER+211)						//游戏关闭
#define IDM_GAME_BET				(WM_USER+212)						//游戏下注

//动画场景
#define	ONE_NULL					0									//无
#define	ONE_GUANDONG				1									//滚动
#define	ONE_ZHONGJIANG				2									//中奖线
#define	ONE_KAIJIANG				3									//开奖
#define	ONE_KAIJIANG_END			4									//开奖
#define	ONE_SHUO_MING				5									//游戏说明

#define	TWO_XIA_ZHU					6									//下注
#define	TWO_KAI_JIAN				7									//开奖

#define	THREE_GUANDONG				9									//开奖
#define	THREE_KAIJIANG				10									//开奖
#define	THREE_KAIJIANG_END			11									//开奖

class CGameClientEngine;
// 界面类型
enum EnumClientView
{
	ClientView_SelectMap = 0,			// 选择地图
	ClientView_Game = 1,				// 游戏中
	ClientView_Max = 2,					// 最大值
};

//发送消息
enum EnumGameMessage
{
	Message_Start = (WM_USER+201),	// 启动游戏
	Message_OpenAdmin,				// 打开控制
};

////////////////////////////////////////////////////////////////////////////////// 

// 游戏视图
class CGameClientView : public dex::CDirectUIWindow
{
	//定时器
	UINT							m_uAccuracy;
	UINT							m_uTimerID;

	CString							m_strTip;
	bool							m_bDrawTop;							//画标题
	bool							m_bDrawFrame;						//画边框
	
	//游戏变量
	bool							m_bShowShuoMing;					//显示说明
	BYTE							m_cbShuoMingIndex;					//说明索引
	BYTE							m_cbPlayIndex;						//播放索引
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//开奖信息
	bool							m_bZhongJiang[ITEM_Y_COUNT][ITEM_X_COUNT];	//中奖信息
	BYTE							m_cbZhongJiangXian;				//中奖线
	CPoint							m_ptZhongJiang[ITEM_X_COUNT];	//中奖位置
	bool							m_bAutoGame;					//自动游戏

	BYTE							m_cbGameStatus;					//游戏状态

	//押线变量
	BYTE							m_cbWaitMode;					//等待进入
	SCORE							m_lOneScoreTemp;				//临时变量
	SCORE							m_lSaveScore;					//保存分数
	SCORE							m_lOneGredit;					//身上分数
	SCORE							m_lOneTotalWin;					//总赢分数
	SCORE							m_lLine;						//押线数量
	SCORE							m_lBet;							//下注数
	SCORE							m_lOneTotalBet;					//总下注数
	SCORE							m_lOneGameScore;				//游戏得分


	//比倍变量
	BYTE							m_cbUser[3];					//玩家表情
	SCORE							m_lTwoGredit;					//身上分数
	SCORE							m_lTwoTotalWin;					//总赢分数
	SCORE							m_lTwoTotalBet;					//总下注数
	SCORE							m_lTwoGameScore;				//游戏得分
	BYTE							m_cbBetIndex;					//下注位置
	bool							m_bShowSice[2];					//显示骰子
	BYTE							m_cbSice[2];					//骰子大小

	//小玛丽变量
	SCORE							m_lThreeGredit;					//身上分数
	SCORE							m_lThreeTotalWin;				//总赢分数
	SCORE							m_lThreeTotalBet;				//总下注数
	SCORE							m_lThreeGameScore;				//游戏得分
	SCORE							m_lThreeTimes;					//游戏得分
	BYTE							m_cbItemType[24];				//图标类型
	BYTE							m_cbThreeItemInfo[4];				//图标类型
	bool							m_bThreeZhongJiang;				//是否中奖
	BYTE							m_cbThreeIndex;					//开奖索引
	BYTE							m_cbThreeZhongtimes;			//中奖位置 0左 1 中500倍 2右



	//界面变量
	bool							m_bInitImage;				//加载图片
	BYTE							m_cbImageLoadPos;			//加载进度
	BYTE							m_cbShuoMingLoadPos;		//加载进度
	CString							m_strPathImage;				//图片目录

	//位置变量
protected:
	CPoint							m_ptItemImage[ITEM_Y_COUNT][ITEM_X_COUNT];	//图片位置
	CPoint							m_ptBianKuang[ITEM_Y_COUNT][ITEM_X_COUNT];	//图片位置
	CPoint							m_ptGredit;						//身上分数
	CPoint							m_ptTotalWin;					//总赢分数
	CPoint							m_ptLine;						//押线数量
	CPoint							m_ptBet;						//下注数
	CPoint							m_ptTotalBet;					//总下注数

	CPoint							m_ptTwoGredit;					//身上分数
	CPoint							m_ptTwoTotalWin;				//总赢分数
	CPoint							m_ptTwoTotalBet;				//总下注数
	CPoint							m_ptTwoJingYuanBao[3];			//总下注数
	CPoint							m_ptDice0[2];					//总下注数
	CPoint							m_ptDice1[2];					//总下注数

	CPoint							m_ptThreeGredit;				//身上分数
	CPoint							m_ptThreeTotalWin;				//总赢分数
	CPoint							m_ptThreeTotalBet;				//总下注数
	CPoint							m_ptThreeTotalTimes;			//总下注数
	CPoint							m_ptThreeItem[24];				//图标位置
	CPoint							m_ptThreeItemImage[4];				//图标位置
	CPoint							m_ptThreeItemTimes[8];				//图标位置
	CPoint							m_ptThreeTimes[3];					//图标位置

	// 资源图片
public:



	// 按钮资源
public:
	CDirectUIButtonExx			m_ButtonWinClose;								// 关闭
	//CDirectUIButtonExx			m_ButtonWinMax;									// 最大化
	CDirectUIButtonExx			m_ButtonWinMin;									// 最小化
	CDirectUIButtonExx			m_ButtonSound;									// 声音
	CDirectUIButtonExx			m_ButtonWinSet;									// 设置
	CDirectUIButtonExx			m_ButtonAdmin;									// 控制
	
	CDirectUIButtonExx			m_ButtonAddBet;									// 控制
	CDirectUIButtonExx			m_ButtonSubBet;									// 控制
	//边框资源
protected:
	CD3DTextureEx				m_ImageGameFrame[8];							//游戏框架	

public:
	
	// 资源字体
public:
	dex::CD3DFont					m_D3DFont;									// 自由字体

	//控件变量
public:
	CGameLogic						m_GameLogic;						//游戏逻辑

	//动画资源
protected:
	CD3DTextureEx				m_TextureShuoMingPosBack;			//资源变量
	CD3DTextureEx				m_TextureShuoMingPos;				//信息数字
	CD3DTextureEx				m_TextureExitShuoMing;				//退出说明
	CAnimation						m_AnimationShuoMing;				//说明


	CAnimation						m_AnimationDaGu;					//打鼓动画
	CAnimation						m_AnimationZhongJiangXian;			//中奖线
	CAnimation						m_AnimationWaitQi;					//等待开始状态下的飘旗
	CAnimation						m_AnimationKaiQi;					//滚动状态下的摇旗
	CAnimation						m_AnimationShuiHuZhuang;			//顶部水浒传3个字体闪光
	CAnimation						m_AnimationSanGuang;				//闪光动画
	CAnimation						m_AnimationBianKuang;				//边框动画
	CAnimationScroll				m_AnimationScrollGunDong;			//滚动动画
	CAnimation						m_AnimationStaticImage;				//静态图标
	CAnimation						m_AnimationXuLieImage[ITEM_COUNT];	//序列动画
	CAnimation						m_AnimationDouDongImage[ITEM_COUNT];//抖动动画
	CAnimation						m_AnimationOneTip;					//提示图标


	CQuicken						m_Quicken[ITEM_Y_COUNT * ITEM_X_COUNT];	//滚动时间
	CQuicken						m_QuickenThreeItem[4];					//滚动时间
	CQuicken						m_QuickenThree;							//滚动时间
	CQuicken						m_QuickenSan;							//滚动时间
	CQuicken						m_QuickenViewSan;						//滚动时间


	CAnimation						m_AnimationHeGuan[6];			//荷官动画
	CAnimation						m_AnimationZuoHeGuan[4];		//左边瘦子各种表情
	CAnimation						m_AnimationYouHeGuan[4];		//右边胖子各种表情
	CAnimation						m_AnimationDice[2];				//骰子
	CAnimation						m_AnimationZuoZhiDouDong;		//桌子
	CAnimation						m_AnimationJingYuanBaoDouDong;	//金元宝
	CAnimation						m_AnimationTwoSanGuang;			//金元宝
	CAnimation						m_AnimationTwoTip;				//提示图标

	//资源变量
protected:
	CD3DTextureEx				m_TextureOneBack;				//资源变量
	CD3DTextureEx				m_TextureOneTipNumber;			//提示数字
	CD3DTextureEx				m_TextureTwoTipNumber;			//提示数字
	CD3DTextureEx				m_TextureCellNumber;			//信息数字
	CD3DTextureEx				m_TextureCellNumberS;			//信息数字
	CD3DTextureEx				m_TextureWin;					//信息数字
	CD3DTextureEx				m_TextureWinNumber;				//信息数字

	CD3DTextureEx				m_TextureTwoBack;				//资源变量
	CD3DTextureEx				m_TextureTwoJingYuanBao;		//资源变量
	CD3DTextureEx				m_TextureTwoJingYuanBaoEx;		//资源变量

	//第三部分
	CD3DTextureEx				m_TextureThreeBack;				//资源变量
	CD3DTextureEx				m_TextureTimesNumber;			//信息数字
	CAnimation						m_AnimationThreeDeng;			//各跑灯图标
	CAnimation						m_AnimationThreeTimes;			//倍数
	CAnimation						m_AnimationThreeTimes20L;		//倍数
	CAnimation						m_AnimationThreeTimes20R;		//倍数
	CAnimation						m_AnimationThreeTimes500;		//倍数

	//线程
public:
	dex::CD3DDevice *				m_pD3DDevice;
	CMyUIThread*					m_pMyUIThread;

	//组件
public:
	CString							m_StrGameText;									// 标题
	IClientUserItem *				m_pClientUserItem;								//
	CGameClientEngine*				m_pGameClientEngine;
	HINSTANCE						m_hInst;
	IClientControlDlg*				m_pClientControlDlg;	
	bool							m_bShowClientControlDlg;

	// 函数定义 
public:
	// 构造函数
	CGameClientView();
	// 析构函数
	virtual ~CGameClientView();

	// 调用函数
public:
	// 重置界面
	void ResetGameView();
	//调整控件
	void RectifyControl(uint nWidth, uint nHeight);
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
	// 更新缓冲 
	void OnWindowUpdataTextureBuffer( dex::CD3DDevice * pD3DDevice );
	// 按键按下
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// 窗口显示 
	void OnWindowShow(dex::CD3DDevice * pD3DDevice, bool bVisible );

	//绘画框架
	void DrawGameFrame(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// 窗口消息
public:
	// 鼠标移动 
	void OnMouseMove( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite );
	// 左键点下 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite );
	// 左键抬起 
	void OnLButtonUp( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite );
	// 消息处理
	virtual bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// 消息解释
	virtual bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// 窗口大小 
	void OnWindowSize(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// 设置函数
public:
	// 设置游戏名字
	void SetGameText( CString StrGameText ) { m_StrGameText = StrGameText; }
	// 设置玩家指针
	void SetClientUserItem(IClientUserItem * pClientUserItem ) { m_pClientUserItem = pClientUserItem; };
	// 设置游戏引擎
	void SetGameClientEngine(CGameClientEngine * pGameClientEngine ) { m_pGameClientEngine = pGameClientEngine; }

	// 按钮消息
public:
	// 关闭 
	void OnButtonWinClose();
	// 最大化 
	void OnButtonWinMax();
	// 最小化 
	void OnButtonWinMin();	
	// 声音 
	void OnButtonSound();
	// 设置 
	void OnButtonWinSet();
	// 加注 
	void OnButtonAddBet();
	// 减注 
	void OnButtonSubBet();
	// 控制
	void OnButtonAdmin();
	// 初始化变量
	void ResetView(bool bResetEngine=false);

	//动画函数
public:
	//动画驱动
	VOID CartoonMovie();
	//开始开牌
	VOID StartOneGuanDong(bool bStart, BYTE cbItemInfo[][ITEM_X_COUNT] = NULL);
	//开始开奖
	VOID StartZhongJiangXian(BYTE cbZhongJiangXian, CPoint ptZhongJiang[ITEM_X_COUNT] = NULL);
	//开始开奖
	VOID StartKaiJang(bool bZhongJiang[][ITEM_X_COUNT] = NULL);
	//开始开奖
	VOID StartKaiJangEnd(SCORE lScore, bool bLong = false);
	//开始比倍
	VOID StartTwoWiatAddScore(bool bStart);
	//开始开牌
	VOID StartThreeGunDong(BYTE cbItemType[4]);
	//开始开牌
	VOID StartThreeGunDongEnd(SCORE lScore, BYTE cbPos);
	//开始开牌
	VOID StartThreeKaiJian(BYTE cbIndex);
	//开始开牌
	VOID StartThreeKaiJianEnd(SCORE lScore);



	//控制函数
public:
	//设置控制
	VOID ShowClientControlDlg(bool bShowClientControlDlg);
	//设置说明索引
	BYTE GetShuoMingPos() { return m_cbShuoMingLoadPos;}
	//设置说明索引
	VOID SetShuoMingPos(BYTE cbShuoMingPos) { m_cbShuoMingLoadPos = cbShuoMingPos;}
	//设置说明索引
	VOID SetShuoMingIndex(BYTE cbIndex) { m_cbShuoMingIndex = cbIndex; };
	//取得说明索引
	BYTE GetShuoMingIndex() { return m_cbShuoMingIndex; }
	//取得说明索引
	VOID ShowShuoMing(bool bShowShuoMing);
	//设置自动
	VOID SetAutoGame(bool bAutoGame) { m_bAutoGame = bAutoGame; }
	//设置下注
	VOID SetOneBet(SCORE lBet, SCORE lOneTotalBet) { m_lBet = lBet; m_lOneTotalBet = lOneTotalBet; }
	//设置下注
	VOID SetSaveScore(SCORE lSaveScore);
	//开始开奖
	VOID SetOneScore(SCORE lScore) { m_lOneGameScore = lScore; }
	//开始开奖
	VOID SetOneTotalWin(SCORE lTotalWin, SCORE lScoreXian) { m_lOneTotalWin = lTotalWin; m_lOneScoreTemp = lScoreXian;}
	//等待进入
	VOID SetWaitMode(BYTE cbWaitMode) { m_cbWaitMode = cbWaitMode; }
	//开始开牌
	VOID SetItemInfo(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//设置下注
	VOID SetUser(BYTE cbUser[3], bool bOne, BYTE cbExp);
	//设置下注
	VOID SetTwoDouDong();
	//设置下注
	VOID SetTwoBetIndex(BYTE cbBetIndex);
	//设置下注
	VOID SetTwoSice(BYTE cbSice[2]);
	//设置下注
	VOID SetTwoShowSice(bool bSice0, bool bSice1);
	//开始开奖
	VOID SetTwoScore(SCORE lScore) { m_lTwoGameScore = lScore; }
	//开始开奖
	VOID SetThreeScore(SCORE lScore) { m_lThreeGameScore = lScore; }
	//开始开奖
	VOID SetThreeTotalWin(SCORE lTotalWin) { m_lThreeTotalWin += lTotalWin; }
	//开始开奖
	VOID SetThreeTimes(SCORE lThreeTimes) { m_lThreeTimes = lThreeTimes; }
	//设置分数
	VOID SetOneScore(SCORE lTotalWin, SCORE lTotalBetScore);
	//设置分数
	VOID SetTwoScore(SCORE lTotalWin, SCORE lTotalBet, SCORE lExpScore = 0);
	//设置分数
	VOID SetThreeScore(SCORE lTotalWin, SCORE lTotalBet, SCORE lTimes, SCORE lExpScore = 0);

	//退出比倍
	VOID ExitTwo();
	//设置提醒
	VOID SetTip(CString str) {m_strTip = str;}

	//状态函数
public:
	//设置状态
	VOID SetGameStatus(BYTE cbGameStatus);
	//加载进度
	BYTE GetImageLoad() { return m_bInitImage;}
	//进入场景
	VOID EnterScene(BYTE cbEnterScene);
	//取得时间
	WORD GetUserClock(WORD wChairID);

	//加载图片
public:
	//加载图片
	bool LoadImage();
	//加载图片
	VOID ThreadInfo();
	//更新按钮
	VOID UpDateTop(bool bDrawTop);

	DECLARE_WHDUI_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

#endif
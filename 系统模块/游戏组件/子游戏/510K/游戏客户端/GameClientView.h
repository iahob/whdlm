#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "CardControl.h"
#include "ScoreControl.h"
#include "HistoryRecord.h"
#include "RememberCardWnd.h"

//////////////////////////////////////////////////////////////////////////////////

//视图位置
#define ME_VIEW_ID					2									//视图位置

//控制消息
#define IDM_START					(WM_USER+200)						//开始消息
#define IDM_OUT_CARD				(WM_USER+201)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+202)						//PASS消息
#define IDM_OUT_PROMPT				(WM_USER+203)						//提示消息
#define IDM_CALL_SCORE				(WM_USER+204)						//叫分消息
#define	IDM_LEFT_HIT_CARD			(WM_USER+205)						//左击扑克
#define IDM_SORT_HAND_CARD			(WM_USER+206)						//排列扑克
#define IDM_LAST_TURN_CARD			(WM_USER+207)						//上轮扑克
#define IDM_TRUSTEE_CONTROL			(WM_USER+208)						//托管控制
#define IDM_XUAN_ZHAN						(WM_USER+209)						//宣战消息
#define IDM_FIND_FRIEND						(WM_USER+210)						//找同盟消息
#define IDM_ASK_FRIEND						(WM_USER+211)						//询问同盟消息
#define IDM_ADD_TIMES						(WM_USER+212)						//加倍消息
//辅助消息
#define IDM_DISPATCH_FINISH			(WM_USER+300)						//发牌完成
#define IDM_SEARCH_CARD				(WM_USER+320)						//牌型搜索

#define SEARCH_MISSILE				0									//搜索火箭
#define SEARCH_BOMB					1									//搜索炸弹
#define SEARCH_THREE_LINE_TAKE_NTWO	2									//搜索飞机
#define SEARCH_DOUBLE_LINE			3									//搜索双顺
#define SEARCH_SINGLE_LINE			4									//搜索单顺
#define SEARCH_THREE_TAKE_TWO		5									//搜索三带一对

//////////////////////////////////////////////////////////////////////////////////
class CGameClientEngine;
//游戏视图
class CGameClientView : public CGameFrameViewD3D
{
	//状态变量
public:
	bool							m_bShowScore;						//积分状态
	bool							m_bLastTurnState;					//上轮标志
	bool							m_bWaitCallScore;					//等待叫分
	bool							m_bUserPass[GAME_PLAYER];			//放弃标志

	tagHistoryScore	*		m_pHistoryScore[GAME_PLAYER];		//积分信息

	bool							m_bWaitXuanZhan;
	bool							m_bWaitFindFriend;
	bool							m_bWaitAskFriend;
	bool							m_bWaitAddTimes;
	bool							m_bShowFindFriend;//显示选择队友的牌
	bool							m_b2Biggest;						//开启2最大模式
	BYTE							m_cbSelectedCardData;
	int								m_nWidth;
	int								m_nHeight;
	//游戏变量
protected:
	
	LONG						m_lCellScore;						//单元分数
	BYTE							m_cbXuanZhanFlag[GAME_PLAYER];
	BYTE							m_cbAskFlag[GAME_PLAYER];
	BYTE							m_cbAddTimesFlag[GAME_PLAYER];			//加倍信息
	BYTE							m_cbFriendFlag[GAME_PLAYER];				//队友标签
	BYTE							m_cbTrusteeFlag[GAME_PLAYER];			//托管标签
	int								m_cbGameScore[GAME_PLAYER];
	BYTE							m_cbTurnScore;

	WORD							m_wBankerUser;						//庄家用户
	//移动变量
protected:
	bool							m_bMoveMouse;						//移动标志
	bool							m_bSwitchCard;						//交换标志
	bool							m_bSelectCard;						//选择标志
	BYTE							m_cbHoverCardItem;					//盘旋索引
	BYTE							m_cbMouseDownItem;					//点击索引
	BYTE							m_cbSwitchCardItem;					//交换索引


	bool							m_bMoveMouseEx;						//移动标志
	bool							m_bSwitchCardEx;						//交换标志
	bool							m_bSelectCardEx;						//选择标志
	BYTE							m_cbHoverCardItemEx;					//盘旋索引
	BYTE							m_cbMouseDownItemEx;					//点击索引
	BYTE							m_cbSwitchCardItemEx;					//交换索引
	//位置变量
protected:
	CPoint							m_ptUserFrame[GAME_PLAYER];			//用户位置
	CPoint							m_ptPass[GAME_PLAYER];				//放弃位置
	CPoint							m_ptBanker[GAME_PLAYER];			//庄家位置
	CPoint							m_ptCountWarn[GAME_PLAYER];			//报警位置
	CPoint							m_ptCallScore[GAME_PLAYER];			//叫分位置
	CPoint							m_ptCardFrame[GAME_PLAYER];			//牌框位置
	CPoint							m_ptTrustee[GAME_PLAYER];					//托管标识
	//爆炸动画
protected:
	bool							m_bCartoonBomb;						//动画标志
	WORD							m_wCartoonBombIndex;				//帧数索引
	CLapseCount						m_CartoonBombLapseCount;			//流逝计数

	//顺子动画
protected:
	bool							m_bCartoonLine;						//动画标志
	WORD							m_wCartoonLineIndex;				//帧数索引
	CLapseCount						m_CartoonLineLapseCount;			//流逝计数
	//连对动画
protected:
	bool							m_bCartoonDoubleLine;						//动画标志
	WORD							m_wCartoonDoubleLineIndex;				//帧数索引
	CLapseCount						m_CartoonDoubleLineLapseCount;			//流逝计数
	//三同张动画
protected:
	bool							m_bCartoonThreeLine;						//动画标志
	WORD							m_wCartoonThreeLineIndex;				//帧数索引
	CLapseCount						m_CartoonThreeLineLapseCount;			//流逝计数
	//三带牌动画
protected:
	bool							m_bCartoonThreeTake;						//动画标志
	WORD							m_wCartoonThreeTakeIndex;				//帧数索引
	CLapseCount						m_CartoonThreeTakeLapseCount;			//流逝计数
	//三连带动画
protected:
	bool							m_bCartoonThreeTakeLine;						//动画标志
	WORD							m_wCartoonThreeTakeLineIndex;				//帧数索引
	CLapseCount						m_CartoonThreeTakeLineLapseCount;			//流逝计数
	//飞机动画
protected:
	bool							m_bCartoonPlan;						//动画标志
	WORD							m_wCartoonPlanIndex;				//帧数索引
	CPoint							m_ptCartoonPlan;					//飞机位置
	CLapseCount						m_CartoonPlanLapseCount;			//流逝计数

	//翻牌动画
protected:
	bool							m_bReversalCard;					//动画标志
	WORD							m_wReversalCardIndex;				//帧数索引
	CLapseCount						m_ReversalCardLapseCount;			//流逝计数

	//发牌动画
protected:
	bool							m_bRiffleCard;						//动画标志
	bool							m_bDispatchCard;					//动画标志
	WORD							m_wDispatchCardIndex;				//帧数索引
	CLapseCount						m_DispatchCardLapseCount;			//流逝计数

	//发牌数据
protected:
	WORD							m_wStartUser;						//起始用户
	BYTE							m_cbHandCardData[NORMAL_CARD_COUNT];//发牌数据

	//报警动画
protected:
	bool							m_bCountWarn[GAME_PLAYER];			//动画标志
	WORD							m_wCountWarnIndex[GAME_PLAYER];		//帧数索引
	CLapseCount						m_CountWarnLapseCount[GAME_PLAYER];	//流逝计数

	//游戏按钮
public:
	CVirtualButton					m_btScore;							//看分按钮
	CVirtualButton					m_btStart;							//开始按钮
	CVirtualButton					m_btOutCard;						//出牌按钮
	CVirtualButton					m_btPassCard;						//PASS按钮
	CVirtualButton					m_btOutPrompt;						//提示按钮

	//功能按钮
public:
	CVirtualButton					m_btCancelTrustee;					//取消托管按钮
	CVirtualButton					m_btTrustee;						//托管按钮
	CVirtualButton					m_btLastTurn;						//上轮按钮
	CVirtualButton					m_btSortCard;						//排序按钮
	CVirtualButton					m_btRememberCard;					//看牌器按钮

	//叫分按钮
public:
	CVirtualButton					m_btXuan;
	CVirtualButton					m_btNoXuan;
	CVirtualButton					m_btAsk;
	CVirtualButton					m_btNoAsk;
	CVirtualButton					m_btAdd;
	CVirtualButton					m_btNoAdd;
	CVirtualButton					m_btRespond;
	CVirtualButton					m_btNoRespond;
	CVirtualButton					m_btSelectFriend;

	//资源变量
protected:
	CD3DSprite						m_TextureBankerFlag;				//庄家标志
	CD3DSprite						m_TextureFriendFlag;					//队友标识
	
	CD3DTexture						m_TextureFill;						//背景资源
	CD3DTexture						m_TextureBack;						//背景资源
	CD3DTexture						m_TextureGameInfo;						//房间设置
	CD3DTexture						m_TextureUserBG;				//用户框架
	CD3DTexture						m_TextureIcon;						//金币标
	CD3DTexture						m_TextureIconNum;				//金币数
	CD3DTexture						m_TextureRemnantCardNumber;//剩余牌数
	CD3DTexture						m_TextureClockBack;					//时钟背景
	CD3DTexture						m_TextureClockNum;					//时钟数字
	CD3DTexture						m_TextureTurnScoreBG;				//本轮分背景
	CD3DTexture						m_TextureTurnScoreNum;				//本轮分数字
	CD3DTexture						m_TextureSelectFriendBg;			//选队友背景
	CD3DTexture						m_TextureUserPass;					//放弃标志
	CD3DTexture						m_TextureCountWarn;					//扑克警报
	CD3DTexture						m_TextureCartoonRocket;				//火箭动画
	CD3DTexture						m_TextureCartoonRocketLight;		//
	CD3DTexture						m_TextureCartoonBomb;				//炸弹动画
	CD3DTexture						m_TextureCartoonPlan;				//飞机动画
	CD3DTexture						m_TextureCartoonLine;				//顺子动画
	CD3DTexture						m_TextureCartoonDoubleLine;		//连对动画
	CD3DTexture						m_TextureCartoonThreeLine;		//三同张动画
	CD3DTexture						m_TextureCartoonThreeTake;		//三带动画
	CD3DTexture						m_TextureCartoonThreeTakeLine;//三连带动画
	CD3DTexture						m_TextureDispatchCard;				//资源变量
	CD3DTexture						m_TextureHistoryScore;				//资源变量
	CD3DTexture						m_TextureTrustee;						//托管标识
	CD3DTexture						m_TextureAddTimes;
	CD3DTexture						m_TextureNoAddTimes;
	CD3DTexture						m_TextureAsk;
	CD3DTexture						m_TextureNoAsk;
	CD3DTexture						m_TextureXuanZhan;
	CD3DTexture						m_TextureNoXuanZhan;
	CD3DTexture						m_TextureRespond;
	CD3DTexture						m_TextureNoRespond;
	//控件变量
public:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CScoreControl					m_ScoreControl;						//积分控件
	CGameClientEngine					*m_pGameClientDlg;					//父类指针
	CRememberCardWnd				m_RememberCardWnd;					//记牌器窗口

	//扑克控件
public:
	CCardControl					m_SelectFriendCardControl;	//选择队友控件
	CCardControl					m_HandCardControl[GAME_PLAYER];		//用户扑克
	CCardControl					m_UserCardControl[GAME_PLAYER];		//扑克视图

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//界面函数
protected:
	//动画驱动
	virtual VOID CartoonMovie();
	//配置设备
	virtual VOID InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//庄家设置
public:
	//设置庄家
	VOID SetBankerUser(WORD wBankerUser);

	//功能函数
public:
	//刷新视图
	VOID RefreshGameView();
	//单元积分
	VOID SetCellScore(LONG lCellScore);
	//设置结束
	VOID ShowGameScoreInfo(tagScoreInfo & ScoreInfo);
	// 绘画数字
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// 绘画数字
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// 绘画数字
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	// 绘画时钟
	VOID DrawClock(CD3DDevice * pD3DDevice, WORD wUserClock,WORD wChair);
	//显示选朋友
	VOID	 ShowFindFriend(bool bShow);
	//牌型动画
public:
	//飞机动画
	VOID ShowCartoonPlan(bool bCartoon);
	//爆炸动画
	VOID ShowCartoonBomb(bool bCartoon);
	//顺子动画
	VOID ShowCartoonLine(bool bCartoon);
	//连对动画
	VOID ShowCartoonDoubleLine(bool bCartoon);
	//三同张动画
	VOID ShowCartoonThreeLine(bool bCartoon);
	//三带牌动画
	VOID ShowCartoonThreeTake(bool bCartoon);
	//三连带动画
	VOID ShowCartoonThreeTakeLine(bool bCartoon);
	//状态函数
public:
	//设置上轮
	VOID SetLastTurnState(bool bLastTurnState);
	//设置放弃
	VOID SetUserPassState(WORD wChairID, bool bUserPass);
	//设置警告
	VOID SetUserCountWarn(WORD wChairID, bool bCountWarn);
	//设置积分
	VOID SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore);

	//设置宣战
	VOID SetXuanZhan(WORD wChairID,bool bXuanZhan);
	//设置询问
	VOID SetAsk(WORD wChairID,bool bAsk,bool bRespond=false);
	//设置加倍
	VOID	 SetAddTimes(WORD wChairID,bool bAddTimes);
	//设置得分
	VOID SetGameScore(WORD wChairID,int cbScore);
	//本轮牌分
	VOID SetTurnScore(BYTE cbTurnScore);
	//队友标签
	VOID	 SetFriendFlag(WORD wChairID,BYTE cbFlag);
	//托管标签
	VOID SetTrusteeFlag(WORD wChairID,BYTE cbFlag);
	//发牌函数
public:
	//发牌动画
	VOID StopDispatchCard();
	//发牌动画
	VOID ShowDispatchCard(BYTE cbCardData[], BYTE cbCardCount, WORD wStartUser);

	//按钮消息
public:
	//开始按钮
	VOID OnStart();
	//积分按钮
	VOID OnLookScore();
	//出牌按钮
	VOID OnOutCard();
	//排序按钮
	VOID OnSortCard();
	//上轮按钮
	VOID OnLastTurn();
	//托管按钮
	VOID OnTrustee();
	//托管按钮
	VOID OnCancelTrustee();
	//PASS按钮
	VOID OnPassCard();
	//提示按钮
	VOID OnOutPrompt();
	//叫分按钮
	VOID OnCallScore(UINT nCtrlID);
	//宣战按钮
	VOID OnXuanZhan(UINT nCtrlID);
	//问同盟按钮
	VOID OnAskFriend(UINT nCtrlID);
	//加倍按钮
	VOID OnAddTimes(UINT nCtrlID);
	//选队友
	VOID	 OnSelectFriend();
	//重载函数
public:
	//消息命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//消息函数
public:
	//鼠标消息
	VOID OnRButtonUp(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnMouseMove(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint MousePoint);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif
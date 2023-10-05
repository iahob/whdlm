#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//积分信息
struct tagScoreInfo
{

	//游戏成绩
	SCORE							lCellScore;								//单元积分
	SCORE							lGameScore[GAME_PLAYER];				//游戏成绩
	SCORE							lCollectScore[GAME_PLAYER];				//汇总成绩
	int									cbScore[GAME_PLAYER];					//游戏得分
	BOOL							cbUserTimes[GAME_PLAYER];				//玩家倍数
	BYTE								cbBaseTimes;										//基础倍速
	//玩家信息
	WORD							wMeChairID;								//自己位置
	WORD							wBankerUser;							//庄家用户

	//用户信息
	IClientUserItem*				pIClientUserItem[GAME_PLAYER];	
	DWORD							dwUserID[GAME_PLAYER];					//用户标识
	TCHAR							szNickName[GAME_PLAYER][LEN_NICKNAME];	//用户昵称
};

//////////////////////////////////////////////////////////////////////////////////

//积分控件
class CScoreControl : public CVirtualWindow
{
	//变量定义
protected:
	tagScoreInfo					m_ScoreInfo;						//积分信息

	//灰度动画
protected:
	BYTE							m_cbAlphaIndex;						//透明索引
	CLapseCount						m_AlphaLapseCount;					//流逝计数

	//成绩动画
protected:
	BYTE							m_cbCartoonIndex;					//动画索引
	CLapseCount						m_CartoonLapseCount;				//流逝计数

	//控件变量
protected:
	CVirtualButton					m_btClose;							//关闭按钮
	CVirtualButton					m_btAgain;							//再来一局
	//资源变量
protected:
	CD3DFont						m_D3DFont;						//积分字体

	CD3DTexture						m_TextureGameScoreBG;					//积分视图
	CD3DTexture						m_TextureWinFlag;					//赢标志
	CD3DTexture						m_TextureLostFlag;					//输标志
	CD3DTexture						m_TextureEqualFlag;					//和标志
	CD3DTexture						m_TextureWinNum;					//赢数字
	CD3DTexture						m_TextureLostNum;					//输数字



	//函数定义
public:
	//构造函数
	CScoreControl();
	//析构函数
	virtual ~CScoreControl();

	//系统事件
protected:
	//动画消息
	virtual VOID OnWindowMovie();
	//创建消息
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	//销毁消息
	virtual VOID OnWindowDestory(CD3DDevice * pD3DDevice);

	//重载函数
protected:
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	//按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	//绘画窗口
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);

	//功能函数
public:
	//隐藏窗口
	VOID CloseControl();
	//设置积分
	VOID SetScoreInfo(tagScoreInfo & ScoreInfo);
	// 绘画数字
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif
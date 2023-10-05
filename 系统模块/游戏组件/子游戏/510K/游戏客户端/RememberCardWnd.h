#ifndef REMEMBER_CARD_WND_HEAD_FILE
#define REMEMBER_CARD_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <list>
#include <algorithm>
using namespace std;

#define				CARD_VALUE_COUNT				15				  //牌数值个数（从A到大王)
#define				MOST_CARD_COUNT					8				  //最大张数
#define				MASK_VALUE						0x0F			  //数值掩码
//////////////////////////////////////////////////////////////////////////////////


class CRememberCardWnd :public CVirtualWindow
{

protected:
	list<BYTE>						m_ListCard[CARD_VALUE_COUNT];		//每个数值的牌用一链表维护
	//元素0到12为 牌A到K
	CSize							m_sizeRememberCard;
	//资源变量
protected:
	CD3DTexture						m_TextureRememberCard;				//记牌
	CD3DTexture						m_TextureRememberNumber;			//数字
	CD3DTexture						m_TextureWndBack;					//背景


	//函数定义
public:
	//构造函数
	CRememberCardWnd();
	//析构函数
	virtual ~CRememberCardWnd();

	//系统事件
protected:
	//创建消息
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	//销毁消息
	virtual VOID OnWindowDestory(CD3DDevice * pD3DDevice);

	//重载函数
protected:
	//绘画窗口
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);
	
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos) { return; }
	//按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos) { return; } 
	//功能函数
public:
	//更新牌张数
	VOID UpdateCardCount(const BYTE cbCardData[33], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }	
	//初始化牌链表
	VOID InitializeCardList();	
	//销毁牌链表
	VOID DestroyCardList();
	// 绘画数字
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
};













//////////////////////////////////////////////////////////////////////////////////

#endif
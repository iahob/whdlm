#pragma once

//夺宝滚动
#include "ObjectMove.h"
#include "D3DTextureIndex.h"


class CIndianaRoll
{
	//信息变量
public:
	//宝石移动
	CIndianaMove						m_IndianaMove;
	CIndianaMove						m_IndianaMoveTow;

	//资源变量
public:
	CD3DTexture							m_ImageBack;
	CD3DTextureIndex					m_ImageGem;
	CSize								m_sizeGem;							//宝石大小


	//构造函数
public:
	CIndianaRoll(void);
	~CIndianaRoll(void);

	//功能函数
public:
	//初始化
	void InitIndiana(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//界面函数
public:
	//动画驱动
	void CartoonMovie();
	//绘画函数
	void DrwaIndiana(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//控制函数
public:
	//开始移动
	void BeginMove();

	//接受消息
	BOOL PreTranslateMessage(MSG* pMsg);


};

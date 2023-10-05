#pragma once
#include "animation.h"

class CAnimationScroll : public CAnimation
{
	bool			m_bScrollX;					//水平滚动
	int				m_nPixels;					//滚动大小
	int				m_nSteps;					//滚动速度

public:
	CAnimationScroll(void);
	~CAnimationScroll(void);

	//界面函数
public:
	//动画驱动
	bool CartoonMovie();
	//启动动画
	VOID ShowAnimation(bool bShow, int nLapseCount = 1, enPlayMode PlayMode = enOnce);
	//配置设备
	bool InitAnimationSingle(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageX, int nImageY, bool bScrollX, int nPixels);
	//绘画界面
	VOID DrawAnimationScroll(dex::CD3DDevice * pD3DDevice);
	//绘画界面
	VOID DrawAnimationScroll(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
	//绘画界面
	VOID DrawAnimationScroll(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawIndex, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
	//绘画界面
	VOID DrawAnimationScrollPos(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
};

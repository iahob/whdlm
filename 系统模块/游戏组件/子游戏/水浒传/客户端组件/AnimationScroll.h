#pragma once
#include "animation.h"

class CAnimationScroll : public CAnimation
{
	bool			m_bScrollX;					//ˮƽ����
	int				m_nPixels;					//������С
	int				m_nSteps;					//�����ٶ�

public:
	CAnimationScroll(void);
	~CAnimationScroll(void);

	//���溯��
public:
	//��������
	bool CartoonMovie();
	//��������
	VOID ShowAnimation(bool bShow, int nLapseCount = 1, enPlayMode PlayMode = enOnce);
	//�����豸
	bool InitAnimationSingle(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageX, int nImageY, bool bScrollX, int nPixels);
	//�滭����
	VOID DrawAnimationScroll(dex::CD3DDevice * pD3DDevice);
	//�滭����
	VOID DrawAnimationScroll(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
	//�滭����
	VOID DrawAnimationScroll(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawIndex, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
	//�滭����
	VOID DrawAnimationScrollPos(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
};

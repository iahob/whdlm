#pragma once

//�ᱦ����
#include "ObjectMove.h"
#include "D3DTextureIndex.h"


class CIndianaRoll
{
	//��Ϣ����
public:
	//��ʯ�ƶ�
	CIndianaMove						m_IndianaMove;
	CIndianaMove						m_IndianaMoveTow;

	//��Դ����
public:
	CD3DTexture							m_ImageBack;
	CD3DTextureIndex					m_ImageGem;
	CSize								m_sizeGem;							//��ʯ��С


	//���캯��
public:
	CIndianaRoll(void);
	~CIndianaRoll(void);

	//���ܺ���
public:
	//��ʼ��
	void InitIndiana(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//���溯��
public:
	//��������
	void CartoonMovie();
	//�滭����
	void DrwaIndiana(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//���ƺ���
public:
	//��ʼ�ƶ�
	void BeginMove();

	//������Ϣ
	BOOL PreTranslateMessage(MSG* pMsg);


};

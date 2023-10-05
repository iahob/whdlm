#ifndef REMEMBER_CARD_WND_HEAD_FILE
#define REMEMBER_CARD_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <list>
#include <algorithm>
using namespace std;

#define				CARD_VALUE_COUNT				15				  //����ֵ��������A������)
#define				MOST_CARD_COUNT					8				  //�������
#define				MASK_VALUE						0x0F			  //��ֵ����
//////////////////////////////////////////////////////////////////////////////////


class CRememberCardWnd :public CVirtualWindow
{

protected:
	list<BYTE>						m_ListCard[CARD_VALUE_COUNT];		//ÿ����ֵ������һ����ά��
	//Ԫ��0��12Ϊ ��A��K
	CSize							m_sizeRememberCard;
	//��Դ����
protected:
	CD3DTexture						m_TextureRememberCard;				//����
	CD3DTexture						m_TextureRememberNumber;			//����
	CD3DTexture						m_TextureWndBack;					//����


	//��������
public:
	//���캯��
	CRememberCardWnd();
	//��������
	virtual ~CRememberCardWnd();

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	//������Ϣ
	virtual VOID OnWindowDestory(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);
	
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos) { return; }
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos) { return; } 
	//���ܺ���
public:
	//����������
	VOID UpdateCardCount(const BYTE cbCardData[33], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }	
	//��ʼ��������
	VOID InitializeCardList();	
	//����������
	VOID DestroyCardList();
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
};













//////////////////////////////////////////////////////////////////////////////////

#endif
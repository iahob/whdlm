#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagScoreInfo
{

	//��Ϸ�ɼ�
	SCORE							lCellScore;								//��Ԫ����
	SCORE							lGameScore[GAME_PLAYER];				//��Ϸ�ɼ�
	SCORE							lCollectScore[GAME_PLAYER];				//���ܳɼ�
	int									cbScore[GAME_PLAYER];					//��Ϸ�÷�
	BOOL							cbUserTimes[GAME_PLAYER];				//��ұ���
	BYTE								cbBaseTimes;										//��������
	//�����Ϣ
	WORD							wMeChairID;								//�Լ�λ��
	WORD							wBankerUser;							//ׯ���û�

	//�û���Ϣ
	IClientUserItem*				pIClientUserItem[GAME_PLAYER];	
	DWORD							dwUserID[GAME_PLAYER];					//�û���ʶ
	TCHAR							szNickName[GAME_PLAYER][LEN_NICKNAME];	//�û��ǳ�
};

//////////////////////////////////////////////////////////////////////////////////

//���ֿؼ�
class CScoreControl : public CVirtualWindow
{
	//��������
protected:
	tagScoreInfo					m_ScoreInfo;						//������Ϣ

	//�Ҷȶ���
protected:
	BYTE							m_cbAlphaIndex;						//͸������
	CLapseCount						m_AlphaLapseCount;					//���ż���

	//�ɼ�����
protected:
	BYTE							m_cbCartoonIndex;					//��������
	CLapseCount						m_CartoonLapseCount;				//���ż���

	//�ؼ�����
protected:
	CVirtualButton					m_btClose;							//�رհ�ť
	CVirtualButton					m_btAgain;							//����һ��
	//��Դ����
protected:
	CD3DFont						m_D3DFont;						//��������

	CD3DTexture						m_TextureGameScoreBG;					//������ͼ
	CD3DTexture						m_TextureWinFlag;					//Ӯ��־
	CD3DTexture						m_TextureLostFlag;					//���־
	CD3DTexture						m_TextureEqualFlag;					//�ͱ�־
	CD3DTexture						m_TextureWinNum;					//Ӯ����
	CD3DTexture						m_TextureLostNum;					//������



	//��������
public:
	//���캯��
	CScoreControl();
	//��������
	virtual ~CScoreControl();

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowMovie();
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	//������Ϣ
	virtual VOID OnWindowDestory(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);

	//���ܺ���
public:
	//���ش���
	VOID CloseControl();
	//���û���
	VOID SetScoreInfo(tagScoreInfo & ScoreInfo);
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif
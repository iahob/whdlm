#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//�궨��

//�Ʊ�����
#define CARD_LAND					0									//��������
#define CARD_BOOR					1									//ũ�����

//��������
#define INVALID_ITEM				0xFF								//��Ч����

//���Զ���
#define MAX_CARD_COUNT				13									//�˿���Ŀ
#define SPACE_CARD_DATA				255									//����˿�

//��ֵ����
#define	CARD_MASK_COLOR				0xF0								//��ɫ����
#define	CARD_MASK_VALUE				0x0F								//��ֵ����

//��ඨ��
#define DEF_X_DISTANCE				23									//Ĭ�ϼ��
#define DEF_Y_DISTANCE				20									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE			20									//Ĭ�ϼ��

//��ඨ��
#define DEF_X_DISTANCE_SMALL		9									//Ĭ�ϼ��
#define DEF_Y_DISTANCE_SMALL		8									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE_SMALL	10									//Ĭ�ϼ��

#define DEF_X_DISTANCE_BANKER_CARD	25									//���Ƽ��

//������Ϣ
#define IDM_OUT_CARD_FINISH			(WM_USER+400)						//�������

//////////////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//X ���з�ʽ
enum enXCollocateMode
{ 
	enXLeft,						//�����
	enXCenter,						//�ж���
	enXRight,						//�Ҷ���
};

//Y ���з�ʽ
enum enYCollocateMode 
{ 
	enYTop,							//�϶���
	enYCenter,						//�ж���
	enYBottom,						//�¶���
};

//////////////////////////////////////////////////////////////////////////////////

//�˿˽ṹ
struct tagCardItem
{
	bool							bShoot;								//�����־
	bool							bFlag;								//��Ӱ���
	BYTE							cbCardData;							//�˿�����
};

//////////////////////////////////////////////////////////////////////////////////

//�˿���Դ
class CCardResource
{
	//��Ԫ����
	friend class CCardControl;

	//�˿���Դ
protected:
	CSize							m_CardSize;							//�˿˴�С
	CSize							m_CardSizeSmall;					//�˿˴�С
	CSize							m_CardSizeBanker;

	//�˿���Դ
protected:
	CD3DTexture						m_TextureCard;						//ͼƬ��Դ
	CD3DTexture						m_TextureSmallCard;					//ͼƬ��Դ
	CD3DTexture						m_TextureNum;							//ʣ������
	//��������
public:
	//���캯��
	CCardResource();
	//��������
	virtual ~CCardResource();

	//��Դ����
public:
	//������Դ
	VOID Initialize(CD3DDevice * pD3DDevice);
	//��ȡ��С
	VOID GetCardSize(bool bSamllMode, CSize & CardSize);
	//��ȡ��Դ
	VOID GetCardTexture(bool bSmallMode, CD3DTexture * * pD3DTexture);
	//��ȡ��С
	VOID GetbBankerCardSize(CSize & CardSize) { CardSize = m_CardSizeBanker ;}
};

//////////////////////////////////////////////////////////////////////////////////

//�˿˿ؼ�
class CCardControl
{
	//���Ա���
protected:
	bool							m_bSmallMode;						//С��ģʽ
	bool							m_bHorizontal;						//��ʾ����
	bool							m_bPositively;						//��Ӧ��־
	bool							m_bDisplayItem;						//��ʾ��־
	BYTE							m_cbBackGround;						//������־
	bool							m_bShowNum;
	//ѡ�����
protected:
	BYTE							m_cbHeadIndex;						//��ʼ����
	BYTE							m_cbTailIndex;						//��������

	//���Ʊ���
protected:
	bool							m_bOutCard;							//���Ʊ�־
	WORD							m_wOutCardIndex;					//��������
	CLapseCount						m_OutCardLapseCount;				//���ż���

	//�������
protected:
	UINT							m_nXDistance;						//������
	UINT							m_nYDistance;						//������
	UINT							m_nShootDistance;					//����߶�

	//λ�ñ���
protected:
	CPoint							m_DispatchPos;						//�ɷ�λ��
	CPoint							m_BenchmarkPos;						//��׼λ��
	enXCollocateMode				m_XCollocateMode;					//��ʾģʽ
	enYCollocateMode				m_YCollocateMode;					//��ʾģʽ

	//�˿�����
protected:
	BYTE							m_cbCardCount;						//�˿���Ŀ
	BYTE							m_cbShowCount;						//��ʾ��Ŀ
	CPoint							m_CardItemPoint[MAX_CARD_COUNT];	//�˿�λ��
	tagCardItem						m_CardItemArray[MAX_CARD_COUNT];	//�˿�����

	//�߼�����
protected:
	CGameLogic						m_GameLogic;						//�߼�����

	//��Դ����
public:
	static CCardResource			m_CardResource;						//�˿���Դ

	//��������
public:
	//���캯��
	CCardControl();
	//��������
	virtual ~CCardControl();

	//�����˿�
public:
	//�����˿�
	bool SetCardData(BYTE cbCardCount);
	//�����˿�
	bool SetCardData(BYTE cbCardData[], BYTE cbCardCount);
	//�Ƿ���ʾʣ����
	VOID	 SetShowNum(bool bShow){m_bShowNum=bShow;};
	//�����˿�
public:
	//�����˿�
	bool SetShootArea(BYTE cbHeadIndex, BYTE cbTailIndex);
	//�����˿�
	bool SetShootCard(BYTE cbCardData[], BYTE cbCardCount);
	//������ͬ����
	bool SetShootSameCard(BYTE cbCardData);
	//����˳��
	bool SetShootOrderCard( BYTE cbCardOne, BYTE cbCardTwo );
	//���ñ��
	bool	SetFlag(BYTE cbIndex);
	//�ɷ��˿�
public:
	//�Ƿ����
	bool IsOutCard() { return m_bOutCard; }
	//���ó���
	bool ShowOutCard(BYTE cbCardData[], BYTE cbCardCount);

	//�˿˿���
public:
	//ɾ���˿�
	bool RemoveShootItem();
	//�ƶ��˿�
	bool MoveCardItem(BYTE cbTargerItem);
	//ѡ���˿�
	bool SetSelectIndex(BYTE cbHeadIndex, BYTE cbTailIndex);
	//�����˿�
	bool SwitchCardItem(BYTE cbSourceItem, BYTE cbTargerItem);

	//��ȡ�˿�
public:
	//��ȡ�˿�
	tagCardItem * GetCardFromIndex(BYTE cbIndex);
	//��ȡ�˿�
	tagCardItem * GetCardFromPoint(CPoint & MousePoint);

	//��ȡ�˿�
public:
	//�˿���Ŀ
	BYTE GetCardCount() { return m_cbCardCount; }
	//��ȡ�˿�
	BYTE GetCardData(BYTE cbCardData[], BYTE cbBufferCount);
	//��ȡ�˿�
	BYTE GetShootCard(BYTE cbCardData[], BYTE cbBufferCount);

	//״̬��ѯ
public:
	//��ѯ����
	bool GetDirection() { return m_bHorizontal; }
	//��ѯ��Ӧ
	bool GetPositively() { return m_bPositively; }
	//��ѯ��ʾ
	bool GetDisplayItem() { return m_bDisplayItem; }

	//״̬����
public:
	//����ģʽ
	VOID SetSmallMode(bool bSmallMode) { m_bSmallMode=bSmallMode; }
	//���÷���
	VOID SetDirection(bool bHorizontal) { m_bHorizontal=bHorizontal; }
	//������Ӧ
	VOID SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//������ʾ
	VOID SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }

	//�˿˿���
public:
	//������ʾ
	VOID SetShowCount(BYTE cbShowCount);
	//���ñ���
	VOID SetBackGround(BYTE cbBackGround);
	//���þ���
	VOID SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance);

	//���ƺ���
public:
	//��ȡ����
	VOID GetCenterPoint(CPoint & CenterPoint);
	//�ɷ�λ��
	VOID SetDispatchPos(INT nXPos, INT nYPos);
	//��׼λ��
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//�¼�����
public:
	//ִ�ж���
	VOID CartoonMovie();
	//�����Ϣ
	bool OnEventSetCursor(CPoint Point);
	//�滭�˿�
	VOID DrawCardControl(CD3DDevice * pD3DDevice);
	//�ڲ�����
public:
	//��ȡ��С
	VOID GetControlSize(CSize & ControlSize);
	//��ȡԭ��
	VOID GetOriginPoint(CPoint & OriginPoint);
	//�����л�
	BYTE SwitchCardPoint(CPoint & MousePoint);
	// �滭����
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	//��̬����
public:
	//������Դ
	static VOID LoadCardResource(CD3DDevice * pD3DDevice);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "PolygonShape.h"


// ͣ����Ϣ
struct tagStayInfo
{
	uint		nStayStart;								// ͣ����ʼ
	uint		nStayTime;								// ͣ��ʱ��
};
typedef CWHArray< tagStayInfo >		CArrayStay;

// ����ṹ
struct tagFishAndroid
{
	// ������Ϣ
	uint				nFishKey;				// ������
	byte				nFishType;				// ������
	int					nFishScore;				// �����
	word				wHitChair;				// ��ɱλ��
	EnumFishState		nFishState;				// ��״̬

	// ʱ����Ϣ
	uint				unCreateTime;			// ����ʱ��
	uint				unClientTime;			// �ͻ���ʱ��

	// λ����Ϣ
	CDoublePoint		ptLast;					// �ϴ�λ��
	CDoublePoint		ptPosition;				// ��ǰλ��

	// ·��ƫ��
	float				fRotateAngle;			// �Ƕ�
	CShortPoint 		PointOffSet;			// ƫ��

	// ·����Ϣ
	int					nBezierCount;			// ·������
	tagBezierPoint		TBezierPoint[BEZIER_POINT_MAX];		// ·����Ϣ
	CArrayStay			ArrayStayInfo;			// ͣ����Ϣ

	// �������
	CFloatTransform		FTFish;					// ����
	CPolygonShape		PSFish;					// ����

	~tagFishAndroid()
	{
		ArrayStayInfo.RemoveAll();
		ArrayStayInfo.FreeMemory();
	}
};


// �ӵ��ṹ
struct tagBulletAndroid
{
	// ������Ϣ
	int					nBulletKey;				// �ӵ��ؼ�ֵ
	int					nTrackFishIndex;		// ׷��������
	bool				bSpecialt;				// �����ӵ�

	// λ����Ϣ
	CDoublePoint		ptPosition;				// ��ǰλ��
	CDoublePoint		ptOffset;				// ƫ��λ��
	bool				bEjection;				// ����

	// �������
	CFloatTransform		FTBullet;
	CPolygonShape		SPBullet;
};


//////////////////////////////////////////////////////////////////////////

//AI��
class CAndroidUserItemSink : public IAndroidUserItemSink
{

	// ������Ϣ
public:
	word							m_wMeChairID;					// AIλ��
	longlong						m_lAndroidScore;				// AI����
	long							m_lCooling;						// ��ȴʱ��
	uint							m_lCoolingTime;					// ��ȴʱ��
	int								m_nTrackFishIndex;				// ׷������

	long							m_lStopFireTime;				// ֹͣ����
	long							m_lStopFireModify;
	bool							m_bStopFire;
	// ������Ϣ
public:
	bool							m_bFreeFire;					// �������
	uint							m_nFireTime;					// ���ʱ��
	// ���Ա���
protected:
	uint							m_nSceneTime;						// ����ʱ��
	// ��ע����
	longlong						m_lAndroidAreaScore[5];						// AI����
	int								m_nAndroidMultiple[5];					// AI��̨
	longlong						m_lAndroidScoreMax;						// AI�������

	// �ʱ��
protected:
	DWORD							m_dwSecondTime;					// ����ʱ��
	int								m_nLaserTime;					// ����ʱ��
	int								m_nSpeedTime;					// ����ʱ��

	// �����Ϣ
public:
	CPoint							m_PointPlay[PlayChair_Max];			// ���λ��

	// �ӵ���Ϣ
public:
	int								m_nBulletVelocity;					// �ӵ��ٶ�
	int								m_nBulletCoolingTime;				// �ӵ���ȴ

	// ����Ϣ
public:
	uint							nStartTime;						// ��ʼʱ��
	int								m_nBulletKey;					// �ӵ��ؼ�ֵ
	CWHArray< tagFishAndroid >		m_FishInfo;						// ������Ϣ
	CWHArray< tagBulletAndroid >	m_BulletInfo;					// �ڵ��滭��Ϣ

	// ���䱶��
protected:
	int								m_nMultipleValue[Multiple_Max];
	int								m_nMultipleIndex;
	uint							m_nMultipleTime;
	uint							m_nMultipleModify;
	int								m_nScoreRatio;							// ��������
	//�ؼ�����
protected:
	IAndroidUserItem *				m_pIAndroidUserItem;				// �û��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	// ��Ϣ����
public:
	// ͬ����Ϣ
	bool OnSubSynchronizationTime(const void * pBuffer, word wDataSize);
	// ���㴴��
	bool OnSubCreateFish( const void * pBuffer, word wDataSize );
	// �㲶��
	bool OnSubCatchFish( const void * pBuffer, word wDataSize );
	// ��������
	bool OnSubExchangeScene(const void * pBuffer, WORD wDataSize);
	// ���Ϣ
	bool OnSubSupply( const void * pBuffer, WORD wDataSize );
	// ͣ����
	bool OnSubStayFish( const void * pData, WORD wDataSize );

	// ���ܺ���
public:
	// ��ת��
	CDoublePoint CAndroidUserItemSink::Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome );
	// ����������
	CDoublePoint CAndroidUserItemSink::PointOnCubicBezier( tagBezierPoint* cp, double t );
	// AI����ѡ��
	int SelectMultiple(bool bChange = false);
	// AI�������÷���
	void ReSetScore();
	//�޸ı���
	bool ModifyMultiple(bool bMultipleModifyTime, int nMultipleIndex = Multiple_Max);
};

//////////////////////////////////////////////////////////////////////////

#endif
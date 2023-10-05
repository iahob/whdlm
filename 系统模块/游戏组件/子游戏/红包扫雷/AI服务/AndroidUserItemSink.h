#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//AI��
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//������Ϣ
	static int						m_nScoreRatio;						//��������
	static BYTE						m_cbPlayTime;						//����ʱ��
	static BYTE						m_cbBagCount;						//�������
	static LONGLONG					m_lScoreRangeMin;					//�����Сֵ
	static LONGLONG					m_lScoreRangeMax;					//������ֵ
	static LONGLONG					m_lDispatchCondition;				//����Ҫ��
	static DOUBLE					m_dBagTimes;						//����
	static BYTE						m_cbThunderPoint;					//����׺�
	static BYTE						m_cbThunderPositon;					//�׺�λ�ã�0С�����һλ��1С����ڶ�λ

	//AI����
	static BYTE						m_cbMinBagListCount;				//�����б������������ֵʱ��ʼ���ף�
	static BYTE						m_cbMaxRealBagListCount;			//���������б������������ֵʱ�����ף�
	static BYTE						m_cbSetBagCount[2];					//���׸���
	static BYTE						m_cbSetBagTime[2];					//���׼��
	static LONGLONG					m_lSetBagScore[2];					//���׺����С
	static BYTE						m_cbStartGetBagTime;				//�������ʼʱ��
	static BYTE						m_cbGetBagTime[2];					//��������
	static BYTE						m_cbMaxGetBagCount;					//������������
	static BYTE						m_cbLastNoGetBagCount;				//������������
	static BYTE						m_cbRandSetBagCount;				//���׸���
	static BYTE						m_cbRandSetBagTime;					//���׼��
	static BYTE						m_cbRandGetBagTime;					//��������
	static BYTE						m_cbAIGetBagCount;					//AI���������
	static int						m_nRandSetBagMaxCount;				//������������޼��ʱ��
	static DWORD					m_dwLastSetBagTime;					//�������ʱ��
	static DWORD					m_dwLastSetBagCountTime;			//������������޸�ʱ��
	static DWORD					m_dwLastGetBagTime;					//��������ʱ��


	//��Ϸ����
	DWORD							m_dwStartGetBagTime;				//��ʼ�Ƴٺ��ʱ��
	
	WORD							m_wBankerUser;						//��ǰ�������
	BYTE							m_cbLeftTime;						//ʣ������ʱ��
	LONGLONG						m_lBagKey;							//��ǰ�����ʶ
	LONGLONG						m_lBagScore;						//ׯ�Һ������
	LONGLONG						m_lSetBagTotalScore;				//���ͺ������
	
	BYTE							m_cbGetCount;						//��������
	WORD							m_wGetUser[MAX_BAG_COUNT];			//�������
	DOUBLE							m_dBagDetail[MAX_BAG_COUNT];		//������
	DOUBLE							m_dBagBalance;						//������
	bool							m_bThunderPoint[MAX_BAG_COUNT];		//�Ƿ�����
	std::vector<stRedBagInfo>		m_vecRedBag;						//�����б�

	//�ؼ�����
protected:
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() {delete this; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//��ʼ�ӿ�
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool  RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool  OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool  OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool  OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool  OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	
	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID *pData, WORD wDataSize);
	//�ɷ����
	bool OnSubDispatchBag(VOID *pData, WORD wDataSize);
	//ȡ�����
	bool OnSubCancelBag(VOID *pData, WORD wDataSize);
	//�����
	bool OnSubGetBag(VOID *pData, WORD wDataSize);
	//����ʧ��
	bool OnSubGetBagFail(VOID *pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);


	//��������
protected:
	//�Ƿ�����
	bool IsSetBag();
	//�Ƿ�����
	bool IsGetBag();
};

//////////////////////////////////////////////////////////////////////////

#endif

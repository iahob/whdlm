#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FREE					105									//����ʱ��
#define IDI_USER_POINT				106									//��עʱ��
#define IDI_GAME_END				107									//����ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{

	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
VOID *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_FREE:
		{	
			m_pIAndroidUserItem->KillGameTimer(IDI_FREE);
			if(rand()%10==0)m_pIAndroidUserItem->SendSocketData(SUB_C_LEAVE);

			return true;
		}

	case IDI_USER_POINT:
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_POINT);
			//LONGLONG lMinValue = GetPrivateProfileInt(TEXT("Android"), TEXT("MinValue"),100, TEXT("FiveStar.ini"));
			//LONGLONG lMaxValue = GetPrivateProfileInt(TEXT("Android"), TEXT("MaxValue"),100000, TEXT("FiveStar.ini"));
			const LONGLONG BetScore[5] = {1000,10000,100000,1000000,10000000};
			// ������ע����
			CMD_C_PlaceBet Point;
			ZeroMemory(&Point, sizeof(Point));
			Point.cbBetArea = rand() % 5;
			Point.lBetScore = BetScore[rand()%5]; //(lMinValue + rand() % (lMaxValue - lMinValue)) / 10;
			//for(int i=enAreaBlack;i<enAreaCount;i++)
			//{
			//	// ȥ��λ��
			//	Point.lAreaPoint[i] = (lMinValue + rand()%(lMaxValue-lMinValue))/10;
			//	Point.lAreaPoint[i] *= 10;
			//}
			//CString aa;
			//aa.Format(TEXT("��������ע����%d��ע����%I64d"), Point.cbBetArea, Point.lBetScore);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			m_pIAndroidUserItem->SendSocketData(SUB_C_SET_POINT,&Point,sizeof(Point));
			//������ע
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_POINT, 2);
			return true;
		}
	case IDI_GAME_END:

		return true;
	}
	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{

	switch (wSubCmdID)
	{
	case SUB_S_USERPOINT:
		{
			return true;
		}
	case SUB_S_GAME_FREE:		//��Ϸ����
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_GameFree));
			if (wDataSize!=sizeof(CMD_S_GameFree)) return false;
			//����ע
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_POINT);
			//��Ϣ����
			CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pData;

			//����ʱ��
			m_pIAndroidUserItem->SetGameTimer(IDI_FREE,pGameFree->cbTimeLeave);

			return true;
		}
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_GameStart));
			if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

			srand(time(NULL));

			//��Ϣ����
			CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

			m_pIAndroidUserItem->SetGameTimer(IDI_USER_POINT,2);

			return true;
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
		    m_pIAndroidUserItem->KillGameTimer(IDI_USER_POINT);
			ASSERT(wDataSize == sizeof(CMD_S_GameEnd));
			if (wDataSize != sizeof(CMD_S_GameEnd))	return true;
			CMD_S_GameEnd *pEnd = (CMD_S_GameEnd *)pData;
			//m_pIAndroidUserItem->SetGameTimer(IDI_GAME_END,pEnd->cbTimeEnd);
			return true;
		}
	default:
	{
		return true;
	}
	}

	//�������
	ASSERT(FALSE);
	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case  GAME_STATUS_FREE:			//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			return true;
		}
	case GAME_STATUS_PLAY:		//��Ϸ״̬
	case GAME_SCENE_END:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			return true;
		}
	}

	return true;
}
//�û�����
VOID  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
VOID  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
VOID  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
VOID  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}
//��Ϣ����----------------------------------------------------

//////////////////////////////////////////////////////////////////////////



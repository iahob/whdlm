#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////
//����ʱ��
#define TIME_LESS					2									//����ʱ��
//��Ϸʱ��
#define IDI_ANDROID_ITEM_SINK       100
#define IDI_START_GAME			(IDI_ANDROID_ITEM_SINK+0)			//��ʼʱ��
#define IDI_USER_ADD_SCORE		(IDI_ANDROID_ITEM_SINK+2)			//��עʱ��
#define IDI_HUIQI				(IDI_ANDROID_ITEM_SINK+1)			//����ʱ��
#define	IDI_QIUHE				(IDI_ANDROID_ITEM_SINK+3)			//���
//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	XPos	=	100;
	YPos	=	100;
	initGame();
	//�ӿڱ���
	m_pIAndroidUserItem=NULL;

	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//���ýӿ�
bool  CAndroidUserItemSink::RepositionSink()
{
	initGame();
	return true;
}

//ʱ����Ϣ
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//��ʼ��ʱ��
		{
			//����׼��
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
			m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_USER_ADD_SCORE:	//��ע��ʱ
		{
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
			
			SetChess();
			return true;
		}
	case IDI_HUIQI:				//����Ӧ��
		{
			//BYTE aa =(BYTE)(2.0 * rand()/(RAND_MAX + 1.0));
			//������Ϣ
			CMD_C_RegretAnswer ReqretAnswer;
			ReqretAnswer.cbApprove = 0;
			m_pIAndroidUserItem->SendSocketData(SUB_C_REGRET_ANSWER,&ReqretAnswer,sizeof(ReqretAnswer));
			return true;
		}
	case IDI_QIUHE:				//���Ӧ��
		{
			//BYTE aa =(BYTE)(2.0 * rand()/(RAND_MAX + 1.0));
			//������Ϣ
			CMD_C_PeaceAnswer PeaceAnswer;
			PeaceAnswer.cbApprove=1;
			m_pIAndroidUserItem->SendSocketData(SUB_C_PEACE_ANSWER,&PeaceAnswer,sizeof(PeaceAnswer));
			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{		
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}		
	case SUB_S_PLACE_CHESS:		//��������
		{
			return OnSubPlaceChess(pData,wDataSize);
		}
	case SUB_S_REGRET_REQ:		//��������
		{
			return OnSubRegretReq(pData,wDataSize);
		}
	case SUB_S_REGRET_FAILE:	//����ʧ��
		{
			return OnSubRegretFaile(pData,wDataSize);
		}
	case SUB_S_REGRET_RESULT:	//������
		{
			return OnSubRegretResult(pData,wDataSize);
		}
	case SUB_S_PEACE_REQ:		//��������
		{
			return OnSubPeaceReq(pData,wDataSize);
		}
	case SUB_S_PEACE_ANSWER:	//����Ӧ��
		{
			return OnSubPeaceAnser(pData,wDataSize);
		}
	case SUB_S_BLACK_TRADE:		//�����Լ�
		{
			return OnSubBlackTrade(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_CHESS_MANUAL:	//��Ϸ����
		{
			return OnSubChessManual(pData,wDataSize);
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//��ʼʱ��
			UINT nElapse=rand()%(5)+TIME_LESS;
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

			return true;
		}
	case GAME_STATUS_PLAY:	//��Ϸ״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//��Ϣ����
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//���ñ���
			m_wBlackUser=pStatusPlay->wBlackUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bRestrict=pStatusPlay->cbRestrict?true:false;
			m_bTradeUser=pStatusPlay->cbTradeUser?true:false;
			m_bDoubleChess=pStatusPlay->cbDoubleChess?true:false;

			return true;
		}
		default:
			return true;
	}

	ASSERT(FALSE);

	return false;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
void  CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}
//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//��Ϸ����
	m_wBlackUser=pGameStart->wBlackUser;
	m_wCurrentUser=pGameStart->wBlackUser;
	m_bRestrict=pGameStart->cbRestrict?true:false;
	m_bTradeUser=pGameStart->cbTradeUser?true:false;
	m_bDoubleChess=pGameStart->cbDoubleChess?true:false;

	//���ñ���
	m_GameLogic.InitGameLogic();

	//��ʼʱ��
	//���������ʱ������                
	srand((unsigned int)time(NULL));

	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		XPos		=	7;
		YPos		=	8;
		board[XPos][YPos]	=	1;
		UINT nElapse=1 + (UINT)(1.0 * rand()/(RAND_MAX + 1.0));
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
	}

	return true;
}
//�û�����
bool CAndroidUserItemSink::OnSubPlaceChess(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceChess));
	if (wDataSize!=sizeof(CMD_S_PlaceChess)) return false;

	//��Ϣ����
	CMD_S_PlaceChess * pPlaceChess=(CMD_S_PlaceChess *)pBuffer;

	//��Ϸ����
	m_wCurrentUser	=	pPlaceChess->wCurrentUser;
	BYTE	xx		=	pPlaceChess->cbXPos;
	BYTE	yy		=	pPlaceChess->cbYPos;

	board[xx][yy]	=	0;		//�趨Ϊ���������
	for(int i=0; i<572; i++)
	{
		if(ptab[xx][yy][i])
		{
			win[0][i]++;
			ctab[xx][yy][i] = false;
			win[1][i] = 7;
		}
	}
	
	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		
		int  num = 0;
		for (int i=0; i<15; i++)
		{
			
			for (int j=0; j<15; j++)
			{
				
				if (board[i][j] != 2)
				{
					
					num++;
				}
				
			}
			//MessageBox(NULL,"111","111",NULL);
		}
		
			//int a = GetCount();
		//CString SS;
		//SS.Format(TEXT("%d"),num);
		//MessageBox(NULL,SS,"222",NULL);
		if (num >= 100)
		{
			//MessageBox(NULL,"222","222",NULL);
			m_pIAndroidUserItem->SendSocketData(SUB_C_PEACE_REQ);
			return true;
		}
		ComTurn();
	}

	return true;
}
//��������
bool CAndroidUserItemSink::OnSubRegretReq(const void * pBuffer, WORD wDataSize)
{
	UINT nElapse=1 + (UINT)(2.0 * rand()/(RAND_MAX + 1.0));
	m_pIAndroidUserItem->SetGameTimer(IDI_HUIQI,nElapse);

	return true;
}
//����ʧ��
bool CAndroidUserItemSink::OnSubRegretFaile(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_RegretFaile));
	if (wDataSize!=sizeof(CMD_S_RegretFaile)) return false;

	//��Ϣ����
	CMD_S_RegretFaile * pRegretFaile=(CMD_S_RegretFaile *)pBuffer;

	//��������
	if (pRegretFaile->cbFaileReason==FR_COUNT_LIMIT)
	{
		return true;
	}

	//�Լҷ���
	if (pRegretFaile->cbFaileReason==FR_PLAYER_OPPOSE)
	{
		return true;
	}
	return true;
}
//������
bool CAndroidUserItemSink::OnSubRegretResult(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_RegretResult));
	if (wDataSize!=sizeof(CMD_S_RegretResult)) return false;

	//��������
	CMD_S_RegretResult * pRegretResult=(CMD_S_RegretResult *)pBuffer;

	//�޸�����
	m_GameLogic.RegretChess(pRegretResult->wRegretCount);

	return true;
}
//��������
bool CAndroidUserItemSink::OnSubPeaceReq(const void * pBuffer, WORD wDataSize)
{
	UINT nElapse=1 + (UINT)(2.0 * rand()/(RAND_MAX + 1.0));
	m_pIAndroidUserItem->SetGameTimer(IDI_QIUHE,nElapse);
	return true;
}
//����Ӧ��
bool CAndroidUserItemSink::OnSubPeaceAnser(const void * pBuffer, WORD wDataSize)
{
	return true;
}
//�����Լ�
bool CAndroidUserItemSink::OnSubBlackTrade(const void * pBuffer, WORD wDataSize)
{
	//�Ի��û�
	m_wCurrentUser=m_wBlackUser;
	m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

	return true;
}
//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;

	initGame();
	XPos	=	100;
	YPos	=	100;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);

	//���ö�ʱ��
	//��ʼʱ��
	UINT nElapse=rand()%(5)+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}
//��Ϸ����
bool CAndroidUserItemSink::OnSubChessManual(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagChessManual)==0);
	if (wDataSize%sizeof(tagChessManual)!=0) return false;

	//��������
	WORD wManualCount=wDataSize/sizeof(tagChessManual);
	for (WORD wIndex=0;wIndex<wManualCount;wIndex++) 
	{
		tagChessManual * pChessManual=(((tagChessManual *)pBuffer)+wIndex);
		m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor);
	}
	return true;
}
//��������
void CAndroidUserItemSink::SetChess()
{
	//������Ϣ
	CMD_C_PlaceChess PlaceChess;
	PlaceChess.cbXPos	=	XPos;
	PlaceChess.cbYPos	=	YPos;

	m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_CHESS,&PlaceChess,sizeof(PlaceChess));
}
//���÷����ʱ��
void CAndroidUserItemSink::SetPlaceTimer()
{
	UINT nElapse	=	1 + (UINT)(3.0 * rand()/(RAND_MAX + 1.0));
	m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
}

//��ʼ����
void CAndroidUserItemSink::initGame()
{
	int i,j,k;
	int count=0;
 
	//�趨����������ڸ�����ʤ����е�������
	for(i=0; i<572; i++)
	{
		win[0][i] = 0;
		win[1][i] = 0;
	}
 
	//��ʼ������״̬
	for(i=0; i<15; i++) 
	{
		for(j=0; j<15; j++)
		{
			board[i][j] = 2;		//�趨Ϊ�ո�
		}
	}
	//�趨ˮƽ����Ļ�ʤ���
	for(i=0; i<15; i++)    
	{
		for(j=0; j<11; j++)
		{
			for(k=0; k<5; k++)
			{
				ptab[i][j+k][count] = true;
				ctab[i][j+k][count] = true;
			}
			count++;
		}
	}
   
   //�趨��ֱ����Ļ�ʤ���
	for(i=0; i<15; i++)  
	{
		for(j=0; j<11; j++)
		{
			for(k=0; k<5; k++)
			{
				ptab[j+k][i][count] = true;
				ctab[j+k][i][count] = true;
			}
			count++;
		}
	}
    //�趨���Խ��߷���Ļ�ʤ���
    for(i=0; i<11; i++) 
	{
		for(j=0; j<11; j++)
		{
			for(k=0; k<5; k++)
			{
				ptab[j+k][i+k][count] = true;
				ctab[j+k][i+k][count] = true;
			}
			count++;
		}
	}
     //�趨���Խ����ϵĻ�ʤ���
	for(i=0; i<11; i++)   
	{
		for(j=14; j>=4; j--)
		{
			for(k=0; k<5; k++)
			{
				ptab[j-k][i+k][count] = true;
				ctab[j-k][i+k][count] = true;
			}
			count++;
		}
	}
}
//����������λ��
void CAndroidUserItemSink::ComTurn()
{
	int grades[2][15][15];
	int m,n,i,max=0;
	int u,v;

	for(m=0; m<15; m++) 
	{
		for(n=0; n<15; n++)
		{
			grades[0][m][n] = 0;
			grades[1][m][n] = 0;

			if(board[m][n] == 2)//�ո�
			{
				for(i=0; i<572; i++)
				{
					//��������ڿ�λ���ϵĻ�ʤ����
					if(ptab[m][n][i] && win[0][i] != 7)
					{
						switch(win[0][i])
						{
						case 0:
							grades[0][m][n] += 1;
							break;
						case 1:
							grades[0][m][n] += 200;
							break;
						case 2:
							grades[0][m][n] += 400;
							break;
						case 3:
							grades[0][m][n] += 2000;
							break;
						case 4:
							grades[0][m][n] += 10000;
							break;
						}
					}				

					//���������ڿ�λ���ϵĻ�ʤ����
					if(ctab[m][n][i] && win[1][i] != 7)
					{
						switch(win[1][i])
						{
						case 0:
							grades[1][m][n] += 1;
							break;
						case 1:
							grades[1][m][n] += 220;
							break;
						case 2:
							grades[1][m][n] += 420;
							break;
						case 3:
							grades[1][m][n] += 2100;
							break;
						case 4:
							grades[1][m][n] += 20000;
							break;
						}
					}
				}
			}

			if(max == 0)
			{
				u = m;
				v = n;
			}

			if(grades[0][m][n] > max)
			{
				max = grades[0][m][n];
				u = m;
				v = n;
			}
			else if(grades[0][m][n] == max)
			{
				if(grades[1][m][n] > grades[1][u][v])
				{
					u = m;
					v = n;
				}
			}

			if(grades[1][m][n] > max)
			{
				max = grades[1][m][n];
				u = m;
				v = n;
			}
			else if(grades[1][m][n] == max)
			{
				if(grades[0][m][n] > grades[0][u][v])
				{
					u = m;
					v = n;
				}
			}
		}
	}

	board[u][v] = 1;      //�趨Ϊ�������������

	//ȥ���Է���ʤ�����
	for(i=0; i<572; i++)
	{
		if(ctab[u][v][i])
		{
			win[1][i]++;
			ptab[u][v][i] = false;
			win[0][i] = 7;
		}
	}

	//������Ϣ
	XPos	=	u;
	YPos	=	v;
	SetPlaceTimer();
}
//�������ж�������
int  CAndroidUserItemSink::GetCount()
{
	int  num = 0;
	for (int i=0; i<15; i++)
	{
		for (int j=0; i<15; j++)
		{
			if (board[i][j] != 2)
			{
				num++;
			}
		}
	}
	return num;
}
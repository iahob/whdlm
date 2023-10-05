#ifndef CMD_THIRTEEN_HEAD_FILE
#define CMD_THIRTEEN_HEAD_FILE

// 1�ֽڶ���
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��
#define KIND_ID							51									//��Ϸ I D
#define GAME_PLAYER						4									//��Ϸ����
#define GAME_NAME						TEXT("�㽭ʮ��ˮ")					//��Ϸ����
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//��Ϸ����

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//����汾

//��������
#define ME_VIEW_CHAIR					2									//�Լ���ͼ
#define HAND_CARD_COUNT					13									//�˿���Ŀ
#define DISPATCH_COUNT					52									//�ɷ���Ŀ

#define SET_CHIP_COUNT					4									//��ע����

#define MAX_RECORD_COUNT				32									//��������ͳ��������
//��Ϸ״̬
#define GS_WK_FREE				    	GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_WK_CALL_BANKER				GAME_STATUS_PLAY					//��ׯ״̬
#define GS_WK_SETCHIP			    	GAME_STATUS_PLAY+1					//������ע
#define GS_WK_PLAYING			    	GAME_STATUS_PLAY+2					//��Ϸ����

#define TIME_DISPATCH_CARD				5									//��������ƶ���ʱ��
//�ֶ�����
enum enSegmentType
{
	enSegFront,																//ǰ������
	enSegMid,																//�ж�����
	enSegBack,																//�������
	enSegErr																//��������
};

//�ֶ���Ϣ
struct tagSegInfo
{
	bool bHaveSeg[3];		//�Ƿ�ֶ�
	BYTE bSegData[3][5];	//�ֶ�����
	tagSegInfo() 
	{
		Init();
	}

	void Init()
	{ 
		ZeroMemory(bHaveSeg, sizeof(bHaveSeg)); 
		ZeroMemory(bSegData, sizeof(bSegData)); 
	}

	void SetSegData(BYTE bSegNum, BYTE bSegCard[], BYTE bSegCount)
	{
		if (bSegNum < 0 || bSegNum > 3 || (bSegCount!=3&&bSegCount!=5))		return;
		if (bSegCard == NULL)
		{
			bHaveSeg[bSegNum] = false;
			ZeroMemory(bSegData[bSegNum], sizeof(bSegData[bSegNum]));
			return;
		}
		ZeroMemory(bSegData[bSegNum], sizeof(bSegData[bSegNum]));
		bHaveSeg[bSegNum] = true;
		CopyMemory(bSegData[bSegNum], bSegCard, sizeof(BYTE)*bSegCount); 
	}

	BYTE GetSegData(BYTE bSegNum, BYTE bSegCard[], BYTE bSegCount)
	{
		if (bSegNum < 0 || bSegNum > 3 || (bSegCount!=3&&bSegCount!=5) || bSegCard == NULL)		return 0;
		if (!bHaveSeg[bSegNum])		return 0;
		CopyMemory(bSegCard, bSegData[bSegNum], sizeof(BYTE)*bSegCount);
		//ʵ�ʷ���
		BYTE bRealCount = bSegCount;
		for (int i = 0; i < bSegCount; i++)
		{
			if (bSegData[i] == 0)
				bRealCount--;
		}
		return bRealCount;
	}

	BYTE GetCardCount(BYTE bSegNum = 255)
	{
		BYTE bCCount = 0;
		for (int i = 0; i < 3; i++)
		{
			if (bSegNum == i || bSegNum == 255)
			{
				for (int j = 0; j < 5; j++)
				{
					if (bSegData[i][j] != 0)
						bCCount++;
				}
			}
		}
		return bCCount;
	}
};

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#define myatoi      _wtoi
#define myatoi64	_wtoi64
#endif

//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_SETSEGMENT				202									//���÷ֶ�
#define SUB_S_SHOW_CARD					203									//���̯��
#define SUB_S_GAME_END					204									//��Ϸ����
#define SUB_S_COMPARE_CARD				205									//�Ƚ��˿�
#define SUB_S_GAME_START				206									//��Ϸ��ʼ
#define SUB_S_PLAYER_EXIT				210									//�û�ǿ��
#define SUB_S_DISPATCH_START			211									//��ʼ����
#define SUB_S_DISPATCH_FINISH			212									//���ƽ���
#define SUB_S_TRUSTEE					214									//�й���Ϣ
#define SUB_S_SEND_CARD_EX				215									//ȫ������
#define SUB_S_VOICE_PHRASE				216									//��������
#define SUB_S_ROBOT_DEBUG             218									//AI����
#define SUB_S_MOBILE_PUTCARD			219									//�ֻ��й���
#define SUB_S_RECORD					220									//��Ϸ��¼
#define SUB_S_ANDROID_INFO				221									//AI����
//////////////////////////////////////////////////////////////////////////
//��Ϸ��¼
struct CMD_S_Record
{
	int		nCount;		//��Ϸ����
	SCORE	lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
	SCORE	lAllScore[GAME_PLAYER];							//�ܷ�
	BYTE	cbWinCount[GAME_PLAYER];						//Ӯ��
	BYTE	cbLoseCount[GAME_PLAYER];						//���
	BYTE	cbEqualCount[GAME_PLAYER];						//ƽ��
};

//¼������
struct Video_SendCard
{
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�		
	WORD							wTableID;							//����ID

	WORD							wCurrentUser;						//��ǰ���
	WORD							wBanker;							//ׯ�����
	BYTE							bCardData[HAND_CARD_COUNT];			//�����˿�
	BYTE							cbPlayCount;						//��Ϸ����
	bool							bGameStatus[GAME_PLAYER];			//��Ϸ״̬
	LONGLONG						lChipArray[GAME_PLAYER];			//�����ע

	SCORE							lScore;								//����
	LONG							lCellScore;							//�׷�
	BYTE							cbMaCard;							//����
};

//�����˿�
struct CMD_S_SendCard
{
	WORD								wCurrentUser;						//��ǰ���
	WORD								wBanker;							//ׯ�����

	BYTE								bCardData[HAND_CARD_COUNT];			//�����˿�
	BYTE								cbPlayCount;						//��Ϸ����
	bool								bGameStatus[GAME_PLAYER];			//��Ϸ״̬
	LONGLONG							lChipArray[GAME_PLAYER];			//�����ע
};

//���÷ֶ�
struct CMD_S_SetSegment
{
	WORD								wCurrentUser;						//��ǰ���
	enSegmentType						SegmentType;						//�ֶ�����
};

//���̯��
struct CMD_S_ShowCard
{
	WORD								wCurrentUser;						//��ǰ���
	bool								bCanSeeShowCard;					//�ܷ���
	BYTE								bFrontCard[3];						//ǰ���˿�
	BYTE								bMidCard[5];						//�ж��˿�
	BYTE								bBackCard[5];						//����˿�
};

//��ʼ����
struct CMD_S_CompareCard
{
	WORD								wBankUser;							//ׯ�����
	BYTE								bSegmentCard[GAME_PLAYER][3][5];	//�ֶ��˿�
	SCORE								lScoreTimes[GAME_PLAYER][GAME_PLAYER][3];//�ֶα���
	BYTE								bSpecialType[GAME_PLAYER];			//��������
	BYTE								cbPlayCount;						//��Ϸ����
	int									bAllWin[GAME_PLAYER][GAME_PLAYER];	//��ǹ��Ϣ
	WORD								wAllKillUser;						//ȫ�ݴ����
	SCORE								lScoreTimesPlus[GAME_PLAYER][4];	//�ֶ��ܷ�
};

//�û��й�
struct CMD_S_Trustee
{
	WORD								wChairID;							//�й��û�
	bool								bTrustee;							//�Ƿ��й�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	SCORE								lGameTax;									//��Ϸ˰��
	SCORE								lGameScore[GAME_PLAYER];					//��Ϸ����
	SCORE								lScoreTimes[GAME_PLAYER];					//��Ӯ����
	SCORE								lCompareResult[GAME_PLAYER][3];				//���ƽ��
	BYTE								bEndMode;									//������ʽ
	bool								bDragonInfo[GAME_PLAYER];					//������Ϣ
	BYTE								bWinInfo[GAME_PLAYER];						//ʤ����Ϣ(0�� 1ƽ 2ʤ)
	BYTE								cbHandCard[GAME_PLAYER][HAND_CARD_COUNT];	//�������
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD						      	wPlayerID;							//�˳��û�
	SCORE								lScore[GAME_PLAYER];				//����
};
//�ֻ��й���
struct CMD_S_MobilePutCard
{
	BYTE								cbMobilePutCount;					//�й���Ŀ
	BYTE								cbMobilePutCard[3][13];				//�ֻ��й���
	BYTE								cbMobilePutCardType[3][3];			//�й�����
	BYTE								cbSpecialType;						//��������
	BYTE								cbSpecialData[3][5];				//������
};
//����״̬
struct CMD_S_StatusFree
{	
	//��Ϸ����
	LONG								lBaseScore;							//��������
	bool								bGameStatus[GAME_PLAYER];			//��Ϸ״̬
	bool								bHaveBanker;						//����ׯģʽ
	INT 								nAllWinTimes;						//��ǹ���ⷬ��
	BYTE								cbPlayCount;						//�������
	//��ʷ�ɼ�
	SCORE								lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE								lCollectScore[GAME_PLAYER];			//������Ϣ

	//ʱ�䶨��
	BYTE								cbTimeStartGame;					//��ʼʱ��
	BYTE								cbTimeCallBanker;					//��ׯʱ��
	BYTE								cbTimeSetChip;						//��עʱ��
	BYTE								cbTimeRangeCard;					//����ʱ��
	BYTE								cbTimeShowCard;						//����ʱ��

	WORD								wServerID;							//�����ʶ
	BYTE								cbMaCard;							//����
	//���� 0 ���ֳ�	1 ��ҳ�  
	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	//ͬ���Ƚ�0���ȱȵ�����1�ȱȻ�ɫ
	BYTE								cbTongHuaMode;
	//���ٱ���0����ͨ��1����
	BYTE								cbQuickMode;
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	LONG								lBaseScore;							//��������
	SCORE								lChip[GAME_PLAYER];					//��ע��С
	BYTE								bHandCardData[HAND_CARD_COUNT];		//�˿�����
	BYTE								bSegmentCard[GAME_PLAYER][3][5];	//�ֶ��˿�
	bool								bFinishSegment[GAME_PLAYER];		//��ɷֶ�
	WORD								wBanker;							//ׯ�����
	bool								bGameStatus[GAME_PLAYER];			//��Ϸ״̬
	SCORE								lScoreTimes[GAME_PLAYER][GAME_PLAYER][3];//�ֶα���
	SCORE								lScoreTimesPlus[GAME_PLAYER][4];	//�ֶ��ܷ�
	BYTE								bSpecialType[GAME_PLAYER];			//��������
	BYTE								cbPlayCount;						//��Ϸ����
	bool								bHaveBanker;						//����ׯģʽ
	INT 								nAllWinTimes;						//��ǹ���ⷬ��

	//��ʷ�ɼ�
	SCORE								lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE								lCollectScore[GAME_PLAYER];			//������Ϣ

	//ʱ�䶨��
	BYTE								cbTimeStartGame;					//��ʼʱ��
	BYTE								cbTimeCallBanker;					//��ׯʱ��
	BYTE								cbTimeSetChip;						//��עʱ��
	BYTE								cbTimeRangeCard;					//����ʱ��
	BYTE								cbTimeShowCard;						//����ʱ��

	WORD								wServerID;							//�����ʶ
	BYTE								cbMaCard;							//����

	BYTE								cbTimeLeave;						//ʣ��ʱ��

	int									bAllWin[GAME_PLAYER][GAME_PLAYER];	//��ǹ��Ϣ
	WORD								wAllKillUser;						//ȫ�ݴ����
	//���� 0 ���ֳ�	1 ��ҳ�  
	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	//ͬ���Ƚ�0���ȱȵ�����1�ȱȻ�ɫ
	BYTE								cbTongHuaMode;
	//���ٱ���0����ͨ��1����
	BYTE								cbQuickMode;
};

//�����˿�
struct CMD_S_DebugCard
{
	WORD							wCardUser[GAME_PLAYER ];					//�������
	BYTE							cbUserCount;								//��������
	BYTE							cbCardData[GAME_PLAYER ][HAND_CARD_COUNT];	//�˿��б�
	BYTE							cbCardCount[GAME_PLAYER ];					//�˿�����
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_BANKER				300									//��ҽ�ׯ
#define SUB_C_NO_CALL_BANKER			301									//��Ҳ���
#define SUB_C_SETCHIP					302									//ѹע����
#define SUB_C_SEGCARD					303									//�ֶ�����
#define SUB_C_SHOWARD					304									//���̯��
#define SUB_C_COMPLETE_COMPARE			306									//��ɱȽ�
#define SUB_C_SHUFFLE_FINISH			307									//ϴ�ƽ���
#define SUB_C_SPLIT_CARD				308									//�������
#define SUB_C_SPLIT_FINISH				309									//���ƽ���
#define SUB_C_TRUSTEE					310									//�й���Ϣ
//���÷ֶ�
struct CMD_C_SegCard
{
	enSegmentType						SegmentType;						//�ֶ�����
};

//�ֶ���Ϣ
struct CMD_C_ShowCard
{
	BYTE								bFrontCard[3];						//ǰ���˿�
	BYTE								bMidCard[5];						//�ж��˿�
	BYTE								bBackCard[5];						//����˿�
	BYTE								cbSpecialCardType;					//��������
};

//�����й�
struct CMD_C_Trustee
{
	bool								bTrustee;							//�Ƿ��й�
};

//////////////////////////////////////////////////////////////////////////
///������Ϣ
//Ԥ�������˴��ȡ��
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet;
	SCORE								lRobotBankGetBanker;
	SCORE								lRobotBankStoMul;
};

//��������
typedef enum{ CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL }DEBUG_TYPE;

//���Խ��      ���Գɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{ DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID }DEBUG_RESULT;

//�û���Ϊ
typedef enum{ USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT }USERACTION;

#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�µ��Է����
#define SUB_S_ROOMCARD_RECORD			114									//������¼

#define SUB_S_REQUEST_QUERY_RESULT		115									//��ѯ�û����
#define SUB_S_USER_DEBUG				116									//�û�����
#define SUB_S_USER_DEBUG_COMPLETE		117									//�û��������
#define SUB_S_OPERATION_RECORD		    118									//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 119


typedef struct
{
	DEBUG_TYPE						debug_type;							//��������
	BYTE								cbDebugCount;					  //���Ծ���
	bool							    bCancelDebug;					  //ȡ����ʶ
}USERDEBUG;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//AI��ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERDEBUG							userDebug;						//�û�����
}ROOMUSERDEBUG;

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//�û���Ϣ
	bool								bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_RESULT						debugResult;						//���Խ��
	DEBUG_TYPE						debugType;						//��������
	BYTE								cbDebugCount;						//���Ծ���
};

//�û�����
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_TYPE						debugType;						//��������
	BYTE								cbRemainDebugCount;				//ʣ����Ծ���
};

//���Է���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;						//������ʼ���
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR								szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//��¼���²�����20����¼
};

//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				//��ǰ��ѯ�û���Ϣ
	bool								bExistDebug;						//��ѯ�û����ڵ��Ա�ʶ
	USERDEBUG							currentuserdebug;
};
#define SUB_C_STORAGE					6									//���¿��
#define	SUB_C_STORAGEMAXMUL				7									//��������
#define SUB_C_REQUEST_RCRecord			12									//��ѯ������¼

#define SUB_C_REQUEST_QUERY_USER		13									//�����ѯ�û�
#define SUB_C_USER_DEBUG				14									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMINFO	15									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	16


struct CMD_C_UpdateStorage
{
	LONGLONG							lRoomStorageCurrent;				//�����ֵ
	LONGLONG							lRoomStorageDeduct;					//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG							lMaxRoomStorage[2];					//�������
	WORD								wRoomStorageMul[2];					//Ӯ�ָ���
};


struct CMD_C_RequestQuery_User
{
	DWORD								dwGameID;							//��ѯ�û�GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERDEBUG							userDebugInfo;					//
};
// ��ԭ������
#pragma pack()
//////////////////////////////////////////////////////////////////////////
#endif

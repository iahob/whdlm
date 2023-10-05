#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE


#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ�б�

//��Ϸ����
struct tagGameType
{
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//��Ϸ����
struct tagGameKind
{
	WORD							wTypeID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wGameID;							//ģ������
	WORD							wRecommend;							//�Ƽ���Ϸ
	WORD							wGameFlag;							//��Ϸ��־
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
	DWORD							dwSetCount;							//��������
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	TCHAR							szProcessName[LEN_PROCESS];			//��������
};

//Lua��Ϸ����
struct tagLuaGameKind
{
	WORD							wKindID;							//��ϷID
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	WORD							wTypeID;							//�ҽ�����
	WORD							wSortID;							//��������
	TCHAR							szModuleName[LEN_PROCESS];			//��������
	DWORD							dwClientVersion;					//�ͻ��˰汾��
	DWORD							dwResVersion;						//�Ƽ���Ϸ
	BYTE							cbNullity;							//���ñ�־ 0 ������ 1 ����
};

//H5��Ϸ����
struct tagH5GameKind
{
	WORD							wKindID;							//��ϷID
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	WORD							wTypeID;							//�ҽ�����
	WORD							wSortID;							//��������
	TCHAR							szModuleName[LEN_PROCESS];			//��������
	DWORD							dwClientVersion;					//�ͻ��˰汾��
	DWORD							dwResVersion;						//�Ƽ���Ϸ
	BYTE							cbNullity;							//���ñ�־ 0 ������ 1 ����
};

//U3D��Ϸ����
struct tagU3DGameKind
{
	WORD							wKindID;							//��ϷID
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	WORD							wTypeID;							//�ҽ�����
	WORD							wSortID;							//��������
	TCHAR							szModuleName[LEN_PROCESS];			//��������
	DWORD							dwClientVersion;					//�ͻ��˰汾��
	DWORD							dwResVersion;						//�Ƽ���Ϸ
	BYTE							cbNullity;							//���ñ�־ 0 ������ 1 ����
};

//��Ϸ�ڵ�
struct tagGameNode
{
	WORD							wKindID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//��������
struct tagGamePage
{
	WORD							wPageID;							//ҳ������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wOperateType;						//��������
	TCHAR							szDisplayName[LEN_PAGE];			//��ʾ����
};

//��Ϸ����
struct tagGameServer
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wServerID;							//��������
	WORD                            wServerKind;                        //��������
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	WORD							wServerPort;						//����˿�
	BYTE							cbPlayMode;							//0����ģʽ1���ģʽ
	SCORE							lCellScore;							//��Ԫ����
	BYTE							cbEnterMember;						//�����Ա
	SCORE							lEnterScore;						//�������
	DWORD							dwServerRule;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//AI��
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szServerAddr[32];					//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
	//˽�˷����
	DWORD							dwSurportType;						//֧������
	WORD							wTableCount;						//������Ŀ

	//��������
	DWORD							dwSetPlayerCount;					//��������
	TCHAR							szGameInfomation[LEN_SERVER];		//��Ϸ˵��

	//���������
	bool							bGroupPersonalMaxChair;				//�����˰�����Ϸ
};

//�����б�
struct tagAgentGameKind
{
	WORD							wKindID;							//��������
	WORD							wSortID;							//��������
};

//��������
struct tagMatchRounds
{
	BYTE							cbPlayCount;						//��Ϸ����
	WORD							wPromotion;							//��������
//	BOOL							bResetFlag;							//��λor�ۼ�
};

//��ʱ��
struct tagLockTimeMatch
{
	//ʱ������
	SYSTEMTIME						MatchStartTime;						//��ʼ����
	SYSTEMTIME						SignupStartTime;					//������ʼ
	SYSTEMTIME						SignupEndTime;						//������ֹ	

	//��������
	BYTE							cbMatchMode;						//����ģʽ
	WORD							wMaxSignupUser;						//��������
	WORD							wStartUserCount;					//��������	
	
	//������Ϣ
	LONGLONG						lInitalBase;						//��ʼ�׷�

	//��������
	LONGLONG						lMatchInitScore;					//��ʼ����
	tagMatchRounds					hMatchRounds[MAX_MATCH_ROUNDS];		//��������

	//������Ϣ
	BYTE							cbTotalRounds;						//������
	BYTE							cbTotalPlayCount;					//�ܾ���
};

//DWORD							wEndDelayTime;						//�ӳ�ʱ��
//SYSTEMTIME					MatchEndTime;						//��������
//LONGLONG						lMatchCullScore;					//��̭����
//��������
//WORD							wMinPlayCount;						//���پ���
//WORD							wMaxPlayCount;						//������
//��������
//BYTE							cbReviveEnabled;					//���ñ�ʶ
//BYTE							cbSafeCardEnabled;					//���ñ�ʶ
//BYTE							cbReviveTimes;						//�������
//LONGLONG						lReviveFee;							//�������
//LONGLONG						lSafeCardFee;						//���շ���

//��ʱ��
struct tagImmediateMatch
{
	//������Ϣ
	WORD							wStartUserCount;					//��������	
	WORD							wAndroidUserCount;					//��������
	WORD							wAndroidDelaySignupTime;			//�ӳٱ���

	//������Ϣ
	LONGLONG						lInitalBase;						//��ʼ�׷�

	LONGLONG						lInitalScore;						//��ʼ����
	tagMatchRounds					hMatchRounds[MAX_MATCH_ROUNDS];		//��������

	//������Ϣ
	BYTE							cbTotalRounds;						//������
	BYTE							cbTotalPlayCount;					//�ܾ���

	
	//AI��������
	BYTE							cbInitAndroidMin;					//��ʼAI��Сֵ		
	BYTE							cbInitAndroidMax;					//��ʼAI���ֵ
	BOOL							bAndroidDebug;						//AI����
	BYTE							cbNo1Ratio1;						//�߳���һ���ٷֱ�
	BYTE							cbNo1Ratio2;						//�߳���һ���ٷֱ�
	BYTE							cbNo1Ratio3;						//�߳���һ���ٷֱ�
	
	//AI����
	WORD							wAndroidFullTime;					//AI����ʱ��
	BYTE							cbTotalSection;						//�ܽ׶���
	BYTE							cbSectionCount[MAX_MATCH_SECTION];	//ÿ���׶�AI��
};	

//������Ϣ
struct tagMatchRewardInfo
{
	WORD							wRankID;							//��������
	SCORE							lRewardGold;						//�������
	SCORE							lRewardIngot;						//����Ԫ��	
	DWORD							dwRewardTicket;						//������ȯ
};

//������Ϣ
struct tagGameMatch
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ	
	BYTE							cbMatchType;						//��������
	TCHAR							szMatchName[32];					//��������

	//������Ϣ
	BYTE							cbFeeType;							//��������
	BYTE							cbDeductArea;						//�ɷ�����
	LONGLONG						lSignupFee;							//��������	
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	LONG							lExperience;						//��Ҿ���

	//������ʽ
	BYTE							cbRankingMode;						//������ʽ	
	WORD							wCountInnings;						//ͳ�ƾ���
	BYTE							cbFilterGradesMode;					//ɸѡ��ʽ
	
	//������Ϣ
	WORD							wRewardCount;						//��������
	tagMatchRewardInfo				MatchRewardInfo[3];					//��������

	//��������
	BYTE							cbMatchRule[512];					//��������
};

//��������
struct tagMatchSignUpCount
{
	LONGLONG						lMatchNo;							//�������				
	DWORD							dwMatchCount;						//��������
	WORD							wServerID;							//�����ʶ
};

//������Ϣ
struct tagChatServer
{
	WORD							wChatID;							//�㳡��ʶ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//���ֲ���Ϣ
struct tagClubServer
{
	WORD							wClubID;							//�㳡��ʶ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��Ƶ����
struct tagAVServerOption
{
	WORD							wAVServerPort;						//��Ƶ�˿�
	DWORD							dwAVServerAddr;						//��Ƶ��ַ
};

//������Ϣ
struct tagOnLineInfoKind
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwOnLineCount;						//��������
};

//����ͳ��
struct tagOnLineInfoKindEx
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//AI��
	DWORD							dwSetCount;							//��������
};

//������Ϣ
struct tagOnLineInfoServer
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwOnLineCount;						//��������
};

//��������
struct tagLotteryItem
{
	BYTE							cbItemIndex;						//��������
	BYTE							cbItemType;							//��������
	SCORE							lItemQuota;							//�������
};
//////////////////////////////////////////////////////////////////////////////////
//��ʱ����(GameAIParameter.h���ж���)
#define AI_CARRY_GRADE				4											//Я���ȼ�

//��������
struct tagAndroidParameter
{
	//���Ա���	
	DWORD							dwAndroidCount;								//������Ŀ
	DWORD							dwSwitchMinInnings;							//��������(Ĭ�ϲ�����)
	DWORD							dwSwitchMaxInnings;							//��������(Ĭ�ϲ�����)
	DWORD							dwServiceMode;								//����ģʽ(Ĭ�ϲ�����)
	DWORD							dwAIHangingMinRate;							//�һ�����
	DWORD							dwAIHangingMaxRate;							//�һ�����
	DWORD							dwAICarryScoreMinMultiple[AI_CARRY_GRADE];	//Я������
	DWORD							dwAICarryScoreMaxMultiple[AI_CARRY_GRADE];	//Я������
	DWORD							dwAIScoreLevelRate[AI_CARRY_GRADE];			//Я��ռ��
	DWORD							dwAIScoreFloatMinRate[AI_CARRY_GRADE];		//����ռ��
	DWORD							dwAIScoreFloatMaxRate[AI_CARRY_GRADE];		//����ռ��
};

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ

//����״̬
struct tagTableStatus
{
	BYTE							cbTableLock;						//������־
	BYTE							cbPlayStatus;						//��Ϸ��־
	LONG							lCellScore;							//��Ԫ����
};

//�û�״̬
struct tagUserStatus
{
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
	BYTE							cbUserStatus;						//�û�״̬
};

//�û�����
struct tagUserAttrib
{
	BYTE							cbCompanion;						//�û���ϵ
};

//�û�����
struct tagUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lIngot;								//�û�Ԫ��
	DOUBLE							dBeans;								//�û���Ϸ��

	//��Ӯ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//ȫ����Ϣ
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
};

//�û�����
struct tagMobileUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	DOUBLE							dBeans;								//�û���Ϸ��

	//��Ӯ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//ȫ����Ϣ
	DWORD							dwExperience;						//�û�����
};

//���߽ڵ�
struct tagPropertyTypeItem
{
	DWORD							dwTypeID;							//���ͱ�ʶ
	DWORD							dwSortID;							//�����ʶ
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//���߽ڵ�
struct tagPropertyRelatItem
{
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwTypeID;							//���ͱ�ʶ
};

//������Ϣ
struct tagPropertyItem
{
	//������Ϣ
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwPropertyKind;						//��������

	BYTE							cbUseArea;							//ʹ�÷�Χ
	BYTE							cbServiceArea;						//����Χ
	BYTE							cbRecommend;						//�Ƽ���ʶ

	BYTE							cbExchangeType;						//�һ�����
	DWORD							dwExchangeRatio;					//��ʯ�һ����߱���

	//��òƸ�
	SCORE							lSendLoveLiness;					//������������ͷ���
	SCORE							lRecvLoveLiness;					//������������շ���
	SCORE							lUseResultsGold;					//��ý��

	//����Ч��
	DWORD							dwUseResultsValidTime;				//��Чʱ��
	DWORD							dwUseResultsValidTimeScoreMultiple;	//��Ч����

	//������
	DWORD							dwUseResultsGiftPackage;			//������

	DWORD							dwSortID;							 //�����ʶ
	TCHAR							szName[PROPERTY_LEN_NAME];			 //��������
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//ʹ����Ϣ
	BYTE							cbPlatformKind;						 //ƽ̨���0:ȫ�� 1:LUA,2:H5,3:U3D
};

//�ӵ�����Ϣ
struct tagPropertySubItem
{
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwOwnerPropertyID;					//���߱�ʶ
	DWORD                           dwPropertyCount;                    //������Ŀ
	DWORD							dwSortID;							//�����ʶ
};

//�������
struct tagGiftPropertyInfo
{
	WORD                            wPropertyCount;                     //������Ŀ
	WORD							wPropertyID;						//���߱�ʶ
};

//����ʹ��
struct tagUsePropertyInfo
{
	WORD                            wPropertyCount;                     //������Ŀ
	WORD                            dwValidNum;						    //��Ч����
	DWORD                           dwEffectTime;                       //��Чʱ��
};


//�û�����
struct tagUserProperty
{
	WORD                            wPropertyUseMark;                   //���߱�ʾ
	tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//ʹ����Ϣ   
};

//���߰���
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //С������
	WORD                            wTyphonCount;                      //��������
};

//ʱ����Ϣ
struct tagTimeInfo
{
	DWORD						dwEnterTableTimer;						//��������ʱ��
	DWORD						dwLeaveTableTimer;						//�뿪����ʱ��
	DWORD						dwStartGameTimer;						//��ʼ��Ϸʱ��
	DWORD						dwEndGameTimer;							//�뿪��Ϸʱ��
};

//////////////////////////////////////////////////////////////////////////
//������Ϣ

struct tagNearUserInfo
{
	//�û�����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	DWORD							dwFaceID;							//ͷ�� I D
	DWORD							dwCustomID;							//ͷ�� I D
	BYTE							cbGender;							//�û��Ա�
	WORD							wMemberOrder;						//��Ա�ȼ�
	WORD							wGrowLevel;							//�û��ȼ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//����γ��
	DOUBLE							dLongitude;							//���꾭��
	DOUBLE							dLatitude;							//����γ��
	DWORD							dwDistance;							//Ŀ�����	
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//����

	//�û���ַ
	DWORD							dwClientAddr;						//�û���ַ
};

//�û���Ϣ
struct tagFriendUserInfo
{
	//�û�����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	DWORD							dwFaceID;							//ͷ�� I D
	DWORD							dwCustomID;							//ͷ�� I D
	BYTE							cbGender;							//�û��Ա�
	WORD							wMemberOrder;						//��Ա�ȼ�
	WORD							wGrowLevel;							//�û��ȼ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����

	//��������
	TCHAR							szQQ[LEN_QQ];						//�û�QQ
	TCHAR							szEMail[LEN_EMAIL];					//�û�Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
	TCHAR							szPostalCode[LEN_POSTALCODE];		//��������

	//�û�״̬
	BYTE							cbMainStatus;						//�û�״̬
	BYTE							cbGameStatus;						//��Ϸ״̬
	WORD							wKindID;							//��Ϸ��ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//����λ��
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	DWORD							dwClientAddr;						//�û���ַ

	//����γ��
	BYTE							cbCoordinate;						//������ʶ
	DOUBLE							dLongitude;							//���꾭��
	DOUBLE							dLatitude;							//����γ��
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//����
};

//�û�����
struct tagClientGroupInfo
{
	BYTE							cbGroupIndex;						//��������
	TCHAR							szGroupName[GROUP_LEN_NAME];		//��������		
};

//������Ϣ
struct tagServerFriendInfo
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMainStatus;						//�û�״̬
	BYTE							cbGameStatus;						//�û�״̬
	BYTE							cbGroupID;							//����ʶ	
};

//������Ϣ
struct tagClientFriendInfo
{
	//�û�����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	DWORD							dwFaceID;							//ͷ�� I D
	DWORD							dwCustomID;							//ͷ�� I D
	BYTE							cbGender;							//�û��Ա�
	WORD							wMemberOrder;						//�û��ȼ�
	WORD							wGrowLevel;							//�û��ȼ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	BYTE							cbGroupID;							//����ʶ	

	//������Ϣ
	BYTE							cbMainStatus;						//�û�״̬
	BYTE							cbGameStatus;						//��Ϸ״̬
	WORD							wKindID;							//��Ϸ��ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//����λ��
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//�û�����
struct tagUserIndividual
{
	DWORD							dwUserID;							//�û���ʶ
	//��������
	TCHAR							szQQ[LEN_QQ];						//�û�QQ
	TCHAR							szEMail[LEN_EMAIL];					//�û�Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
	TCHAR							szPostalCode[LEN_POSTALCODE];		//��������
};

//��ע��Ϣ
struct tagUserRemarksInfo
{
	DWORD							dwFriendUserID;						//���ѱ�ʶ
	TCHAR							szRemarksInfo[LEN_REMARKS];			//��ע��Ϣ
};

//�Ƹ���Ϣ
struct tagUserWealthInfo
{
	DWORD							dwUserID;							//�û���ʶ
	LONGLONG						lUserGold;							//�û����	
};

//////////////////////////////////////////////////////////////////////////

//�û���Ϣ
struct tagUserInfo
{
	//��������
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwGroupID;							//���� I D
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szQQ[LEN_QQ];						//QQ����
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	DWORD							dwCustomID;							//�Զ���ʶ

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wLastTableID;					    //��Ϸ����
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lIngot;								//�û�Ԫ��
	DOUBLE							dBeans;								//��Ϸ��
	//˽�˷����
	SCORE							lDiamond;							//��ʯ����
	SCORE							lHistoryKindIDWin;					//����Ϸ��ʷ����Ӯ

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������	
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//������Ϣ
	DWORD							dwAgentID;							//���� I D

	//ʱ����Ϣ
	tagTimeInfo						TimerInfo;							//ʱ����Ϣ	
};

//�û���Ϣ
struct tagUserInfoHead
{
	//�û�����
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGroupID;							//���� I D

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	DWORD							dwCustomID;							//�Զ���ʶ

	//�û�����
	bool							bIsAndroid;							//������ʶ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lIngot;								//�û�Ԫ��
	DOUBLE							dBeans;								//�û���Ϸ��
	SCORE							lFirstScore;						//���뷿�����
	SCORE							lHistoryKindIDWin;					//����Ϸ��ʷ����Ӯ

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//������Ϣ
	DWORD							dwAgentID;							//���� I D
};

//ͷ����Ϣ
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//���ݴ�С
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ
};
//ͷ����Ϣ96*96
struct tagCustomFaceInfo96
{
	DWORD							dwDataSize;							//���ݴ�С
	DWORD							dwCustomFace[BIGFACE_CX*BIGFACE_CY];//ͼƬ��Ϣ
};

//�û���Ϣ
struct tagUserRemoteInfo
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	WORD							wFaceID;							//ͷ������

	//�ȼ���Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//λ����Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	TCHAR							szGameServer[LEN_SERVER];			//����λ��

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wLastTableID;					    //��Ϸ����
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬
};

//////////////////////////////////////////////////////////////////////////////////
//�������
struct tagTaskParameter
{
	//������Ϣ
	WORD							wTaskID;							//�����ʶ
	WORD							wTaskType;							//��������
	WORD							wTaskObject;						//����Ŀ��

	WORD							wKindID;							//���ͱ�ʶ

	WORD							wServerID;							//�����ʶ
	WORD							wTaskProgress;						//�������
	BYTE							cbTaskStatus;						//����״̬
	WORD							wTimeType;							//ʱ������
	
	//������Ϣ
	
	DWORD							dwAwardType;						//��������
	TCHAR							szAwardValue[100];					//����ֵ

	//������Ϣ
	TCHAR							szTaskName[LEN_TASK_NAME];			//��������	TCHAR							szTaskDescribe[320];				//��������
	TCHAR							szTaskDescribe[32];					//��������
};

//��������
struct tagTaskConfigInfo
{
	//������Ϣ
	WORD							wTaskID;							//�����ʶ
	WORD							wTaskType;							//��������
	WORD							wUserType;							//��������
	WORD							wTaskObject;						//����Ŀ��
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTimeType;							//ʱ������
};

//����״̬
struct tagTaskStatus
{
	WORD							wTaskID;							//�����ʶ
	WORD							wTaskProgress;						//�������
	BYTE							cbTaskStatus;						//����״̬
};

//�ͱ�����
struct tagBaseEnsureParameter
{
	SCORE							lScoreCondition;					//��Ϸ������
	SCORE							lScoreAmount;						//��Ϸ������
	BYTE							cbTakeTimes;						//��ȡ����	
};

//�ƹ���Ϣ
struct tagUserSpreadInfo
{
	DWORD							dwSpreadCount;						//�ƹ�����
	SCORE							lSpreadReward;						//�ƹ㽱��
};

//�ȼ�����
struct tagGrowLevelConfig
{
	WORD							wLevelID;							//�ȼ� I D
	DWORD							dwExperience;						//��Ӧ����
};

//�ȼ�����
struct tagGrowLevelParameter
{
	WORD							wCurrLevelID;						//��ǰ�ȼ�
	DWORD							dwExperience;						//��ǰ����
	DWORD							dwUpgradeExperience;				//�¼�����
	SCORE							lUpgradeRewardGold;					//��������
	SCORE							lUpgradeRewardIngot;				//��������
};

//��Ա����
struct tagMemberParameter
{
	BYTE							cbMemberOrder;						//��Ա��ʶ
	TCHAR							szMemberName[16];					//��Ա����
	SCORE							lMemberPrice;						//��Ա�۸�
	SCORE							lPresentScore;						//������Ϸ��
};

//��Ա����
struct tagMemberParameterNew
{
	BYTE							cbMemberOrder;						//��Ա��ʶ
	TCHAR							szMemberName[16];					//��Ա����
	DWORD                           dwMemberRight;                      //��ԱȨ��
	DWORD                           dwMemberTask;                       //��Ա����
	DWORD                           dwMemberShop;                       //��Ա�̳�
	DWORD                           dwMemberInsure;                     //��Ա����
	DWORD                           dwMemberDayPresent;                 //��Ա�ͽ�
	DWORD                           dwMemberDayGiftID;                  //��Ա���
};

//ʵ������
struct tagAuthRealParameter
{
	DWORD							dwAuthentDisable;					//��֤����
	SCORE							dwAuthRealAward;					//�������
};
//////////////////////////////////////////////////////////////////////////////////

//�㳡����
struct tagGamePlaza
{
	WORD							wPlazaID;							//�㳡��ʶ
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[32];					//��������
};

//��������
struct tagLevelItem
{
	LONG							lLevelScore;						//�������
	TCHAR							szLevelName[16];					//��������
};

//��Ա����
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//�ȼ���ʶ
	TCHAR							szMemberName[16];					//�ȼ�����
};

//��������
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//�ȼ���ʶ
	TCHAR							szMasterName[16];					//�ȼ�����
};

//�б�����
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//�б���
	BYTE							cbDataDescribe;						//�ֶ�����
	TCHAR							szColumnName[16];					//�б�����
};

//�ȼ�����
struct tagServerLevelItem
{
	DWORD							dwSortID;							//���б�ʶ
	BYTE							cbServerLevelIndex;					//����ȼ�����
	TCHAR							szServerLevelName[16];				//����ȼ�����
};

//��Ϸ��������
struct tagGameChatItem
{
	DWORD							dwSortID;							//���б�ʶ
	TCHAR							szGameChatString[MAX_CHATSTRINGLEN];//�����ַ���
	WORD							wChatTypeIndex;						//������������
	BYTE							cbChatChance;						//�������
};

//��ַ��Ϣ
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//�����ַ
};

//������Ϣ
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//���ݿ�˿�
	TCHAR							szDataBaseAddr[32];					//���ݿ��ַ
	TCHAR							szDataBaseUser[32];					//���ݿ��û�
	TCHAR							szDataBasePass[32];					//���ݿ�����
	TCHAR							szDataBaseName[32];					//���ݿ�����
};

//��������
struct tagServerOptionInfo
{
	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//˰������
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//�û���Ϣ
struct tagMobileUserInfoHead
{
	//�û�����
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwUserID;							//�û� I D

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	DWORD							dwCustomID;							//�Զ���ʶ

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lIngot;								//�û�Ԫ��
	DOUBLE							dBeans;								//�û���Ϸ��

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwExperience;						//�û�����
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//������Ϣ
	DWORD							dwAgentID;							//���� I D
	DWORD							dwClientAddr;						//�û���ַ
};
//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagUpdateInfo
{
	WORD							wStructSize;						//�ṹ��С
	WORD							wUpdateCount;						//������Ŀ
	char							szDownLoadAddress[MAX_PATH];		//���ص�ַ
};

//��������
struct tagUpdateItem
{
	WORD							wStructSize;						//�ṹ��С
	char							szFileCheckSum[33];					//�ļ�У���
	char							szFileName[128];					//�ļ�����
};

//˽�˷�������
struct tagPersonalTableParameter
{
	BYTE								cbGameMode;									//��Ϸģʽ 0:���� 1��ʱ�� 2��Ȧ�� 3������
	LONGLONG							lIniScore;									//��ʼ�λ���

	WORD								wJoinGamePeopleCount;						//�μ���Ϸ���������
	SCORE								lCellScore;									//��Ϸ�׷�

	DWORD								dwPlayTurnCount; 							//˽�˷Ž�����Ϸ��������
	DWORD								dwPlayTimeLimit;							//˽�˷�������Ϸ�����ʱ�� ��λ��
	BYTE								cbIsJoinGame;								//�Ƿ������Ϸ

	DWORD								dwTimeAfterBeginCount;						//һ�ֿ�ʼ�೤ʱ����ɢ���� ��λ��
	DWORD								dwTimeOffLineCount;							//���߶೤ʱ����ɢ����  ��λ��
	DWORD								dwTimeNotBeginGame;							//�೤ʱ��δ��ʼ��Ϸ��ɢ����	 ��λ��
	DWORD								dwTimeAfterCreateRoom;						//˽�˷������೤ʱ�������������ɢ����

	WORD								wBeginFreeTime;								//���ʱ�ο�ʼ
	WORD								wEndFreeTime;								//���ʱ�ν���
};


//˽�˷�������
struct tagPersonalTableFeeList
{
	BYTE							cbPlayMode;							//0 ���� 1 ���
	DWORD							dwDrawTimeLimit;					//ʱ������
	DWORD							dwDrawCountLimit;					//��������
	LONGLONG						lIniScore;							//��ʼ����
	LONGLONG						lFeeScore;							//��������
	WORD							wAAPayFee;							//AA����
	BYTE							cbGameMode;							//��Ϸģʽ
};

//˽�˶�������
struct tagGetPersonalRule
{
	BYTE							cbPersonalRule[2][1024];			//(0 ���� 1 ���)˽�˶�������
};
//����״̬
enum enPasswordState
{
	enUnShare = 0,							//δ����״̬
	enShare = 1,							//�ѷ���״̬
	enUsed = 2,								//��ʹ��״̬
};
//��������
struct tagRoomPassword
{
	enPasswordState		enPS;				//����״̬
	DWORD				dwRoomPass;			//��������
	DWORD				dwUserID;			//�û�ID
};

//˽�˷�����Ϣ
struct tagPersonalTableInfo
{
	DWORD							dwServerID;							//����I D
	DWORD							dwKindID;							//��������
	DWORD							dwTableID;							//����I D
	DWORD							dwUserID;							//����I D
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	SCORE							lCellScore;							//����׷�
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	BYTE							cbPassword;							//�Ƿ���������
	tagRoomPassword					hRoomPassword[MAX_PASSWORD_COUNT];  //��������
	WORD							wBeginFreeTime;						//���ʱ�ο�ʼ
	WORD							wEndFreeTime;						//���ʱ�ν���
	WORD							wCurSitPlayer;						//��ǰ���µ����
	BYTE							cbPlayMode;							//0 ����ģʽ	1 ���ģʽ
	SCORE							lEnterScoreLimit;					//���뷿������
	SCORE							lLeaveScoreLimit;					//�뿪����
	BYTE							cbJoinMode;							//�Ƿ�����İ���˼���	0 ������	1 ����
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	DWORD							dwGroupID;							//���ֲ�id�� �Ǿ��ֲ�ģʽ �� 0
	bool							bGameStart;							//��Ϸ��ʼ
	TCHAR							szNickName[LEN_NICKNAME];			//��������
	TCHAR							szCreateTime[32];					//����ʱ��
	BYTE							cbPayMode;							//֧��ģʽ
	DWORD							dwPlayerUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//������������
	DWORD							dwBankerUserID;						//������Ϸׯ��USERID(��ЧΪ0)
	SCORE							lMaxChairGameMinChip;				//������Ϸ��С��ע
	DWORD							dwConfigID;							//�淨id
	TCHAR							szConfigName[13];					//�淨����
	//���캯��
	tagPersonalTableInfo()
	{
		ZeroMemory(this, sizeof(tagPersonalTableInfo));
	}
};


//˽�˷�����Ϣ
struct tagPersonalTableInfoEx
{
	DWORD							dwServerID;							//����I D
	DWORD							dwKindID;							//��������
	DWORD							dwTableID;							//����I D
	DWORD							dwUserID;							//����I D
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	SCORE							lCellScore;							//����׷�
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	BYTE							cbPassword;							//�Ƿ���������
	tagRoomPassword					hRoomPassword[MAX_PASSWORD_COUNT];  //��������
	WORD							wBeginFreeTime;						//���ʱ�ο�ʼ
	WORD							wEndFreeTime;						//���ʱ�ν���
	WORD							wCurSitPlayer;						//��ǰ���µ����
	BYTE							cbPlayMode;							//0 ����ģʽ	1 ���ģʽ
	SCORE							lEnterScoreLimit;					//���뷿������
	SCORE							lLeaveScoreLimit;					//�뿪����
	BYTE							cbJoinMode;							//�Ƿ�����İ���˼���	0 ������	1 ����
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	DWORD							dwGroupID;							//���ֲ�id�� �Ǿ��ֲ�ģʽ �� 0
	bool							bGameStart;							//��Ϸ��ʼ
	TCHAR							szNickName[LEN_NICKNAME];			//��������
	TCHAR							szCreateTime[32];					//����ʱ��
	BYTE							cbPayMode;							//֧��ģʽ
	DWORD							dwJoinUserID[PERSONAL_ROOM_CHAIR];	//Լս�����ڵ����
	DWORD							dwFaceID[PERSONAL_ROOM_CHAIR];		//���ͷ��
	DWORD							dwCustomID[PERSONAL_ROOM_CHAIR];						//����Զ���ͷ��
	TCHAR							szJoinUserNickName[PERSONAL_ROOM_CHAIR][LEN_NICKNAME];	//����ǳ�

	//���캯��
	tagPersonalTableInfoEx()
	{
		ZeroMemory(this, sizeof(tagPersonalTableInfoEx));
	}
};


//˽�˷�����ѡ��
struct tagPersonalRoomOption
{
	BYTE							cbPlayMode;							//0 ���ģʽ	1 ����ģʽ
	BYTE							cbIsJoinGame;						// 0 ��ʾ��������Ϸ  1 ��ʶ������Ϸ	
	BYTE							cbMinPeople;						//������С����	
	BYTE							cbMaxPeople;						//�����������	
	SCORE							lMinCellScore;						//˽�˷������׷�
	SCORE							lMaxCellScore;						//˽�˷������׷�

	SCORE							lPersonalRoomTax;					//˽�˷�˰��

	WORD							wCanCreateCount;					//����ܹ�������˽�˷��������Ŀ
	DWORD							dwPlayTurnCount; 					//˽�˷Ž�����Ϸ��������
	DWORD							dwPlayTimeLimit;					//˽�˷�������Ϸ�����ʱ�� ��λ��

	DWORD							dwTimeAfterBeginCount;				//һ�ֿ�ʼ�೤ʱ����ɢ���� ��λ��
	DWORD							dwTimeOffLineCount;					//���߶೤ʱ����ɢ����  ��λ��
	DWORD							dwTimeNotBeginGame;					//�೤ʱ��δ��ʼ��Ϸ��ɢ����	 ��λ��
	DWORD							dwTimeAfterCreateRoom;				//˽�˷������೤ʱ�������������ɢ����
	WORD							wBeginFreeTime;						//���ʱ�ο�ʼ
	WORD							wEndFreeTime;						//���ʱ�ν���
	SCORE							lMinEnterScore;						//˽�˷������׷�
	SCORE							lMaxEnterScore;						//˽�˷������׷�
	DWORD							dwTimeRespondDismiss;				//���������Ӧ��ɢ����ʱ��
};



//˽�˷�����ѡ��
const int FEE_OPTION_COUNT = 5;
struct CreateRoomPayFee
{
	BYTE							cbGameMode;							//��Ϸģʽ
	DWORD							dwPlayTurnLimit;					//�����������
	DWORD							dwPlayTimeLimit;					//����ʱ������
	DWORD							dwPayFee;							//����������Ϣ����
	WORD							wAAPayFee;							//AA�Ʒ���
	DWORD							dwIniScore;							//�����ʼ����
	BYTE							cbPlayMode;							//0 ����  1 ���
	CreateRoomPayFee()
	{
		cbGameMode = 0;
		dwPlayTurnLimit = 0;
		dwPlayTimeLimit = 0;
		dwPayFee = 0;
		wAAPayFee = 0;
		dwIniScore = 0;
		cbPlayMode = 0;
	}
};

struct tagPersonalRoomOptionRightAndFee
{
	byte							cbCreateRight;						//����˽�˷����Ȩ��
	int								nCellScore[CELLSCORE_COUNT];
	CreateRoomPayFee				stCreateRoomPayFee[FEE_OPTION_COUNT];
};

const int MAX_CREATE_PERSONAL_ROOM = 20;
const int MAX_CREATE_SHOW_ROOM = 20;

struct tagRoomCreateInfo
{
	DWORD					dwPersonalRoomID;						//Լս����ID
	WORD					wBeginFreeTime;							//��ʼ���ʱ��
	WORD					wEndFreeTime;							//�������ʱ��
	WORD					wCurSitCount;							//��ǰ���µ��������
};

//��������������Ϣ
struct tagHostCreatRoomInfo
{
	DWORD					dwUserID;
	DWORD					dwKindID;
	tagRoomCreateInfo		hRoomCreateInfo[MAX_CREATE_PERSONAL_ROOM];

	tagHostCreatRoomInfo()
	{
		memset(&hRoomCreateInfo, 0, sizeof(tagRoomCreateInfo) * MAX_CREATE_PERSONAL_ROOM);
	}
};


//Լս���������Ϣ
struct tagPersonalUserScoreInfo
{
	DWORD							dwUserID;							//���ID
	TCHAR							szUserNicname[LEN_NICKNAME];		//�û��ǳ�

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lTaxCount;							//˰������

	tagPersonalUserScoreInfo()
	{
		memset(this, 0, sizeof(tagPersonalUserScoreInfo));
	}

};

//�������󵥸�Լս������Ϣ
struct tagPersonalRoomInfo
{
	DWORD					dwPersonalRoomID;							//Լս����ID
	DWORD					dwGameKindID;								//��Ϸ����

	DWORD					dwPlayTurnCount; 							//˽�˷Ž�����Ϸ��������
	DWORD					dwPlayTimeLimit;							//˽�˷�������Ϸ�����ʱ�� ��λ��

	BYTE 					cbIsDisssumRoom;							//����״̬ 0 ��ʾδ��ʼ 1 ��ʾ ��Ϸ�� 2 �ѽ�ɢ
	SYSTEMTIME				sysCreateTime;								//˽�˷��䴴��ʱ��
	SYSTEMTIME				sysDissumeTime;								//˽�˷������ʱ



	BYTE					cbPayMode;									//֧��ģʽ
	BYTE					cbNeedRoomCard;								//��Ҫ���ĵķ�������
	BYTE					cbPlayerCount;								//������Ϸ������
	WORD					wBeginFreeTime;								//��ʼ���ʱ��
	WORD					wEndFreeTime;								//�������ʱ��
	WORD					wCurSitCount;								//��ǰ���µ��������
	BYTE					cbGameMode;									//��Ϸģʽ 0:���� 1��ʱ�� 2��Ȧ�� 3������
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];//˽�˷������������Ϣ 
	tagPersonalRoomInfo()
	{
		dwPlayTurnCount = 0;
		dwPlayTimeLimit = 0;
		cbIsDisssumRoom = 0;
		cbPayMode = 0;
		cbNeedRoomCard = 0;
		cbPlayerCount = 0;
		dwPersonalRoomID = 0;
		wBeginFreeTime = 0;
		wEndFreeTime = 0;
		wCurSitCount = 0;
		dwGameKindID = 0;
		cbGameMode = 0;
	}
};

//�������󵥸�˽�˷�����Ϣ
struct tagQueryPersonalRoomUserScore
{
	DWORD			dwPersonalRoomID;					//Լս����ID
	TCHAR			szUserNicname[LEN_NICKNAME];		//�û��ǳ�
	DWORD			dwPlayTurnCount; 					//˽�˷Ž�����Ϸ��������
	DWORD			dwPlayTimeLimit;					//˽�˷�������Ϸ�����ʱ�� ��λ��
	SYSTEMTIME		sysCreateTime;						//˽�˷��䴴��ʱ��
	SYSTEMTIME		sysDissumeTime;						//˽�˷������ʱ

	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];//˽�˷������������Ϣ 
	tagQueryPersonalRoomUserScore()
	{
		dwPlayTurnCount = 0;
		dwPlayTimeLimit = 0;
		dwPersonalRoomID = 0;
	}
};

struct tagPersonalGeneralRule
{
	int nTurnCount;				//Լս����
	int nPlayerCount;			//Լս����
	int nFee;					//Լս����
	int nIniScore;				//��ʼ����
	int nExOne;					//��չ����1
	int nExTwo;					//��չ����2
	int nExThree;				//��չ����3
	tagPersonalGeneralRule()
	{
		memset(this, 0, sizeof(tagPersonalGeneralRule));
	}
};


//���ýṹ
struct tagPersonalRule
{
	byte	cbSpecialRule;											//�Ƿ����������
	tagPersonalGeneralRule personalRule[PERSONAL_OPTION_COUNT];		//���ò���
	int nCellScore[CELLSCORE_COUNT];														//��ʼ����
	tagPersonalRule()
	{
		memset(nCellScore, 0, sizeof(nCellScore));
	}
};

//���������
struct tagShareStorageRule
{
	BYTE							cbShareStorageRule;								//���ñ�ʶ 

	//�������
	SCORE							lConfigSysStorage;								//����ϵͳ���
	SCORE							lConfigPlayerStorage;							//������ҿ��
	SCORE							lConfigParameterK;								//���õ���ϵ��(�ٷֱ�)
	SCORE							lConfigSysStorResetRate;						//����ϵͳ������ñ���

	//��������
	SCORE							lConfigAnChouRate;								//���ð������

	//������
	SCORE							lConfigMosgoldRate;								//���ô󽱳�ȡ����
	SCORE							lConfigMosgoldDispatchRate;						//�����ɽ�����
	SCORE							lConfigMosgoldStorageRate;						//���ô󽱿�����	
};

//////////////////////////////////////////////////////////////////////////////////

enum enUserLogonMode
{
	//��½��ʽ
	ULMByAccount,
	ULMByGameID,
	ULMByVisitor,
	ULMBySina,
	ULMByTencent,
	ULMByRenRen,
};

//ϵͳ��Ϣ
struct tag_GR_SystemMessage
{
	DWORD                           dwMessageID;                        //��ϢID
	BYTE							cbMessageType;						//��Ϣ����
	BYTE                            cbAllRoom;                          //ȫ�巿��
	DWORD							dwTimeRate;						    //ʱ��Ƶ��
	__time64_t						tStartTime;							//��ʼʱ��
	__time64_t                      tConcludeTime;                      //����ʱ��
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};

//ϵͳ��Ϣ
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //����ʱ��
	tag_GR_SystemMessage            SystemMessage;                     //ϵͳ��Ϣ
};
/////////////////////////////////////////////////////////////////////////////////////////////////
//���ֲ�����

//����������
struct tagServiceItem
{
	WORD							wServiceID;							//�߼���ʶ
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ

};

//������Ϣ
struct tagIMSocketInfo
{
	WORD							wAccessID;							//�����ʶ
	DWORD							dwTokenID;							//���Ʊ�ʶ
};

//Ⱥ��Լս����
struct tagIMGroupOption
{
	DWORD							dwStationID;						//վ���ʶ
	BYTE							cbPayTypeMask;						//֧������
	BYTE							cbCurrencyKindMask;					//��������
};

//Ⱥ��Ƹ�
struct tagIMGroupWealth
{
	LONGLONG						lIngot;								//ʣ����ʯ
	LONGLONG						lRoomCard;							//ʣ�෿��
	LONGLONG						lScore;								//ѫ��
};

//Ⱥ������
struct tagIMGroupProperty
{
	//������Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ	
	DWORD							dwCreaterID;						//Ⱥ����ʶ
	DWORD							dwCreaterGameID;					//Ⱥ����ʶ	
	WORD							wGroupLevel;						//Ⱥ�鼶��
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
	TCHAR							szCreaterNickName[LEN_ACCOUNTS];	//Ⱥ���ǳ�

	//��Ա��Ϣ
	WORD							wMemberCount;						//��Ա����
	WORD							wMaxMemberCount;					//��Ա����

	//������Ϣ
	SYSTEMTIME						CreateDateTime;						//����ʱ��
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//Ⱥ�����	
	BYTE							cbGroupStatus;						//Ⱥ��״̬0��ɾ����1��Ч��2����
};

//Ⱥ�����
struct tagIMGroupBattleRule
{
	BYTE							cbPayType;							//֧������
	BYTE							cbCurrencyKind;						//��������
	BYTE							cbBattleMask;						//Լս����
	LONGLONG						lManagerRight;						//����ԱȨ��
	LONGLONG						lCaptainRight;						//�ӳ�Ȩ��
	BYTE							cbMedalMode;						//ѫ��ģʽ0�رգ�1����
	BYTE							cbMedalShow;						//ѫ������ʾ0����ʾ��1��ʾ
	BYTE							cbCurMedalShow;						//��ǰѫ����ʾ0����ʾ��1��ʾ
	BYTE							cbMedalRevenueShow;					//ѫ�������ʾ0����ʾ��1��ʾ
	BYTE							cbMedalClear;						//ѫ������0�����㣬1�������
	BYTE							cbInMode;							//�������0������ˣ�1���
	BYTE							cbOutMode;							//�˳�����0������ˣ�1���
	BYTE							cbFreeTableLimit;					//��������
	BYTE							cbBigWinnerShow;					//0����ʾ��1��ʾ
	BYTE							cbGroupIDShow;						//0����ʾ��1��ʾ
};

//Ⱥ�鸽����Ϣ
struct tagIMGroupExtraInfo
{
	DWORD							dwGroupID;							//����ȦID
	DWORD							dwUserID;							//���ID
	INT								nMemberRight;						//������
	LONGLONG						lInvitationCode;					//������
	BYTE							cbGroupIDShow;						//����ȦID��ʾ��0����ʾ��1��ʾ
	//�ϼ���Ϣ
	TCHAR							szQQ[LEN_NICKNAME];					//�û�QQ
	TCHAR							szWX[LEN_NICKNAME];					//�û�WX
	TCHAR							szPhone[12];						//�û��ֻ���
};

//Ⱥ������
struct tagIMGroupItem
{
	tagIMGroupWealth				GroupWealth;						//Ⱥ��Ƹ�
	tagIMGroupProperty				GroupProperty;						//Ⱥ������
	tagIMGroupBattleRule			GroupBattleRule;					//Ⱥ�����
};

//������Ϣ����
struct tagIMCommonItem
{
	TCHAR							szOptionName[32];//��������	
	INT								nOptionValue;	//������ֵ
};

//�û���Ϣ
struct tagIMUserInfo
{
	//��������	
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D	
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbUserStatus;						//�û�״̬
	WCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�	

	//�Զ�ͷ��
	DWORD							dwCustomID;							//�Զ���ʶ	
};

//���·ֽ��
struct tagIMUserScore
{
	//��������	
	DWORD							dwUserID;							//�û�ID
	BYTE							cbSuccess;							//0ʧ�ܣ�1�ɹ�
	SCORE							lCurScore;							//��ǰ��
};

//ɾ����Ա
struct tagIMDeleteMember
{
	DWORD							dwMemberID;							//��ԱID
	BYTE							cbSuccess;							//0ʧ�ܣ�1�ɹ�
};

//Ⱥ���Ա
struct tagIMGroupMember
{
	DWORD							dwMemberID;							//��Ա��ʶ	
	DWORD							dwBattleCount;						//Լս����
	SYSTEMTIME						JoinDateTime;						//����ʱ��
	LONGLONG						lScore;								//ѫ��
	INT								nMemberRight;						//Ȩ��
	DWORD							dwCaptainID;						//�ӳ�ID
	BYTE							cbStatus;							//0���ߣ�1����
	DWORD							dwFavoriteConfig;					//�ղ��淨
	BYTE							cbManagerRightFlag;					//�Ƿ񵥶����ù���ԱȨ��
	LONGLONG						lMemberManagerRight;				//��Ա����ԱȨ��
	INT								nAwardPoint;						//�ӳ���λ
	LONGLONG						lInvitationCode;					//������

	TCHAR							szQQ[LEN_NICKNAME];					//�û�QQ
	TCHAR							szWX[LEN_NICKNAME];					//�û�WX
	TCHAR							szPhone[12];						//�û��ֻ���
};

//Ⱥ���Ա��Ϸ��Ϣ
struct tagIMMemberDayRecord
{
	DWORD							dwMemberID;							//��Ա��ʶ	
	INT								nPlayCount;							//��Ϸ����
	INT								nBigWinnerCount;					//��Ӯ�Ҵ���
	LONGLONG						lScore;								//�������ѫ��
	LONGLONG						lWinScore;							//������Ӯ��
	LONGLONG						lRevenue;							//���չ���˰��
	BYTE							cbFlag;								//����
};

//�ӳ��Լ�������Ϸͳ��
struct tagTeamerDayRecord
{
	DWORD							dwUserID;							//��Ա��ʶ
	DWORD							dwGameID;
	TCHAR							szName[LEN_NICKNAME];
	INT								nPlayCount;							//��Ϸ����
	INT								nBigWinnerCount;					//��Ӯ�Ҵ���
	LONGLONG						lRevenue;							//���ղ���˰��
	LONGLONG						lContribution;						//���ϼ��Ĺ���ֵ
	BYTE							cbFlag;								//����
};

//��ҵ���������Ϣ
struct tagIMMemberDayDetail
{
	DWORD							dwRoomID;							//�����ʶ	
	BYTE							cbPlayMode;							//��������
	TCHAR							szServerName[32];					//��������
	INT								nDrawCount;							//����
	DWORD							dwBigWinnerID;						//��Ӯ��ID
	TCHAR							szBigWinnerName[32];				//��Ӯ���ǳ�
	LONGLONG						lScore;								//�÷�
	SYSTEMTIME						EndTime;							//��Ϸ����ʱ��
};

//���䵥��������Ϣ
struct tagIMRoomTurnDetail
{
	INT			nTurnIndex;
	SYSTEMTIME	StartTime;
	SCORE	lTurnScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
	bool	bFlag[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
};

//���ֲ�����������Ϣ
struct tagIMGroupDayDetail
{
	DWORD							dwRoomID;							//�����ʶ	
	BYTE							cbPlayMode;							//��������
	TCHAR							szServerName[32];					//��������
	INT								nDrawCount;							//����
	DWORD							dwBigWinnerID;						//��Ӯ��ID
	TCHAR							szBigWinnerName[32];				//��Ӯ���ǳ�
	LONGLONG						lDiamond;							//��ʯ����
	LONGLONG						lRoomRevenue;						//��������Ĺ���
	SYSTEMTIME						EndTime;							//��Ϸ����ʱ��
};

//Ⱥ���Ա
struct tagIMGroupMemberUser
{
	tagIMUserInfo					UserInfo;							//�û���Ϣ
	tagIMGroupMember				MemberInfo;							//��Ա��Ϣ	
};

//������Ϣ
struct tagIMApplyMessage
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageStatus;					//��Ϣ״̬
	BYTE							cbMessageType;						//��Ϣ����
	//������Ϣ
	DWORD							dwApplyerID;						//�û���ʶ
	DWORD							dwApplyerGameID;					//��Ϸ��ʶ
	DWORD							dwApplyerCustomID;					//ͷ���ʶ
	TCHAR							szApplyerNickName[LEN_NICKNAME];	//�û��ǳ�
	SYSTEMTIME						ApplyDateTime;						//����ʱ��

	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ	
	DWORD							dwCreaterID;						//��ע��ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������	
	//DWORD							dwOperatorID;						//�����ID
	//TCHAR							szOperatorName[LEN_NICKNAME];		//������ǳ�	
};

//������ȡ��¼
struct tagIMAwardBoxRecord
{
	SCORE							lBeforeScore;						//��ȡǰ
	SCORE							lScore;								//��ȡ���
	SCORE							lAfterScore;						//��ȡ��
	SYSTEMTIME						DateTime;							//��ȡʱ��
};

//���ֲ����ͼ�¼
struct tagIMPresentItem
{
	DWORD							dwGroupID;							//���ֲ���ʶ
	DWORD							dwPresentID;						//�������
	DWORD							dwPresentGameID;					//���������ϷID
	TCHAR							szPresentNickName[LEN_ACCOUNTS];	//��������ǳ�
	BYTE							cbPresentUserRight;					//����������1Ȧ����2����Ա
	DWORD							dwReceiveID;						//�������
	DWORD							dwReceiveGameID;					//���������ϷID
	TCHAR							szReceiveNickName[LEN_ACCOUNTS];	//��������ǳ�
	LONGLONG						lChangeScore;						//�����仯ֵ
	LONGLONG						lCurScore;							//�ı�����
	BYTE							cbType;								//����0�����ͣ�1������
	SYSTEMTIME						CollectDateTime;					//ʱ��
};

//���ֲ��۷Ѽ�¼
struct tagIMRevenueItem
{
	DWORD							dwGroupID;							//���ֲ���ʶ
	TCHAR							szKindName[LEN_ACCOUNTS];			//��Ϸ����
	LONGLONG						lScore;								//����
	LONGLONG						lRevenue;							//˰��
	SYSTEMTIME						CollectDateTime;					//ʱ��
};

//˽�˷�����Ϣ
struct tagClubTableInfo
{
	DWORD							dwServerID;							//����I D
	DWORD							dwKindID;							//��������
	WORD							wTableID;							//����I D
	DWORD							dwUserID;							//����I D
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	SCORE							lCellScore;							//����׷�
	DWORD							dwPersonalRoomID;					//Լս����ID
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	WORD							wCurSitPlayer;						//��ǰ���µ����
	BYTE							cbPlayMode;							//0 ����ģʽ	1 ���ģʽ
	SCORE							lEnterScoreLimit;					//���뷿������
	SCORE							lLeaveScoreLimit;					//�뿪����
	BYTE							cbJoinMode;							//�Ƿ�����İ���˼���	0 ������	1 ����
	DWORD							dwGroupID;							//���ֲ�id�� �Ǿ��ֲ�ģʽ �� 0
	bool							bGameStart;							//��Ϸ��ʼ
	TCHAR							szNickName[LEN_NICKNAME];			//��������
	TCHAR							szCreateTime[32];					//����ʱ��
	BYTE							cbPayMode;							//֧��ģʽ
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	DWORD							dwPlayerID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//���ID
	DWORD							dwBankerUserID;						//������Ϸׯ��USERID(��ЧΪ0)
	SCORE							lMaxChairGameMinChip;				//������Ϸ��С��ע
	DWORD							dwConfigID;							//�淨ID
	TCHAR							szConfigName[13];					//�淨����
	//���캯��
	tagClubTableInfo()
	{
		ZeroMemory(this, sizeof(tagClubTableInfo));
	}

};

//���ֲ��淨��Ϣ
struct tagGroupConfigInfo
{
	DWORD							dwConfigID;								//�淨ID
	DWORD							dwGroupID;								//���ֲ�id
	TCHAR							szConfigName[13];						//�淨����
	WORD							wKindID;								//����ID
	BYTE							cbPlayMode;								//����ģʽ0���֣�1ѫ��
	LONGLONG						lCellScore;							    //�׷�����
	DWORD							dwDrawCountLimit;						//��������
	DWORD							dwDrawTimeLimit;						//ʱ������
	WORD							wJoinGamePeopleCount;					//������Ϸ������
	DWORD							dwRoomTax;
	BYTE							cbPassword;								//�������� 0:������ 1:��Ҫ����
	BYTE							cbPayMode;								//֧����ʽ									
	BYTE							cbGameRule[RULE_LEN];					//��Ϸ���� ��0λ��ʶ �Ƿ����ù��� 0 ����δ���� 1 ��������
	BYTE							cbGameMode;								//��Ϸģʽ
	LONGLONG						lEnterScore;							//���뷿��ķ�������
	LONGLONG						lLeaveScore;							//�뿪�����������
	BYTE							cbJoinMode;								//����ģʽ
	BYTE							cbTaxMode;								//˰��ģʽ��1�̶�ģʽ������ң�2�̶�ģʽ��Ӯ�ң�3����ģʽ������ң�4����ģʽ��Ӯ��
	DWORD							dwTaxCount;								//˰����ֵ	
	BYTE							cbLessScoreDismiss;						//����������ɢ
	LONGLONG						lTimeSecond;							//ʱ���
	BYTE							cbGroupPersonalMaxChair;				//�����˰�����Ϸ(�����淨���Ǵ�������)

	//���캯��
	tagGroupConfigInfo()
	{
		ZeroMemory(this, sizeof(tagGroupConfigInfo));
	}

};

//���ֲ�Լս��Ϣ
struct tagClubSettleBattleInfo
{
	DWORD							dwUserID;							//�û�I D
	DWORD							dwBattleCount;						//Լս����
	DWORD							dwSettleCount;						//�������
};

//���ֲ��������Ϣ
struct tagClubRollMsg
{
	DWORD							dwUserID;							//�û�I D
	BYTE							cbRollMsgFlag;						//�����״̬
	TCHAR							szRollMsg[33];						//���������
	INT								nRollMsgTime;						//�������
};

//////////////////////////////////////////////////////////////////////////
//��Ϸ���


//ǩ�����
struct tagSignInReward
{
	DWORD								nPackageID;							//���ID
	TCHAR								szPackageName[32];					//�������
	BYTE								cbType;								//������ͣ�0ÿ��ǩ�� 1�ۼ�ǩ����
	DWORD								nNeedDay;							//��Ҫʱ��
	DWORD								nSortID;							//�������
	DWORD								nProbability;						//�������
	int									nPlatformKindID;					//ƽ̨���
};

//�������
struct tagPackageInfo
{
	DWORD								nPackageID;							//���ID
	DWORD								dwGoodsCount;						//��Ʒ��Ŀ
	TCHAR								szPackageInfo[64];					//�������
	TCHAR								szResourceURL[50];					//ͼƬ��ַ
};

//�������
struct tagRewardInfo
{
	DWORD								dwGoodsCount;						//��Ʒ��Ŀ
	TCHAR								szPackageInfo[64];					//�������
	TCHAR								szResourceURL[50];					//ͼƬ��ַ
};

//������״̬
typedef enum
{
	_NULL_STATUS_ = 0,								//��״̬
	_ADD_SUBITEM_ = 11,								//�������(Ԥ��)
	_MODIFY_SUBITEM_ = 22,							//�޸�����	
	_REMOVE_SUBITEM_ = 33,							//�Ƴ�����(Ԥ��)
	_CLIENTQUERY_SUBITEM_ = 44,						//�ͻ��˲�ѯ����	
	_SERVERCUR_SUBITEM_ = 55,						//����˵�ǰ����
	_MODIFY_SUBITEM_RES_SUCCEED_ = 66,				//�޸��������ɹ�
	_MODIFY_SUBITEM_RES_FAILED_ = 77,				//�޸�������ʧ��
	_CLIENTQUERY_SUBITEM_RES_SUCCEED_ = 88,			//�ͻ��˲�ѯ����ɹ�	
	_CLIENTQUERY_SUBITEM_RES_FAILED_ = 99,			//�ͻ��˲�ѯ����ʧ��	
}ENUM_SHARESTORAGE_STATUS;		

//�޸���������
typedef enum
{
	_NULL_SUBITEM_ = 0,				//������
	_MODIFY_SUBSTORAGE_ = 11,		//�޸Ŀ������
	_MODIFY_SUBANCHOU_ = 22,		//�޸İ�������
	_MODIFY_SUBMOSGOLD_ = 33,		//�޸Ĵ�����
	_MODIFY_SUBTEMPSTORAGE_ = 44,	//�޸���ʱ�������
}ENUM_MODIFY_SUBITEM_TYPE;

//���ÿ������
struct tagConfigSubStorageItem
{
	//�������
	SCORE							lConfigSysStorage;								//����ϵͳ���
	SCORE							lConfigPlayerStorage;							//������ҿ��
	SCORE							lConfigParameterK;								//���õ���ϵ��(�ٷֱ�)
	SCORE							lConfigSysStorResetRate;						//����ϵͳ������ñ���

	//��������
	SCORE							lConfigAnChouRate;								//���ð������

	//������
	SCORE							lConfigMosgoldRate;								//���ô󽱳�ȡ����
	SCORE							lConfigMosgoldDispatchRate;						//�����ɽ�����
	SCORE							lConfigMosgoldStorageRate;						//���ô󽱿�����

	//��ʱ�������
	SCORE							lConfigTempSysStorage;							//������ʱϵͳ���
	SCORE							lConfigTempPlayerStorage;						//������ʱ��ҿ��
	SCORE							lConfigTempParameterK;							//������ʱ����ϵ��(�ٷֱ�)
	SCORE							lConfigTempSysStorResetRate;					//������ʱϵͳ������ñ���
};

//��ǰ�������
struct tagCurSubStorageItem
{
	//��������
	SCORE							lCurStatisticSysWin;							//��ǰͳ��(����ͨ�ÿ�棬��������������ϵͳ��Ӯ�Ͱ����)
	SCORE							lCurStatisticService;							//��ǰ�����������
	SCORE							lCurSysStorage;									//��ǰϵͳ���
	SCORE							lCurPlayerStorage;								//��ǰ��ҿ��
	SCORE							lCurParameterK;									//��ǰ����ϵ��(�ٷֱ�):
	SCORE							lCurSysStorResetRate;							//��ǰϵͳ������ñ���
	SCORE							lCurWinChance;									//��ǰ�н���
	SCORE							lCurResetCount;									//��ǰ������ô���
	SCORE							lCurSysWin;										//��ǰͨ�ÿ���ϵͳ����Ӯ

	//��������
	SCORE							lCurAnChouRate;									//��ǰ�������
	SCORE							lCurAnChouTotal;								//��ǰ��������

	//������
	SCORE							lCurMosgoldRate;								//��ǰ�󽱳�ȡ����
	SCORE							lCurMosgoldDispatchRate;						//��ǰ�ɽ�����
	SCORE							lCurMosgoldStorageRate;							//��ǰ�󽱿�����

	//��ʱ�������
	SCORE							lCurTempSysStorage;								//��ǰ��ʱϵͳ���
	SCORE							lCurTempPlayerStorage;							//��ǰ��ʱ��ҿ��
	SCORE							lCurTempParameterK;								//��ǰ��ʱ����ϵ��(�ٷֱ�)
	SCORE							lCurTempSysStorResetRate;						//��ǰ��ʱϵͳ������ñ���
	SCORE							lCurTempWinChance;								//��ǰ��ʱ�н���
};

//����������
struct tagShareStorageItem
{
	ULONGLONG						ulItemID;										//����ID(dwGroupID��wKindID���)
	DWORD							dwGroupID;										//����Ȧ��ʶ
	WORD							wKindID;										//���ͱ�ʶ
	BYTE							cbPlayMode;										//0����1���
	ENUM_MODIFY_SUBITEM_TYPE		emModifySubItemType;							//�޸���������
	tagConfigSubStorageItem			ConfigSubStorageItemInfo;						//���ÿ��������Ϣ
	tagCurSubStorageItem			CurSubStorageItemInfo;							//��ǰ���������Ϣ
};

//���������Ϣ
struct tagShareServerInfo
{
	WORD							wServerID;										//�����ʶ
	WORD							wKindID;										//���ͱ�ʶ
	BYTE							cbPlayMode;										//0����1���
};

//������������
struct tagShareConfig
{
	WORD							wShareStorageItemCount;							//������������Ŀ
	WORD							wLoadServerCount;								//���ط�����Ŀ
	tagShareServerInfo				tagServerInfoArray[MAX_SERVER];					//��������
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;							//��ǰ״̬
};

//////////////////////////////////////////////////////////
//������AI
struct tagAIUserInfo
{
	WORD		wServerID;	//�����ʶ
	DWORD		dwUserID;	//AI��ʶ
	byte		cbStatus;	//0 δ����	1 �ѷ��� 2 ����ɹ� 3 �˳�����
	SCORE		lScore;		//AI��ǰ����
	time_t		tTime;		//AI������ֲ�ʱ��
	time_t		tEnterTime;	//AI������ֲ�ʱ��
	byte		cbIsLeave;	//0 ���뿪 1 �뿪 2 �˳�
};


//�뿪����Ȩ������
struct tagWeightOutRatio
{
	DWORD dwTime;		//������������ʱ��
	DWORD dwOutRatio;	//�����뿪����
};

//�뿪����Ȩ�����ü���
struct WeightOutOption
{
	tagWeightOutRatio WeightOutRatio[10];
};

//������ϷȨ������
struct tagWeightInRatio
{
	DWORD dwTime;		//������������ʱ��
	DWORD dwInRatio;	//�����뿪����
};
//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif
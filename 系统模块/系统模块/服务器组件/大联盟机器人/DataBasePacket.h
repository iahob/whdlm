//�ر���IT������28xin.com��
#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
#define  DATA_BASE_REQUEST_ID 10000000

//�������ݰ�
#define	DBR_GR_AI_PARAMETER					1							//AI����
#define	DBR_GR_LOAD_AI_PARAMETER			2							//����AI����
#define	DBR_GR_LOAD_AI_USER					3							//����AI
#define	DBR_GR_UNLOCK_AI_USER				4							//�ͷ�AI
#define	DBR_GR_LOAD_SINGLE_AI				5							//���ص���AI
#define	DBR_GR_LOAD_ALL_AI_OPTION			6							//��������AI����
#define	DBR_GR_CLEAR_AI						7							//����AI
#define	DBR_GR_AI_QUIT_GROUP				8							//AI�뿪������

//////////////////////////////////////////////////////////////////////////////////

//�����������ݱ�
#define DBO_GR_AI_OPERATE_RESULT			1							//������
#define DBO_GR_LOAD_AI_PARAMETER_RESULT		2							//����AI�������
#define DBO_GR_LOAD_AI_PARAMETER_END		3							//����AI��������
#define DBO_GR_LOAD_AI_USER_RESULT			4							//����AI��ҽ��
#define DBO_GR_LOAD_AI_USER_END				5							//����AI��ҽ���

#define DBO_GR_AI_TIME_CHANGE				6							//����AIʱ��仯

#define DBO_GR_AI_WEIGHT_IN					7							//AI������ϷȨ��
#define DBO_GR_AI_WEIGHT_OUT				8							//AI�뿪������Ȩ��
#define DBO_GR_CLEAR_AI_RESULT				9							//����AI���





//////////////////////////////////////////////////////////////////////////////////
//AI ����
struct DBR_GR_GroupAIParameter
{
	DWORD							dwSocketID;					
	byte							cbStatus;					//���ֲ�ID
	DWORD							dwGroupID;					//���ֲ�ID
	SCORE							lDistirbuteTotalScore;		//�����ܷ���
	DWORD							dwAICount;					//AI����
	WORD							wKindID;					//��Ϸ��ʶ
	DWORD							dwPlayingAIRateMin;			//��Ϸ�е�AI
	DWORD							dwPlayingAIRateMax;			//�ȴ��е�AI
	SCORE							lDistirbuteSingleScore;		//����AI�������
	SCORE							lDistirbuteScoreMinRatio;	//����AI���������������
	SCORE							lDistirbuteScoreMaxRatio;	//����AI���������������
};


//�������
struct DBR_GR_AIOperateResult
{	
	DWORD							dwSocketID;							//�����ʶ
	//��¼��Ϣ
	WORD							wOperateID;							//������ʶ
	BYTE							cbResult;							//�������
	TCHAR							szDescripe[260];					//��¼����
};


//������AI
struct DBR_GR_AIUserInfo
{
	DWORD	dwGroupID;	//���ֲ�ID
	WORD	wKindID;	//��Ϸ����ID
	DWORD	dwUserID;	//AI��ʶ
	byte	cbStatus;	//0 δ����	1 �ѷ���
	SCORE	lScore;		//�����˵�ǰ����
};

//����������AI
struct DBR_GR_UnLockAI
{
	DWORD	dwAICount;		//AI����
	DWORD	dwGroupID;		//���ֲ�ID
	WORD	wKindID;		//��Ϸ����ID
	DWORD	dwUserID[10];	//���ID
};

//AI�˳�������
struct DBR_GR_AIQuitGroup
{
	DWORD	dwAICount;		//AI����
	DWORD	dwGroupID;		//���ֲ�ID
	WORD	wKindID;		//��Ϸ����ID
	DWORD	dwUserID[10];	//���ID
};

//���ص���AI
struct DBR_GR_LoadSingleAI
{
	DWORD	dwAICount;		//AI����
	DWORD	dwGroupID;		//���ֲ�ID
	WORD	wKindID;		//��Ϸ����ID
};



//������AI
struct DBR_GR_LoadAIEnd
{
	DWORD	dwGroupID;	//���ֲ�ID
};



//////////////////////////////////////////////////////////////////////////////////

#endif
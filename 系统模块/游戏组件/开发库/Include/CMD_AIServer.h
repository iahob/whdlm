#ifndef CMD_AI_DISTRIBUT_SERVER_HEAD_FILE
#define CMD_AI_DISTRIBUT_SERVER_HEAD_FILE

#pragma pack(1)

//ƽ̨����
#include "Platform.h"

//AI������ʱ��仯
struct TimeChange
{
	DWORD dwTimeChange[24];
	//ʱ��仯
	TimeChange()
	{
		ZeroMemory(dwTimeChange, sizeof(dwTimeChange));
	}
};



//������ϷȨ�����ü���
struct WeightInOption
{
	tagWeightInRatio WeightInRatio[10];
};

struct tagAIStatusInfo
{
	//��¼��Ϣ
	byte							cbStatus;							//0 δ���䣬1 �ѷ��䣬 2 ����ɹ�
	DWORD							dwGroupID;							//�û� I D
	DWORD							dwUserID;							//�û� I D
	WORD							wKindID;							//��������
	SCORE							lScore;								//AIѫ��
};

struct tagAISitFailure
{
	//��¼��Ϣ
	DWORD							dwGroupID;							//���ֲ� I D
	DWORD							dwUserID;							//�û� I D
	WORD							wKindID;							//��������
	WORD							wServerID;							//��������ʶ
	WORD							wTableID;							//���ӱ�ʶ
	SCORE							lEnterLimiteScore;					//����ѫ������
};

//////////////////////////////////////////////////////////////////////////////////
//�������
#define MDM_SS_S_SERVICE						1						//AI���

#define SUB_SS_S_REGISTER_SERVER				1						//������ע��

#define SUB_SS_S_AI_SERVER						2						//AI����
#define SUB_SS_S_AI_UPDATE_BATTLE_RULE			3						//����Լս����
#define SUB_SS_S_AI_UPDATE_USER_SCORE			4						//������ҷ���



//////////////////////////////////////////////////////////////////
//AI���
#define MDM_CS_S_AI_ACTION			2000								//AI���

#define SUB_CS_S_AI_PARAMETER					1						//AI����
#define SUB_CS_S_PSRNL_ROOM_CREATE_SUCCESS		2						//���䴴���ɹ�
#define SUB_CS_C_LOGON_AIID						3						//AI�û����뷿��
#define SUB_CS_C_LOGON_AI_RESULT				4						//AI���뷿����
#define SUB_CS_C_UPDATE_AI_STATUS				5						//����AI״̬
#define SUB_CS_C_AI_UPDATE_TABLE_COUNT			6						//������������
#define SUB_SG_G_UPDATE_GROUP_STATUS			7						//���¾��ֲ�״̬
#define SUB_SG_G_RELOAD_GROUP_MEMBER			8						//֪ͨ���ֲ������µĳ�Ա
#define SUB_CS_C_AI_FAILURE						9						//AI����ʧ��


//////////////////////////////////////////////////////////////////////////////////

//������AI����
struct tagGroupAIParameter
{
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

//������AI����
struct CMD_SS_G_PersonalRoomCreateSuccess
{
	DWORD							dwGroupID;					//���ֲ�ID
	WORD							wKindID;					//��Ϸ��ʶ
	WORD							wServerID;					//�����ʶ
	WORD							wTableID;					//���ӱ�ʶ
	WORD							wChairCount;				//��������
};

//I D ��¼
struct CMD_GR_LogonAI
{
	//��¼��Ϣ
	DWORD							dwGroupID;							//�û� I D
	DWORD							dwUserID;							//�û� I D
	WORD							wKindID;							//��������
	WORD							wTableID;							//��������
};

//I D ��¼
struct CMD_GR_LogonAI_Result
{
	//��¼��Ϣ
	byte							cbCode;								//0 ���ɹ���1 �ɹ�
	DWORD							dwGroupID;							//�û� I D
	DWORD							dwUserID;							//�û� I D
	WORD							wKindID;							//��������
	SCORE							lScore;								//AIѫ��
};

//������AI����
struct CMD_SS_G_AI_Update_Table_Count
{
	DWORD							dwGroupID;					//���ֲ�ID
	WORD							wKindID;					//��Ϸ��ʶ
	WORD							wServerID;					//�����ʶ
	int								nTableChange;				//���ӱ仯
	CMD_GR_AICreateTable			CreateTable;				//�淨
};


//������״̬����
struct CMD_SS_G_Update_Group_Status
{
	DWORD							dwGroupID;					//���ֲ�ID
	DWORD							dwUserID;					//��ұ�ʶ
	byte							cbStatus;					//���״̬
};


//I D ��¼
struct CMD_WS_AI_PARAMETER
{
	DWORD							dwGroupID;							//�û� I D
	WORD							wKindID;							//��������
};


//���¼��ش����˲���
struct CMD_SS_G_ReloadGroupMember
{
	DWORD							dwGroupID;					//���ֲ�ID
};

//�������淨����
struct CMD_SS_G_UpdateGroupGameRule
{
	byte							cbStatus;					//0 ɾ�� 1 ��� 2 �޸�
	tagGroupConfigInfo				GroupConfigInfo;			//�淨����
};

struct tagDeleteGroupUser
{
	byte  cbOperateID;	//��������	1 ���� 2 �˳����ֲ�
	DWORD dwGroupID;	//���ֲ�ID
	DWORD dwUserID;		//�û�ID
	WORD  wKindID;		//��Ϸ����
};

//Ȧ�ѷ����仯
struct CMD_SS_S_UpdateClubUserScore
{
	WORD							wKindID;							//��Ϸ����
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwGroupID;							//���ֲ�ID
	DWORD							dwUserID;							//�����û�
	DWORD							wChairID;							//����λ��
	SCORE							lScore;								//����
};

#pragma pack()

#endif
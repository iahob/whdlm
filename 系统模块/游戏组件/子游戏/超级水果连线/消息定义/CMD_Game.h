#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)
#include "../���������/GameControlDefine.h"
//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						522									//��Ϸ I D
#define GAME_NAME					TEXT("����ˮ������")				//��Ϸ����

//�������
#define GAME_PLAYER					1									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

//��Ŀ����
#define IMAGE_COUNT					11									//���ͼ������
#define ITEM_X_COUNT				5									//������ͼ������
#define ITEM_Y_COUNT				3									//������ͼ������
#define MAX_LINE_COUNT				25									//�������
#define MAX_BET_COUNT				10									//�����ע����
#define INVALID_ITEM				255									//��Ч����
#define MAX_SUPRABBIT_INDEX			8									//���������н�������0~7��

//ͼ���߼�����
#define	CT_ORANGE					0									//����
#define	CT_APPLE					1									//ƻ��
#define	CT_BANANA					2									//�㽶
#define	CT_LEMON					3									//����
#define	CT_STRAWBERRY				4									//��ݮ
#define	CT_WATERMELON				5									//����
#define	CT_STAR						6									//����
#define	CT_BAR						7									//BAR
#define	CT_COMPOTE					8									//���̣���ӣ�
#define	CT_ICECREAM					9 									//ʥ��
#define	CT_SEVEN					10									//SEVEN

#ifndef SCOREEX
#define SCOREEX SCORE
#endif
#ifndef SCOREEX_STRING
#define SCOREEX_STRING TEXT("%I64d")
#endif
//����
inline bool D_GreaterThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 > dVal2;
}
//С��
inline bool D_LessThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 < dVal2;
}
//С�ڻ����
inline bool D_LessThenEquals(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 <= dVal2;
}
inline bool D_UnGreaterThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return D_LessThenEquals(dVal1, dVal2);
}
//���ڻ����
inline bool D_GreaterThenEquals(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 >= dVal2;
}
inline bool D_UnLessThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return D_GreaterThenEquals(dVal1, dVal2);
}

#define maxex(a,b)    (D_GreaterThen(a, b) ? (a) : (b))
#define minex(a,b)    (D_LessThen(a, b) ? (a) : (b))

//�û���Ϊ
typedef enum { USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT } EM_USER_ACTION;

//�������ͣ����ȼ�������>����>ϵͳ���ԣ�
typedef enum
{
	USER_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

//�����û���Ϣ
typedef struct
{
	WORD							wTableID;							//����ID
	DWORD							dwGameID;							//GAMEID
	bool							bAndroid;							//AI��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbUserStatus;						//�û�״̬
	BYTE							cbGameStatus;						//��Ϸ״̬
	SCOREEX							dGameScore;							//��Ϸ��Ӯ��

	//���˵��Բ���
	bool							bDebugIndividual;					//���Ը��˱�ʶ	
	SCOREEX							dIndividualInitialCtrlSysStorage;	//���˸��˵���ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							dIndividualInitialCtrlPlayerStorage;//���˸��˵�����ҿ��(ϵͳҪӮ��Ǯ)
	SCOREEX							dIndividualCtrlSysStorage;			//���˸��˵���ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							dIndividualCtrlPlayerStorage;		//���˸��˵�����ҿ��(ϵͳҪӮ��Ǯ)
	LONGLONG						lIndividualCtrlParameterK;			//���˸��˵��Ե���ϵ��(�ٷֱ�):
} tagRoomUserInfo;

//������ԣ�Ψһ��DWORD��Ӧһ�����ԣ�
typedef struct
{
	//������Բ���
	DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
	SCOREEX							dRoomCtrlInitialSysStorage;			//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							dRoomCtrlInitialPlayerStorage;		//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
	SCOREEX							dRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							dRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
	LONGLONG						lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
	bool							bRoomCtrlValid;						//���������Ч��־
}ROOMCTRL;

//��ҵ��Խṹ
struct tagUserDebug
{
	BYTE							cbLineCount;						//Ѻ������
	BYTE							cbBetIndex;							//Ѻ����ע
	SCOREEX							dBetScore;							//Ѻ����ע
	int								nGiveAwayCount;						//���ʹ���
};

//���ýṹ
struct tagCustomRule
{
	//���
	SCOREEX							dConfigSysStorage;					//ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							dConfigPlayerStorage;				//��ҿ��(���ҪӮ��Ǯ)
	LONGLONG						lConfigParameterK;					//����ϵ��(�ٷֱ�):

	//�ʽ���ʾ
	SCOREEX							dConfigSysWinLostVal;				//ϵͳ��Ӯֵ
	SCOREEX							lMosaicGoldShowPercent;				//�ʽ���ʾ����
	SCOREEX							dDispatchMosaicGoldCon;				//�ɲ����
};

//////////////////////////////////////////////////////////////////////////////////
//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//����
#define GAME_SCENE_WIAT				GAME_STATUS_PLAY					//�ȴ�
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//��Ϸ��

//����״̬
struct CMD_S_StatusFree
{
    //��Ϸ����
	SCOREEX							lCellScore;							//��������

    //��עֵ
    BYTE							cbBetCount;							//��ע����
	SCOREEX							dBetScore[MAX_BET_COUNT];			//��ע��С

    //����ʽ�
	SCOREEX							dLotteryScore;						//����ʽ�
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
    //��Ϸ����
	LONGLONG						lCellScore;							//��������

    //��עֵ
    BYTE							cbBetCount;							//��ע����
	LONGLONG						dBetScore[MAX_BET_COUNT];			//��ע��С

    BYTE							cbLineCount;						//Ѻ������
    BYTE							cbBetIndex;							//Ѻ������
	SCOREEX							dLineScore[MAX_LINE_COUNT];			//�ߵ÷�
	BYTE							cbItemType[MAX_LINE_COUNT];			//������
	int								nSuperRabbit;						//̰�����ӵı���(0������̰������)
	SCOREEX							dSuperRabbitScore;					//��Ϸ��̰������,�ܵ÷�ΪѺ�߻�ý����X ̰������Ϸ�н�����
    int								nGiveAwayCount;						//���ʹ���
	SCOREEX							dTotalGameScore;					//��Ϸ�ܷ�

    //����ʽ�
	SCOREEX							dLotteryScore;						//����ʽ�
};

//////////////////////////////////////////////////////////////////////////////////
//�����
#define SUB_S_GAME_START				100									//Ѻ�߿�ʼ
#define SUB_S_GAME_CONCLUDE				101									//Ѻ�߽���
#define SUB_S_GAME_LOTTERY				102									//����ʽ�
#define SUB_S_SET_BASESCORE				104									//���û���
#define SUB_S_SUPERRABBIT_START			105									//�������ӿ�ʼ
#define SUB_S_SUPERRABBIT_END			106									//�������ӽ���

//��Ϸ��ʼ
struct CMD_S_GameStart
{
    BYTE								cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];			//������Ϣ
	SCOREEX								dLineScore[MAX_LINE_COUNT];						//��Ϸ����
	BYTE								cbItemType[MAX_LINE_COUNT];						//������
	int									nSuperRabbit;									//̰�����ӵı���(0������̰������)
	SCOREEX								dSuperRabbitScore;								//��Ϸ��̰������,�ܵ÷�ΪѺ�߻�ý����X ̰������Ϸ�н�����
    int									nGiveAwayCount;									//���ʹ���
};

//����ʽ�
struct CMD_S_Lottery
{
	SCOREEX								dLotteryScore;									//�ʽ�(ǰ���ۼ�ֵ)
};

//�������ӿ�ʼ
struct CMD_S_SupperRabbitStart
{
	int									nSuperRabbit;									//̰�����ӵı���(0������̰������)
	SCOREEX								dSuperRabbitScore;								//��Ϸ��̰������,�ܵ÷�ΪѺ�߻�ý����X ̰������Ϸ�н�����
};

//�������ӽ���
struct CMD_S_SupperRabbitEnd
{
	int									nCurSuperRabbit;								//̰�����ӵı���(0������̰������)
	SCOREEX								dCurSuperRabbitScore;							//��Ϸ��̰������,�ܵ÷�ΪѺ�߻�ý����X ̰������Ϸ�н�����
};

//////////////////////////////////////////////////////////////////////////////////
//�����
#define SUB_C_GAME_START			100									//Ѻ�߿�ʼ
#define SUB_C_GIVE_AWAY_START		101									//Ѻ�߿�ʼ
#define SUB_C_GAME_CONCLUDE			102									//����Ѻ��
#define SUB_C_SUPERRABBIT_START		103									//�������ӿ�ʼ��û����Ϣ����
#define SUB_C_SUPERRABBIT_END		104									//�������ӽ���

//�û��з�
struct CMD_C_GameStart
{
    BYTE							cbLineCount;							//Ѻ������
    BYTE							cbBetIndex;								//Ѻ������
};

//�������ӽ���
struct CMD_C_SuperRabbitEnd
{
	int								nCurSuperRabbit;						//��ǰ̰�����ӵı���
};

#pragma pack()

#endif
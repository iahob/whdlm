#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

// 1�ֽڶ���
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
// ������

// ��Ϸ����
#define KIND_ID						520									// ��Ϸ I D
#define GAME_NAME					TEXT("��������")					// ��Ϸ����

// �������
#define GAME_PLAYER					1									// ��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				// ����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				// ����汾

//////////////////////////////////////////////////////////////////////////////////
// ״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					// �ȴ���ʼ
#define GAME_SCENE_SLOW             GAME_SCENE_SLOW                     // ת������
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					// ��Ϸ����

//////////////////////////////////////////////////////////////////////////////////

// ������Ϣ
#define IDM_START_GAME				(WM_USER + 200)						// ��ʼ��Ϣ
#define IDM_STOP_GAME				(WM_USER + 201)						// ��ʼ��Ϣ
#define IDM_TURN_OVER				(WM_USER + 202)						// ��ת����
#define IDM_START_HOSTED			(WM_USER + 203)						// ��ʼ�й�
#define IDM_STOP_HOSTED				(WM_USER + 204)						// �����й�
#define IDM_SCORE_LESS				(WM_USER + 205)						// ��������
#define IDM_SCORE_ADD				(WM_USER + 206)						// ��������
#define IDM_SCORE_MAX				(WM_USER + 207)						// �������
#define IDM_LINE_CHECK				(WM_USER + 208)						// ��������
#define IDM_OPEN_CONTROL			(WM_USER + 1023)					// �򿪿���

// ����
#define	MAX_VOLUME					(3000)

// ��Ļ����
#define SCREEN_CY					738									// ��С�߶�
#define SCREEN_CX					1024								// ��С���

//////////////////////////////////////////////////////////////////////////////////
// ��Ϣ����

// ��¼��Ϣ
enum EnumRecord
{
	Record_ThreeTotal	= 0,			// �ۼ�������
	Record_NiceTotal	= 1,			// �ۼƾ�����
	Record_GrandTotal	= 2,			// ��Ӯ��
	Record_WinMaxl		= 3,			// �������
};

// ����Ԫ��
enum EnumTextureElement
{
	TextureElement_A,
	TextureElement_B,
	TextureElement_C,
	TextureElement_D,
	TextureElement_E,
	TextureElement_F,
	TextureElement_G,
	TextureElement_H,
	TextureElement_I,
	TextureElement_Deform,
	TextureElement_BCD,
	TextureElement_Null,
	TextureElement_Any,
	TextureElement_Max
};

// ����Ԫ��
enum EnumTextureElementType
{
	TextureElementType_Small,
	TextureElementType_Medium,
	TextureElementType_Big,
	TextureElementType_Max,
};

// Ԫ�ض���
enum EnumElement
{
	Element_A =					0,
	Element_B =					1,
	Element_C =					2,
	Element_D =					3,
	Element_E =					4,
	Element_F =					5,
	Element_G =					6,
	Element_H =					7,
	Element_I =					8,
	Element_Normal_Max =		9,									// �����������		
	Element_Deform =			9,									// ����Ԫ��
	Element_Max =				10,									// ���Ԫ������			
	Element_Invalid =			-1,									// ��ЧԪ��	
};

// ��Ч����	
#define MULTIPLE_INVALID		-1									

// ������ʾ
enum EnumMultipleType
{
	MultipleType_Three,
	MultipleType_Nine,
	MultipleType_Max,
};

// 3������
enum EnumMultipleThree
{
	MultipleThree_AAA = 		0,
	MultipleThree_BBB =			1,
	MultipleThree_CCC =			2,
	MultipleThree_DDD =			3,
	MultipleThree_EEE =			4,
	MultipleThree_FFF =			5,
	MultipleThree_GGG =			6,
	MultipleThree_BCD =			7,
	MultipleThree_HHH =			8,
	MultipleThree_III =			9,
	MultipleThree_II =			10,
	MultipleThree_I =			11,
	MultipleThree_Max =			12,									// 3������
	MultipleThree_Invalid =		MULTIPLE_INVALID,					// ��Ч����	
};


// 9������
enum EnumMultipleNine
{
	MultipleNine_A =			0,
	MultipleNine_B =			1,
	MultipleNine_C =			2,
	MultipleNine_D =			3,
	MultipleNine_E =			4,
	MultipleNine_F =			5,
	MultipleNine_G =			6,
	MultipleNine_H =			7,
	MultipleNine_I =			8,
	MultipleNine_Max =			9,									// 9������
	MultipleNine_Invalid =		MULTIPLE_INVALID,					// ��Ч����	
};


// ��������
#define LINE_WIDTH_1				0
#define LINE_WIDTH_2				1
#define LINE_WIDTH_3				2
#define LINE_HEIGHT_4				3
#define LINE_HEIGHT_5				4
#define LINE_HEIGHT_6				5
#define LINE_OBLIQUE_7				6
#define LINE_OBLIQUE_8				7
#define LINE_INDEX_MAX				8

// ������ϸ
//#define LINE_WIDTH_1_1				0][0
//#define LINE_WIDTH_1_2				1][0
//#define LINE_WIDTH_1_3				2][0
//
//#define LINE_WIDTH_2_1				0][1
//#define LINE_WIDTH_2_2				1][1
//#define LINE_WIDTH_2_3				2][1
//
//#define LINE_WIDTH_3_1				0][2
//#define LINE_WIDTH_3_2				1][2
//#define LINE_WIDTH_3_3				2][2
//
//#define LINE_HEIGHT_4_1				0][0
//#define LINE_HEIGHT_4_2				0][1
//#define LINE_HEIGHT_4_3				0][2
//
//#define LINE_HEIGHT_5_1				1][0
//#define LINE_HEIGHT_5_2				1][1
//#define LINE_HEIGHT_5_3				1][2
//
//#define LINE_HEIGHT_6_1				2][0
//#define LINE_HEIGHT_6_2				2][1
//#define LINE_HEIGHT_6_3				2][2
//
//#define LINE_OBLIQUE_7_1			2][0
//#define LINE_OBLIQUE_7_2			1][1
//#define LINE_OBLIQUE_7_3			0][2
//
//#define LINE_OBLIQUE_8_1			2][2
//#define LINE_OBLIQUE_8_2			1][1
//#define LINE_OBLIQUE_8_3			0][0

// ������Ϣ
#define	TASK_INDEX_MAX				3

// ת������
#define TURN_WIDTH_COUNT			3									// ת��������
#define TURN_HEIGHT_COUNT			3									// ת��������

// ��ע��Χ
#define BET_INDEX_MAX				10

// ·����Ϣ
#define ROAD_MULTIPLE_MAX			10

//////////////////////////////////////////////////////////////////////////////////

// ��Ϸ״̬
//struct CMD_S_Status
//{
//	int								nTaskProgress[TASK_INDEX_MAX];				// �������
//	int								nTaskTarget[TASK_INDEX_MAX];				// ����Ŀ��
//	int								nTaskCount[TASK_INDEX_MAX];					// ��������
//	int								nTaskScore[TASK_INDEX_MAX];					// ������
//	int								nThreeLineMultiple[MultipleThree_Max];		// 3������
//	int								nNineLineMultiple[MultipleNine_Max];		// 9������
//	int								nBetScore[BET_INDEX_MAX];					// ��ע����
//	int								nRoadMultipleIndex[ROAD_MULTIPLE_MAX];		// ·����Ϣ
//	__int64							nRoomLotteryPool;							// ����ʳ�
//	unsigned __int64				nThreeTotal;								// �����н�
//	unsigned __int64				nNiceTotal;									// �����н�
//	unsigned __int64				nGrandTotal;								// �н��ۼ�
//	unsigned __int64				nWinMax;									// ����н�
//};


//////////////////////////////////////////////////////////////////////////////////
// ����������ṹ
#define SUB_S_ELEMENT_STOP					101						// Ԫ����Ϣ
#define SUB_S_LOTTERY_POOL					102						// �ʳ�����
#define SUB_S_TASK_INFO						103						// ������Ϣ
#define SUB_S_AWARD_INFO					104						// �н���Ϣ
//#define SUB_S_CONTROL						105						// ������Ϣ

// Ԫ��ֹͣ
struct CMD_S_ElementStop
{
	__int64						lPlayWinScore;											// ���Ӯ����
	int							nPlayTaskScore;											// �������
	int							nMultipleIndex;											// �н���Ϣ
	bool						bMagnification;											// �󱶷���
	bool						bStopNew;												// ����ֹͣ
	bool						bTaskComplete[TASK_INDEX_MAX];							// �������
	bool						bPlayWinLine[LINE_INDEX_MAX];							// Ӯ����
	int							nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// Ԫ����Ϣ
};

// �ʳ�����
//struct CMD_S_LotteryPool
//{
//	__int64						nRoomLotteryPool;										// �ʳ�����
//};

// ������Ϣ
struct CMD_S_TaskInfo
{
	int							nTaskProgress[TASK_INDEX_MAX];							// �������
};

// �н���Ϣ
struct CMD_S_AwardInfo
{
	TCHAR						szPlayName[40];											// �������
	int							nTableID;												// ����ID
	int							nMultipleIndex;											// �н���Ϣ
	__int64						nAwardScore;											// �н�����
};

// �ṹ��Ϣ
//struct CMD_S_Control
//{
//	unsigned int				nMessageID;
//	unsigned int				nMessageSize;
//	byte						cbMessageInfo[512];
//};


//////////////////////////////////////////////////////////////////////////////////
// �ͻ�������ṹ
#define SUB_C_START							101							// ��ʼ��Ϣ
#define SUB_C_STOP							102							// ������Ϣ
#define SUB_C_CONTROL						103							// ������Ϣ

// ��ʼ��Ϣ
struct CMD_C_Start
{
	int							nBetScore;								// ��ע����
	bool						nBetLine[LINE_INDEX_MAX];				// ��ע����
};

// ֹͣ��Ϣ
struct CMD_C_Stop
{
};

// �ṹ��Ϣ
struct CMD_C_Control
{
	unsigned int					nMessageID;
	unsigned int					nMessageSize;
	byte							cbMessageInfo[512];
};

// ��ԭ������
#pragma pack()

#endif


#ifndef CMD_LOG_SERVER_HEAD_FILE
#define CMD_LOG_SERVER_HEAD_FILE

#pragma pack(1)

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//������Ϣ

#define MDM_S_S_LOG_INFO			1						//������Ϣ

//������Ϣ
#define SUB_CS_C_SERVER_LOG		1							//��������

#define CORRESPOND_SERVER		0							//Э��������
#define PERSONAL_SERVER			1							//Լս������
#define LOGON_SERVER			2							//��¼������
#define GAME_SERVER				3							//��Ϸ������
#define CHAT_SERVER				4							//���������
#define GROUP_SERVER			5							//���루���ֲ���������

#define POINT_WRITE				0							//��ʱд��־
#define IMMEDIATE_WRITE			1							//��ʱд��־

//��־���
#define LOG_CONTENT_LEN			256							//��־���ݳ���

#define POINT_WRITE				0							//��ʱд��־
#define IMMEDIATE_WRITE			1							//��ʱд��־
#define CLOSE_LOG				2							//�ر���־
#define OPEN_LOG				3							//������־
#define CLOSE_LOG_EX			0							//�ر���־
#define OPEN_LOG_EX				1							//������־

//�߳̿�ʼ�������־
#define THREAD_CLOSE			1							//�����߳�
#define THREAD_CONTINUE			0							//�����߳�	
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ��������־ͨ�����ݽṹ
struct tagLogUserInfo
{
	BYTE							cbServerSign;					//0 Э���������� 1 Լս�������� 2 ��¼�������� 3 ��Ϸ�������� 4 ����������� 5 ���������
	TCHAR							szServerName[LEN_SERVER];		//����������(����������)
	SYSTEMTIME						sysTime;						//��־����ʱ��
	TCHAR							szLogContent[LOG_CONTENT_LEN];	//��־����
	WORD							wServerID;						//��������ʶ
	WORD							wKindID;						//��Ϸ����
	DWORD							dwUserID;						//�û�UserID
	WORD							wTableID;						//��������
	WORD							wChairID;						//��������
	TCHAR							szNickName[LEN_NICKNAME];		//�û��ǳ�
	TCHAR							szGameName[LEN_KIND];			//��Ϸ����
	TCHAR							szDescName[LEN_SERVER];			//�����ļ�����(���������־�������־��������־��Ĭ��ͨ����־
};


//������������־ͨ�����ݽṹ
struct tagOtherServerLogInfo
{
	BYTE							cbServerSign;					//0 Э���������� 1 Լս�������� 2 ��¼�������� 3 ��Ϸ�������� 4 ����������� 5 ���������
	TCHAR							szServerName[LEN_SERVER];		//����������(����������)
	SYSTEMTIME						sysTime;						//��־����ʱ��
	TCHAR							szLogContent[LOG_CONTENT_LEN];	//��־����
};

#pragma pack()

#endif
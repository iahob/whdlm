#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//�¼����
#define ER_FAILURE					0									//ʧ�ܱ�־
#define ER_SUCCESS					1									//�ɹ���־

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

#define UI_CORRESPOND_RESULT		1									//Э�����
#define UI_BATTLE_RESULT			2									//Լս���
#define UI_LOAD_GROUP_OPTION		3									//��������

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

#define CT_CONNECT_CORRESPOND		100									//����Э��
#define CT_CONNECT_BATTLE			101									//����Լս
#define CT_LOAD_GROUP_OPTION		102									//Ⱥ������
#define CT_LOAD_GROUP_LIST			103									//����Ⱥ��
#define CT_CONNECT_LOG_SERVER		104									//������־������
#define CT_CONNECT_AI_SERVER		105									//����AI������
//////////////////////////////////////////////////////////////////////////

//���ƽ��
struct CP_ControlResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//////////////////////////////////////////////////////////////////////////

#endif
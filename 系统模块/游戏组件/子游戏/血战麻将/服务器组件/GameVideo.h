#pragma once

//��Ϸ��Ƶ����
class IGameVideo
{
public:
	IGameVideo(void){};
	virtual ~IGameVideo(void){};

public:
	//��ʼ¼��
	virtual bool __cdecl	StartVideo(ITableFrame	*pTableFrame, BYTE cbPlayerCount) = NULL;
	//ֹͣ�ͱ���
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID,WORD wTableID)	= NULL;
	//����¼������
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,Video_GameStart *pVideoGameStart,bool first) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, CMD_S_CallCard *pVideoCallCard) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, CMD_S_BankerInfo *pVideoBankerInfo, bool first) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_OutCard *pVideoOutCard) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_SendCard *pVideoSendCard) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_OperateResult *pVideoOperateResult) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_GameEnd *pVideoGameEnd) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_ChiHu *pVideoChiHu) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_ChangeCardResult *pVideoChangeCardResult) = NULL;
};

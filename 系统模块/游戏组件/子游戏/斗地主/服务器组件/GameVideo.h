#pragma once

//��Ϸ��Ƶ����
class IGameVideo
{
public:
	IGameVideo(void){};
	virtual ~IGameVideo(void){};

public:
	//��ʼ¼��
	virtual bool __cdecl	StartVideo(ITableFrame	*pTableFrame)		= NULL;
	//ֹͣ�ͱ���
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID,WORD wTableID)	= NULL;
	//����¼������
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,Video_GameStart *pVideoGameStart,bool first)		= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_CallScore *pVideoCall)				= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_BankerInfo *pVideoBanker)			= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_StatusPlay *pVideoPlay)			= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_OutCard *pVideoOutCard)			= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_PassCard *pVideoPassCard)			= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_GameConclude *pVideoConclude)		= NULL;	
};

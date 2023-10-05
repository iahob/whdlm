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
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID,WORD wTableID, BYTE cbPlayCount)	= NULL;
	//����¼������
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_SendCard *pVideoSendCard, bool bFirst)	= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_ShowCard *pVideoShowCard)					= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_CompareCard *pVideoCompareCard)			= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_StatusPlay *pVideoPlay)					= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_PlayerExit *pVideoPlayerExit)				= NULL;	
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_GameEnd *pVideoGameEnd)					= NULL;	
};

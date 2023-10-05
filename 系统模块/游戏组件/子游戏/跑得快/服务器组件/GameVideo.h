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
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_GameStart *pVideoGameStart, bool bFirst) = NULL;
	//virtual bool __cdecl    AddVideoData(WORD wMsgKind, CMD_S_GameStart *pVideoGameStart) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, CMD_S_OutCard *pVideoOutCard) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, CMD_S_PassCard *pVideoPassCard) = NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, CMD_S_GameEnd *pVideoGameEnd) = NULL;
};

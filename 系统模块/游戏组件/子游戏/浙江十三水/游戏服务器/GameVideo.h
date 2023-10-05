#pragma once

//游戏视频基类
class IGameVideo
{
public:
	IGameVideo(void){};
	virtual ~IGameVideo(void){};

public:
	//开始录像
	virtual bool __cdecl	StartVideo(ITableFrame	*pTableFrame)		= NULL;
	//停止和保存
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID,WORD wTableID, BYTE cbPlayCount)	= NULL;
	//增加录像数据
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, Video_SendCard *pVideoSendCard, bool bFirst)	= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_ShowCard *pVideoShowCard)					= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_CompareCard *pVideoCompareCard)			= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_StatusPlay *pVideoPlay)					= NULL;
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_PlayerExit *pVideoPlayerExit)				= NULL;	
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,CMD_S_GameEnd *pVideoGameEnd)					= NULL;	
};

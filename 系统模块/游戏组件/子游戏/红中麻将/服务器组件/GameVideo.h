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
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID,WORD wTableID,BYTE cbPlayerCount)	= NULL;
	//����¼������
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,void* pPack,int nSize,bool bFirst)=NULL;

};

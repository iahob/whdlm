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
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID, WORD wTableID, BYTE cbPlayCount) = NULL;
	//����¼������
	virtual bool __cdecl    AddVideoData(WORD wMsgKind, VOID *pData, size_t sDataSize, bool bFirst) = NULL;

};

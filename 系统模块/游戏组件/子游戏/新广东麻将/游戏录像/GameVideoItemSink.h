#pragma once
#include "..\���������\GameVideo.h"

class CGameVideoItemSink : public IGameVideo
{
public:
	CGameVideoItemSink(void);
	virtual ~CGameVideoItemSink(void);

public:		
	//��ʼ¼��
	virtual bool __cdecl	StartVideo(ITableFrame	*pTableFrame);
	//ֹͣ�ͱ���
	virtual bool __cdecl	StopAndSaveVideo(WORD wServerID,WORD wTableID);
	//����¼������
	virtual bool __cdecl    AddVideoData(WORD wMsgKind,void* pPack,int nSize,bool bFirst);
protected:
	void					ResetVideoItem();
	bool					RectifyBuffer(size_t iSize);	
	VOID					BuildVideoNumber(CHAR szVideoNumber[], WORD wLen,WORD wServerID,WORD wTableID);

	size_t					Write(const void* data, size_t size);
	size_t					WriteUint8(UINT8 val)   { return Write(&val, sizeof(val)); }
	size_t					WriteUint16(UINT16 val) { return Write(&val, sizeof(val)); }
	size_t					WriteUint32(UINT32 val) { return Write(&val, sizeof(val)); }
	size_t					WriteUint64(UINT64 val) { return Write(&val, sizeof(val)); }
	size_t					WriteInt8(INT8 val)		{ return Write(&val, sizeof(val)); }
	size_t					WriteInt16(INT16 val)	{ return Write(&val, sizeof(val)); }
	size_t					WriteInt32(INT32 val)	{ return Write(&val, sizeof(val)); }
	size_t					WriteInt64(INT64 val)	{ return Write(&val, sizeof(val)); }	

	//���ݱ���
private:
	ITableFrame	*					m_pITableFrame;						//��ܽӿ�
	size_t							m_iCurPos;							//����λ��	
	size_t							m_iBufferSize;						//���峤��
	LPBYTE							m_pVideoDataBuffer;					//����ָ��
};

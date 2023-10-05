#ifndef AI_CONFIG_DEBUG_HEAD_FILE
#define AI_CONFIG_DEBUG_HEAD_FILE
#pragma once

// ���ƻص�
interface IAIConfigDebugCallback
{
	// ������Ϣ
	virtual bool OnAIConfigDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;
};

// �ͻ��˽ӿ�
interface IAIConfigDebug
{
	// �ͷŽӿ�
	virtual void Release() = NULL;
	// ��������
	virtual bool Create(CWnd * pParentWnd, IAIConfigDebugCallback * pIAIConfigDebugCallback) = NULL;
	// ��ʾ����
	virtual bool ShowWindow(bool bShow) = NULL;
	// ��Ϣ����
	virtual bool OnAIConfigDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#endif

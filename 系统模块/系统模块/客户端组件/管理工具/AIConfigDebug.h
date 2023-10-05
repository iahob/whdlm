#ifndef AI_CONFIG_DEBUG_HEAD_FILE
#define AI_CONFIG_DEBUG_HEAD_FILE
#pragma once

// 控制回调
interface IAIConfigDebugCallback
{
	// 控制信息
	virtual bool OnAIConfigDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;
};

// 客户端接口
interface IAIConfigDebug
{
	// 释放接口
	virtual void Release() = NULL;
	// 创建函数
	virtual bool Create(CWnd * pParentWnd, IAIConfigDebugCallback * pIAIConfigDebugCallback) = NULL;
	// 显示窗口
	virtual bool ShowWindow(bool bShow) = NULL;
	// 消息函数
	virtual bool OnAIConfigDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#endif

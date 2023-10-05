#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//////////////////////////////////////////////////////////////////////////////////

//���ͷ�ļ�
#include "..\..\..\SDKCondition.h"

#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//��������
#include "..\..\..\������\Include\GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���
//
#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../������/lib/Ansi/KernelEngine.lib")
#else
#pragma comment (lib,"../../../������/lib/Unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../������/lib/Ansi/KernelEngineD.lib")
#else
#pragma comment (lib,"../../../������/lib/Unicode/KernelEngineD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

#include "..\..\..\..\ϵͳģ��\���������\��Ϸ����\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/KernelEngine.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/KernelEngineD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngineD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif


// ��Ϣͷ
#include "..\��Ϣ����\CMD_Game.h"
#include "..\��Ϣ����\DebugHead.h"

#ifndef _UNICODE
	#error "������֧�ֶ��ֽڰ汾������ʹ��Unicode�汾���롣"
#endif
	
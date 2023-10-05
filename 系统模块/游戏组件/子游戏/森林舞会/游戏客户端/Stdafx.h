// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// ��Ŀ�ض��İ����ļ�

#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ��ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0601		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0601		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0601 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0601	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ��������������ȫ���Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <AtlBase.h>
#include <AtlDbcli.h>
#include <AfxDtctl.h>

//////////////////////////////////////////////////////////////////////////
// irrlicht����
#include "..\irrlicht-1.7.2\include\irrlicht.h"

//#include ".\log\SDBase.h"
//using namespace SGDP;
//////////////////////////////////////////////////////////////////////////
//���뻷��
//#include "..\..\..\SDKCondition.h"
//////////////////////////////////////////////////////////////////////////////////
#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//��������

#include "..\..\..\������\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../������/Lib/Ansi/WHImage.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ServiceCore.lib")
#else
#pragma comment (lib,"../../../������/Lib/Unicode/WHImage.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../������/Lib/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/ServiceCoreD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

#include "..\..\..\..\ϵͳģ��\�ͻ������\��Ϸ���\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ShareControl.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/ServiceCore.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Release_Unicode/ShareControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ShareControlD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ShareControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif

#define  __TEST__  1
#define  __BANKER___
#define  __SPECIAL___

//////////////////////////////////////////////////////////////////////////

#include "..\��Ϣ����\CMD_Baccarat.h"
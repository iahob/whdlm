// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows 95 �� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define WINVER 0x0400		// ���˸���Ϊ����� Windows 98 �� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0400	// ���˸���Ϊ����� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ���˸���Ϊ����� Windows Me ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 4.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0400	// ���˸���Ϊ����� IE 5.0 ����߰汾�ĺ��ʵ�ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯����Ϊ��ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//#define __SPECIAL___
//���뻷��
#include "..\..\..\..\ϵͳģ��\SDKCondition.h"

//////////////////////////////////////////////////////////////////////////////////


#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//��������

#include "..\..\..\..\ϵͳģ��\������\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/ServiceCore.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/������/Lib/Unicode/ServiceCoreD.lib")
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
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCoreD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif
//���ͷ�ļ�
#include "resource.h"
#include "..\��Ϣ����\cmd_twoeightbattle.h"

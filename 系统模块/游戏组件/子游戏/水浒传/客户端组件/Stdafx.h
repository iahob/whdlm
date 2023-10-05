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
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

////////////////////////////////////////////////////////////////////////////////// 

// MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <AtlBase.h>
#include <AtlDbcli.h>
#include <AfxDtctl.h>

////////////////////////////////////////////////////////////////////////////////// 

#include "..\..\..\SDKCondition.h"
#include "GameOption.h"

#ifdef SDK_CONDITION

////////////////////////////////////////////////////////////////////////////////// 
// 开发环境

#include "..\..\..\开发库\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ShareControl.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ServiceCore.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ShareControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ShareControlD.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ShareControlD.lib")
#endif
#endif

////////////////////////////////////////////////////////////////////////////////// 

#else

////////////////////////////////////////////////////////////////////////////////// 
// 系统环境

#include "..\..\..\..\系统模块\客户端组件\游戏框架\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/WHImage.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/ShareControl.lib")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/WHImage.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ServiceCore.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ShareControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/ShareControlD.lib")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ShareControlD.lib")
#endif
#endif

////////////////////////////////////////////////////////////////////////////////// 

#endif
////////////////////////////////////////////////////////////////////////////////// 

#include "..\DexterSDK\Include\DexterD3DHead.h"
#include "..\消息定义\ControlHead.h"
#include "..\消息定义\CMD_Game.h"
#include "Global.h"
#include "GameLogic.h"
#include "Sound.h"
#include "Windows.h"

#ifdef _DEBUG
	#pragma comment (lib,"../DexterSDK/Lib/DexterD3D_D.lib")
#else
	#pragma comment (lib,"../DexterSDK/Lib/DexterD3D.lib")
#endif


#ifndef _UNICODE
#error "The program must be compiled Unicode."
#endif

#define				WM_TIP_CLOSE			WM_USER+1					// 提示窗口关闭
#define				WM_HELP_CLOSE			WM_USER+2					// 帮助窗口关闭
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件

#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0601		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0601		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0601 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0601	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <AtlBase.h>
#include <AtlDbcli.h>
#include <AfxDtctl.h>

//////////////////////////////////////////////////////////////////////////
// irrlicht引擎
#include "..\irrlicht-1.7.2\include\irrlicht.h"

//#include ".\log\SDBase.h"
//using namespace SGDP;
//////////////////////////////////////////////////////////////////////////
//编译环境
//#include "..\..\..\SDKCondition.h"
//////////////////////////////////////////////////////////////////////////////////
#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//开发环境

#include "..\..\..\开发库\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCore.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/ServiceCoreD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//系统环境

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
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/WHImage.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/ServiceCore.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Release_Unicode/ShareControl.lib")
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

#define  __TEST__  1
#define  __BANKER___
#define  __SPECIAL___

//////////////////////////////////////////////////////////////////////////

#include "..\消息定义\CMD_Baccarat.h"
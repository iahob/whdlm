// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
//////////////////////////////////////////////////////////////////////////////////
//MFC 文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#define _AFX_ALL_WARNINGS						// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏

#include <AfxWin.h>				// MFC 核心组件和标准组件
#include <AfxExt.h>				// MFC 扩展
#include <AfxDisp.h>			// MFC 自动化类
#include <AfxDtctl.h>			// MFC 对 Internet Explorer 4 公共控件的支持
#include <AfxCmn.h>				// MFC 对 Windows 公共控件的支持

//////////////////////////////////////////////////////////////////////////////////

//组件头文件
#include "resource.h"
#include "..\消息定义\CMD_Baccarat.h"

//////////////////////////////////////////////////////////////////////////////////

//编译环境
#include "..\..\..\SDKCondition.h"

//////////////////////////////////////////////////////////////////////////////////
#define SDK_CONDITION
#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//开发环境

#include "..\..\..\开发库\Include\GameFrameHead.h"
#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ShareControl.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/FGuilib.lib")
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

int RandInt(int nMin,int nMax);
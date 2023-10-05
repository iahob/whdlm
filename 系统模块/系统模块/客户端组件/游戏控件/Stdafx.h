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

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

#include "..\..\全局定义\Define.h"
#include "..\..\全局定义\GameAIParameter.h"

//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../链接库/Ansi/WHImage.lib")
	#pragma comment (lib,"../../链接库/Ansi/FGuilib.lib")
	#pragma comment (lib,"../../链接库/Ansi/DownLoad.lib")
	#pragma comment (lib,"../../链接库/Ansi/ServiceCore.lib")
	#pragma comment (lib,"../../链接库/Ansi/SkinControl.lib")
	#pragma comment (lib,"../../链接库/Ansi/PlatformData.lib")
	#pragma comment (lib,"../../链接库/Ansi/FGuimfcFactory.lib")
#else
	#pragma comment (lib,"../../链接库/Unicode/WHImage.lib")
	#pragma comment (lib,"../../链接库/Unicode/FGuilib.lib")
	#pragma comment (lib,"../../链接库/Unicode/DownLoad.lib")
	#pragma comment (lib,"../../链接库/Unicode/ServiceCore.lib")
	#pragma comment (lib,"../../链接库/Unicode/SkinControl.lib")
	#pragma comment (lib,"../../链接库/Unicode/PlatformData.lib")
	#pragma comment (lib,"../../链接库/Unicode/FGuimfcFactory.lib")	
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../链接库/Ansi/WHImageD.lib")
	#pragma comment (lib,"../../链接库/Ansi/FGuilibD.lib")
	#pragma comment (lib,"../../链接库/Ansi/DownLoadD.lib")
	#pragma comment (lib,"../../链接库/Ansi/ServiceCoreD.lib")
	#pragma comment (lib,"../../链接库/Ansi/SkinControlD.lib")
	#pragma comment (lib,"../../链接库/Ansi/PlatformDataD.lib")
	#pragma comment (lib,"../../链接库/Ansi/FGuimfcFactoryD.lib")	
#else
	#pragma comment (lib,"../../链接库/Unicode/WHImageD.lib")
	#pragma comment (lib,"../../链接库/Unicode/FGuilibD.lib")
	#pragma comment (lib,"../../链接库/Unicode/DownLoadD.lib")
	#pragma comment (lib,"../../链接库/Unicode/ServiceCoreD.lib")
	#pragma comment (lib,"../../链接库/Unicode/SkinControlD.lib")
	#pragma comment (lib,"../../链接库/Unicode/PlatformDataD.lib")
	#pragma comment (lib,"../../链接库/Unicode/FGuimfcFactoryD.lib")	
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

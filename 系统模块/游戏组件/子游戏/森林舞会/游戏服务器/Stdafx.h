#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC �ļ�

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#define _AFX_ALL_WARNINGS						// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������

#include <AfxWin.h>				// MFC ��������ͱ�׼���
#include <AfxExt.h>				// MFC ��չ
#include <AfxDisp.h>			// MFC �Զ�����
#include <AfxDtctl.h>			// MFC �� Internet Explorer 4 �����ؼ���֧��
#include <AfxCmn.h>				// MFC �� Windows �����ؼ���֧��

//////////////////////////////////////////////////////////////////////////////////

////���ͷ�ļ�
//#include "..\��Ϣ����\CMD_Baccarat.h"
//#include "..\..\..\������\Include\GameServiceExport.h"
#include "string"

#include "SDBase.h"
using namespace SGDP;

//////////////////////////////////////////////////////////////////////////

//ȫ�ֺ���
//////////////////////////////////////////////////////////////////////////
int RandInt(int nMin, int nMax);
const char*  GetDllPath(void);
const char* GetRootPath(void);

//////////////////////////////////////////////////////////////////////////

//���ͷ�ļ�
//���뻷��
#include "SDKCondition.h"

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

//ƽ̨����
#include "..\..\..\..\ϵͳģ��\���������\��Ϸ����\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"��֧��Ansi��ʽ")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"��֧��Ansi��ʽ")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngine.lib")
#endif
#endif

#include "..\��Ϣ����\CMD_Baccarat.h"
#pragma once


#include "stdafx.h"
#include "windows.h"

#include <DbgHelp.h>
#pragma comment(lib, "Imagehlp.lib")
#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI CleanToolExceptionFun(struct _EXCEPTION_POINTERS* pei);

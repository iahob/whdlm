#include "stdafx.h"
#include <stdio.h>
#include "sdsystem.h"

bool SDAPI SGDP::SGSYS_CreateDirectory(const char* pszDir)
{
	char szPath[MAX_PATH];
	lstrcpyn(szPath, pszDir, sizeof(szPath));

	char* p1 = szPath;

	while(*p1)
	{
		if('/' == *p1) *p1 = '\\';
		p1++;
	}

	if( ((szPath[0] & 0x80) == 0) && (szPath[1] == ':') )
	{
		p1 = szPath+2;

		//d:
		if('\0' == *p1)
			return true;

		if('\\' == *p1)
		{
			p1++;
			if('\0' == *p1)
				return true;
		}
	}
	else
	{
		p1 = szPath;
	}
	
	do
	{
		p1 = strchr(p1, '\\');
		if(p1 != NULL)
			*p1 = '\0';
		
		if(FALSE == CreateDirectory(szPath, NULL))
		{
			if(GetLastError() != ERROR_ALREADY_EXISTS)
			{
				return false;
			}
		}
		if(p1 != NULL)
			*p1++ = '\\';
	}while(p1 != NULL);

	return true;
}

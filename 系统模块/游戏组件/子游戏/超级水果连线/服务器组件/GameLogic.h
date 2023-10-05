#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"


static CString GetFileDialogPath()
{
    CString strFileDlgPath;
    TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
    GetModuleFileName(AfxGetInstanceHandle(), szModuleDirectory, sizeof(szModuleDirectory));
    int nModuleLen = lstrlen(szModuleDirectory);
    int nProcessLen = lstrlen(AfxGetApp()->m_pszExeName) + lstrlen(TEXT(".EXE")) + 1;
    if(nModuleLen <= nProcessLen)
    {
        return TEXT("");
    }
    szModuleDirectory[nModuleLen - nProcessLen] = 0;
    strFileDlgPath = szModuleDirectory;
    return strFileDlgPath;
}

static void NcaTextOut(CString strInfo, CString strRoomName)
{
    CreateDirectory(GetFileDialogPath() + TEXT("\\SuperFruitLog"), NULL);

    CTime time = CTime::GetCurrentTime() ;

    CString strName;
    strName.Format(TEXT("%s\\SuperFruitLog\\%s[����]%04d%02d%02d.log"),
                   GetFileDialogPath(),
                   strRoomName,
                   time.GetYear(),
                   time.GetMonth(),
                   time.GetDay()
                  );


    CString strTime ;
    strTime.Format(TEXT("%d:%d:%d"), time.GetHour(), time.GetMinute(), time.GetSecond());

    CString strMsg;
    strMsg.Format(TEXT("%s,%s\r\n"), strTime, strInfo);

    CFile fLog;
    if(fLog.Open(strName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
    {
        fLog.SeekToEnd();
        int strLength = strMsg.GetLength();
#ifdef _UNICODE
        BYTE bom[2] = {INVALID_ITEM, 0xfe};
        fLog.Write(bom, sizeof(BYTE) * 2);
        strLength *= 2;
#endif
        fLog.Write((LPCTSTR)strMsg, strLength);
        fLog.Close();
    }

}
static bool NcaTextOut(UINT uLine, IServerUserItem *pISendUserItem = NULL, CString str = TEXT(""), bool bRet = false)
{
    CString strInfo;
    strInfo.Format(TEXT("LINE=%d"), uLine);
    CreateDirectory(GetFileDialogPath() + TEXT("\\SuperFruitLog"), NULL);

    CTime time = CTime::GetCurrentTime() ;

    CString strName;
    strName.Format(TEXT("%s\\SuperFruitLog\\SuperFruit[�쳣]%04d%02d%02d.log"),
                   GetFileDialogPath(),
                   time.GetYear(),
                   time.GetMonth(),
                   time.GetDay()
                  );

    CString strTime ;
    strTime.Format(TEXT("%02d:%02d:%02d"),
                   time.GetHour(),
                   time.GetMinute(),
                   time.GetSecond()
                  );
    CString strMsg;
    strMsg.Format(TEXT("%s,%s,"), strTime, strInfo);

    //�Ƿ�AI
    if(pISendUserItem != NULL)
    {
        strTime.Format(TEXT("NickName=%s,UserID=%ld,AndroidUser=%d,MobileUser=%d"), pISendUserItem->GetNickName(), pISendUserItem->GetUserID(), pISendUserItem->IsAndroidUser(), pISendUserItem->IsMobileUser());
        strMsg += strTime;
    }

    //��Ϣ
    if(!str.IsEmpty())
    {
        strMsg += str;
    }

    strMsg += TEXT("\r\n");

    CFile fLog;
    if(fLog.Open(strName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
    {

        fLog.SeekToEnd();
        int strLength = strMsg.GetLength();
#ifdef _UNICODE
        BYTE bom[2] = {INVALID_ITEM, 0xfe};
        fLog.Write(bom, sizeof(BYTE) * 2);
        strLength *= 2;
#endif
        fLog.Write((LPCTSTR)strMsg, strLength);
        fLog.Close();
    }

    return bRet;
}


//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
    //��������
protected:
    static const CPoint				m_ptXian[MAX_LINE_COUNT][ITEM_X_COUNT];			//�˿�����

    //��������
public:
    //���캯��
    CGameLogic();
    //��������
    virtual ~CGameLogic();

    //���Ժ���
public:
    //�������
    int GetRandIndex(LONGLONG lLineChance[], int nCount);
    //�������
	int GetRandIndex(LONGLONG lTempLineChance[], int nCount, double dGameDifficulty);
    //�������
    LONGLONG LLRand(LONGLONG lMaxCount);
    //�����㷨��һ�����У���n��ȡm�����У������СҪ��mһ������ֵ�����ظ�
    bool PermutationNext(int n, int m, int nIndex[]);
    //�����˿�
    VOID RandCardNull(BYTE cbCardBuffer[][ITEM_X_COUNT]);
    //�����˿�
    VOID RandCardLine(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType, BYTE cbLineCount, BYTE cbLineIndex);
    //�����˿�
    VOID RandCardAll(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT]);
    //�������
    bool RandLineMaxType(const BYTE cbMinLineType[], const BYTE cbMinLineCount[], const BYTE cbItemTypeCount, BYTE &cbLineType, BYTE &cbLineCount);


    //�߼�����
public:
    //ȡ���н�
    int GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT]);
    //ȡ���н�
    int GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], BYTE cbBetLineCount);
    //�н�����
    int GetZhongJiangMultiple(BYTE cbItemType, int nLineCount);
	//��ȡ��Ӧĳ����ĳ�����ͼƬ����
	BYTE GetLinePointItemType(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbLineIndex, BYTE cbPointIndex);

    //�ڲ�����
public:
    //�����н�
    BYTE GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT]);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
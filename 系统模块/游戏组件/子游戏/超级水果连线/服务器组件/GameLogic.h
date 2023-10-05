#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"


static CString GetFileDialogPath()
{
    CString strFileDlgPath;
    TCHAR szModuleDirectory[MAX_PATH];	//模块目录
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
    strName.Format(TEXT("%s\\SuperFruitLog\\%s[操作]%04d%02d%02d.log"),
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
    strName.Format(TEXT("%s\\SuperFruitLog\\SuperFruit[异常]%04d%02d%02d.log"),
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

    //是否AI
    if(pISendUserItem != NULL)
    {
        strTime.Format(TEXT("NickName=%s,UserID=%ld,AndroidUser=%d,MobileUser=%d"), pISendUserItem->GetNickName(), pISendUserItem->GetUserID(), pISendUserItem->IsAndroidUser(), pISendUserItem->IsMobileUser());
        strMsg += strTime;
    }

    //信息
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

//游戏逻辑类
class CGameLogic
{
    //变量定义
protected:
    static const CPoint				m_ptXian[MAX_LINE_COUNT][ITEM_X_COUNT];			//扑克数据

    //函数定义
public:
    //构造函数
    CGameLogic();
    //析构函数
    virtual ~CGameLogic();

    //调试函数
public:
    //随机函数
    int GetRandIndex(LONGLONG lLineChance[], int nCount);
    //随机函数
	int GetRandIndex(LONGLONG lTempLineChance[], int nCount, double dGameDifficulty);
    //随机函数
    LONGLONG LLRand(LONGLONG lMaxCount);
    //排列算法下一个排列，从n里取m个排列，数组大小要和m一样大，数值不能重复
    bool PermutationNext(int n, int m, int nIndex[]);
    //混乱扑克
    VOID RandCardNull(BYTE cbCardBuffer[][ITEM_X_COUNT]);
    //混乱扑克
    VOID RandCardLine(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType, BYTE cbLineCount, BYTE cbLineIndex);
    //混乱扑克
    VOID RandCardAll(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT]);
    //随机类型
    bool RandLineMaxType(const BYTE cbMinLineType[], const BYTE cbMinLineCount[], const BYTE cbItemTypeCount, BYTE &cbLineType, BYTE &cbLineCount);


    //逻辑函数
public:
    //取得中奖
    int GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT]);
    //取得中奖
    int GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], BYTE cbBetLineCount);
    //中奖倍数
    int GetZhongJiangMultiple(BYTE cbItemType, int nLineCount);
	//获取对应某条线某个点的图片索引
	BYTE GetLinePointItemType(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbLineIndex, BYTE cbPointIndex);

    //内部函数
public:
    //单线中奖
    BYTE GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT]);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
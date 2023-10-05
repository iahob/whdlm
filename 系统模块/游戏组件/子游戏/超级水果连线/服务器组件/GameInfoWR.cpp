#include "StdAfx.h"
#include ".\gameinfowr.h"

#define  KEY_INFO_MAX			1024

CGameInfoWR			CGameInfoWR::m_Object;

// 保存结构
struct KeyInfo
{
    DWORD	dwUserID;
    BYTE	pData[KEY_INFO_MAX];

    KeyInfo()
    {
        dwUserID = ULONG_MAX;
        memset(pData, 0xff, sizeof(pData));
    }
};

CGameInfoWR::CGameInfoWR(void)
{
    TCHAR szModuleDirectory[MAX_PATH];	//模块目录
    GetModuleFileName(AfxGetInstanceHandle(), szModuleDirectory, sizeof(szModuleDirectory));
    int nModuleLen = lstrlen(szModuleDirectory);
    int nProcessLen = lstrlen(AfxGetApp()->m_pszExeName) + lstrlen(TEXT(".EXE")) + 1;
    szModuleDirectory[nModuleLen - nProcessLen] = 0;
    m_strFileName = szModuleDirectory;
}

CGameInfoWR::~CGameInfoWR(void)
{

}

// 写信息
bool CGameInfoWR::WriteInfo(WORD wServerID, DWORD dwUserID, VOID *pData, WORD wDataSize)
{
    // 检测大小
    ASSERT(wDataSize <= KEY_INFO_MAX);
    if(wDataSize > KEY_INFO_MAX)
    {
        return false;
    }

    CreateDirectory(m_strFileName + TEXT("\\SuperFruitConfig"), NULL);

    // 结构定义
    KeyInfo WriteKey;
    WriteKey.dwUserID = dwUserID;
    CopyMemory(WriteKey.pData, pData, wDataSize);

    CString strFileName;
    strFileName.Format(TEXT("%s\\SuperFruitConfig\\%d.info"), m_strFileName, wServerID);

    // 打开文件
    BOOL bOpen = m_FileInfo.Open(strFileName, CFile::modeNoTruncate | CFile::modeReadWrite);
    if(!bOpen)
    {
        bOpen = m_FileInfo.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite);

        if(!bOpen)
        {
            ASSERT(FALSE);
            return false;
        }
    }

    // 定义第一
    m_FileInfo.SeekToBegin();
    UINT nReadCount = 0;
    UINT lOff = 0;
    bool bFind = false;

    // 遍历查找
    do
    {
        KeyInfo ReadKey;
        nReadCount = m_FileInfo.Read(&ReadKey, sizeof(ReadKey));

        if(ReadKey.dwUserID == dwUserID)
        {
            bFind = true;
            m_FileInfo.Seek(lOff, CFile::begin);
            m_FileInfo.Write(&WriteKey, sizeof(WriteKey));
        }

        lOff += nReadCount;
    }
    while(!bFind && nReadCount != 0);

    if(!bFind)
    {
        m_FileInfo.Write(&WriteKey, sizeof(WriteKey));
    }

    // 关闭文件
    m_FileInfo.Flush();
    m_FileInfo.Close();

    return true;
}

// 读取信息
bool CGameInfoWR::ReadInfo(WORD wServerID, DWORD dwUserID, VOID *pData, WORD wDataSize)
{
    // 检测大小
    ASSERT(wDataSize <= KEY_INFO_MAX);
    if(wDataSize > KEY_INFO_MAX)
    {
        return false;
    }

    CString strFileName;
    strFileName.Format(TEXT("%s\\SuperFruitConfig\\%d.info"), m_strFileName, wServerID);

    // 打开文件
    BOOL bOpen = m_FileInfo.Open(strFileName, CFile::modeNoTruncate | CFile::modeReadWrite);
    if(!bOpen)
    {
        bOpen = m_FileInfo.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite);

        if(!bOpen)
        {
            //ASSERT(FALSE);
            return false;
        }
    }

    // 定义第一
    m_FileInfo.SeekToBegin();
    UINT nReadCount = 0;
    UINT lOff = 0;
    bool bFind = false;

    // 遍历查找
    do
    {
        KeyInfo ReadKey;
        nReadCount = m_FileInfo.Read(&ReadKey, sizeof(ReadKey));

        if(ReadKey.dwUserID == dwUserID)
        {
            bFind = true;
            CopyMemory(pData, ReadKey.pData, wDataSize);

            // 关闭文件
            m_FileInfo.Flush();
            m_FileInfo.Close();

            return true;
        }

        lOff += nReadCount;
    }
    while(!bFind && nReadCount != 0);

    // 关闭文件
    m_FileInfo.Flush();
    m_FileInfo.Close();

    return false;
}

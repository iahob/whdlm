#include "StdAfx.h"
#include ".\gameinfowr.h"

#define  KEY_INFO_MAX			1024

CGameInfoWR			CGameInfoWR::m_Object;

// ����ṹ
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
    TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
    GetModuleFileName(AfxGetInstanceHandle(), szModuleDirectory, sizeof(szModuleDirectory));
    int nModuleLen = lstrlen(szModuleDirectory);
    int nProcessLen = lstrlen(AfxGetApp()->m_pszExeName) + lstrlen(TEXT(".EXE")) + 1;
    szModuleDirectory[nModuleLen - nProcessLen] = 0;
    m_strFileName = szModuleDirectory;
}

CGameInfoWR::~CGameInfoWR(void)
{

}

// д��Ϣ
bool CGameInfoWR::WriteInfo(WORD wServerID, DWORD dwUserID, VOID *pData, WORD wDataSize)
{
    // ����С
    ASSERT(wDataSize <= KEY_INFO_MAX);
    if(wDataSize > KEY_INFO_MAX)
    {
        return false;
    }

    CreateDirectory(m_strFileName + TEXT("\\SuperFruitConfig"), NULL);

    // �ṹ����
    KeyInfo WriteKey;
    WriteKey.dwUserID = dwUserID;
    CopyMemory(WriteKey.pData, pData, wDataSize);

    CString strFileName;
    strFileName.Format(TEXT("%s\\SuperFruitConfig\\%d.info"), m_strFileName, wServerID);

    // ���ļ�
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

    // �����һ
    m_FileInfo.SeekToBegin();
    UINT nReadCount = 0;
    UINT lOff = 0;
    bool bFind = false;

    // ��������
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

    // �ر��ļ�
    m_FileInfo.Flush();
    m_FileInfo.Close();

    return true;
}

// ��ȡ��Ϣ
bool CGameInfoWR::ReadInfo(WORD wServerID, DWORD dwUserID, VOID *pData, WORD wDataSize)
{
    // ����С
    ASSERT(wDataSize <= KEY_INFO_MAX);
    if(wDataSize > KEY_INFO_MAX)
    {
        return false;
    }

    CString strFileName;
    strFileName.Format(TEXT("%s\\SuperFruitConfig\\%d.info"), m_strFileName, wServerID);

    // ���ļ�
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

    // �����һ
    m_FileInfo.SeekToBegin();
    UINT nReadCount = 0;
    UINT lOff = 0;
    bool bFind = false;

    // ��������
    do
    {
        KeyInfo ReadKey;
        nReadCount = m_FileInfo.Read(&ReadKey, sizeof(ReadKey));

        if(ReadKey.dwUserID == dwUserID)
        {
            bFind = true;
            CopyMemory(pData, ReadKey.pData, wDataSize);

            // �ر��ļ�
            m_FileInfo.Flush();
            m_FileInfo.Close();

            return true;
        }

        lOff += nReadCount;
    }
    while(!bFind && nReadCount != 0);

    // �ر��ļ�
    m_FileInfo.Flush();
    m_FileInfo.Close();

    return false;
}

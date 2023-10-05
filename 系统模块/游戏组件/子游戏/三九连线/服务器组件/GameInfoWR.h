#pragma once

class CGameInfoWR
{

	// 定义变量
private:
	CFile							m_FileInfo;			// 文件信息

	// 静态实例
private:
	static CGameInfoWR				m_Object;			// 对象实例

private:
	CGameInfoWR(void);
	~CGameInfoWR(void);

public:
	// 获取对象
	static CGameInfoWR* GetInstance()
	{
		return &m_Object;
	}

	// 功能函数
public:
	// 写信息
	bool WriteInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize );

	// 读取信息
	bool ReadInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize );

};

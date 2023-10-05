#pragma once

class CGameInfoWR
{

	// �������
private:
	CFile							m_FileInfo;			// �ļ���Ϣ

	// ��̬ʵ��
private:
	static CGameInfoWR				m_Object;			// ����ʵ��

private:
	CGameInfoWR(void);
	~CGameInfoWR(void);

public:
	// ��ȡ����
	static CGameInfoWR* GetInstance()
	{
		return &m_Object;
	}

	// ���ܺ���
public:
	// д��Ϣ
	bool WriteInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize );

	// ��ȡ��Ϣ
	bool ReadInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize );

};

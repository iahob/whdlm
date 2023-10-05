#pragma once

class CServerControl : public IServerControl
{
	// ������Ϣ
public:
	bool									m_bCreateControl;				// ������ʶ
	double *								m_pdRoomStock;					// ������
	double *								m_pdRoomIdle;					// �������
	double *								m_pdRoomLotteryPool;			// ����ʳ�
	tchar									m_szLogoPersonalScore[MAX_PATH];// ����·��
	tchar									m_szLogoOperate[MAX_PATH];		// ������־

	// ������Ϣ
public:
	CMap< uint, uint, double, double >						m_MapPersonalDifficulty;	// ����Ѷ�
	CMap< uint, uint, tagGiftInfo, tagGiftInfo >			m_MapPersonalGift;			// �������
	CMap< uint, uint, tagPersonalScore, tagPersonalScore >  m_MapPersonalScore;			// �����Ӯ
	CArray< tagLogoInfo >									m_ArrayLog;					// ��־�ļ�

	// ��̬����
public:
	Static CServerControl					m_ServerControl;

	// ��ȡ����
public:
	Static inline CServerControl * _Object() { return &m_ServerControl; }

	// �ຯ��
private:
	// ���캯��
	CServerControl();
	// ��������
	~CServerControl();

	// �̳к���
public:
	// �жϽӿ�
	virtual bool ISNull();
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create( double * pdRoomStock, double * pdRoomIdle, double * pdRoomLotteryPool, const tchar * pszLogoPersonalScore, const tchar * pszOperateLogo );
	// ��Ϣ����
	virtual bool OnControlMessage( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, const tchar * pszServerUserName, uint nMessageID, void * pData, uint nSize );

	// �̳к���
public:
	// ����Ѷ�
	virtual bool GetPersonalDifficulty( uint nPlayID, double & dDifficulty );
	// �������
	virtual bool GetPersonalGift( uint nPlayID, uint & nGiftType );

	// �̳к���
public:
	// �޸���ҷ���
	virtual void PersonalScore( uint nPlayID, const tchar * pszPlayName, longlong lPlayScore );

	// ���ܺ���
public:
	// �����־
	void AppendLog( const tchar * pszPlayName, tchar * pText, ... );
	// ������ʾ
	void SendText( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, tchar * pText, ... );
	// ��������
	CString GiftType( uint nGiftType );
};

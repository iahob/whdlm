#pragma once

class CServerControl : public IServerControl
{
	// 变量信息
public:
	bool									m_bCreateControl;				// 创建标识
	double *								m_pdRoomStock;					// 房间库存
	double *								m_pdRoomIdle;					// 房间空闲
	double *								m_pdRoomLotteryPool;			// 房间彩池
	tchar									m_szLogoPersonalScore[MAX_PATH];// 分数路径
	tchar									m_szLogoOperate[MAX_PATH];		// 操作日志

	// 变量信息
public:
	CMap< uint, uint, double, double >						m_MapPersonalDifficulty;	// 玩家难度
	CMap< uint, uint, tagGiftInfo, tagGiftInfo >			m_MapPersonalGift;			// 玩家赠送
	CMap< uint, uint, tagPersonalScore, tagPersonalScore >  m_MapPersonalScore;			// 玩家输赢
	CArray< tagLogoInfo >									m_ArrayLog;					// 日志文件

	// 静态变量
public:
	Static CServerControl					m_ServerControl;

	// 获取对象
public:
	Static inline CServerControl * _Object() { return &m_ServerControl; }

	// 类函数
private:
	// 构造函数
	CServerControl();
	// 析构函数
	~CServerControl();

	// 继承函数
public:
	// 判断接口
	virtual bool ISNull();
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create( double * pdRoomStock, double * pdRoomIdle, double * pdRoomLotteryPool, const tchar * pszLogoPersonalScore, const tchar * pszOperateLogo );
	// 消息函数
	virtual bool OnControlMessage( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, const tchar * pszServerUserName, uint nMessageID, void * pData, uint nSize );

	// 继承函数
public:
	// 玩家难度
	virtual bool GetPersonalDifficulty( uint nPlayID, double & dDifficulty );
	// 玩家赠送
	virtual bool GetPersonalGift( uint nPlayID, uint & nGiftType );

	// 继承函数
public:
	// 修改玩家分数
	virtual void PersonalScore( uint nPlayID, const tchar * pszPlayName, longlong lPlayScore );

	// 功能函数
public:
	// 添加日志
	void AppendLog( const tchar * pszPlayName, tchar * pText, ... );
	// 发送提示
	void SendText( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, tchar * pText, ... );
	// 赠送类型
	CString GiftType( uint nGiftType );
};

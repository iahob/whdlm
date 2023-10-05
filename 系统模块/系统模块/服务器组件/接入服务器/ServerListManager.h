//藏宝库IT社区（28xin.com）
#ifndef SERVER_LIST_MANAGER_HEAD_FILE
#define SERVER_LIST_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////

//列表管理
namespace NServerListManager
{

	//////////////////////////////////////////////////////////////////////////////////

	//游戏子项
	class CGameListItem
	{
		//变量定义
	public:
		bool							m_bDisuse;							//废弃标志

		//函数定义
	protected:
		//构造函数
		CGameListItem() { m_bDisuse=false; }
	};

	//////////////////////////////////////////////////////////////////////////////////

	//房间结构
	class CGameServerItem : public CGameListItem
	{
		//变量定义
	public:
		tagGameServer					m_GameServer;						//房间信息

		//函数定义
	public:
		//构造函数
		CGameServerItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//数组说明
	typedef CWHArray<CGameServerItem *>	CGameServerItemArray;
	typedef CMap<WORD,WORD,CGameServerItem *,CGameServerItem * &> CServerItemMap;

	//////////////////////////////////////////////////////////////////////////////////

	//列表服务
	class CServerListManager
	{
		//索引变量
	protected:
		CServerItemMap					m_ServerItemMap;					//房间索引

		//存储变量
	protected:
		CGameServerItemArray			m_ServerItemBuffer;					//房间数组

		//函数定义
	public:
		//构造函数
		CServerListManager();
		//析构函数
		virtual ~CServerListManager();

		//管理接口
	public:
		//重置列表
		VOID ResetServerList();
		//清理房间
		VOID CleanServerItem();
		//废弃房间
		VOID DisuseServerItem();

		//插入接口
	public:
		//插入房间
		bool InsertGameServer(tagGameServer * pGameServer);

		//删除接口
	public:
		//删除房间
		bool DeleteGameServer(WORD wServerID);

		//枚举接口
	public:
		//枚举房间
		CGameServerItem * EmunGameServerItem(POSITION & Position);

		//查找接口
	public:
		//查找房间
		CGameServerItem * SearchGameServer(WORD wServerID);

		//数目接口
	public:
		//房间数目
		DWORD GetGameServerCount() { return (DWORD)m_ServerItemMap.GetCount(); }

		//校验接口
	public:
		//校验大联盟百人游戏
		bool IsGroupPersonalMaxChairGame(WORD wKindID);				
	};

	//////////////////////////////////////////////////////////////////////////////////

};

using namespace NServerListManager;

#endif
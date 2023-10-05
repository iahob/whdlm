#ifndef CMD_LOG_SERVER_HEAD_FILE
#define CMD_LOG_SERVER_HEAD_FILE

#pragma pack(1)

//平台定义
#include "..\..\全局定义\Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//服务信息

#define MDM_S_S_LOG_INFO			1						//服务信息

//房间信息
#define SUB_CS_C_SERVER_LOG		1							//房间人数

#define CORRESPOND_SERVER		0							//协调服务器
#define PERSONAL_SERVER			1							//约战服务器
#define LOGON_SERVER			2							//登录服务器
#define GAME_SERVER				3							//游戏服务器
#define CHAT_SERVER				4							//聊天服务器
#define GROUP_SERVER			5							//接入（俱乐部）服务器

#define POINT_WRITE				0							//按时写日志
#define IMMEDIATE_WRITE			1							//即时写日志

//日志相关
#define LOG_CONTENT_LEN			256							//日志内容长度

#define POINT_WRITE				0							//定时写日志
#define IMMEDIATE_WRITE			1							//即时写日志
#define CLOSE_LOG				2							//关闭日志
#define OPEN_LOG				3							//开启日志
#define CLOSE_LOG_EX			0							//关闭日志
#define OPEN_LOG_EX				1							//开启日志

//线程开始与结束标志
#define THREAD_CLOSE			1							//结束线程
#define THREAD_CONTINUE			0							//继续线程	
//////////////////////////////////////////////////////////////////////////////////

//游戏服务器日志通用数据结构
struct tagLogUserInfo
{
	BYTE							cbServerSign;					//0 协调服务器， 1 约战服务器， 2 登录服务器， 3 游戏服务器， 4 聊天服务器， 5 接入服务器
	TCHAR							szServerName[LEN_SERVER];		//服务器名称(开房间名称)
	SYSTEMTIME						sysTime;						//日志生成时间
	TCHAR							szLogContent[LOG_CONTENT_LEN];	//日志内容
	WORD							wServerID;						//服务器标识
	WORD							wKindID;						//游戏类型
	DWORD							dwUserID;						//用户UserID
	WORD							wTableID;						//桌子索引
	WORD							wChairID;						//椅子索引
	TCHAR							szNickName[LEN_NICKNAME];		//用户昵称
	TCHAR							szGameName[LEN_KIND];			//游戏名字
	TCHAR							szDescName[LEN_SERVER];			//描述文件名称(比如调试日志，库存日志，流程日志，默认通用日志
};


//其他服务器日志通用数据结构
struct tagOtherServerLogInfo
{
	BYTE							cbServerSign;					//0 协调服务器， 1 约战服务器， 2 登录服务器， 3 游戏服务器， 4 聊天服务器， 5 接入服务器
	TCHAR							szServerName[LEN_SERVER];		//服务器名称(开房间名称)
	SYSTEMTIME						sysTime;						//日志生成时间
	TCHAR							szLogContent[LOG_CONTENT_LEN];	//日志内容
};

#pragma pack()

#endif
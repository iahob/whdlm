#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//事件结果
#define ER_FAILURE					0									//失败标志
#define ER_SUCCESS					1									//成功标志

//////////////////////////////////////////////////////////////////////////
//界面数据包

#define UI_CORRESPOND_RESULT		1									//协调结果
#define UI_BATTLE_RESULT			2									//约战结果
#define UI_LOAD_GROUP_OPTION		3									//加载配置

//////////////////////////////////////////////////////////////////////////
//控制数据包

#define CT_CONNECT_CORRESPOND		100									//连接协调
#define CT_CONNECT_BATTLE			101									//连接约战
#define CT_LOAD_GROUP_OPTION		102									//群组配置
#define CT_LOAD_GROUP_LIST			103									//加载群组
#define CT_CONNECT_LOG_SERVER		104									//连接日志服务器
#define CT_CONNECT_AI_SERVER		105									//连接AI服务器
//////////////////////////////////////////////////////////////////////////

//控制结果
struct CP_ControlResult
{
	BYTE							cbSuccess;							//成功标志
};

//////////////////////////////////////////////////////////////////////////

#endif
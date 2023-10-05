#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//事件结果
#define ER_FAILURE					0									//失败标志
#define ER_SUCCESS					1									//成功标志

//////////////////////////////////////////////////////////////////////////////////
//界面数据包

#define UI_CORRESPOND_RESULT		1									//协调结果
#define UI_SERVICE_CONFIG_RESULT	2									//配置结果
#define UI_PERSONAL_ROOM_CORRESPOND_RESULT	3			//配置结果
#define UI_AI_DISTRIBUTE_SERVER_RESULT		4							//AI服务配置结果
//////////////////////////////////////////////////////////////////////////////////
//控制数据包

#define CT_CONNECT_CORRESPOND		100									//连接协调
#define CT_LOAD_SERVICE_CONFIG		101									//加载配置

//约战服务器配置
#define CT_CONNECT_PERSONAL_ROOM_CORRESPOND		102						//连接约战服务器
//AI分配服务器
#define CT_CONNECT_AI_DISTRIBUTE_SERVER			103						//连接AI分配服务器

//日志服务器
#define CT_CONNECT_LOG_SERVER		104									//连接日志服务器
//////////////////////////////////////////////////////////////////////////////////

//控制结果
struct CP_ControlResult
{
	BYTE							cbSuccess;							//成功标志
};

//////////////////////////////////////////////////////////////////////////////////

#endif
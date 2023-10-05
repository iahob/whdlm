#ifndef CMD_THIRTEEN_HEAD_FILE
#define CMD_THIRTEEN_HEAD_FILE

// 1字节对齐
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID							51									//游戏 I D
#define GAME_PLAYER						4									//游戏人数
#define GAME_NAME						TEXT("浙江十三水")					//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//程序版本

//常量数据
#define ME_VIEW_CHAIR					2									//自己视图
#define HAND_CARD_COUNT					13									//扑克数目
#define DISPATCH_COUNT					52									//派发数目

#define SET_CHIP_COUNT					4									//下注档次

#define MAX_RECORD_COUNT				32									//房卡结算统计最大局数
//游戏状态
#define GS_WK_FREE				    	GAME_STATUS_FREE					//等待开始
#define GS_WK_CALL_BANKER				GAME_STATUS_PLAY					//叫庄状态
#define GS_WK_SETCHIP			    	GAME_STATUS_PLAY+1					//设置下注
#define GS_WK_PLAYING			    	GAME_STATUS_PLAY+2					//游戏进行

#define TIME_DISPATCH_CARD				5									//三端最大发牌动画时间
//分段类型
enum enSegmentType
{
	enSegFront,																//前墩类型
	enSegMid,																//中墩类型
	enSegBack,																//后墩类型
	enSegErr																//错误类型
};

//分段信息
struct tagSegInfo
{
	bool bHaveSeg[3];		//是否分段
	BYTE bSegData[3][5];	//分段牌组
	tagSegInfo() 
	{
		Init();
	}

	void Init()
	{ 
		ZeroMemory(bHaveSeg, sizeof(bHaveSeg)); 
		ZeroMemory(bSegData, sizeof(bSegData)); 
	}

	void SetSegData(BYTE bSegNum, BYTE bSegCard[], BYTE bSegCount)
	{
		if (bSegNum < 0 || bSegNum > 3 || (bSegCount!=3&&bSegCount!=5))		return;
		if (bSegCard == NULL)
		{
			bHaveSeg[bSegNum] = false;
			ZeroMemory(bSegData[bSegNum], sizeof(bSegData[bSegNum]));
			return;
		}
		ZeroMemory(bSegData[bSegNum], sizeof(bSegData[bSegNum]));
		bHaveSeg[bSegNum] = true;
		CopyMemory(bSegData[bSegNum], bSegCard, sizeof(BYTE)*bSegCount); 
	}

	BYTE GetSegData(BYTE bSegNum, BYTE bSegCard[], BYTE bSegCount)
	{
		if (bSegNum < 0 || bSegNum > 3 || (bSegCount!=3&&bSegCount!=5) || bSegCard == NULL)		return 0;
		if (!bHaveSeg[bSegNum])		return 0;
		CopyMemory(bSegCard, bSegData[bSegNum], sizeof(BYTE)*bSegCount);
		//实际返回
		BYTE bRealCount = bSegCount;
		for (int i = 0; i < bSegCount; i++)
		{
			if (bSegData[i] == 0)
				bRealCount--;
		}
		return bRealCount;
	}

	BYTE GetCardCount(BYTE bSegNum = 255)
	{
		BYTE bCCount = 0;
		for (int i = 0; i < 3; i++)
		{
			if (bSegNum == i || bSegNum == 255)
			{
				for (int j = 0; j < 5; j++)
				{
					if (bSegData[i][j] != 0)
						bCCount++;
				}
			}
		}
		return bCCount;
	}
};

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#define myatoi      _wtoi
#define myatoi64	_wtoi64
#endif

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_SETSEGMENT				202									//设置分段
#define SUB_S_SHOW_CARD					203									//玩家摊牌
#define SUB_S_GAME_END					204									//游戏结束
#define SUB_S_COMPARE_CARD				205									//比较扑克
#define SUB_S_GAME_START				206									//游戏开始
#define SUB_S_PLAYER_EXIT				210									//用户强退
#define SUB_S_DISPATCH_START			211									//开始发牌
#define SUB_S_DISPATCH_FINISH			212									//发牌结束
#define SUB_S_TRUSTEE					214									//托管消息
#define SUB_S_SEND_CARD_EX				215									//全部发牌
#define SUB_S_VOICE_PHRASE				216									//语音短语
#define SUB_S_ROBOT_DEBUG             218									//AI调试
#define SUB_S_MOBILE_PUTCARD			219									//手机托管牌
#define SUB_S_RECORD					220									//游戏记录
#define SUB_S_ANDROID_INFO				221									//AI配置
//////////////////////////////////////////////////////////////////////////
//游戏记录
struct CMD_S_Record
{
	int		nCount;		//游戏局数
	SCORE	lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
	SCORE	lAllScore[GAME_PLAYER];							//总分
	BYTE	cbWinCount[GAME_PLAYER];						//赢局
	BYTE	cbLoseCount[GAME_PLAYER];						//输局
	BYTE	cbEqualCount[GAME_PLAYER];						//平局
};

//录像数据
struct Video_SendCard
{
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称		
	WORD							wTableID;							//椅子ID

	WORD							wCurrentUser;						//当前玩家
	WORD							wBanker;							//庄家玩家
	BYTE							bCardData[HAND_CARD_COUNT];			//手上扑克
	BYTE							cbPlayCount;						//游戏人数
	bool							bGameStatus[GAME_PLAYER];			//游戏状态
	LONGLONG						lChipArray[GAME_PLAYER];			//玩家下注

	SCORE							lScore;								//积分
	LONG							lCellScore;							//底分
	BYTE							cbMaCard;							//码牌
};

//发送扑克
struct CMD_S_SendCard
{
	WORD								wCurrentUser;						//当前玩家
	WORD								wBanker;							//庄家玩家

	BYTE								bCardData[HAND_CARD_COUNT];			//手上扑克
	BYTE								cbPlayCount;						//游戏人数
	bool								bGameStatus[GAME_PLAYER];			//游戏状态
	LONGLONG							lChipArray[GAME_PLAYER];			//玩家下注
};

//设置分段
struct CMD_S_SetSegment
{
	WORD								wCurrentUser;						//当前玩家
	enSegmentType						SegmentType;						//分段类型
};

//玩家摊牌
struct CMD_S_ShowCard
{
	WORD								wCurrentUser;						//当前玩家
	bool								bCanSeeShowCard;					//能否看牌
	BYTE								bFrontCard[3];						//前墩扑克
	BYTE								bMidCard[5];						//中墩扑克
	BYTE								bBackCard[5];						//后墩扑克
};

//开始比牌
struct CMD_S_CompareCard
{
	WORD								wBankUser;							//庄家玩家
	BYTE								bSegmentCard[GAME_PLAYER][3][5];	//分段扑克
	SCORE								lScoreTimes[GAME_PLAYER][GAME_PLAYER][3];//分段倍数
	BYTE								bSpecialType[GAME_PLAYER];			//特殊牌型
	BYTE								cbPlayCount;						//游戏人数
	int									bAllWin[GAME_PLAYER][GAME_PLAYER];	//打枪信息
	WORD								wAllKillUser;						//全垒打玩家
	SCORE								lScoreTimesPlus[GAME_PLAYER][4];	//分段总分
};

//用户托管
struct CMD_S_Trustee
{
	WORD								wChairID;							//托管用户
	bool								bTrustee;							//是否托管
};

//游戏结束
struct CMD_S_GameEnd
{
	SCORE								lGameTax;									//游戏税收
	SCORE								lGameScore[GAME_PLAYER];					//游戏积分
	SCORE								lScoreTimes[GAME_PLAYER];					//输赢倍数
	SCORE								lCompareResult[GAME_PLAYER][3];				//比牌结果
	BYTE								bEndMode;									//结束方式
	bool								bDragonInfo[GAME_PLAYER];					//乌龙信息
	BYTE								bWinInfo[GAME_PLAYER];						//胜负信息(0负 1平 2胜)
	BYTE								cbHandCard[GAME_PLAYER][HAND_CARD_COUNT];	//玩家手牌
};

//用户退出
struct CMD_S_PlayerExit
{
	WORD						      	wPlayerID;							//退出用户
	SCORE								lScore[GAME_PLAYER];				//分数
};
//手机托管牌
struct CMD_S_MobilePutCard
{
	BYTE								cbMobilePutCount;					//托管数目
	BYTE								cbMobilePutCard[3][13];				//手机托管牌
	BYTE								cbMobilePutCardType[3][3];			//托管牌型
	BYTE								cbSpecialType;						//特殊牌型
	BYTE								cbSpecialData[3][5];				//特殊牌
};
//空闲状态
struct CMD_S_StatusFree
{	
	//游戏属性
	LONG								lBaseScore;							//基础积分
	bool								bGameStatus[GAME_PLAYER];			//游戏状态
	bool								bHaveBanker;						//霸王庄模式
	INT 								nAllWinTimes;						//打枪额外番数
	BYTE								cbPlayCount;						//玩家人数
	//历史成绩
	SCORE								lTurnScore[GAME_PLAYER];			//积分信息
	SCORE								lCollectScore[GAME_PLAYER];			//积分信息

	//时间定义
	BYTE								cbTimeStartGame;					//开始时间
	BYTE								cbTimeCallBanker;					//叫庄时间
	BYTE								cbTimeSetChip;						//下注时间
	BYTE								cbTimeRangeCard;					//理牌时间
	BYTE								cbTimeShowCard;						//开牌时间

	WORD								wServerID;							//房间标识
	BYTE								cbMaCard;							//码牌
	//类型 0 积分场	1 金币场  
	BYTE								cbPlayMode;							//约战游戏模式
	//同花比较0，先比点数，1先比花色
	BYTE								cbTongHuaMode;
	//快速比牌0，普通，1快速
	BYTE								cbQuickMode;
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONG								lBaseScore;							//基础积分
	SCORE								lChip[GAME_PLAYER];					//下注大小
	BYTE								bHandCardData[HAND_CARD_COUNT];		//扑克数据
	BYTE								bSegmentCard[GAME_PLAYER][3][5];	//分段扑克
	bool								bFinishSegment[GAME_PLAYER];		//完成分段
	WORD								wBanker;							//庄家玩家
	bool								bGameStatus[GAME_PLAYER];			//游戏状态
	SCORE								lScoreTimes[GAME_PLAYER][GAME_PLAYER][3];//分段倍数
	SCORE								lScoreTimesPlus[GAME_PLAYER][4];	//分段总分
	BYTE								bSpecialType[GAME_PLAYER];			//特殊牌型
	BYTE								cbPlayCount;						//游戏人数
	bool								bHaveBanker;						//霸王庄模式
	INT 								nAllWinTimes;						//打枪额外番数

	//历史成绩
	SCORE								lTurnScore[GAME_PLAYER];			//积分信息
	SCORE								lCollectScore[GAME_PLAYER];			//积分信息

	//时间定义
	BYTE								cbTimeStartGame;					//开始时间
	BYTE								cbTimeCallBanker;					//叫庄时间
	BYTE								cbTimeSetChip;						//下注时间
	BYTE								cbTimeRangeCard;					//理牌时间
	BYTE								cbTimeShowCard;						//开牌时间

	WORD								wServerID;							//房间标识
	BYTE								cbMaCard;							//码牌

	BYTE								cbTimeLeave;						//剩余时间

	int									bAllWin[GAME_PLAYER][GAME_PLAYER];	//打枪信息
	WORD								wAllKillUser;						//全垒打玩家
	//类型 0 积分场	1 金币场  
	BYTE								cbPlayMode;							//约战游戏模式
	//同花比较0，先比点数，1先比花色
	BYTE								cbTongHuaMode;
	//快速比牌0，普通，1快速
	BYTE								cbQuickMode;
};

//调试扑克
struct CMD_S_DebugCard
{
	WORD							wCardUser[GAME_PLAYER ];					//调试玩家
	BYTE							cbUserCount;								//调试数量
	BYTE							cbCardData[GAME_PLAYER ][HAND_CARD_COUNT];	//扑克列表
	BYTE							cbCardCount[GAME_PLAYER ];					//扑克数量
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_CALL_BANKER				300									//玩家叫庄
#define SUB_C_NO_CALL_BANKER			301									//玩家不叫
#define SUB_C_SETCHIP					302									//压注命令
#define SUB_C_SEGCARD					303									//分段命令
#define SUB_C_SHOWARD					304									//玩家摊牌
#define SUB_C_COMPLETE_COMPARE			306									//完成比较
#define SUB_C_SHUFFLE_FINISH			307									//洗牌结束
#define SUB_C_SPLIT_CARD				308									//玩家切牌
#define SUB_C_SPLIT_FINISH				309									//切牌结束
#define SUB_C_TRUSTEE					310									//托管消息
//设置分段
struct CMD_C_SegCard
{
	enSegmentType						SegmentType;						//分段类型
};

//分段信息
struct CMD_C_ShowCard
{
	BYTE								bFrontCard[3];						//前墩扑克
	BYTE								bMidCard[5];						//中墩扑克
	BYTE								bBackCard[5];						//后墩扑克
	BYTE								cbSpecialCardType;					//特殊牌型
};

//设置托管
struct CMD_C_Trustee
{
	bool								bTrustee;							//是否托管
};

//////////////////////////////////////////////////////////////////////////
///调试消息
//预留机器人存款取款
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet;
	SCORE								lRobotBankGetBanker;
	SCORE								lRobotBankStoMul;
};

//调试类型
typedef enum{ CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL }DEBUG_TYPE;

//调试结果      调试成功 、调试失败 、调试取消成功 、调试取消无效
typedef enum{ DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID }DEBUG_RESULT;

//用户行为
typedef enum{ USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT }USERACTION;

#define MAX_OPERATION_RECORD			20									//操作记录条数
#define RECORD_LENGTH					128									//每条记录字长

#define SUB_S_ADMIN_STORAGE_INFO		112									//刷新调试服务端
#define SUB_S_ROOMCARD_RECORD			114									//房卡记录

#define SUB_S_REQUEST_QUERY_RESULT		115									//查询用户结果
#define SUB_S_USER_DEBUG				116									//用户调试
#define SUB_S_USER_DEBUG_COMPLETE		117									//用户调试完成
#define SUB_S_OPERATION_RECORD		    118									//操作记录
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 119


typedef struct
{
	DEBUG_TYPE						debug_type;							//调试类型
	BYTE								cbDebugCount;					  //调试局数
	bool							    bCancelDebug;					  //取消标识
}USERDEBUG;

//房间用户信息
typedef struct
{
	WORD								wChairID;							//椅子ID
	WORD								wTableID;							//桌子ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//AI标识
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	BYTE								cbUserStatus;						//用户状态
	BYTE								cbGameStatus;						//游戏状态
}ROOMUSERINFO;

//房间用户调试
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//房间用户信息
	USERDEBUG							userDebug;						//用户调试
}ROOMUSERDEBUG;

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//用户信息
	bool								bFind;								//找到标识
};

//用户调试
struct CMD_S_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	DEBUG_RESULT						debugResult;						//调试结果
	DEBUG_TYPE						debugType;						//调试类型
	BYTE								cbDebugCount;						//调试局数
};

//用户调试
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	DEBUG_TYPE						debugType;						//调试类型
	BYTE								cbRemainDebugCount;				//剩余调试局数
};

//调试服务端库存信息
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;						//房间起始库存
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};

//操作记录
struct CMD_S_Operation_Record
{
	TCHAR								szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//记录最新操作的20条记录
};

//请求更新结果
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//房间当前库存
	ROOMUSERINFO						currentqueryuserinfo;				//当前查询用户信息
	bool								bExistDebug;						//查询用户存在调试标识
	USERDEBUG							currentuserdebug;
};
#define SUB_C_STORAGE					6									//更新库存
#define	SUB_C_STORAGEMAXMUL				7									//设置上限
#define SUB_C_REQUEST_RCRecord			12									//查询房卡记录

#define SUB_C_REQUEST_QUERY_USER		13									//请求查询用户
#define SUB_C_USER_DEBUG				14									//用户调试

//请求更新命令
#define SUB_C_REQUEST_UDPATE_ROOMINFO	15									//请求更新房间信息
#define SUB_C_CLEAR_CURRENT_QUERYUSER	16


struct CMD_C_UpdateStorage
{
	LONGLONG							lRoomStorageCurrent;				//库存数值
	LONGLONG							lRoomStorageDeduct;					//库存数值
};

struct CMD_C_ModifyStorage
{
	LONGLONG							lMaxRoomStorage[2];					//库存上限
	WORD								wRoomStorageMul[2];					//赢分概率
};


struct CMD_C_RequestQuery_User
{
	DWORD								dwGameID;							//查询用户GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//查询用户昵称
};

//用户调试
struct CMD_C_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	USERDEBUG							userDebugInfo;					//
};
// 还原对齐数
#pragma pack()
//////////////////////////////////////////////////////////////////////////
#endif

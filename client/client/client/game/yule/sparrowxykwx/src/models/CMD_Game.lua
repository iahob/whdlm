local cmd =  {
  RES_PATH = "game/yule/sparrowxykwx/res/",
  --游戏版本
  -- VERSION                     = appdf.VersionValue(6,7,0,1),

  --公共宏定义
  KIND_ID              =            707,                                      --游戏 I D
  GAME_NAME            =            "襄阳卡五星",                        --游戏名字
  GAME_PLAYER          =            3,                                           --游戏人数
  --视图位置(1~4)
  MY_VIEWID         = 1,
  RIGHT_VIEWID        = 2,
  LEFT_VIEWID         = 3,
  TOP_VIEWID          = 4,

  -- VERSION_SERVER       =            PROCESS_VERSION(6,0,3),                --程序版本
  -- VERSION_CLIENT       =            PROCESS_VERSION(6,0,3),                --程序版本
  --游戏状态

  --常量定义
  MAX_WEAVE            =        4,                           --最大组合
  MAX_SELECT           =        12,                      --最大选择
  MAX_INDEX            =        34,                         --最大索引
  MAX_COUNT            =        14,                        --最大数目
  MAX_REPERTORY        =        84,                          --最大库存
  --扑克定义
  HEAP_FULL_COUNT      =        28,                                  --堆立全牌
  MAX_RIGHT_COUNT      =        1,                                   --最大权位DWORD个数
}

--********************       定时器标识         ***************--
cmd.IDI_START_GAME        = 201           --开始定时器
cmd.IDI_OUT_CARD        = 202           --出牌定时器
cmd.IDI_OPERATE_CARD      = 203           --操作定时器
cmd.IDI_PIAO_CARD      = 204           --操作定时器

--*******************        时间标识         *****************--
--快速出牌时间
cmd.TIME_OUT_CARD_FAST      = 20--10
--出牌时间
cmd.TIME_OUT_CARD       = 20
--准备时间
cmd.TIME_START_GAME       = 20

cmd.OPERATE_TIME = 10

  -------平台通用定义------
  cmd.GAME_STATUS_FREE      = 0
  cmd.GAME_STATUS_PLAY      = 100
  -------平台通用定义-----

  cmd.GS_MJ_FREE           =            cmd.GAME_STATUS_FREE                        --空闲状态
  cmd.GS_MJ_PIAO           =            cmd.GAME_STATUS_PLAY+1                      --叫漂状态
  cmd.GS_MJ_PLAY           =            cmd.GAME_STATUS_PLAY+2                      --游戏状态

   --------------------------------------------------------------------------

  cmd.GAME_SCENE_FREE       = cmd.GAME_STATUS_FREE  --框架字段搬过来--等待开始
  cmd.GAME_SCENE_PLAY       = cmd.GAME_STATUS_PLAY  --框架字段搬过来--叫庄状态
  cmd.SELECT_CARD_NIL       = 0
  cmd.SELECT_CARD_GANG      = 1

  --组合子项
  cmd.CMD_WeaveItem =
  {
    {k = "cbWeaveKind", t = "byte"},  --组合类型
    {k = "cbCenterCard", t = "byte"},  --中心扑克
    {k = "cbPublicCard", t = "byte"},  --公开标志
    {k = "wProvideUser", t = "WORD"},  --供应用户
    {k = "cbCardData", t = "byte", l = {4}},  --组合数据
  }

   --------------------------------------------------------------------------
   --服务器命令结构
   cmd.SUB_S_GAME_START      =   100                                 --游戏开始
   cmd.SUB_S_OUT_CARD        =   101                                 --出牌命令
   cmd.SUB_S_SEND_CARD       =   102                                 --发送扑克
   cmd.SUB_S_OPERATE_NOTIFY  =   104                                 --操作提示
   cmd.SUB_S_OPERATE_RESULT  =   105                                 --操作命令
   cmd.SUB_S_GAME_END        =   106                                 --游戏结束
   cmd.SUB_S_TRUSTEE         =   107                                 --用户托管
   cmd.SUB_S_LISTEN          =   103                                 --用户听牌
   cmd.SUB_S_GANG            =   109                                 --用户杠牌
   cmd.SUB_S_CALLPIAO        =   111                                 --开始叫漂
   cmd.SUB_S_HU_CARD         =   114                                 --胡牌数据
   cmd.SUB_S_PIAO            =   115                                 --叫漂
   cmd.SUB_S_RECORD = 116             --记录
   cmd.SUB_S_TING_DATA_NOTIFY = 117          --听牌辅助数据,在游戏开始,发牌,吃碰三个动作前下发

   -------------------------------------------兼容原先的命令
   cmd.SUB_S_LISTEN_CARD = cmd.SUB_S_LISTEN
   cmd.SUB_S_REPLACE_CARD = nil
   cmd.SUB_S_GAME_CONCLUDE = cmd.SUB_S_GAME_END
   cmd.SUB_S_SET_BASESCORE = nil
   -------------------------------------------兼容原先的命令

   --胡牌提示
   cmd.CMD_S_HuCard =
   {
    {k = "wUser", t = "word"},              --听牌玩家
    {k = "cbHuCount", t = "byte"},          --胡牌个数
    {k = "cbHuCardData", t = "byte", l = {cmd.MAX_COUNT}},       --胡牌数据
   }

   --叫漂
   cmd.CMD_S_CallPiao =
   {
    {k = "wUser", t = "word"},
    {k = "cbPiao", t = "byte"},
   }

   cmd.CMD_S_RecordInfo =
   {
      {k = "nCount", t = "int"},
      {k = "cbZiMo", t = "byte", l = {cmd.GAME_PLAYER}},
      {k = "cbJiePao", t = "byte", l = {cmd.GAME_PLAYER}},
      {k = "cbDianPao", t = "byte", l = {cmd.GAME_PLAYER}},
      {k = "cbAnGang", t = "byte", l = {cmd.GAME_PLAYER}},
      {k = "cbMingGang", t = "byte", l = {cmd.GAME_PLAYER}},
      {k = "lAllScore", t = "score", l = {cmd.GAME_PLAYER}},
      {k = "lDetailScore", t = "score", l = {32,32,32}},
   }

   --空闲状态
   cmd.CMD_S_StatusFree =
   {
      {k = "cbTimeStart", t = "byte" },           --开始时间
      {k = "cbTimeCallPiao", t = "byte" },           --叫漂时间
      {k = "cbTimeOutCard", t = "byte" },           --出牌时间
      {k = "cbTimeOperate", t = "byte" },           --操作时间

      {k = "lCellScore", t = "score" },           --基础金币
      {k = "cbPlayerCount", t = "byte" },           --玩家人数
      {k = "cbPiaoMode", t = "byte" },           --漂 0不漂，1首次漂，2每局漂
      {k = "cbMaMode", t = "byte" },           --0不买马，1自摸买马，2亮倒自摸买马
      {k = "cbMaCount", t = "byte" },           --0无，1买1马，2买6马，3买1送1
      {k = "cbRule", t = "byte" },           --0全频道，1半频道
      {k = "wTableOwnerID", t = "dword" },
      {k = "cbPlayMode", t = "byte" }, --游戏模式

   }

      --定漂状态
   cmd.CMD_S_StatusPiao =
   {
      {k = "cbTimeStart", t = "byte" },           --开始时间
      {k = "cbTimeCallPiao", t = "byte" },           --叫漂时间
      {k = "cbTimeOutCard", t = "byte" },           --出牌时间
      {k = "cbTimeOperate", t = "byte" },           --操作时间

      {k = "lCellScore", t = "score" },           --基础金币
      {k = "cbPlayerCount", t = "byte" },           --玩家人数
      {k = "cbPiaoMode", t = "byte" },           --漂 0不漂，1首次漂，2每局漂
      {k = "cbMaMode", t = "byte" },           --0不买马，1自摸买马，2亮倒自摸买马
      {k = "cbMaCount", t = "byte" },           --0无，1买1马，2买6马，3买1送1
      {k = "cbRule", t = "byte" },           --0全频道，1半频道

      {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}}, --玩家游戏状态
      {k = "bTrustee", t = "bool", l = {cmd.GAME_PLAYER} },           --是否托管

      {k = "bCallPiao", t = "bool", l = {cmd.GAME_PLAYER} },   --是否叫漂
      {k = "cbUserPiao", t = "byte", l = {cmd.GAME_PLAYER} }, --叫漂倍数

      {k = "wTableOwnerID", t = "dword" },
      {k = "cbPlayMode", t = "byte" }, --游戏模式

   }

   --游戏状态
   cmd.CMD_S_StatusPlay =
   {
      {k = "cbTimeStart", t = "byte" },           --开始时间
      {k = "cbTimeCallPiao", t = "byte" },           --叫漂时间
      {k = "cbTimeOutCard", t = "byte" },           --出牌时间
      {k = "cbTimeOperate", t = "byte" },           --操作时间

      {k = "lCellScore", t = "score" },           --单元积分
      {k = "cbPlayerCount", t = "byte" },           --玩家人数
      {k = "cbPiaoMode", t = "byte" },           --漂 0不漂，1首次漂，2每局漂
      {k = "cbMaMode", t = "byte" },           --0不买马，1自摸买马，2亮倒自摸买马
      {k = "cbMaCount", t = "byte" },           --0无，1买1马，2买6马，3买1送1
      {k = "cbRule", t = "byte" },           --0全频道，1半频道

      --游戏变量
      {k = "wBankerUser", t = "word" },           --庄家用户
      {k = "wCurrentUser", t = "word" },           --当前用户

      --状态变量
      {k = "cbActionCard", t = "byte" },           --动作扑克
      {k = "cbActionMask", t = "byte" },           --动作掩码
      {k = "cbHearStatus", t = "byte", l = {cmd.GAME_PLAYER} },       --听牌状态
      {k = "cbLeftCardCount", t = "byte" },                           --剩余数目
      {k = "bTrustee", t = "bool", l = {cmd.GAME_PLAYER} },           --是否托管

      --出牌信息
      {k = "wOutCardUser", t = "word" },            --出牌用户
      {k = "cbOutCardData", t = "byte" },           --出牌扑克
      {k = "cbDiscardCount", t = "byte", l = {cmd.GAME_PLAYER} },             --丢弃数目
      {k = "cbDiscardCard", t = "byte", l = {60, 60, 60} },          --丢弃记录

      --扑克数据
      {k = "cbCardCount", t = "byte", l = {cmd.GAME_PLAYER} },                                       --扑克数目
      {k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT} },                   --扑克列表
      {k = "cbSendCardData", t = "byte" },                                    --发送扑克
      {k = "cbCardIndex", t = "byte", l = {cmd.MAX_INDEX,cmd.MAX_INDEX,cmd.MAX_INDEX} }, --听牌数据
      {k = "cbLinstenCardData", t = "byte", l = {cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT} },                                 --听牌数据

      --组合扑克
      {k = "cbWeaveCount", t = "byte", l = {cmd.GAME_PLAYER} },     --组合数目
      {k = "WeaveItemArray", t = "table", d=cmd.CMD_WeaveItem, l = {cmd.MAX_WEAVE, cmd.MAX_WEAVE, cmd.MAX_WEAVE} },  --组合扑克

      --堆立信息
      {k = "wHeapHead", t = "word" }, --堆立头部
      {k = "wHeapTail", t = "word" }, --堆立尾部
      {k = "cbHeapCardInfo", t = "byte", l = {2, 2, 2} }, --堆牌信息

      --胡牌信息
      {k = "cbHuCardCount", t = "byte", l = {cmd.GAME_PLAYER} },
      {k = "cbHuCardData", t = "byte",l = {cmd.MAX_COUNT, cmd.MAX_COUNT, cmd.MAX_COUNT} },

      --叫漂情况
      {k = "cbUserPiao", t = "byte", l = {cmd.GAME_PLAYER} },
      --房主
      {k = "wTableOwnerID", t = "dword" },

      {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}}, --玩家游戏状态

      {k = "cbPlayMode", t = "byte" }, --游戏模式
   }

   --游戏开始
   cmd.CMD_S_GameStart =
   {
      {k = "lSiceCount", t = "int" },     --骰子点数
      {k = "wBankerUser", t = "word" },   --庄家用户
      {k = "wCurrentUser", t = "word" },  --当前用户
      {k = "cbUserAction", t = "byte" },  --用户动作
      {k = "cbCardData", t = "byte", l={cmd.MAX_COUNT} },    --扑克列表
      {k = "wHeapHead", t = "word" },     --堆立牌头
      {k = "wHeapTail", t = "word" },     --堆立牌尾
      {k = "cbHeapCardInfo", t = "byte", l = {cmd.GAME_PLAYER, cmd.GAME_PLAYER} },   --堆立信息
      {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}}, --玩家游戏状态
   }

   --出牌命令
   cmd.CMD_S_OutCard =
   {
      {k = "wOutCardUser", t = "word" },     --出牌用户
      {k = "cbOutCardData", t = "byte" },     --出牌扑克
      {k = "bSysOut", t = "bool" },     --托管出牌
   }

   --发送扑克
   cmd.CMD_S_SendCard =
   {
      {k = "cbCardData", t = "byte" },     --扑克数据
      {k = "cbActionMask", t = "byte" },     --动作掩码
      {k = "wCurrentUser", t = "word" },     --当前用户
      {k = "bTail", t = "bool"},                  --末尾发牌
   }

   --操作提示
   cmd.CMD_S_OperateNotify =
   {
      {k = "wProvideUser", t = "word" },     --供应用户
      {k = "cbActionMask", t = "byte" },     --动作掩码
      {k = "cbActionCard", t = "byte" },     --动作扑克
   }

   --操作命令
   cmd.CMD_S_OperateResult =
   {
      {k = "wOperateUser", t = "word" },     --操作用户
      {k = "wProvideUser", t = "word" },     --供应用户
      {k = "cbOperateCode", t = "byte" },     --操作代码
      {k = "cbOperateCard", t = "byte", l={3} },     --操作扑克
      {k = "cbActionMask", t = "byte" },     --是否亮倒
   }

   --游戏结束
   cmd.CMD_S_GameEnd =
   {
      --结束信息
      {k = "wProvideUser", t = "word" },     --供应用户
      {k = "cbProvideCard", t = "byte" },     --供应扑克
      {k = "dwChiHuKind", t = "dword", l={cmd.GAME_PLAYER} },     --胡牌类型
      {k = "dwChiHuRight", t = "dword", l={cmd.GAME_PLAYER} },     --胡牌类型
      {k = "cbHuaCardCount", t = "byte" },     --花牌个数
      {k = "cbFanCount", t = "byte", l={cmd.GAME_PLAYER} },     --总番数

      --积分信息
      {k = "lGameScore", t = "score", l={cmd.GAME_PLAYER} },     --游戏积分
      --扑克信息
      {k = "cbCardCount", t = "byte", l={cmd.GAME_PLAYER} },     --扑克数目
      {k = "cbCardData", t = "byte", l={cmd.MAX_COUNT, cmd.MAX_COUNT, cmd.MAX_COUNT,} },     --扑克数据

      {k = "cbWeaveItemCount", t = "byte", l={cmd.GAME_PLAYER} },     --组合数目
      {k = "WeaveItemArray", t = "table", d=cmd.CMD_WeaveItem, l={cmd.MAX_WEAVE, cmd.MAX_WEAVE, cmd.MAX_WEAVE} },     --组合数目
      {k = "lPiaoScore", t = "score", l={cmd.GAME_PLAYER} },     --漂输赢
      {k = "lGangScore", t = "score", l={cmd.GAME_PLAYER} },     --杠输赢
      {k = "lMaScore", t = "score", l={cmd.GAME_PLAYER} },     --马输赢
      {k = "cbMaFan", t = "byte" },     --中码倍数
      {k = "cbMaCount", t = "byte" },     --码数量
      {k = "cbMaData", t = "byte", l={6} },     --码数据
   }

   --用户托管
   cmd.CMD_S_Trustee =
   {
      {k = "bTrustee", t = "bool" },     --是否托管
      {k = "wChairID", t = "word" },     --托管用户
   }

   --用户听牌--亮倒返回
   cmd.CMD_S_Listen =
   {
      {k = "wChairId", t = "word" },     --听牌用户
      {k = "cbCardIndex", t = "byte", l = {cmd.MAX_INDEX} },     --扑克数据
      {k = "cbSelectCard", t = "byte", l = {cmd.MAX_COUNT} },     --被盖住的牌
      {k = "cbSelectCount", t = "byte"},     --盖住牌的数量
   }

   --杠牌命令
   cmd.CMD_S_GangCard =
   {
      {k = "wChairId", t = "word" },     --杠牌用户
      {k = "cbGangScore", t = "score", l = {cmd.GAME_PLAYER} },     --杠牌分数
      {k = "cbCardIndex", t = "byte", l = {cmd.MAX_INDEX} },     --玩家扑克数据
      {k = "cbSelectCount", t = "byte" },     --盖牌数量
      {k = "cbSelectCard", t = "byte", l = {cmd.MAX_COUNT} },     --盖住的牌数据
   }

   --这个是玩家叫漂的消息,不是叫漂通知的消息
   cmd.CMD_S_BeginPiao =
   {
      {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}}, --玩家游戏状态
   }

   cmd.SUB_S_TING_DATA = {
    {k = "cbOutCardCount", t = "byte" },     --出那几张能听
    {k = "cbOutCardData", t = "byte", l = {cmd.MAX_COUNT} },
    {k = "cbHuCardCount", t = "byte", l = {cmd.MAX_COUNT}}, --听以后能胡那几张牌
    {k = "cbHuCardData", t = "byte", l = {28,28,28,28,28,
                                          28,28,28,28,28,
                                          28,28,28,28} },
    {k = "cbHuCardRemainingCount", t = "byte", l = {28,28,28,28,28,
                                                    28,28,28,28,28,
                                                    28,28,28,28} }, --胡牌剩余张数
    --胡牌番数
    {k = "cbHuFan", t = "byte", l={ 28,28,28,28,28,
                                    28,28,28,28,28,
                                    28,28,28,28} },
    --能盖的牌
    {k="cbSelectCount", t="byte"},
    {k="cbSelectCardData", t="byte", l={4}}, --盖住的牌就不能打了,这里要注意1122233，
    --如果盖住了222就不能再打2了,客户端要判断
  }

   --------------------------------------------------------------------------
  --客户端命令结构
  cmd.SUB_C_OUT_CARD           =    1                                   --出牌命令
  cmd.SUB_C_OPERATE_CARD       =    2                                   --操作扑克
  cmd.SUB_C_CALLPIAO          =    3                                   --玩家叫漂
  cmd.SUB_C_LISTEN             =    4                                   --用户听牌
  cmd.SUB_C_TRUSTEE            =    5                                   --用户托管
  cmd.SUB_C_RECORD            =     6                                    --

   --叫漂命令
   cmd.CMD_C_CallPiao =
   {
      {k = "cbPiao", t = "byte" } --0,1,2
   }
   --出牌命令
   cmd.CMD_C_OutCard =
   {
      {k = "cbCardData", t = "byte" }   --扑克数据
   }

   --操作命令
   cmd.CMD_C_OperateCard =
   {
      {k = "cbOperateCode", t = "byte" },   --操作代码
      {k = "cbOperateCard", t = "byte", l={3} },   --操作扑克
   }

   --用户听牌--亮倒
   cmd.CMD_C_Listen =
   {
      {k = "cbListen", t = "byte" },   --听牌用户--写死1
      {k = "cbSelectCard", t = "byte", l={14} },  --被盖住的牌
      {k = "cbSelectCount", t = "byte" },   --盖住牌的数量
   }

   --用户托管
   cmd.CMD_C_Trustee =
   {
      {k = "bTrustee", t = "bool" }   --是否托管
   }
   --------------------------------------------------------------------------

return cmd

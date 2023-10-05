local initpath = string.format("%syule.%s.src.init", appdf.GAME_SRC, "sparrowxykwx")
require(initpath)

--
-- Author: David
-- Date: 2017-08-10 15:30:04
--
-- 包含录像处理, 单局结算详情界面

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.sparrowxykwx.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
--local GameResultLayer = appdf.req(module_pre .. ".views.layer.ResultLayer")
local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src.views.layer.ResultLayer")

local VideoResultLayer = appdf.req(module_pre .. ".gamevideo.GameVideoResultLayer")


function GameVideoReplay:ctor( scene )
    GameVideoReplay.super.ctor(self)
end

-- 回放邀请信息
function GameVideoReplay:getShareMessage( tabParam )
    local playbackcode = tabParam.dwPlayBackCode or 0
    local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
    return
    {
        title = "回放码:" .. playbackcode,
        content = "卡五星牌局回放,广东麻将游戏精彩刺激，一起来玩吧！",
        url = url,
        imagepath = "",
        isImage = "false",
    }
end

-- 获取单局结算界面
function GameVideoReplay:getSingleGameSettlement( tabParam )
    -- todo
    return nil
end

-- 开始回放
function GameVideoReplay:onStartReplay(gameLayer)
    -- 隐藏设置
end

--[[
//录像数据
struct Video_GameStart
{
    TCHAR                           szNickName[LEN_NICKNAME];           //用户昵称
    SCORE                           lScore;                             //积分
    LONG                                lCellScore;                         //底分
    WORD                            wChairID;                           //椅子ID
    WORD                            wBankerUser;                        //庄家
    LONG                                lSiceCount;                         //发牌骰子
    BYTE                                cbUserAction;                       //用户动作
    BYTE                                cbCardData[MAX_COUNT];  //扑克列表
    WORD                            wHeapHead;                                  //堆立牌头
    WORD                            wHeapTail;                                      //堆立牌尾
    BYTE                                cbHeapCardInfo[GAME_PLAYER][2]; //堆立信息

                                                                        //规则
    BYTE                                cbPlayerCount;          //玩家人数
    BYTE                                cbPiaoMode;         //漂模式 0不漂，1漂一次，2每次漂
    BYTE                                cbMaMode;               //0不买马，1自摸买马，2亮倒自摸买马
    BYTE                                cbMaCount;              //0无，1买1马，2买6马，3买1送1
    BYTE                                cbRule;                 //0全频道，1半频道
};
]]
local video_struct = {
    {k = "szNickName", t = "string", s = 32},
    {k = "lScore", t = "score"},
    {k = "lCellScore", t = "int"},
    {k = "wChairID", t = "word"},
    {k = "wBankerUser", t = "word"},
    {k = "wSiceCount", t = "int"},
    {k = "cbUserAction", t = "byte"},
    {k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},
    {k = "wHeapHead", t = "word"},
    {k = "wHeapTail", t = "word"},
    {k = "cbHeapCardInfo", t = "byte", l = {2,2,2}},

    {k = "cbPlayerCount", t = "byte"},
    {k = "cbPiaoMode", t = "byte"},
    {k = "cbMaMode", t = "byte"},
    {k = "cbMaCount", t = "byte"},
    {k = "cbRule", t = "byte"},

    {k = "cbUserPiao", t = "byte"},
}


-- 消息读取
function GameVideoReplay:onReadMessage( pBuffer )
    local datalen = pBuffer:getlen()
    local curlen = pBuffer:getcurlen()
    if curlen < datalen then
        local sub = pBuffer:readword()
        print("GameVideoReplay:onReadMessage======================================", sub, curlen, datalen)
        if cmd.SUB_S_GAME_START == sub then
            -- 发牌消息
            --[[
            local t = --发送扑克/游戏开始
            {
                {k = "szNickName", t = "string", s = 32},                   --用户昵称
                {k = "lScore", t = "score"},                                --用户分数(游戏币)
                {k = "lCellScore", t = "int"},                              --底分
                {k = "wChairID", t = "word"},                               --椅子ID
                {k = "wBankerUser", t = "word"},                            --庄家
                {k = "wSiceCount", t = "word"},                             --骰子点数
                {k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},        --麻将列表
                {k = "cbMagicIndex", t = "byte", l = {cmd.MAX_MAGIC_COUNT}},   --鬼牌
                {k = "cbUserAction", t = "byte"},                           --用户动作
                {k = "cbFanCardData", t = "byte"},                           --翻鬼牌

                --规则
                {k = "cbAllCardCount", t = "byte"},                         --牌数量
                {k = "cbPlayerCount", t = "byte"},                          --玩家人数
                {k = "cbMaCount", t = "byte"},                              --码数
                {k = "cbMagicMode", t = "byte"},                            --翻鬼模式
                {k = "bQiangGangHu", t = "bool"},                           --抢杠胡
                {k = "bHuQiDui", t = "bool"},                               --胡七对
                {k = "bHaveZiCard", t = "bool"},                            --带字牌
                {k = "bNoMagicDouble", t = "bool"},                         --无鬼加倍
            }
            ]]
            local t = video_struct

            local cmdtable = {}
            cmdtable.msgsub = cmd.SUB_S_GAME_START
            cmdtable.startinfo = {}

            local playerData = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, playerData)

            for i=1,playerData.cbPlayerCount-1 do
                playerData = ExternalFun.read_netdata(t, pBuffer)
                table.insert(cmdtable.startinfo, playerData)
            end

            dump(cmdtable.startinfo, "#cmdtable.startinfo==", 10)
            return cmdtable
        elseif cmd.SUB_S_OUT_CARD == sub then
            -- 用户出牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_OutCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_OUT_CARD
            dump(cmdtable, "#SUB_S_OUT_CARD==")
            return cmdtable
        elseif cmd.SUB_S_SEND_CARD == sub then
            -- 用户发牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_SendCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_SEND_CARD
            dump(cmdtable, "#SUB_S_SEND_CARD==")
            return cmdtable
        elseif cmd.SUB_S_OPERATE_NOTIFY == sub then
            -- 操作命令
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_OperateNotify, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_OPERATE_NOTIFY
            dump(cmdtable, "#SUB_S_OPERATE_NOTIFY==")
            return cmdtable
        elseif cmd.SUB_S_OPERATE_RESULT == sub then
            -- 操作结果
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_OperateResult, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_OPERATE_RESULT
            dump(cmdtable, "#SUB_S_OPERATE_RESULT==")
            return cmdtable
        elseif cmd.SUB_S_GAME_END == sub then
            -- 游戏结果
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_GAME_END
            dump(cmdtable, "#SUB_S_GAME_END==")
            return cmdtable
        -- elseif 112 == sub then
        --     -- 操作提示
        --     local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_RECORD, pBuffer)
        --     cmdtable.msgsub = 112
        --     return cmdtable
        elseif cmd.SUB_S_GANG == sub then
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GangCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_GANG
            return cmdtable
        end
    end
    return nil
end

-- 约战总结算
function GameVideoReplay:onGetPrivateEndMessage( videoDetailInfo, userList )
    local cmdtable = {}
    cmdtable.msgsub = 2000

    local scorelist = {}
    for k,v in pairs(userList) do
        scorelist[v.wChairID + 1] = v.dwTotalScore
    end
    cmdtable.lScore = {}
    cmdtable.lScore[1] = scorelist

    return cmdtable
end

-- 消息处理
function GameVideoReplay:onHandleMessage( gameLayer, msgTable, bForward, bBackward )
    bForward = bForward or false
    bBackward = bBackward or false
    local sub = msgTable.msgsub
    if nil == gameLayer or nil == gameLayer._gameView then
        return nil, false, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_VIEW_ERROR
    end
    -- dump(msgTable, "#onHandleMessage===", 6)
    if cmd.SUB_S_GAME_START == sub then
        -- 发牌消息
        print("回放 开始")
        return self:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_OUT_CARD == sub then
        -- 发牌
        print("回放 出牌")
        return self:onGetVideoOutcard(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_SEND_CARD == sub then
        -- 发牌
        print("回放 发牌")
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_OPERATE_NOTIFY == sub then
        -- 开局换牌
        print("回放 提示")
        return self:onGetVideoOperate(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_OPERATE_RESULT == sub then
        -- 不出
        print("回放 操作")
        return self:onGetVideoOperateResult(gameLayer, msgTable, bForward, bBackward)
    elseif 112 == sub then
        -- 牌局统计
        print("回放 统计")
        return self:onGetVideoGameRecord(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_GAME_END == sub then
        -- 牌局统计
        print("回放 结果")
        return self:onGetVideoGameConclude(gameLayer, msgTable, bForward, bBackward)
    elseif 2000 == sub then
        -- 总结算
        print("回放 总结算")
        return self:onGetPrivateRoundEnd(gameLayer, msgTable, bForward, bBackward)
    end
    return 10
end

-- 回放场景
function GameVideoReplay:onGetVideoScene(gameLayer, msgTable, bForward, bBackward)
    return 5, true
end

-- 游戏开始
function GameVideoReplay:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    -- 界面重置
    gameLayer:OnResetGameEngine()
    gameLayer._gameView.bIsVideo = true
    gameLayer.cbTimeOutCard = 0
    gameLayer.cbTimeOperateCard = 0
    gameLayer.cbTimeStartGame = 0

    if nil ~= gameLayer._gameView._resultLayer then
        gameLayer._gameView._resultLayer:hideLayer()
        gameLayer._gameView._resultLayer = nil
    end

    if bBackward then
        print("快退处理 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        -- 更新
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    local msg = {}
    msg.msgsub = msgTable.msgsub

    local userCard = {}
    local wViewBankerUser

    for k,v in pairs(msgTable.startinfo) do
        print("发牌椅子 ==> ", v.wChairID)
        -- 视图转换
        local viewId = gameLayer:SwitchViewChairID(v.wChairID)
        print("转换视图 ==> ", viewId)

        -- 发牌
        gameLayer.wBankerUser = v.wBankerUser
        print("gameLayer.wBankerUser==",gameLayer.wBankerUser)
        wViewBankerUser = gameLayer:SwitchViewChairID(gameLayer.wBankerUser)

        userCard[viewId] = {}
        for i=1,cmd.MAX_COUNT do
            if v.cbCardData[1][i] ~= 0 then
                table.insert(userCard[viewId], v.cbCardData[1][i])
                table.insert(gameLayer.cbHandCardData[viewId], v.cbCardData[1][i])
            end
        end

        local cbCardCount = {0, 0, 0, 0}

        local nChairCount = gameLayer._gameFrame:GetChairCount()
        for i = 1, nChairCount do
            local wViewChairId = gameLayer:SwitchViewChairID(i -1)
            cbCardCount[wViewChairId] = 13
            if wViewChairId == wViewBankerUser then
                cbCardCount[wViewChairId] = cbCardCount[wViewChairId] + 1
            end
        end
        if v.wChairID == gameLayer:GetMeChairID() then

            gameLayer.lCellScore = v.lCellScore
            --历史积分

            --规则
            gameLayer.cbPlayerCount = v.cbPlayerCount or 4
            gameLayer.cbMaCount = v.cbMaCount
            gameLayer.cbMagicMode = v.cbMagicMode
            gameLayer.bQiangGangHu = v.bQiangGangHu
            gameLayer.bHuQiDui = v.bHuQiDui
            gameLayer.bHaveZiCard = v.bHaveZiCard
            gameLayer.bNoMagicDouble = v.bNoMagicDouble
            gameLayer.cbAllCardCount =84 --v.cbAllCardCount
            gameLayer._gameView.m_nAllCard = gameLayer.cbAllCardCount


            --设置癞子
            -- gameLayer._gameView._cardLayer.cbMagicData = v.cbMagicIndex[1]
            --设置信息
            local piaoMode, channel, maMode, maNum = v.cbPiaoMode, v.cbRule, v.cbMaMode, v.cbMaCount
            gameLayer._gameView:onshowRule( piaoMode, channel, maMode, maNum)
            --设置牌数
            gameLayer._gameView:onUpdataLeftCard(gameLayer.cbAllCardCount)
        end

        if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            if 1 == msgTable.nGameRound then
                PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
            end
            print("==========================================》》》》", PriRoom:getInstance().m_tabPriData.lIniScore)
        end

        -- 用户游戏币(分数)
        gameLayer._gameView.m_tabUserHead[viewId].m_userItem.lScore = v.lScore
        -- gameLayer._gameView.m_tabUserHead[viewId]:updateStatus()
        gameLayer._gameView.m_tabUserHead[viewId]:refresh(gameLayer._gameView.m_tabUserHead[viewId].m_userItem)

    end
    msg.userHandCards = userCard

    gameLayer._gameView:gameVideoStart(gameLayer.wBankerUser, gameLayer.cbHandCardData, gameLayer._gameView._cardLayer.cbMagicData)
    --设置时间
    gameLayer:SetGameClock(wViewBankerUser, cmd.IDI_OUT_CARD, 0) --加的8为发牌时间

    print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
    GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg

    -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("回放约战房")
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end

    --存储数据
    --鬼牌
    gameLayer.cbMagicCard = gameLayer._gameView._cardLayer.cbMagicData
    gameLayer.leftCard = gameLayer.leftCard -13 *4 -1


    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 100, true
end

-- 显示癞子
function GameVideoReplay:onGetVideoMagicCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        gameLayer._gameView._cardLayer.cbMagicData = msgTable.m_magicReplaceCard[#msgTable.m_magicReplaceCard]
        gameLayer.m_magicReplaceCard = msgTable.m_magicReplaceCard


        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 10, true
end

-- 开局换牌
function GameVideoReplay:onGetVideoReplceCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")

        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

        -- 用户切换
        for k,v in pairs(gameLayer._gameView.m_tabUserHead) do
            v:reSet()
        end

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        -- 更新
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    local nChairCount = gameLayer._gameFrame:GetChairCount()
    for i=1,nChairCount do
        local viewId = gameLayer:SwitchViewChairID(i -1)
        gameLayer.m_userReplaceCard[viewId] = msgTable.m_userReplaceCard[i -1]
    end

    if true then
        print("开局换牌 @@@@@@@@@@@@@@@@@@@@@@========>>", msgTable.msgsub)
        gameLayer._gameView:gameVideoStart(gameLayer.wBankerUser, gameLayer.cbHandCardData)
    end



    --存储数据
    local flowerNum = 0 --花牌总数
    for i=1,nChairCount do
        local viewId = gameLayer:SwitchViewChairID(i -1)
        --手牌
        local replaceInfo = msgTable.m_userReplaceCard[i -1]
        for j=1,#replaceInfo.cardData do
            local cardData = replaceInfo.cardData[j]
            local cardHuaData = replaceInfo.cardHuaData[j]
            for m=1,#cardData do
                table.insert(gameLayer.cbHandCardData[viewId], cardData[m])
                table.insert(gameLayer.cbFlowerCardData[viewId], cardHuaData[m])
                --删掉手牌中花牌
                for n = #gameLayer.cbHandCardData[viewId], 1, -1 do
                    if gameLayer.cbHandCardData[viewId][n] == cardHuaData[m] then
                        table.remove(gameLayer.cbHandCardData[viewId], n)
                        break
                    end
                end
            end
        end
        flowerNum = flowerNum + #gameLayer.cbFlowerCardData[viewId]
    end
    --翻鬼牌中花牌
    if #gameLayer.m_magicReplaceCard > 2 then
        local viewId = gameLayer:SwitchViewChairID(gameLayer.wBankerUser)
        for i=1,#gameLayer.m_magicReplaceCard -2 do
            table.insert(gameLayer.cbFlowerCardData[viewId], gameLayer.m_magicReplaceCard[i])
        end
    end
    --鬼牌
    gameLayer.cbMagicCard = gameLayer.m_magicReplaceCard[#gameLayer.m_magicReplaceCard]
    gameLayer.leftCard = gameLayer.leftCard -13 *4 -#gameLayer.m_magicReplaceCard - flowerNum

    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 90, true
end
-- 回放出牌

function GameVideoReplay:onGetVideoOutcard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    local wViewId = gameLayer:SwitchViewChairID(msgTable.wOutCardUser)
    if true then
        gameLayer._gameView._cardLayer:outCardVedio(wViewId, msgTable.cbOutCardData, true)
        gameLayer:playCardDataSound(msgTable.wOutCardUser, msgTable.cbOutCardData)
    end

    --存储界面数据
    table.insert(gameLayer.cbOutCardData[wViewId], msgTable.cbOutCardData)
    for i=1,#gameLayer.cbHandCardData[wViewId] do
        if gameLayer.cbHandCardData[wViewId][i] == msgTable.cbOutCardData then
            table.remove(gameLayer.cbHandCardData[wViewId], i)
            break
        end
    end

    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 20, true
end

-- 回放发牌
function GameVideoReplay:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    -- 视图转换
    local viewId = gameLayer:SwitchViewChairID(msgTable.wCurrentUser)
    if true then
        gameLayer._gameView:gameSendCard(viewId, msgTable.cbCardData)
    end

    table.insert(gameLayer.cbHandCardData[viewId], msgTable.cbCardData)
    gameLayer.leftCard = gameLayer.leftCard -1

    --print("转换视图 ==> ", viewId)
    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 15, true
end


function GameVideoReplay:onGetVideoOperate(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    return 5, true
end

-- 回放操作结果
function GameVideoReplay:onGetVideoOperateResult(gameLayer, msgTable, bForward, bBackward)
    local bNoDelay = (bBackward or bForward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    local wOperateViewId = gameLayer:SwitchViewChairID(msgTable.wOperateUser)
    local wProvideViewId = gameLayer:SwitchViewChairID(msgTable.wProvideUser)
    local tagAvtiveCard = {}
    tagAvtiveCard.cbType = GameLogic.SHOW_NULL
    if msgTable.cbOperateCode == GameLogic.WIK_GANG then
        --判断杠类型
        if wOperateViewId == wProvideViewId then  --暗杠
            tagAvtiveCard.cbType = GameLogic.SHOW_AN_GANG
        else
            for i=1,#gameLayer._gameView._cardLayer.cbActiveCardData[wOperateViewId] do
                --查找是否已经碰
                local activeInfo = gameLayer._gameView._cardLayer.cbActiveCardData[wOperateViewId][i]
                if activeInfo.cbCardValue[1] == msgTable.cbOperateCard[1][1] then --有碰
                    tagAvtiveCard.cbType = GameLogic.SHOW_MING_GANG
                end
            end
            if tagAvtiveCard.cbType == GameLogic.SHOW_NULL then
                tagAvtiveCard.cbType = GameLogic.SHOW_FANG_GANG
            end
        end
        tagAvtiveCard.cbCardNum = 4
        tagAvtiveCard.cbCardValue = {}
        for i=1,tagAvtiveCard.cbCardNum do
            tagAvtiveCard.cbCardValue[i] = msgTable.cbOperateCard[1][1]
        end
        --再加一个
        --tagAvtiveCard.cbCardValue[4] = cmd_data.cbOperateCard[1][1]
        print("操作的麻将信息", tagAvtiveCard.cbType, tagAvtiveCard.cbCardNum, tagAvtiveCard.cbCardValue[1])
    end

    if msgTable.cbOperateCode == GameLogic.WIK_PENG then
        tagAvtiveCard.cbType = GameLogic.SHOW_PENG
        tagAvtiveCard.cbCardValue = {}
        tagAvtiveCard.cbCardNum = 3
        for i=1,tagAvtiveCard.cbCardNum do
            tagAvtiveCard.cbCardValue[i] = msgTable.cbOperateCard[1][1]
        end
    end

    tagAvtiveCard.wProvideViewId = wProvideViewId

    -- 显示操作动作
    gameLayer._gameView._cardLayer:createActiveCard(wOperateViewId, tagAvtiveCard)
    gameLayer._gameView:showOperateAction(wOperateViewId, msgTable.cbOperateCode)
    gameLayer:playCardOperateSound(msgTable.wOperateUser, false, msgTable.cbOperateCode)

    table.insert(gameLayer.cbActivityCardData[wOperateViewId], tagAvtiveCard)
    local cbCardData = clone(tagAvtiveCard.cbCardValue)
    for i=1,tagAvtiveCard.cbCardNum do
        if msgTable.cbOperateCard[1][1] == cbCardData[i] and wOperateViewId ~= wProvideViewId then
            table.remove(cbCardData, i)
            table.remove(gameLayer.cbOutCardData[wProvideViewId], #gameLayer.cbOutCardData[wProvideViewId])
            break
        end
    end
    --删除手牌
    for i=1,#cbCardData do
        for j = #gameLayer.cbHandCardData[wOperateViewId], 1, -1 do
            if gameLayer.cbHandCardData[wOperateViewId][j] == cbCardData[i] then
                table.remove(gameLayer.cbHandCardData[wOperateViewId], j)
                break
            end
        end
    end

    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 20, false
end

-- 回放数据记录
function GameVideoReplay:onGetVideoGameRecord(gameLayer, msgTable, bForward, bBackward)
    local bNoDelay = (bBackward or bForward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    gameLayer.m_userRecord = {}
    local nInningsCount = msgTable.nCount
    local nChairCount = gameLayer._gameFrame:GetChairCount()
    for i = 1, nChairCount do
        gameLayer.m_userRecord[i] = {}
        gameLayer.m_userRecord[i].cbHuCount = msgTable.cbHuCount[1][i]
        gameLayer.m_userRecord[i].cbMingGang = msgTable.cbMingGang[1][i]
        gameLayer.m_userRecord[i].cbAnGang = msgTable.cbAnGang[1][i]
        gameLayer.m_userRecord[i].cbMaCount = msgTable.cbMaCount[1][i]
    end

end

-- 回放结算
function GameVideoReplay:onGetVideoGameConclude(gameLayer, msgTable, bForward, bBackward)

    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end


    -- local jsonStr = cjson.encode(msgTable)
    -- LogAsset:getInstance():logData(jsonStr, true)

    dump(msgTable, "#msgTable===")

   local bMeWin = false
    --先显示翻马界面
    --获取胡牌玩家
    local HuView = 0
    local HuChair = yl.INVALID_CHAIR

    local playerNum = gameLayer:getPlayerNum()
    for i = 1, playerNum do
        local viewid = gameLayer:SwitchViewChairID(i - 1)
        if msgTable.lGameScore[1][i] > 0 then
            HuView = viewid
            HuChair = i - 1

            --显示胡牌动画
            gameLayer._gameView:showOperateAction(viewid, GameLogic.WIK_CHI_HU)
        end
    end


    --如果没人胡，则是荒庄
    if 0 ==  HuView and msgTable.wFleeUser == yl.INVALID_CHAIR then
        --显示荒庄
        gameLayer._gameView:showNoWin(true)
        --显示玩家手牌

        for i = 1, playerNum do
            --手牌
            local cbCardData = {}
            local viewid = gameLayer:SwitchViewChairID(i - 1)
            for j = 1, msgTable.cbCardCount[1][i] do
                cbCardData[j] = msgTable.cbHandCardData[i][j]
            end
            --显示玩家手牌
            gameLayer._gameView._cardLayer:showUserTileMajong( viewid, cbCardData)
            --杠碰牌显示
            gameLayer._gameView._cardLayer:tileActiveCard(viewid)
        end
        return
    end

    --开启了无鬼翻倍，而且没有鬼牌
    local isNoMagicCard = (msgTable.bNoMagicCard and gameLayer.bNoMagicDouble)
    print("@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#", isNoMagicCard)

    local maNum = msgTable.cbMaCount;
    local validMaNum = msgTable.cbMaFan;
    local maData = {}
    for i = 1, maNum do
        maData[i] = msgTable.cbMaData[1][i]
    end
    local validMaData = {}
    for i=1,validMaNum do
        validMaData[i] = msgTable.cbZhongMaData[1][i]
    end
    if 0 ~= HuView and maNum > 0 then
        gameLayer._gameView:showMoMaAction(gameLayer.wBankerUser, HuChair, HuView, maNum, maData, validMaNum, validMaData, 2)
    end

    local resultList = {}
    for i = 1, playerNum do
        local viewid = gameLayer:SwitchViewChairID(i - 1)
        local lScore = msgTable.lGameScore[1][i]
        local lHuScore = msgTable.lPiaoScore[1][i] --msgTable.lHuScore[1][i]
        if lHuScore > 0 and viewid == cmd.MY_VIEWID then
            bMeWin = true
        end

        local result = {}
        --result.userItem = gameLayer._gameFrame:getTableUserItem(gameLayer:GetMeTableID(), i - 1)
        result.userItem = gameLayer._gameView.m_tabUserHead[viewid].m_userItem

        result.lScore = lScore
        result.lHuScore = lHuScore
        result.lGangScore = msgTable.lGangScore[1][i]
        result.lMaScore = msgTable.lMaScore[1][i]
        result.cbChHuKind = msgTable.dwChiHuKind[1][i]
        --胡牌类型
        result.dwChiHuRight = {}
        for j=1,cmd.MAX_RIGHT_COUNT do
            local dwChiHuRightPlayer = msgTable.dwChiHuRight[i] or {}
            result.dwChiHuRight[j] = dwChiHuRightPlayer[j] or 0
        end

        result.cbCardData = {}
        --手牌
        for j = 1, msgTable.cbCardCount[1][i] do
            result.cbCardData[j] = msgTable.cbCardData[i][j]
        end
        --如果是我自摸
        if HuChair == i -1 and 0 ~= msgTable.cbProvideCard then
            table.insert(result.cbCardData, msgTable.cbProvideCard)
        end

        --显示玩家手牌
        gameLayer._gameView._cardLayer:showUserTileMajong( viewid, result.cbCardData )
        --杠碰牌显示
        gameLayer._gameView._cardLayer:tileActiveCard(viewid)

        --碰杠牌
        result.cbActiveCardData = gameLayer._gameView._cardLayer.cbActiveCardData[viewid]

        --插入
        table.insert(resultList, result)
    end

    --显示结算框
    local delayTime = maNum *1+ 2
    if msgTable.wFleeUser ~= yl.INVALID_CHAIR then
        delayTime = 0
    end
    if nil == gameLayer._gameView._resultLayer then
        -- gameLayer._gameView._resultLayer = ResultLayer:create(gameLayer._gameView):addTo(gameLayer._gameView, gameLayer._gameView.ZORDER_RESULT):setVisible(false) --结算框
        self:showResultLayer(gameLayer, msgTable)
    end
    gameLayer:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function(ref)
        -- gameLayer._gameView._resultLayer:showLayer(resultList, gameLayer.wBankerUser, gameLayer.myChirID, msgTable.cbProvideCard, maData, validMaNum, validMaData, msgTable.wFleeUser, isNoMagicCard)
    end)))
    --播放音效
    if bMeWin then
        gameLayer:PlaySound(cmd.RES_PATH.."sound/ZIMO_WIN.wav")
    else
        gameLayer:PlaySound(cmd.RES_PATH.."sound/ZIMO_LOSE.wav")
    end


    return 60 +maNum *10, false
end

function GameVideoReplay:showResultLayer(gameLayer, cmd_data)
    local parent = nil
    local dataList = {}
    -- local playerNum = gameLayer:getPlayerNum()
    local playerNum = gameLayer._gameFrame:GetChairCount()
    local cbCardCount = cmd_data.cbCardCount[1]
    local cbCardData = cmd_data.cbCardData
    local WeaveItemArray = cmd_data.WeaveItemArray
    local cbWeaveItemCount = cmd_data.cbWeaveItemCount[1]
    local lPiaoScore = cmd_data.lPiaoScore[1]
    local lGangScore = cmd_data.lGangScore[1]
    local cbFanCount = cmd_data.cbFanCount[1]
    local lGameScore = cmd_data.lGameScore[1]
    local cbMaData = cmd_data.cbMaData[1]
    local provideSid = cmd_data.wProvideUser --谁放炮
    local huType = cmd_data.dwChiHuKind[1] --胡牌类型

    dataList.desCardType = ""
    dataList.maPai = {}
    for k,v in ipairs(cbMaData) do
        if v > 0 then
            table.insert(dataList.maPai, v)
        end
    end

    for playerIdx=1,playerNum do
        local playerData = {}
        playerData.cardData = {}
        --手牌
        for cardIdx=1,cbCardCount[playerIdx] do
            local cardValue = cbCardData[playerIdx][cardIdx]
            if cardValue > 0 then
                table.insert(playerData.cardData, cardValue)
            end
        end

        playerData.outCardIdx = cbCardCount[playerIdx] + 1
        --碰杠牌
        for weaveIdx=1,cbWeaveItemCount[playerIdx] do
            local playerWeaveItems = WeaveItemArray[playerIdx]  --按照玩家来取,分开写,便于排查错误
            local weaveItemData = playerWeaveItems[weaveIdx]    --按照堆索引取
            local _,cbCardData = next(weaveItemData.cbCardData)
            for _,cardValue in ipairs(cbCardData) do
                if cardValue > 0 then
                    table.insert(playerData.cardData, cardValue)
                end
            end
        end
        playerData.piao = lPiaoScore[playerIdx]
        playerData.gang = lGangScore[playerIdx]
        playerData.fan = cbFanCount[playerIdx]
        --自摸provideSid就是自己
        playerData.isFangPao = (provideSid == (playerIdx - 1)) and huType[playerIdx] <= 0
        playerData.isHuPai = huType[playerIdx] > 0
        playerData.isZiMo = provideSid == (playerIdx - 1) and huType[playerIdx] > 0
        playerData.totalScore = lGameScore[playerIdx]


        local viewid = gameLayer:SwitchViewChairID(playerIdx - 1)
        local item = gameLayer._gameView.m_tabUserHead[viewid].m_userItem
        playerData.useritem = clone(item)
        table.insert(dataList, playerData)
    end

    gameLayer._gameView:showResultLayer(gameLayer._gameView, dataList)
end


function GameVideoReplay:onGetPrivateRoundEnd( gameLayer, msgTable, bForward, bBackward )

    if nil ~= gameLayer._gameView._resultLayer then
        gameLayer._gameView._resultLayer:hideLayer()
        gameLayer._gameView._resultLayer = nil
    end
    gameLayer._gameView:showNoWin(false)

    local scorelist = msgTable.lScore[1]
    gameLayer._gameView._priView:onPriGameEnd(msgTable)

    return 40, false
end

-- 回退数据检查
function GameVideoReplay:backwardCheck( msg, msgTab )
    if type(msg) ~= "table" then
        print("回退消息包异常")
        return false, "回退消息包异常"
    end
    -- 消息不一致
    if msg.msgsub ~= msgTab.msgsub then
        print("回退消息包异常, 消息不一致")
        return false, "回退消息包异常"
    end

    return true
end

-- 回退更新牌
function GameVideoReplay:backwardUpdateUserCards( msg, gameLayer )
    gameLayer:stopAllActions()
    gameLayer._gameView:stopAllActions()

    if nil ~= gameLayer._gameView._resultLayer then
        gameLayer._gameView._resultLayer:hideLayer()
        gameLayer._gameView._resultLayer = nil
    end

    -- 更新玩家牌
    local userHandCards = clone(msg.userHandCards)
    --手牌排序
    local nChairCount = gameLayer._gameFrame:GetChairCount()
    for i=1,nChairCount do
        local viewid = gameLayer:SwitchViewChairID(i - 1)
        if #userHandCards[viewid] > 0 then
            table.sort(userHandCards[viewid], function (a, b)
                    if gameLayer._gameView._cardLayer:isMagicCard(a) and gameLayer._gameView._cardLayer:isMagicCard(b) then
                        return a < b
                    elseif gameLayer._gameView._cardLayer:isMagicCard(a) then
                        return true
                    elseif gameLayer._gameView._cardLayer:isMagicCard(b) then
                        return false
                    else
                        return a < b
                    end
            end)
        end
    end

    local userOutCards = clone(msg.userOutCards)
    local userActivityCards = clone(msg.userActivityCards)

    gameLayer.cbHandCardData = userHandCards
    gameLayer.cbOutCardData = userOutCards
    gameLayer.cbActivityCardData = userActivityCards
    gameLayer.cbMagicCard = msg.cbMagicCard
    gameLayer.leftCard = msg.leftCard

    gameLayer._gameView:updateCardsNode(userHandCards, userOutCards, userActivityCards, gameLayer.cbMagicCard, gameLayer.leftCard)
end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer, isForward)
    -- 三个玩家牌
    local userHandCards = {}
    local userOutCards = {}
    local userActivityCards = {}


    local nChairCount = gameLayer._gameFrame:GetChairCount()
    print("nChairCount===>", nChairCount)

    userHandCards = clone(gameLayer.cbHandCardData)
    --手牌排序
    for i=1,nChairCount do
        local viewid = gameLayer:SwitchViewChairID(i - 1)
        print("SwitchViewChairID==", i, viewid)
        if #userHandCards[viewid] > 0 then
            table.sort(userHandCards[viewid], function (a, b)
                    if gameLayer._gameView._cardLayer:isMagicCard(a) and gameLayer._gameView._cardLayer:isMagicCard(b) then
                        return a < b
                    elseif gameLayer._gameView._cardLayer:isMagicCard(a) then
                        return true
                    elseif gameLayer._gameView._cardLayer:isMagicCard(b) then
                        return false
                    else
                        return a < b
                    end
            end)
        end
    end

    userOutCards = clone(gameLayer.cbOutCardData)
    userActivityCards = clone(gameLayer.cbActivityCardData)

    msg.userHandCards = userHandCards
    msg.userOutCards = userOutCards
    msg.userActivityCards = userActivityCards
    msg.cbMagicCard = gameLayer.cbMagicCard
    msg.leftCard = gameLayer.leftCard


    if isForward then
        gameLayer:stopAllActions()
        --gameLayer._gameView:onResetData()
        gameLayer._gameView:stopAllActions()

        gameLayer._gameView:updateCardsNode(userHandCards, userOutCards, userActivityCards, gameLayer.cbMagicCard, gameLayer.leftCard)
    end
end

-- 获取玩家结算界面
function GameVideoReplay:getVideoResultView( cmdlist )
    local msgTab = nil
    for k,v in pairs(cmdlist) do
        if v.msgsub == 108 then --找到总结算
            msgTab = v
            break
        end
    end
    if type(msgTab) ~= "table" then
        print("结算消息包异常")
        return false, "结算消息包异常"
    end

    -- local jsonStr = cjson.encode(msgTab)
    -- LogAsset:getInstance():logData(jsonStr, true)

    --用户列表
    local tabListInfo = GameVideo:getInstance():getVideoListInfo()
    if 0 == #tabListInfo then
        unGetInfoFun()
        return true
    end
    local iteminfo = tabListInfo[1]
    if 0 == #iteminfo then
        unGetInfoFun()
        return true
    end
    -- local userinfo = iteminfo[1]
    -- if nil == userinfo.szPersonalGUID or "" == userinfo.szPersonalGUID then
    --     unGetInfoFun()
    --     return true
    -- end
    dump(iteminfo, "玩家信息", 10)

    local resultList = {}
    for i = 1, 4 do
        local result = {}
        --玩家头像
        if 0 < #iteminfo then
            for j=1,#iteminfo do
                local Iteminfo = iteminfo[j]
                if Iteminfo.wChairID == i -1 then
                    result.userItem = Iteminfo
                    break
                end
            end
        end

        local lScore = msgTab.lGameScore[1][i]
        local lHuScore = msgTab.lHuScore[1][i]

        result.lScore = lScore
        result.lHuScore = lHuScore
        result.lGangScore = msgTab.lGangScore[1][i]
        result.lMaScore = msgTab.lMaScore[1][i]
        result.cbChHuKind = msgTab.cbChiHuKind[1][i]
        --胡牌类型
        result.dwChiHuRight = {}
        for j=1,cmd.MAX_RIGHT_COUNT do
            result.dwChiHuRight[j] = msgTab.dwChiHuRight[i][j]
        end

        --设置癞子
        result.cbMagicData = msgTab.cbMagicIndex[1]

        result.cbCardData = {}
        --手牌
        for j = 1, msgTab.cbCardCount[1][i] do
            result.cbCardData[j] = msgTab.cbHandCardData[i][j]
        end
        --如果是我自摸
        if HuChair == i -1 and 0 ~= msgTab.cbSendCardData then
            table.insert(result.cbCardData, msgTab.cbSendCardData)
        end

             --先设置碰杠出的牌
        if msgTab.cbWeaveItemCount[1][i] > 0 then
            local tabActiveCardData = {}
            for j = 1, msgTab.cbWeaveItemCount[1][i] do
                local cbOperateData = {} --此处为tagAvtiveCard
                cbOperateData.cbCardValue = msgTab.WeaveItemArray[i][j].cbCardData[1]
                dump(cbOperateData.cbCardValue, "操作的牌")
                local nShowStatus = GameLogic.SHOW_NULL
                local cbParam = msgTab.WeaveItemArray[i][j].cbParam
                if cbParam == GameLogic.WIK_GANERAL then
                    if cbOperateData[1] == cbOperateData[2] then    --碰
                        nShowStatus = GameLogic.SHOW_CHI
                    else                                            --吃
                        nShowStatus = GameLogic.WIK_LEFT
                    end
                    cbOperateData.cbCardNum = 3
                    table.remove(cbOperateData.cbCardValue, 4)--去掉末尾0
                elseif cbParam == GameLogic.WIK_MING_GANG then
                    nShowStatus = GameLogic.SHOW_MING_GANG
                    cbOperateData.cbCardNum = 4
                elseif cbParam == GameLogic.WIK_FANG_GANG then
                    nShowStatus = GameLogic.SHOW_FANG_GANG
                    cbOperateData.cbCardNum = 4
                elseif cbParam == GameLogic.WIK_AN_GANG then
                    nShowStatus = GameLogic.SHOW_AN_GANG
                    cbOperateData.cbCardNum = 4
                end
                cbOperateData.cbType = nShowStatus
                -- local wProvideViewId = self:SwitchViewChairID(cmd_data.WeaveItemArray[i][j].wProvideUser)
                -- cbOperateData.wProvideViewId = wProvideViewId

                --排序
                table.sort(cbOperateData.cbCardValue, function (a, b)
                    return a < b
                end)
                table.insert(tabActiveCardData, cbOperateData)
            end
            result.cbActiveCardData = tabActiveCardData
        end

        --插入
        table.insert(resultList, result)
    end

    local ResultLayer =  VideoResultLayer:create(self)
    if nil ~= ResultLayer then
        print("显示结算框", ResultLayer)
        --dump(resultList, "结算信息", 8)
        ResultLayer:showLayer(resultList, msgTab.cbProvideCard)
        return ResultLayer
    end

    return nil
end

return GameVideoReplay
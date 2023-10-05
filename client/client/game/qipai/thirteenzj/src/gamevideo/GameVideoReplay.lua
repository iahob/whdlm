--
-- Author: zhong
-- Date: 2017-03-22 15:30:04
--
-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.qipai.thirteenzj.src"
local cmd = appdf.req(module_pre .. ".models.cmd_game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")

function GameVideoReplay:ctor( scene )
    GameVideoReplay.super.ctor(self)

    --玩家牌数据
    self.m_cardData = {{},{},{},{}}
    self.m_playercount = 1              --玩家数量
    self.m_gamestatus = {false, false, false, false}
    self._bFinishSegment = {false,false,false,false}    --是否摊牌

end

-- 回放邀请信息
function GameVideoReplay:getShareMessage( tabParam )
    local playbackcode = tabParam.dwPlayBackCode or 0
    local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
    return 
    {
        title = "十三水牌局回放",
        content = "十三水牌局回放 回放码:" .. playbackcode .. " 十三水游戏精彩刺激，一起来玩吧！",
        url = url,
        imagepath = "",
        isImage = "false",
    }
end

-- 支持单局结算功能
function GameVideoReplay:enableResultView()
    print("GameVideoReplay:enableResultView")
    return false
end

-- 消息读取
function GameVideoReplay:onReadMessage( pBuffer )

    local datalen = pBuffer:getlen()
    local curlen = pBuffer:getcurlen()
    if curlen < datalen then
        local sub = pBuffer:readword()
        print("onReadMessage", sub)
        if cmd.SUB_S_SEND_CARD_EX == sub then
            -- 发牌消息
            local t = --发送扑克/游戏开始
            {
                {k = "szNickName", t = "string", s = 32},                   --用户昵称
                {k = "wChairID", t = "word"},                               --椅子ID
                {k = "wCurrentUser", t = "word"},                           --当前玩家
                {k = "wBanker", t = "word"},                                --庄家玩家
                {k = "bCardData", t = "byte", l = {cmd.HAND_CARD_COUNT}},   --手上扑克
                {k = "cbPlayCount", t = "byte"},                            --游戏人数
                {k = "bGameStatus", t = "byte", l = {cmd.GAME_PLAYER}},     --游戏状态
                {k = "lChipArray", t = "score", l = {cmd.GAME_PLAYER}},     --玩家下注(游戏币)
                {k = "lScore", t = "score"},                                --积分
                {k = "lCellScore", t = "int"},                              --底分
                {k = "cbMaCard", t = "byte"}                                --码牌
            }
            local cmdtable = {}
            cmdtable.msgsub = cmd.SUB_S_SEND_CARD_EX
            cmdtable.startinfo = {}
            local tmp1 = ExternalFun.read_netdata(t, pBuffer)
 
            table.insert(cmdtable.startinfo, tmp1)
            for i=1,tmp1.cbPlayCount-1 do
                local tmp = ExternalFun.read_netdata(t, pBuffer)
                table.insert(cmdtable.startinfo, tmp)
            end
            --dump(cmdtable.startinfo, "onReadMessage", 10)
            return cmdtable
        elseif cmd.SUB_S_SHOW_CARD == sub then
            -- 摊牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_ShowCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_SHOW_CARD
            --dump(cmdtable, "onReadMessage", 10)
            return cmdtable
        elseif cmd.SUB_S_COMPARE_CARD == sub then
            --开始比牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_CompareCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_COMPARE_CARD
            dump(cmdtable, "onReadMessage", 10)
            return cmdtable
        elseif cmd.SUB_S_GAME_END == sub then
            -- 游戏结束
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_GAME_END
            --dump(cmdtable, "onReadMessage", 10)
            return cmdtable
        elseif cmd.SUB_S_PLAYER_EXIT == sub then
            -- 用户强退
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_PlayerExit, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_PLAYER_EXIT
            --dump(cmdtable, "onReadMessage", 10)
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
    if cmd.SUB_S_SEND_CARD_EX == sub then
        -- 发牌消息
        print("回放 发牌")
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_SHOW_CARD == sub then
        -- 摊牌
        print("回放 摊牌")
        return self:onGetVideoShowCard(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_COMPARE_CARD == sub then
        -- 开始比牌
        print("回放 开始比牌")
        return self:onGetVideoCompareCard(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_GAME_END == sub then
        -- 结束
        print("回放 游戏结束")
        return self:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
    end
    return 10
end

-- 回放场景
function GameVideoReplay:onGetVideoScene(gameLayer, msgTable, bForward, bBackward)
    return 5, true
end

-- 回放发牌
function GameVideoReplay:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    -- 界面重置
    gameLayer._gameView:gameClean()

    if bBackward then
        print("快退处理 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
    end
    local msg = {}
    msg.msgsub = msgTable.msgsub
    -- 三个玩家牌
    local usercards = {}
    local wbankeruser = yl.INVALID_CHAIR
    local bAnimation = ((not bForward) and (not bBackward))

    local dispatchInfo = {}
    dispatchInfo.playerIndex = {}
    dispatchInfo.cardCount   = cmd.HAND_CARD_COUNT
    for k,v in pairs(msgTable.startinfo) do
        print("发牌椅子 ==> ", v.wChairID)
        -- 视图转换
        local viewId = gameLayer:getUserIndex(v.wChairID)
        print("转换视图 ==> ", viewId)

        local cards = clone(v.bCardData[1])
        if v.wChairID >= 0 and v.wChairID < cmd.GAME_PLAYER then
            local pos = v.wChairID + 1
            self.m_cardData[pos] = cards
            if v.bGameStatus[1][pos] > 0 then
                self.m_gamestatus[pos] = true
            end
        end
        
        if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            if 1 == msgTable.nGameRound then
                PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
            end
        end
        wbankeruser = v.wBanker
        self.m_playercount = v.cbPlayCount
          --设置马牌
        gameLayer._cbMaCardData = v.cbMaCard
        table.insert(dispatchInfo.playerIndex, viewId)
    end
    dispatchInfo.playerCount = self.m_playercount
    gameLayer._gameView:dispatchCard(dispatchInfo,bAnimation,false)

    msg.usercards = usercards
    print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
    GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg

    if wbankeruser ~= yl.INVALID_CHAIR then
        local userIndex = gameLayer:getUserIndex(wbankeruser)
        gameLayer._gameView:showBankerIcon(userIndex)
    end

    for i=1,cmd.GAME_PLAYER do
       gameLayer._gameView:showReady(i, false)
    end

    -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end

    return 20, false
end

-- 回放叫分
function GameVideoReplay:onGetVideoShowCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        gameLayer._gameView:gameClean()
        self._bFinishSegment[msgTable.wCurrentUser + 1] = false

        local dispatchInfo = {}
        dispatchInfo.playerIndex = {}
        dispatchInfo.cardCount   = cmd.HAND_CARD_COUNT
        dispatchInfo.playerCount = self.m_playercount
        for i=1,#self.m_gamestatus do
            if true == self.m_gamestatus[i]  then
                local viewId = gameLayer:getUserIndex(i-1)
                table.insert(dispatchInfo.playerIndex, viewId)
            end
        end
        gameLayer._gameView:GameVideoSendCard(dispatchInfo)
        for i=1,#self._bFinishSegment do
            if true == self._bFinishSegment[i]  then
                local userindex = gameLayer:getUserIndex(i-1)
                gameLayer._gameView:showSelectedCard(userindex, self.m_cardData[i],true)
            end
        end 

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    --玩家摊牌数据
    local cardInfo = {}
    cardInfo.Front = msgTable.bFrontCard[1]
    cardInfo.Mid   = msgTable.bMidCard[1]
    cardInfo.Tail  = msgTable.bBackCard[1]
    local userindex = gameLayer:getUserIndex(msgTable.wCurrentUser)
    gameLayer._sortedCard[userindex] = cardInfo
    self._bFinishSegment[msgTable.wCurrentUser + 1] = true
    local cardData = {}
    for k,v in pairs(cardInfo.Front) do
       table.insert(cardData, v)
    end
    for k,v in pairs(cardInfo.Mid) do
       table.insert(cardData, v)
    end
    for k,v in pairs(cardInfo.Tail) do
       table.insert(cardData, v)
    end
    self.m_cardData[msgTable.wCurrentUser + 1] = cardData
    gameLayer._gameView:showSelectedCard(userindex, cardData,true)
    gameLayer._gameView:setSelectCardData(userindex)

    return 10, true
end

-- 开始比牌
function GameVideoReplay:onGetVideoCompareCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理",msg.msgsub , msgTable.msgsub)
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        gameLayer._gameView:gameClean()

        local dispatchInfo = {}
        dispatchInfo.playerIndex = {}
        dispatchInfo.cardCount   = cmd.HAND_CARD_COUNT
        dispatchInfo.playerCount = self.m_playercount
        for i=1,#self.m_gamestatus do
            if true == self.m_gamestatus[i]  then
                local viewId = gameLayer:getUserIndex(i-1)
                table.insert(dispatchInfo.playerIndex, viewId)
            end
        end
        gameLayer._gameView:GameVideoSendCard(dispatchInfo)
        for i=1,#self._bFinishSegment do
            if true == self._bFinishSegment[i]  then
                local userindex = gameLayer:getUserIndex(i-1)
                gameLayer._gameView:showSelectedCard(userindex, self.m_cardData[i],true)
            end
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    local myChair = gameLayer:GetMeUserItem().wChairID
    local lScoreTimes = {}
    if myChair == 0 then
        lScoreTimes = msgTable.lScoreTimes1
    elseif myChair == 1 then
        lScoreTimes = msgTable.lScoreTimes2
    elseif myChair == 2 then
        lScoreTimes = msgTable.lScoreTimes3
    elseif myChair == 3 then
        lScoreTimes = msgTable.lScoreTimes4
    end

    local viewUser = {}
    local waittime = 10
    for i=1,#self.m_gamestatus do
        local userIndex = gameLayer:getUserIndex(i-1)
        if true == self.m_gamestatus[i]  then
            gameLayer._scoreTimes[userIndex] = lScoreTimes[i]
            --是否特殊牌型
            gameLayer._bSpecialType[userIndex] = msgTable.bSpecialType[1][i]
            if msgTable.bSpecialType[1][i] == 0 then
                table.insert(viewUser,userIndex)
                waittime = waittime + 20
            else
                waittime = waittime + 30 
            end
        else 
            gameLayer._bSpecialType[userIndex] = 0
        end
    end

    local detailTable = {}
    detailTable.bAllWin = clone(msgTable.bAllWin)
    detailTable.wAllKillUser = msgTable.wAllKillUser
    local gunlist = gameLayer._gameView:getGunAcitionList(detailTable)
    local bdetaiall = false
    if msgTable.wAllKillUser >=0 and msgTable.wAllKillUser < 4 then
      bdetaiall = true
    end
    local gunnum = #gunlist
    if #gunlist ~= 0 then
        waittime = waittime + 15 + 8*gunnum
        if bdetaiall == true then
            waittime = waittime + 14
        end
    end

    --比牌动画
    gameLayer._gameView:compareCard(viewUser,detailTable)

    return waittime, true
end

-- 回放结束
function GameVideoReplay:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
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
    gameLayer._bUserGameStatus = clone(self.m_gamestatus)

    gameLayer._scoreEnd = msgTable.lGameScore[1]
    gameLayer._cardData = clone(msgTable.cbHandCard)

    gameLayer._selfscoreEnd = msgTable.lScoreTimes[1][gameLayer:GetMeUserItem().wChairID+1]

    gameLayer._gameView:showEndScore(gameLayer._scoreEnd, msgTable.bEndMode)

    return 30, false
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

return GameVideoReplay
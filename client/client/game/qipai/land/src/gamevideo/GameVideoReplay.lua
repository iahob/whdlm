--
-- Author: zhong
-- Date: 2017-03-22 15:30:04
--
-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.qipai.land.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
local GameResultLayer = appdf.req(module_pre .. ".views.layer.GameResultLayer")

function GameVideoReplay:ctor( scene )
    GameVideoReplay.super.ctor(self)
end

-- 回放邀请信息
function GameVideoReplay:getShareMessage( tabParam )
    local playbackcode = tabParam.dwPlayBackCode or 0
    local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
    return 
    {
        title = "斗地主牌局回放",
        content = "斗地主牌局回放 回放码:" .. playbackcode .. " 斗地主游戏精彩刺激，一起来玩吧！",
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
        if 100 == sub then
            -- 发牌消息
            local t = --发送扑克/游戏开始
            {
                {k = "szNickName", t = "string", s = 32},                   --用户昵称
                {k = "wChairID", t = "word"},                               --椅子ID
                {k = "wStartUser", t = "word"},                             --开始玩家
                {k = "wCurrentUser", t = "word"},                           --当前玩家
                {k = "cbValidCardData", t = "byte"},                        --明牌扑克
                {k = "cbValidCardIndex", t = "byte"},                       --明牌位置
                {k = "cbCardData", t = "byte", l = {cmd.NORMAL_COUNT}},     --扑克列表
                {k = "lScore", t = "score"},                                --用户分数(游戏币)
                {k = "lCellScore", t = "int"},                              --底分
            }
            local cmdtable = {}
            cmdtable.msgsub = 100
            cmdtable.startinfo = {}
            local tmp1 = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, tmp1)
            local tmp2 = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, tmp2)
            local tmp3 = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, tmp3)
            return cmdtable
        elseif 101 == sub then
            -- 叫分
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_CallScore, pBuffer)
            cmdtable.msgsub = 101
            return cmdtable
        elseif 102 == sub then
            -- 庄家信息
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_BankerInfo, pBuffer)
            cmdtable.msgsub = 102
            return cmdtable
        elseif 104 == sub then
            -- 放弃出牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_PassCard, pBuffer)
            cmdtable.msgsub = 104
            return cmdtable
        elseif 1000 == sub then
            -- 游戏状态-play
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, pBuffer)
            cmdtable.msgsub = 1000
            return cmdtable
        elseif 103 == sub then
            -- 出牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_OutCard, pBuffer)
            cmdtable.msgsub = 103
            return cmdtable
        elseif 105 == sub then
            -- 结束
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameConclude, pBuffer)
            cmdtable.msgsub = 105
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
    if 100 == sub then
        -- 发牌消息
        print("回放 发牌")
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    elseif 101 == sub then
        -- 叫分
        print("回放 叫分")
        return self:onGetVideoCallScore(gameLayer, msgTable, bForward, bBackward)
    elseif 102 == sub then
        -- 庄家
        print("回放 庄家")
        return self:onGetVideoBankerInfo(gameLayer, msgTable, bForward, bBackward)
    elseif 104 == sub then
        -- 不出
        print("回放 不出")
        return self:onGetVideoPasscard(gameLayer, msgTable, bForward, bBackward)
    elseif 1000 == sub then
        -- 游戏状态-play
        print("回放 游戏状态")
        return self:onGetVideoScene(gameLayer, msgTable, bForward, bBackward)
    elseif 103 == sub then
        -- 出牌
        print("回放 出牌")
        return self:onGetVideoOutCard(gameLayer, msgTable, bForward, bBackward)
    elseif 105 == sub then
        -- 结束
        print("回放 结束")
        return self:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
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

-- 回放发牌
function GameVideoReplay:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    -- 界面重置
    gameLayer._gameView:reSetForNewGame()
    -- 清理桌面
    gameLayer._gameView.m_outCardsControl:removeAllChildren()
    -- 清理叫分状态
    for k,v in pairs(gameLayer._gameView.m_tabStateSp) do
        v:stopAllActions()
        v:setSpriteFrame("land_blank.png")
    end
    -- 提示置空
    gameLayer._gameView.m_spInfoTip:setSpriteFrame("land_blank.png")

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

    local bAnimation = ((not bForward) and (not bBackward))
    local lDiFeng = 0
    for k,v in pairs(msgTable.startinfo) do
        --print("发牌椅子 ==> ", v.wChairID)
        -- 视图转换
        local viewId = gameLayer:SwitchViewChairID(v.wChairID)
        print("转换视图 ==> ", viewId)
        -- 发牌
        local cards = clone(v.cbCardData[1]) 
        cards = GameLogic:SortCardList(cards, cmd.NORMAL_COUNT, 0)
        gameLayer._gameView.m_tabNodeCards[viewId]:updateCardsNode(cards, true, bAnimation)

        if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            if 1 == msgTable.nGameRound then
                PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
            end

            -- 桌主标识
            local bRoomer = PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == gameLayer._gameView.m_tabUserHead[viewId].m_userItem.dwUserID
            gameLayer._gameView.m_tabUserHead[viewId]:switeRoomerState(bRoomer)
        end

        lDiFeng = v.lCellScore
        -- 用户游戏币(分数)
        gameLayer._gameView.m_tabUserHead[viewId].m_userItem.lScore = v.lScore
        gameLayer._gameView.m_tabUserHead[viewId]:updateScore(v.lScore)

        -- 隐藏牌数
        if nil ~= gameLayer._gameView.m_tabCardCount[viewId] then
            gameLayer._gameView.m_tabCardCount[viewId]:setVisible(false)
        end
    end
    msg.usercards = usercards
    print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
    GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg

    -- 隐藏准备
    gameLayer._gameView.m_btnReady:setVisible(false)

    -- 游戏底分
    gameLayer._gameView:onGetCellScore(lDiFeng)
    return 60, false
end

-- 回放叫分
function GameVideoReplay:onGetVideoCallScore(gameLayer, msgTable, bForward, bBackward)
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

        -- 庄家叫分
        gameLayer._gameView.m_textGameCall:setString("")

        -- 庄家扑克
        for k,v in pairs(gameLayer._gameView.m_tabBankerCard) do
            v:setVisible(false)
            v:setCardValue(0)
        end

        -- 清理状态
        for k,v in pairs(gameLayer._gameView.m_tabStateSp) do
            v:stopAllActions()
            v:setSpriteFrame("land_blank.png")
        end

        -- 清理桌面
        gameLayer._gameView.m_outCardsControl:removeAllChildren()

        -- 提示置空
        gameLayer._gameView.m_spInfoTip:setSpriteFrame("land_blank.png")
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    -- 界面处理叫分
    local curView = gameLayer:SwitchViewChairID(msgTable.wCurrentUser)
    local lastView = gameLayer:SwitchViewChairID(msgTable.wCallScoreUser)    
    gameLayer._gameView:onGetCallScore(curView, lastView, msgTable.cbCurrentScore, msgTable.cbUserCallScore)
    -- 叫分控制
    gameLayer._gameView.m_callScoreControl:setVisible(false)

    -- 隐藏准备
    gameLayer._gameView.m_btnReady:setVisible(false)

    return 20, true
end

-- 回放庄家
function GameVideoReplay:onGetVideoBankerInfo(gameLayer, msgTable, bForward, bBackward)
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

        -- 清理桌面
        gameLayer._gameView.m_outCardsControl:removeAllChildren()
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    
    -- 音效
    ExternalFun.playSoundEffect( "bankerinfo.wav" )

    -- 叫分控制
    gameLayer._gameView.m_callScoreControl:setVisible(false)

    -- 叫分信息
    local bankerCards = GameLogic:SortCardList(msgTable.cbBankerCard[1], 3, 0)
    local bankerViewId = gameLayer:SwitchViewChairID(msgTable.wBankerUser)
    local cbBankerScore = msgTable.cbBankerScore
    -- 更新庄家扑克
    if 3 == #bankerCards then
        for k,v in pairs(bankerCards) do
            gameLayer._gameView.m_tabBankerCard[k]:setVisible(true)
            gameLayer._gameView.m_tabBankerCard[k]:setCardValue(v)
        end
    end
    -- 叫分
    gameLayer._gameView.m_textGameCall:setString(cbBankerScore .. "") 

    -- 庄家切换
    for k,v in pairs(gameLayer._gameView.m_tabUserHead) do
        v:switeGameState(k == bankerViewId)

        gameLayer._gameView.m_tabStateSp[k]:stopAllActions()
        gameLayer._gameView.m_tabStateSp[k]:setSpriteFrame("land_blank.png")
    end
    
    -- 庄家增加牌
    local handCards = gameLayer._gameView.m_tabNodeCards[bankerViewId]:getHandCards()
    local count = #handCards
    handCards[count + 1] = bankerCards[1]
    handCards[count + 2] = bankerCards[2]
    handCards[count + 3] = bankerCards[3]
    handCards = GameLogic:SortCardList(handCards, cmd.MAX_COUNT, 0)
    gameLayer._gameView.m_tabNodeCards[bankerViewId]:addCards(bankerCards, handCards)
    -- 庄家牌标识
    for k,v in pairs(gameLayer._gameView.m_tabNodeCards) do
        v:showLandFlag(bankerViewId)
    end

    -- 庄家信息
    gameLayer.m_cbBankerChair = msgTable.wBankerUser
    gameLayer.m_bIsMyBanker = (bankerViewId == cmd.MY_VIEWID)
    
    -- 提示置空
    gameLayer._gameView.m_spInfoTip:setSpriteFrame("land_blank.png")

    -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("回放约战房")
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end

    -- 隐藏准备
    gameLayer._gameView.m_btnReady:setVisible(false)
    -- 提示置空
    gameLayer._gameView.m_spInfoTip:setSpriteFrame("land_blank.png")

    -- 刷新倍数
    gameLayer._gameView:onGetGameTimes(msgTable.lScoreTimes)
    return 50, true
end

-- 回放不出
function GameVideoReplay:onGetVideoPasscard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

        -- 清理桌面
        gameLayer._gameView.m_outCardsControl:removeAllChildren()
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    --print("不出椅子 ==> ", msgTable.wPassCardUser)
    -- 视图转换
    local viewId = gameLayer:SwitchViewChairID(msgTable.wPassCardUser)
    if nil ~= gameLayer._gameView.m_tabStateSp[viewId] then
        gameLayer._gameView.m_tabStateSp[viewId]:setSpriteFrame("land_blank.png")
    end
    --print("转换视图 ==> ", viewId)
    -- 不出牌
    gameLayer._gameView:onGetPassCard(viewId)

    -- 隐藏准备
    gameLayer._gameView.m_btnReady:setVisible(false)

    return 20, true
end

-- 回放出牌
function GameVideoReplay:onGetVideoOutCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

        -- 清理桌面
        gameLayer._gameView.m_outCardsControl:removeAllChildren()
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    
    --print("出牌椅子 ==> ", msgTable.wOutCardUser)
    -- 视图转换
    local viewId = gameLayer:SwitchViewChairID(msgTable.wOutCardUser)
    --print("转换视图 ==> ", viewId)
    if nil ~= gameLayer._gameView.m_tabStateSp[viewId] then
        gameLayer._gameView.m_tabStateSp[viewId]:setSpriteFrame("land_blank.png")
    end

    local outCards = {}
    local serverOut = msgTable.cbCardData[1]
    for i = 1, msgTable.cbCardCount do
        table.insert(outCards, serverOut[i])
    end
    outCards = GameLogic:SortCardList(outCards, msgTable.cbCardCount, 0)
    local vec = gameLayer._gameView.m_tabNodeCards[viewId]:outCard(outCards, false)
    gameLayer._gameView:outCardEffect(viewId, outCards, vec)

    -- 刷新倍数
    gameLayer._gameView:onGetGameTimes(msgTable.lScoreTimes)

    -- 隐藏准备
    gameLayer._gameView.m_btnReady:setVisible(false)
    -- 提示置空
    gameLayer._gameView.m_spInfoTip:setSpriteFrame("land_blank.png")

    -- 随机速率
    local speed = math.random(20, 30)
    --print("random speed ==> ", speed)
    return speed, true
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
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

        -- 清理桌面
        gameLayer._gameView.m_outCardsControl:removeAllChildren()

        local bankerViewId = gameLayer:SwitchViewChairID(msg.nBankerChair)
        -- 庄家切换
        for k,v in pairs(gameLayer._gameView.m_tabUserHead) do
            --v:switeGameState(k == bankerViewId)

            gameLayer._gameView.m_tabStateSp[k]:stopAllActions()
            gameLayer._gameView.m_tabStateSp[k]:setSpriteFrame("land_blank.png")
        end
        -- 庄家信息
        gameLayer.m_cbBankerChair = msg.nBankerChair
        gameLayer.m_bIsMyBanker = msg.bIsMyBanker
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer)

        -- 庄家信息
        msg.nBankerChair = gameLayer.m_cbBankerChair
        msg.bIsMyBanker = gameLayer.m_bIsMyBanker

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    -- 
    local rs = GameResultLayer.getTagGameResult()
    local scorelist = msgTable.lGameScore[1]
    local countlist = msgTable.cbCardCount[1]
    local cardlist = msgTable.cbHandCardData[1]
    local haveCount = 0
    for i = 1, 3 do
        local chair = i - 1
        local viewId = gameLayer:SwitchViewChairID(chair)

        -- 结算
        local score = scorelist[i]
        -- if score > 0 then
        --     str = "+" .. score
        -- else
        --     str = "" .. score
        -- end
        local settle = GameResultLayer.getTagSettle()
        settle.m_userName = gameLayer._gameView:getUserNick(viewId)
        settle.m_settleCoin = score
        if cmd.MY_VIEWID == viewId then
            rs.enResult = gameLayer:getWinDir(score)
        end
        rs.settles[i] = settle
    end
    -- 标志
    for i = 1, 3 do
        local chair = i - 1
        -- 春天
        if 1 == msgTable.bChunTian then
            rs.cbFlag = cmd.kFlagChunTian
            if chair == gameLayer.m_cbBankerChair then
                rs.settles[i].m_cbFlag = cmd.kFlagChunTian
            end
        end

        -- 反春天
        if 1 == msgTable.bFanChunTian then
            rs.cbFlag = cmd.kFlagFanChunTian
            if chair ~= gameLayer.m_cbBankerChair then
                rs.settles[i].m_cbFlag = cmd.kFlagFanChunTian
            end
        end
    end
    gameLayer._gameView:onGetGameConclude( rs, bNoDelay )
    -- 隐藏准备
    gameLayer._gameView.m_btnReady:setVisible(false)
    -- 提示置空
    gameLayer._gameView.m_spInfoTip:setSpriteFrame("land_blank.png")

    -- 刷新倍数
    gameLayer._gameView:onGetGameTimes(msgTable.lScoreTimes)

    return 60, false
end

function GameVideoReplay:onGetPrivateRoundEnd( gameLayer, msgTable, bForward, bBackward )
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

    -- 玩家牌
    local usercards = msg.usercards
    if type(usercards) ~= "table" then
        print("用户牌错误")
        return false, "用户牌错误"
    end
    return true
end

-- 回退更新牌
function GameVideoReplay:backwardUpdateUserCards( msg, gameLayer )
    -- 更新玩家牌
    local usercards = msg.usercards
    local cards = clone(usercards[cmd.MY_VIEWID])
    cards = GameLogic:SortCardList(cards, #cards, 0)
    print("count ==> ", #cards)
    gameLayer._gameView.m_tabNodeCards[cmd.MY_VIEWID]:updateCardsNode(cards, true, false)

    cards = clone(usercards[cmd.LEFT_VIEWID])
    cards = GameLogic:SortCardList(cards, #cards, 0)
    print("count ==> ", #cards)
    gameLayer._gameView.m_tabNodeCards[cmd.LEFT_VIEWID]:updateCardsNode(cards, true, false)

    cards = clone(usercards[cmd.RIGHT_VIEWID])
    cards = GameLogic:SortCardList(cards, #cards, 0)
    print("count ==> ", #cards)
    gameLayer._gameView.m_tabNodeCards[cmd.RIGHT_VIEWID]:updateCardsNode(cards, true, false)
end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer)
    -- 三个玩家牌
    local usercards = {}
    usercards[cmd.MY_VIEWID] = clone(gameLayer._gameView.m_tabNodeCards[cmd.MY_VIEWID]:getHandCards())
    usercards[cmd.LEFT_VIEWID] = clone(gameLayer._gameView.m_tabNodeCards[cmd.LEFT_VIEWID]:getHandCards())
    usercards[cmd.RIGHT_VIEWID] = clone(gameLayer._gameView.m_tabNodeCards[cmd.RIGHT_VIEWID]:getHandCards())
    msg.usercards = usercards
end
return GameVideoReplay
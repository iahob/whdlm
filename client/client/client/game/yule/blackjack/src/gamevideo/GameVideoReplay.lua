--
-- Author: David
-- Date: 2017-08-10 15:30:04
--
-- 包含录像处理, 单局结算详情界面

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.blackjack.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
--local GameResultLayer = appdf.req(module_pre .. ".views.layer.ResultLayer")
--local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.views.layer.GameEndView")

-- local VideoResultLayer = appdf.req(module_pre .. ".gamevideo.GameVideoResultLayer")


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
        content = "21点牌局回放,21点游戏精彩刺激，一起来玩吧！",
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

-- 消息读取
function GameVideoReplay:onReadMessage( pBuffer )
    local datalen = pBuffer:getlen()
    local curlen = pBuffer:getcurlen()
    if curlen < datalen then
        local sub = pBuffer:readword()
        print("读取消息=========================》", sub)
        if 100 == sub then
            -- 发牌消息
            local t = --发送扑克/游戏开始
            {
                {k = "szNickName", t = "string", s = 32},                   --用户昵称
                {k = "lUserScore", t = "score"},                                  --积分
                {k = "lCellScore", t = "int"},                            --底分
                {k = "wChairID", t = "word"},                               --椅子ID

                {k = "lMaxScore", t = "score"},                             --最大下注
                {k = "wBankerUser", t = "word"},                            --庄家

                {k="cbTimeAddScore" , t = "byte"},      --下注时间
                {k="cbTimeOpertion" , t = "byte"},      --操作时间


                {k="cbBankerMode" , t = "byte"},      --操作时间
                {k = "lBaseJeton", t = "score"},                             --最大下注
                {k = "wPlayerCount", t = "word"},                            --庄家
                
            }
            local cmdtable = {}
            cmdtable.msgsub = 100
            cmdtable.startinfo = {}
            local tmp1 = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, tmp1)
            self.playerCount = tmp1.wPlayerCount
            for i=1, self.playerCount -1 do
                local tmp = ExternalFun.read_netdata(t, pBuffer)
                table.insert(cmdtable.startinfo, tmp)
            end
            return cmdtable
        elseif 101 == sub then
            -- 游戏结束

            print("消息包 游戏结束")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, pBuffer)
            cmdtable.msgsub = 101
            return cmdtable
        elseif 102 == sub then
            -- 发牌消息

            local cmdtable = {}
            cmdtable.msgsub = 102
            cmdtable.sendCardInfo = {}

             for i=1, self.playerCount  do
                local tmp = ExternalFun.read_netdata(cmd.CMD_S_SendCard, pBuffer)
                table.insert(cmdtable.sendCardInfo, tmp)
                
            end

            
            
            return cmdtable
        elseif 103 == sub then
            -- 分牌消息
            print("消息包 分牌消息")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_SplitCard, pBuffer)
            cmdtable.msgsub = 103
            return cmdtable
        elseif 104 == sub then
            -- 停牌消息
            print("消息包 停牌消息")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_StopCard, pBuffer)
            cmdtable.msgsub = 104
            return cmdtable
        elseif 105 == sub then
            -- 加倍消息
            print("消息包 加倍消息")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_DoubleScore, pBuffer)
            cmdtable.msgsub = 105
            return cmdtable
        elseif 106 == sub then
            -- 保险消息
            print("消息包 保险消息")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_Insure, pBuffer)
            cmdtable.msgsub = 106
            return cmdtable
        elseif 107 == sub then
            -- 下注消息
            print("消息包 下注消息")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_AddScore, pBuffer)
            cmdtable.msgsub = 107
            return cmdtable
        elseif 108 == sub then
            -- 要牌消息
            print("消息包 要牌消息")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GetCard, pBuffer)
            cmdtable.msgsub = 108
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
        -- 开始消息
        print("回放 开始")
        return self:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    elseif 101 == sub then
        -- 游戏结束
        print("回放 游戏结束")
        return self:onGetVideoGameConclude(gameLayer, msgTable, bForward, bBackward)
    elseif 102 == sub then
        -- 发牌
        print("回放 发牌")
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    elseif 103 == sub then
        -- 分牌
        print("回放 分牌")
        return self:onGetVideoSplitCard(gameLayer, msgTable, bForward, bBackward)
    elseif 104 == sub then
        -- 停牌
        print("回放 停牌")
        return self:onGetVideoStopCard(gameLayer, msgTable, bForward, bBackward)
    elseif 105 == sub then
        -- 加倍
        print("回放 加倍")
        return self:onGetVideoDoubleScore(gameLayer, msgTable, bForward, bBackward)
    elseif 106 == sub then
        -- 保险
        print("回放 保险")
        return self:onGetVideoInsure(gameLayer, msgTable, bForward, bBackward)
    elseif 107 == sub then
        -- 下注
        print("回放 下注")
        return self:onGetVideoAddSore(gameLayer, msgTable, bForward, bBackward)
    elseif 108 == sub then
        -- 要牌
        print("回放 要牌")
        return self:onGetVideoGetCard(gameLayer, msgTable, bForward, bBackward)
    
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

    

    local userScore = {}
    for k,v in pairs(msgTable.startinfo) do
        --print("发牌椅子 ==> ", v.wChairID)
        dump(v, "玩家信息", 5)
        -- 视图转换
        local viewId = gameLayer:SwitchViewChairID(v.wChairID)
        print("转换视图 ==> ", viewId)

        -- 发牌
        gameLayer.m_wBankerUser = v.wBankerUser

        local wViewBankerUser = gameLayer:SwitchViewChairID(gameLayer.m_wBankerUser)
        print("Banker info =======================>", gameLayer.m_wBankerUser, wViewBankerUser)

        --规则

        gameLayer.m_lMaxCellScore = v.lMaxScore
        gameLayer.m_lCellScore = v.lBaseJeton

        --庄家信息
        gameLayer._gameView:SetBanker(wViewBankerUser)

        --读取游戏状态


        
        gameLayer.m_cbPlayStatus[viewId] = 1

        
        --发牌
        gameLayer:PlaySound(cmd.RES.."sound_res/GAME_START.wav")
       

      


        if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            if 1 == msgTable.nGameRound then
                PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
            end
            print("==========================================》》》》", PriRoom:getInstance().m_tabPriData.lIniScore)
        end

        -- 用户游戏币(分数)
        gameLayer._gameView.m_UserHead[viewId].score:setString("0")

    end

    -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("回放约战房")
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end


    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 10, true
end

--保险
function GameVideoReplay:onGetVideoInsure(gameLayer, msgTable, bForward, bBackward)
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

        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end


    return 5, true
end

-- 加注
function GameVideoReplay:onGetVideoAddSore(gameLayer, msgTable, bForward, bBackward)
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

        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
dump(msgTable , "下注消息")
    local lab = gameLayer._gameView.m_addScore[gameLayer:SwitchViewChairID(msgTable.wAddScoreUser)]
    lab:setVisible(true)
    lab:getChildByTag(10):setString(msgTable.lAddScore)
    gameLayer.userAddScore[gameLayer:SwitchViewChairID(msgTable.wAddScoreUser)] = msgTable.lAddScore
    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 20, true
end


-- 停牌
function GameVideoReplay:onGetVideoStopCard(gameLayer, msgTable, bForward, bBackward)
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

        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
    if msgTable.wStopCardUser == msgTable.wCurrentUser then
        gameLayer.nowOperationCard[gameLayer:SwitchViewChairID(msgTable.wStopCardUser)] = 2
    end

    return 5, true
end

-- 发牌
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

        -- 更新
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    
local userCard = {}
    for k,v in pairs(msgTable.sendCardInfo) do
        dump(v , "发牌消息"..k)
        
        table.insert(userCard , v.cbHandCardData[k])
        local view = gameLayer:SwitchViewChairID(k-1)
        gameLayer.videoHandCard[view] = {}
        table.insert(gameLayer.videoHandCard[view], v.cbHandCardData[k] )
    end

dump(userCard , "所有玩家发的牌牌")

dump(gameLayer.videoHandCard , "回放过程记录牌")

    local bankerChair = gameLayer.m_wBankerUser+1
    local delayCount = 1
    for index = 1 , 2 do
        for i = 1, cmd.GAME_PLAYER do
            if bankerChair>5 then
                bankerChair=0
            end
            if gameLayer.m_cbPlayStatus[gameLayer:SwitchViewChairID(bankerChair)] == 1 then
                local cardIndex = GameLogic:getCardColor(userCard[bankerChair+1][index])*13 + GameLogic:getCardValue(userCard[bankerChair+1][index])
                gameLayer._gameView:SendCard(gameLayer:SwitchViewChairID(bankerChair),index,cardIndex,delayCount*0.1)
                delayCount = delayCount + 1
                gameLayer.cardCount[gameLayer:SwitchViewChairID(bankerChair)][1] = gameLayer.cardCount[gameLayer:SwitchViewChairID(bankerChair)][1] + 1

            end
            bankerChair = bankerChair + 1
        end
    end
    return delayCount*5, true
end

-- 分牌
function GameVideoReplay:onGetVideoSplitCard(gameLayer, msgTable, bForward, bBackward)
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

        -- 更新
        self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    
    local splitUserView = gameLayer:SwitchViewChairID(msgTable.wSplitUser)
    gameLayer._gameView:clearCard(splitUserView)
   
    gameLayer.isSplitCard[splitUserView] = true
    gameLayer.cardCount[splitUserView][1] = 2
    gameLayer.cardCount[splitUserView][2] = 2
    local tempCard = {}
    
    for i=1 , 2 do
        table.insert(tempCard , gameLayer.videoHandCard[splitUserView][1][i])
    end
    for i=1 ,2 do
        gameLayer.videoHandCard[splitUserView][i] = {}
        table.insert(gameLayer.videoHandCard[splitUserView][i] , tempCard[i])
        table.insert(gameLayer.videoHandCard[splitUserView][i] , msgTable.cbCardData[1][i])
    end
        dump(gameLayer.videoHandCard[splitUserView] , "分牌后我的牌")
        
    if splitUserView == 1 then
        gameLayer._gameView.userCard[cmd.MY_VIEWID].area:setPositionX(0)
    end
    
    local delayCount = 1
    for i=1 , 2 do
        for j=1 , 2 do
            local cardIndex = GameLogic:getCardColor(gameLayer.videoHandCard[splitUserView][i][j])*13 + GameLogic:getCardValue(gameLayer.videoHandCard[splitUserView][i][j])
            local index = j + (i-1)*5
            
            gameLayer._gameView:SendCard(splitUserView,index,cardIndex,delayCount*0.1)
            delayCount = delayCount + 1
        end
    end
    return delayCount*5, true
end

-- 回放结果
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
    gameLayer.m_bStartGame = false

    local MyChair = gameLayer:GetMeChairID() + 1

     --清理界面
    local endScore = {}
    for i=1 , 6 do
        local view = gameLayer:SwitchViewChairID(i-1)
        if gameLayer.m_cbPlayStatus[view]==1 then
            endScore[view] = msgTable.lGameScore[1][i]

        end
    end

   
    gameLayer:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
         cc.CallFunc:create(function()
            gameLayer._gameView.btReady:setVisible(true)
            gameLayer._gameView:showGameEnd(endScore , msgTable.cbCardData)
         end)))
    
   
    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 30, false
end

-- 要牌
function GameVideoReplay:onGetVideoGetCard(gameLayer, msgTable, bForward, bBackward)
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

    local cardIndex = GameLogic:getCardColor(msgTable.cbCardData)*13 + GameLogic:getCardValue(msgTable.cbCardData)
    local getCardView = gameLayer:SwitchViewChairID(msgTable.wGetCardUser)

    

    gameLayer.cardCount[getCardView][gameLayer.nowOperationCard[getCardView]] = gameLayer.cardCount[getCardView][gameLayer.nowOperationCard[getCardView]] + 1

    gameLayer._gameView:SendCard(getCardView,
                            (gameLayer.nowOperationCard[getCardView]-1) * 5 + gameLayer.cardCount[getCardView][gameLayer.nowOperationCard[getCardView]],
                            cardIndex,
                            0)
    
    gameLayer._gameView.btReady:setVisible(false)

    return 20, false
end

-- 加倍
function GameVideoReplay:onGetVideoDoubleScore(gameLayer, msgTable, bForward, bBackward)
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

    local cardIndex = GameLogic:getCardColor(msgTable.cbCardData)*13 + GameLogic:getCardValue(msgTable.cbCardData)
    local getCardView = gameLayer:SwitchViewChairID(msgTable.wDoubleScoreUser)

    

    gameLayer.cardCount[getCardView][gameLayer.nowOperationCard[getCardView]] = gameLayer.cardCount[getCardView][gameLayer.nowOperationCard[getCardView]] + 1

    gameLayer._gameView:SendCard(getCardView,
                            (gameLayer.nowOperationCard[getCardView]-1) * 5 + gameLayer.cardCount[getCardView][gameLayer.nowOperationCard[getCardView]],
                            cardIndex,
                            0)
    gameLayer.userAddScore[getCardView] = gameLayer.userAddScore[getCardView] * 2
    gameLayer._gameView.btReady:setVisible(false)

    return 20, false
end



function GameVideoReplay:onGetPrivateRoundEnd( gameLayer, msgTable, bForward, bBackward )

    if nil ~= gameLayer._gameView.m_GameEndView then
        gameLayer._gameView.m_GameEndView:setVisible(false)
    end
    
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

    --清理分数和筹码
    gameLayer.m_lAllTableScore = msg.m_lAllTableScore
    --总下注
    gameLayer._gameView:SetAllTableScore(msg.m_lAllTableScore)

    gameLayer._gameView:CleanAllJettons()

    --玩家下注
    for i=1, cmd.GAME_PLAYER do
        local viewid = gameLayer:SwitchViewChairID(i -1)
        gameLayer.m_lTableScore[i] = msg.m_lTableScore[i]
        gameLayer._gameView:SetUserTableScore(viewid, gameLayer.m_lTableScore[i])
        gameLayer._gameView:PlayerJetton(viewid, gameLayer.m_lTableScore[i], true)
    end

end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer, isForward)
    
end


return GameVideoReplay
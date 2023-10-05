
--
-- 录像处理

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.oxeight.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")

--local VideoResultLayer = appdf.req(module_pre .. ".gamevideo.GameVideoResultLayer")


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
        content = "八人牛牛牌局回放,新六人牛牛游戏精彩刺激，一起来玩吧！",
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
    print("data len ",datalen)
   
    local curlen = pBuffer:getcurlen()
    if curlen < datalen then
        local sub = pBuffer:readword()
        --print("sub is hhhhhhhhhhhhhhh======================",sub)
        if 100 == sub then
            print("回放->游戏开始")
            local t = --游戏开始
            {    
                
                {k = "lCellScore",t="score"},    --游戏底分
                {k = "wPlayerCount",t="word"},--游戏人数
                {k = "wPlayerCountRule",t="word"},      --游戏规则 2-6人为0，其余为人数值
                {k = "wBankerUser",t="word"},                               --用户昵称
                {k = "cbPlayerStatus", t = "byte",l={8}},                   --椅子ID
                {k = "lTurnMaxScor", t = "score"},                          --庄家
                {k = "cbCardData",t = "byte",l = {5,5,5,5,5,5,5,5}},                 --用户扑克
                {k = "ctConfig",t = "int"}, 
                {k = "stConfig",t = "int"},                                      --发牌模式
                {k = "bgtConfig",t = "int"},                                      --庄家玩法
                {k = "btConfig",t = "int"},    
                {k = "gtConfig",t = "int"},    
                {k = "lFreeConfig",t = "int",l = {5}},                --自由配置额度(无效值0)
                {k = "lPercentConfig",t = "int",l = {5}},             --百分比配置额度(无效值0)                                   --下注配置
                {k = "szNickName", t = "string", s = 32},                   --用户昵称
                {k = "wChairID",t="word"}, --用户椅子号 2
                {k = "lScore",t="score"}    --用户分数 8
            }
            local cmdtable = {}   
            cmdtable.msgsub = 100
            cmdtable.startinfo = {}

            local buffer = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, buffer)

            dump(buffer, "game start info===============================")
           
            if buffer.wPlayerCount>=2 then
                for i=1,buffer.wPlayerCount-1 do
                    buffer = ExternalFun.read_netdata(t, pBuffer)
                    table.insert(cmdtable.startinfo, buffer)
                end
            end
            
            return cmdtable
        elseif 101 == sub then --加注
            print("回放->用户加注")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_AddScore, pBuffer)
            cmdtable.msgsub = 101
            return cmdtable
        elseif 103 == sub then --发牌
            print("回放->发牌")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_SendCard, pBuffer)
            cmdtable.msgsub = 103
            return cmdtable
        elseif 104 == sub  then --游戏结束
            print("回放->游戏结束")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, pBuffer)
            cmdtable.msgsub = 104
            return cmdtable    
        elseif 105 == sub  then --用户摊牌
            print("回放->用户摊牌")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_Open_Card, pBuffer)
            cmdtable.msgsub = 105
            return cmdtable
        elseif 106 == sub  then --用户叫庄
            print("回放->用户叫庄")
            local cmdtable =  ExternalFun.read_netdata(cmd.CMD_S_CallBanker, pBuffer)
            cmdtable.msgsub = 106
            return cmdtable 
        elseif 107 == sub  then --用户叫庄
            print("回放->用户叫庄结果")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_CallBankerInfo, pBuffer)
            cmdtable.msgsub = 107
            return cmdtable   
	elseif 120 == sub then
            print("回放->发送4张牌")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_SendFourCard, pBuffer)
            cmdtable.msgsub = 120
            return cmdtable     
        elseif 113 == sub  then --用户叫庄
            print("回放->游戏记录")
            local cmdtable = ExternalFun.read_netdata(cmd.SUB_S_RECORD, pBuffer)
            cmdtable.msgsub = 113
            return cmdtable
        elseif 114 == sub  then --用户叫庄
            print("回放->游戏房卡记录")
            local cmdtable = ExternalFun.read_netdata(cmd.SUB_S_ROOMCARD_RECORD, pBuffer)
            cmdtable.msgsub = 114
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

    gameLayer._gameView.bIsVideo = true
    bForward = bForward or false 
    bBackward = bBackward or false
    local sub = msgTable.msgsub
    if nil == gameLayer or nil == gameLayer._gameView then
        return nil, false, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_VIEW_ERROR
    end

    if 100 == sub then
        -- 发牌消息
        print("回放 开始")
        return self:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    elseif 101 == sub then
       print("回放 用户加注")
       return self:onGetVideoAddScore(gameLayer, msgTable, bForward, bBackward)
    elseif 103 == sub then
        print("回放 发牌")
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    elseif 104 == sub then
        print("回放 游戏结束")
        return self:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
    elseif 105 == sub then
        print("回放->用户摊牌")
        return self:onGetVideoOpenCard(gameLayer, msgTable, bForward, bBackward)
    elseif 106 == sub then
        print("回放->用户叫庄") 
        return self:onGetVideoCallBanker(gameLayer, msgTable, bForward, bBackward) 
    elseif 107 == sub then
        print("回放->用户叫庄结果") 
        return self:onGetVideoCallBankerInfo(gameLayer, msgTable, bForward, bBackward) 
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
    
    if bBackward then
        print("快退处理 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        --庄家等待下注Tips
        for i=1,cmd.GAME_PLAYER do 
            gameLayer._gameView:setBankerWaitIcon(i,false)
        end

        --清理下注数目
        for i=1,cmd.GAME_PLAYER do
            if gameLayer._gameView.m_tabUserHead[i] then
                gameLayer._gameView.m_tabUserHead[i]:showCallInScore("",false)
            end
        end

         --清理桌面扑克
        for i=1,cmd.GAME_PLAYER do
          for j=1, cmd.MAX_CARDCOUNT do
            local pcard = gameLayer._gameView.nodeCard[i][j]
                if gameLayer.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
                    pcard:setVisible(false)
                    gameLayer._gameView:setCardTextureRect(i,j)
                end
          end
        end

        if gameLayer.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_BETFIRST then
            gameLayer.bSendCardCtr = true
        end
    else
        
        --dump(msgTable, "msg table ", 6)
       
         --游戏牌型
        gameLayer.m_tabPrivateRoomConfig.cardType = msgTable.startinfo[1].ctConfig or 22
        --发牌模式
        gameLayer.m_tabPrivateRoomConfig.sendCardType = msgTable.startinfo[1].stConfig or 32
        -- 坐庄模式
        gameLayer.m_tabPrivateRoomConfig.bankGameType = msgTable.startinfo[1].bgtConfig or 52
        --庄家
        gameLayer.wBankerUser = msgTable.startinfo[1].wBankerUser
        --玩家状态
        gameLayer.cbPlayStatus = clone(msgTable.startinfo[1].cbPlayerStatus[1]) 


        local msg = {}
        msg.msgsub = msgTable.msgsub
        msg.userCards = {}

        --扑克数据
        for i = 1, cmd.GAME_PLAYER do
             msg.userCards[i] = {}
             gameLayer.cbCardData[i] = {}
            for j = 1, 5 do
                msg.userCards[i][j] = msgTable.startinfo[1].cbCardData[i][j]
                gameLayer.cbCardData[i][j] = msgTable.startinfo[1].cbCardData[i][j]
            end
        end

        print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
        dump(gameLayer.m_tabPrivateRoomConfig)
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

 
    --游戏规则
    gameLayer._gameView:showRoomRule(gameLayer.m_tabPrivateRoomConfig)
    
    if  gameLayer.m_tabPrivateRoomConfig.bankGameType ~=  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then
        --显示庄家标识
        gameLayer._gameView:setBankerUser(gameLayer:SwitchViewChairID(gameLayer.wBankerUser))
        --只显示庄家倍数
        gameLayer._gameView:setBankerMultiple(gameLayer:SwitchViewChairID(gameLayer.wBankerUser))
    
    

        --庄家等待下注Tips
       for i=1,cmd.GAME_PLAYER do
           if 1 == gameLayer.cbPlayStatus[i]  then
               local viewId = gameLayer:SwitchViewChairID(i-1)
               if (viewId ~= cmd.MY_VIEWID) then 
                    if i-1 ~= gameLayer.wBankerUser  then
                        local bNormal 
                        if gameLayer.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_BETFIRST then
                            bNormal = true
                        elseif gameLayer.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
                            bNormal = false  
                        end
                        
                        gameLayer._gameView:setBankerWaitIcon(viewId,true,bNormal)
                    end
               end
           end
        end

       --发牌
        local sendCount = 0
        if gameLayer.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
            sendCount = 4
        end
        gameLayer.bSendCardCtr = false
        gameLayer._gameView:gameSendCard(gameLayer:SwitchViewChairID(gameLayer.wBankerUser),sendCount)
    end
    -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("回放约战房")
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end

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
    else
        local msg = {}
        msg.msgsub = msgTable.msgsub
        msg.userCards = {}

        --扑克数据
        for i = 1, cmd.GAME_PLAYER do
             msg.userCards[i] = {}
             gameLayer.cbCardData[i] = {}
            for j = 1, 5 do
                msg.userCards[i][j] = msgTable.cbCardData[i][j]
                gameLayer.cbCardData[i][j] = msgTable.cbCardData[i][j]
            end
        end
        print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    --庄家等待下注Tips
    for i=1,cmd.GAME_PLAYER do 
        gameLayer._gameView:setBankerWaitIcon(i,false)
    end

    local sendCount = 5
    if gameLayer.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
        sendCount = 1
    end


    local bankerViewId = gameLayer:SwitchViewChairID(gameLayer.wBankerUser)
    print(gameLayer.m_tabPrivateRoomConfig.bankGameType.."===========================111")
    if  gameLayer.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then
        sendCount = 5 
        for i=1,cmd.GAME_PLAYER do
            local viewid = gameLayer:SwitchViewChairID(i-1)

            --if gameLayer.cbPlayStatus[i] == 1 then
                    gameLayer._gameView:gameAddScore(viewid, 1)
            --end

        end
        gameLayer._gameView:playEffect("ADD_SCORE.WAV")

        bankerViewId = 3
    end

    gameLayer._gameView:gameSendCard(bankerViewId,sendCount)
    
    return 25, true
end

--回放 用户加注 
function GameVideoReplay:onGetVideoAddScore(gameLayer, msgTable, bForward, bBackward)
    gameLayer._gameView.bIsVideo = true
  
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end


        --清理下注数目
        for i=1,cmd.GAME_PLAYER do
            if gameLayer._gameView.m_tabUserHead[i] then
                gameLayer._gameView.m_tabUserHead[i]:showCallInScore("",false)
            end
        end

        --重新显示
        for i=1,cmd.GAME_PLAYER do
            if  gameLayer.m_lAddScoreList[i] > 0 then
                local userViewId = gameLayer:SwitchViewChairID(i-1)
                gameLayer._gameView:gameAddScore(userViewId, gameLayer.m_lAddScoreList[i])
            end
        end  
    else
        gameLayer.m_lAddScoreList[msgTable.wAddScoreUser] = msgTable.lAddScoreCount
    end

    --显示下注数目
    local userViewId = gameLayer:SwitchViewChairID(msgTable.wAddScoreUser)
    gameLayer._gameView:gameAddScore(userViewId, msgTable.lAddScoreCount)
    gameLayer._gameView:setBankerWaitIcon(userViewId,false)
    gameLayer._gameView:playEffect("ADD_SCORE.WAV")

    return 5, true
end

-- 回放操作结果
function GameVideoReplay:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
    local bNoDelay = (bBackward or bForward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
    else

    end


    gameLayer.lCardType = {0,0,0,0,0,0,0,0}

    for i = 1, cmd.GAME_PLAYER do
        gameLayer.lGameEndScore[i] = msgTable.lGameScore[1][i]
        if gameLayer.cbPlayStatus[i] == 1 then
            local wViewChairId = gameLayer:SwitchViewChairID(i - 1)

            gameLayer.lCardType[i] = msgTable.cbCardType[1][i]
        end
    end

    local cbDelayOverGame = msgTable.cbDelayOverGame

     --牌值
    for i = 1, cmd.GAME_PLAYER do
        gameLayer.cbCardData[i] = {}
        for j = 1, cmd.MAX_CARDCOUNT do
            gameLayer.cbCardData[i][j] = msgTable.cbCardData[i][j]
        end
    end

    --开牌动画
    for i = 1, cmd.GAME_PLAYER do
        local viewid = gameLayer:SwitchViewChairID(i - 1)
        if (gameLayer.cbCardData and #gameLayer.cbCardData>0) then
            if gameLayer.cbCardData[i][1] > 0 then
                gameLayer._gameView:openCardAnimate(viewid)
            end
        end
    end

    local lCardType = clone(gameLayer.lCardType)
    gameLayer._gameView:gameEnd(gameLayer.lGameEndScore,lCardType)

    return 50, false
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
end

-- 回放结算
function GameVideoReplay:onGetVideoOpenCard(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
    else
        
    end

    return 5, false
end

function GameVideoReplay:onGetVideoCallBanker( gameLayer, msgTable, bForward, bBackward )

    gameLayer._gameView.bIsVideo = true

    --清理桌面
    gameLayer._gameView:onResetView()
    --游戏牌型
    gameLayer.m_tabPrivateRoomConfig.cardType = msgTable.ctConfig or 22 
    --发牌模式
    gameLayer.m_tabPrivateRoomConfig.sendCardType = msgTable.stConfig or 32 
    -- 坐庄模式
    gameLayer.m_tabPrivateRoomConfig.bankGameType = msgTable.bgtConfig or 52
    --显示规则
    gameLayer._gameView:showRoomRule(gameLayer.m_tabPrivateRoomConfig)
    --显示抢庄状态
    gameLayer._gameView:gameCallBanker(cmd.MY_VIEWID, bFirstTimes)

    --庄家等待下注Tips
    for i=1,cmd.GAME_PLAYER do 
        gameLayer._gameView:setBankerWaitIcon(i,false)
    end

    --清理下注数目
    for i=1,cmd.GAME_PLAYER do
        if gameLayer._gameView.m_tabUserHead[i] then
            gameLayer._gameView.m_tabUserHead[i]:showCallInScore("",false)
        end
    end

     -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("回放约战房")
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end

    return 5, false
end    

function GameVideoReplay:onGetVideoCallBankerInfo( gameLayer, msgTable, bForward, bBackward )
    if bBackward then
        print("快退处理")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end

        --清理桌面
        gameLayer._gameView:onResetView()
        --庄家等待下注Tips
        for i=1,cmd.GAME_PLAYER do 
            gameLayer._gameView:setBankerWaitIcon(i,false)
        end

        --清理下注数目
        for i=1,cmd.GAME_PLAYER do
            if gameLayer._gameView.m_tabUserHead[i] then
                gameLayer._gameView.m_tabUserHead[i]:showCallInScore("",false)
            end
        end

        gameLayer.bSendCardCtr = true
    else

    end

    dump(msgTable, "msg is ")

     --玩家状态
    gameLayer.cbPlayStatus = clone(msgTable.cbCallBankerStatus[1])

    --显示玩家抢庄倍数
    for i=1,cmd.GAME_PLAYER do
         if msgTable.cbCallBankerStatus[1][i] == 1 then
            local wViewChairId = gameLayer:SwitchViewChairID(i - 1)
            --显示倍数
            gameLayer._gameView:setCallMultiple(wViewChairId,msgTable.cbCallBankerTimes[1][i])
         end
    end

    if gameLayer._gameView.animateCard then
       gameLayer._gameView.animateCard:removeAllChildren()
    end

    for i=1,cmd.GAME_PLAYER do
      for j=1, cmd.MAX_CARDCOUNT do
        local pcard = gameLayer._gameView.nodeCard[i][j]
        pcard:setVisible(false)
        gameLayer._gameView:setCardTextureRect(i,j)
      end
    end  

   
   
    return 10, false
end 

function GameVideoReplay:onGetPrivateRoundEnd( gameLayer, msgTable, bForward, bBackward )

    local scorelist = msgTable.lScore[1]
    dump(scorelist, "the score list")
    gameLayer._gameView._priView:onPriGameEnd(msgTable)

    return 20, false
end

-- 回退数据检查
function GameVideoReplay:backwardCheck( msg, msgTab )
   
    return true
end

-- 回退更新牌
function GameVideoReplay:backwardUpdateUserCards( msg, gameLayer )

end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer, isForward)
  
end


return GameVideoReplay
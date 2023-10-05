--
-- Author: David
-- Date: 2017-08-10 15:30:04
--
-- 包含录像处理, 单局结算详情界面

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.shrimp.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
--local GameResultLayer = appdf.req(module_pre .. ".views.layer.ResultLayer")
-- local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.GameEndView")

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
        content = "拼三张牌局回放,拼三张游戏精彩刺激，一起来玩吧！",
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

    -- print(debug.traceback())
    local datalen = pBuffer:getlen()
    local curlen = pBuffer:getcurlen()
    if curlen < datalen then

        local S_NUO_INFO = 
        {
            {t = "byte",k = "old_place"}, 
            {t = "byte",k = "new_place"},
            {t = "score",k = "score"}, 
        }

        local CMD_S_GAMERecord = 
        {
            {t = "word",k = "wChairId"},
            {t = "byte",k = "cbBankerID"},
            {t = "byte", k = "cbSice",l = {3}},
            {t = "score",k = "lUserWinScore",l = {11}},
            {t = "score",k = "lAllAreaInfo",l = {6,6,6,6,6,6,6,6,6,6,6} },
            {t = "score",k = "lAllChuanInfo",l = {15,15,15,15,15,15,15,15,15,15,15} },
            {t = "score",k = "lAllBaoZiInfo",l = {6,6,6,6,6,6,6,6,6,6,6}},
            {t = "table", k = "nuo_dan_info", d = S_NUO_INFO,l = {3,3,3,3,3,3,3,3,3,3,3}},  
            {t = "table", k = "nuo_chuan_info", d = S_NUO_INFO,l = {3,3,3,3,3,3,3,3,3,3,3}},  
            {t = "int", k = "n_UserID" , l = {11}}
        }
        
        local data = ExternalFun.read_netdata(CMD_S_GAMERecord, pBuffer)
        dump( data, "战绩详情", 6)

        return data

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
    -- if 100 == sub then
    --     -- 开始消息
    --     print("回放 开始")
    --     return self:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    -- elseif 101 == sub then
    --     -- 加注
    --     print("回放 加注")
    --     return self:onGetVideoAddSore(gameLayer, msgTable, bForward, bBackward)
    -- elseif 102 == sub then
    --     -- 放弃
    --     print("回放 放弃")
    --     return self:onGetVideoGiveUp(gameLayer, msgTable, bForward, bBackward)
    -- elseif 104 == sub then
    --     -- 开局换牌
    --     print("回放 结果")
    --     return self:onGetVideoGameConclude(gameLayer, msgTable, bForward, bBackward)
    -- elseif 105 == sub then
    --     -- 比牌
    --     print("回放 比牌")
    --     return self:onGetVideoCompareCard(gameLayer, msgTable, bForward, bBackward)
    -- elseif 106 == sub then
    --     -- 看牌
    --     print("回放 看牌")
    --     return self:onGetVideoLookCard(gameLayer, msgTable, bForward, bBackward)
    -- elseif 2000 == sub then
    --     -- 总结算
    --     print("回放 总结算")
    --     return self:onGetPrivateRoundEnd(gameLayer, msgTable, bForward, bBackward)
    -- end
    return 10
end

-- 回放场景
function GameVideoReplay:onGetVideoScene(gameLayer, msgTable, bForward, bBackward)
    return 5, true
end

-- 游戏开始
function GameVideoReplay:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    -- 界面重置
    -- gameLayer:OnResetGameEngine()

    -- if nil ~= gameLayer._gameView.m_GameEndView then
    --     gameLayer._gameView.m_GameEndView:setVisible(false)
    -- end

    -- if bBackward then
    --     print("快退处理 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
    --     local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
    --     if not self:backwardCheck(msg, msgTable) then
    --         return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
    --     end
    -- else
    --     -- 存储数据
    --     local msg = {}
    --     msg.msgsub = msgTable.msgsub

    --     -- 更新
    --     self:cacheUserCards(msg, gameLayer)

    --     GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    -- end

    -- gameLayer.m_lAllTableScore = 0
    -- gameLayer._gameView:CleanAllJettons()

    -- local userScore = {}
    -- for k,v in pairs(msgTable.startinfo) do
    --     --print("发牌椅子 ==> ", v.wChairID)
    --     dump(v, "玩家信息", 5)
    --     -- 视图转换
    --     local viewId = gameLayer:SwitchViewChairID(v.wChairID)
    --     print("转换视图 ==> ", viewId)

    --     -- 发牌
    --     gameLayer.wBankerUser = v.wBankerUser
    --     local wViewBankerUser = gameLayer:SwitchViewChairID(gameLayer.wBankerUser)
    --     print("Banker info =======================>", gameLayer.wBankerUser, wViewBankerUser)

    --     --规则
    --     gameLayer.m_lMaxCellScore = v.lMaxCellScore
    --     gameLayer.m_lCellScore = v.lCellScore
    --     gameLayer._gameView:SetCellScore(gameLayer.m_lCellScore)
    --     gameLayer._gameView:SetMaxCellScore(gameLayer.m_lMaxCellScore)
    --     --庄家信息
    --     gameLayer._gameView:SetBanker(wViewBankerUser)

    --     --读取游戏状态


    --     local data = v.cbPlayStatus[1][v.wChairID +1]
    --     gameLayer.m_cbPlayStatus[v.wChairID +1] = data
    --     if gameLayer.m_cbPlayStatus[v.wChairID +1] == 1 then 
    --         gameLayer.m_lAllTableScore = gameLayer.m_lAllTableScore + gameLayer.m_lCellScore
    --         gameLayer.m_lTableScore[v.wChairID +1] = gameLayer.m_lCellScore
    --         --用户下注
    --         gameLayer._gameView:SetUserTableScore(viewId, gameLayer.m_lCellScore)
    --         --移动筹码
    --         gameLayer._gameView:PlayerJetton(viewId,gameLayer.m_lTableScore[v.wChairID +1])
    --     end

    --     --总计下注
    --     gameLayer._gameView:SetAllTableScore(gameLayer.m_lAllTableScore)
    --     --发牌
    --     gameLayer:PlaySound(cmd.RES.."sound_res/SEND_CARD_BEGIN.wav")
    --     local delayCount = 1
    --     for index = 1 , 3 do
    --         for i = 1, cmd.GAME_PLAYER do
    --             local chair = math.mod(gameLayer.m_wBankerUser + i - 1,cmd.GAME_PLAYER) 
    --             if gameLayer.m_cbPlayStatus[chair + 1] == 1 then
    --                 gameLayer._gameView:SendCard(gameLayer:SwitchViewChairID(chair),index,delayCount*0.1)
    --                 delayCount = delayCount + 1
    --             end
    --         end
    --     end

    --    --手牌显示
    --     gameLayer:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function ()
    --             local cardData = {}
    --             for i = 1, 3 do
    --                 cardData[i] = v.cbHandCardData[v.wChairID +1][i]
    --             end
    --             if gameLayer.m_cbPlayStatus[v.wChairID +1] == 1 then
    --                 gameLayer._gameView.userCard[viewId].area:setVisible(true)
    --                 local cardIndex = {}
    --                 for k = 1 , 3 do
    --                     cardIndex[k] = GameLogic:getCardColor(cardData[k])*13 + GameLogic:getCardValue(cardData[k])
    --                     print("card info =======================>", cardIndex[k], cardData[k])
    --                 end
    --                 gameLayer._gameView:SetUserCard(viewId,cardIndex) 
    --             end
    --         end)))


    --     if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
    --         if 1 == msgTable.nGameRound then
    --             PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
    --         end
    --         print("==========================================》》》》", PriRoom:getInstance().m_tabPriData.lIniScore)
    --     end

    --     -- 用户游戏币(分数)
    --     gameLayer._gameView.m_UserHead[viewId].score:setString("0")

    -- end

    -- -- 约战房刷新信息
    -- if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
    --     print("回放约战房")
    --     PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
    --     gameLayer._gameView._priView:onRefreshInfo()
    -- end


    -- -- 隐藏准备
    -- gameLayer._gameView.btReady:setVisible(false)

    return 50, true
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

    local MyChair = gameLayer:GetMeChairID()

    local int64 = Integer64.new()
    gameLayer.m_wCurrentUser = msgTable.wCurrentUser
    local wAddScoreUser = msgTable.wAddScoreUser
    local wCompareState = msgTable.wCompareState
    local lAddScoreCount = msgTable.lAddScoreCount
    local lCurrentTimes = msgTable.lCurrentTimes

    local viewid = gameLayer:SwitchViewChairID(wAddScoreUser)
    if gameLayer.m_lCurrentTimes < lCurrentTimes then
        gameLayer._gameView:runAddTimesAnimate(viewid)
    end

    gameLayer._gameView:PlayerJetton(viewid, lAddScoreCount)
    gameLayer.m_lTableScore[wAddScoreUser+1] = gameLayer.m_lTableScore[wAddScoreUser+1] + lAddScoreCount
    gameLayer.m_lAllTableScore = gameLayer.m_lAllTableScore + lAddScoreCount
    gameLayer._gameView:SetUserTableScore(viewid, gameLayer.m_lTableScore[wAddScoreUser+1])
    gameLayer._gameView:SetAllTableScore(gameLayer.m_lAllTableScore)

    if wCompareState == 0 then
        --声音
        if gameLayer.m_cbPlayStatus[wAddScoreUser + 1] == 1 then
            if gameLayer.m_lCurrentTimes == lCurrentTimes then
                local strFile = cmd.RES.."sound_res/"..string.format("FOLLOW_COIN_%d.wav", math.mod(gameLayer.nVoiceFollow[viewid], 3))
                gameLayer:PlaySound(strFile)
                gameLayer.nVoiceFollow[viewid] = gameLayer.nVoiceFollow[viewid] + 1
            else
                gameLayer:PlaySound(cmd.RES.."sound_res/RAISE_COIN.wav")
            end
        end
    end
    gameLayer.m_lCurrentTimes = lCurrentTimes

    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 20, true
end

-- 放弃
function GameVideoReplay:onGetVideoGiveUp(gameLayer, msgTable, bForward, bBackward)
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

    local wGiveUpUser = msgTable.wGiveUpUser
    local viewid = gameLayer:SwitchViewChairID(wGiveUpUser)
    gameLayer._gameView:SetUserGiveUp(viewid,true)

    gameLayer.m_cbPlayStatus[wGiveUpUser+1] = 0

    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 10, true
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
    gameLayer._gameView:StopCompareCard()
    gameLayer._gameView:SetCompareCard(false)
    gameLayer._gameView.m_ChipBG:setVisible(false)
    gameLayer._gameView.nodeButtomButton:setVisible(false)
    gameLayer._gameView.m_GameEndView:ReSetData()

    local endShow = false

    local int64 = Integer64.new()

    local lGameTax = msgTable.lGameTax

    local winner 
    local lGameScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lGameScore[i] = msgTable.lGameScore[1][i]
        if lGameScore[i] > 0 then
            winner = i
        else
            winner = 0
        end
    end

    --用户扑克
    local cbCardData = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCardData[i] = {}
        for j = 1, 3 do
            cbCardData[i][j] = msgTable.cbCardData[i][j]
        end
    end

    --比牌用户
    local wCompareUser = {}
    for i = 1, cmd.GAME_PLAYER  do
        wCompareUser[i] = {}
        for j = 1, 4 do
            wCompareUser[i][j] = msgTable.wCompareUser[i][j]
        end
    end

    local wEndState = msgTable.wEndState

    local bDelayOverGame = msgTable.bDelayOverGame

    local wServerType = msgTable.wServerType

    local savetype = {}

    --移动筹码
    for i = 1, cmd.GAME_PLAYER do
        local viewid = gameLayer:SwitchViewChairID(i-1)
        if lGameScore[i] ~= 0 then
            if lGameScore[i] > 0 then
                gameLayer._gameView:SetUserTableScore(viewid,"+"..lGameScore[i])
                gameLayer._gameView:WinTheChip(viewid)
                if viewid == cmd.MY_VIEWID then
                    print("播放胜利音乐")
                    gameLayer:PlaySound(cmd.RES.."sound_res/GAME_WIN.wav")
                else
                    gameLayer:PlaySound(cmd.RES.."sound_res/GAME_LOSE.wav")
                end
            else
                gameLayer._gameView:SetUserTableScore(viewid,lGameScore[i])
            end

            endShow = true
            gameLayer._gameView.m_GameEndView:SetUserScore(viewid, lGameScore[i])
            gameLayer._gameView.m_GameEndView:SetUserCard(viewid,nil,nil,lGameScore[i]<0)
            gameLayer._gameView.m_GameEndView:SetWinFlag(viewid, lGameScore[i])
            savetype[i] = GameLogic:getCardType(cbCardData[i])
            local userItem = gameLayer._gameFrame:getTableUserItem(gameLayer._gameFrame:GetTableID(), i-1)
            gameLayer._gameView.m_GameEndView:SetUserInfo(viewid,userItem)

            print("savetype["..i.."]"..savetype[i])
        else
            savetype[i] = 0
            gameLayer._gameView:SetUserTableScore(viewid)
        end
    end
    
    for i = 1 , cmd.GAME_PLAYER - 1 do
        local wUserID = wCompareUser[MyChair][i]
        if wUserID and wUserID ~= yl.INVALID_CHAIR then
            local viewid = gameLayer:SwitchViewChairID(wUserID)
            local cardIndex = {}
            for k = 1 , 3 do
                cardIndex[k] = GameLogic:getCardColor(cbCardData[wUserID+1][k])*13 + GameLogic:getCardValue(cbCardData[wUserID+1][k])
            end
            gameLayer._gameView:SetUserCard(viewid, cardIndex)
            gameLayer._gameView:SetUserCardType(viewid, savetype[wUserID+1])
            gameLayer._gameView.m_GameEndView:SetUserCard(viewid,cardIndex,savetype[wUserID+1])
        end
    end
    --比过牌或看过牌，才能显示自己的牌
    --if wCompareUser[MyChair][1] ~= yl.INVALID_CHAIR or self.m_bLookCard[MyChair] == true then
        local cardIndex = {}
        for k = 1 , 3 do
             cardIndex[k] = GameLogic:getCardColor(cbCardData[MyChair][k])*13 + GameLogic:getCardValue(cbCardData[MyChair][k])
        end
        gameLayer._gameView:SetUserCard(cmd.MY_VIEWID, cardIndex)
        gameLayer._gameView:SetUserCardType(cmd.MY_VIEWID, savetype[MyChair])
        gameLayer._gameView.m_GameEndView:SetUserCard(cmd.MY_VIEWID,cardIndex,savetype[MyChair])
    --end

    if wEndState == 1 then
        if gameLayer.m_cbPlayStatus[MyChair] == 1 then
            for i =1 , cmd.GAME_PLAYER do
                if gameLayer.m_cbPlayStatus[i] == 1 then
                    local cardIndex = {}
                    for k = 1 , 3 do
                        cardIndex[k] = GameLogic:getCardColor(cbCardData[i][k])*13 + GameLogic:getCardValue(cbCardData[i][k])
                    end
                    local viewid = gameLayer:SwitchViewChairID(i-1)
                    gameLayer._gameView:SetUserCard(viewid, cardIndex)
                    gameLayer._gameView:SetUserCardType(viewid, savetype[i])
                    gameLayer._gameView.m_GameEndView:SetUserCard(viewid,cardIndex,savetype[i])
                end
            end
        end
    end


    print("endShow =========================>", endShow)
    if endShow then
        gameLayer._gameView.m_GameEndView:setVisible(true)
        gameLayer.bIsDismiss = false
    else
        gameLayer.bIsDismiss = true
    end
    
    gameLayer:PlaySound(cmd.RES.."sound_res/GAME_END.wav")

    if 0 ~=  winner then
        if lGameScore[winner]/gameLayer.m_lCellScore > 100 then
            gameLayer:PlaySound(cmd.RES.."sound_res/GET_MORECOIN.wav")
        else
            gameLayer:PlaySound(cmd.RES.."sound_res/GET_COIN.wav")
        end
    end 
   
    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 30, false
end

-- 比牌
function GameVideoReplay:onGetVideoCompareCard(gameLayer, msgTable, bForward, bBackward)
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

    gameLayer.m_wCurrentUser = msgTable.wCurrentUser
    local wCompareUser = {}
    for i = 1, 2 do
        wCompareUser[i] = msgTable.wCompareUser[1][i]
    end
    gameLayer.m_wLostUser = msgTable.wLostUser
    gameLayer.m_wWinnerUser = wCompareUser[1] + wCompareUser[2] - gameLayer.m_wLostUser

    gameLayer.m_cbPlayStatus[gameLayer.m_wLostUser+1] = 0

    gameLayer._gameView:SetCompareCard(false)

    local firstuser =  gameLayer._gameFrame:getTableUserItem(gameLayer._gameFrame:GetTableID(), wCompareUser[1])
    local seconduser =  gameLayer._gameFrame:getTableUserItem(gameLayer._gameFrame:GetTableID(), wCompareUser[2])
    gameLayer._gameView:CompareCard(firstuser,seconduser,nil,nil,wCompareUser[1] == gameLayer.m_wWinnerUser, function()
                gameLayer._gameView:StopCompareCard()
                local myChair = gameLayer:GetMeChairID()
                if myChair == gameLayer.m_wWinnerUser then
                    gameLayer:PlaySound(cmd.RES.."sound_res/COMPARE_WIN.wav")
                elseif myChair == gameLayer.m_wLostUser then
                    gameLayer:PlaySound(cmd.RES.."sound_res/COMPARE_LOSE.wav")
                end
        end)

    gameLayer:PlaySound(cmd.RES.."sound_res/COMPARE_CARD.wav")
    --print("转换视图 ==> ", viewId)
    -- 隐藏准备
    gameLayer._gameView.btReady:setVisible(false)

    return 50, false
end

--看牌
function GameVideoReplay:onGetVideoLookCard(gameLayer, msgTable, bForward, bBackward)
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
    local wLookCardUser = msgTable.wLookCardUser
   
    local viewid = gameLayer:SwitchViewChairID(wLookCardUser)
    gameLayer._gameView:SetLookCard(viewid,true)

    gameLayer._gameView.btReady:setVisible(false)
    return 20, true
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
    msg.m_lAllTableScore = gameLayer.m_lAllTableScore
    msg.m_lTableScore = {}
    for i=1, cmd.GAME_PLAYER do
        msg.m_lTableScore[i] = gameLayer.m_lTableScore[i]
        gameLayer._gameView:SetLookCard(i, false)
    end
    msg.m_lCurrentTimes = gameLayer.m_lCurrentTimes
end

-- 获取玩家结算界面
-- function GameVideoReplay:getVideoResultView( cmdlist )
--     local msgTab = nil
--     for k,v in pairs(cmdlist) do
--         if v.msgsub == 108 then --找到总结算
--             msgTab = v
--             break
--         end
--     end
--     if type(msgTab) ~= "table" then
--         print("结算消息包异常")
--         return false, "结算消息包异常"
--     end

--     local jsonStr = cjson.encode(msgTab)
--     LogAsset:getInstance():logData(jsonStr, true)

--     --用户列表
--     local tabListInfo = GameVideo:getInstance():getVideoListInfo()
--     if 0 == #tabListInfo then
--         unGetInfoFun()
--         return true
--     end
--     local iteminfo = tabListInfo[1]
--     if 0 == #iteminfo then
--         unGetInfoFun()
--         return true
--     end
--     -- local userinfo = iteminfo[1]
--     -- if nil == userinfo.szPersonalGUID or "" == userinfo.szPersonalGUID then
--     --     unGetInfoFun()
--     --     return true
--     -- end
--     dump(iteminfo, "玩家信息", 10)

--     local resultList = {}
--     for i = 1, 4 do
--         local result = {}
--         --玩家头像
--         if 0 < #iteminfo then
--             for j=1,#iteminfo do
--                 local Iteminfo = iteminfo[j]
--                 if Iteminfo.wChairID == i -1 then
--                     result.userItem = Iteminfo
--                     break
--                 end
--             end
--         end

--         local lScore = msgTab.lGameScore[1][i]
--         local lHuScore = msgTab.lHuScore[1][i]
      
--         result.lScore = lScore
--         result.lHuScore = lHuScore
--         result.lGangScore = msgTab.lGangScore[1][i]
--         result.lMaScore = msgTab.lMaScore[1][i]
--         result.cbChHuKind = msgTab.cbChiHuKind[1][i]
--         --胡牌类型
--         result.dwChiHuRight = {}
--         for j=1,cmd.MAX_RIGHT_COUNT do
--             result.dwChiHuRight[j] = msgTab.dwChiHuRight[i][j]
--         end

--         --设置癞子
--         result.cbMagicData = msgTab.cbMagicIndex[1]

--         result.cbCardData = {}
--         --手牌
--         for j = 1, msgTab.cbCardCount[1][i] do
--             result.cbCardData[j] = msgTab.cbHandCardData[i][j]
--         end
--         --如果是我自摸
--         if HuChair == i -1 and 0 ~= msgTab.cbSendCardData then
--             table.insert(result.cbCardData, msgTab.cbSendCardData)
--         end

--              --先设置碰杠出的牌
--         if msgTab.cbWeaveItemCount[1][i] > 0 then
--             local tabActiveCardData = {}
--             for j = 1, msgTab.cbWeaveItemCount[1][i] do
--                 local cbOperateData = {} --此处为tagAvtiveCard
--                 cbOperateData.cbCardValue = msgTab.WeaveItemArray[i][j].cbCardData[1]
--                 dump(cbOperateData.cbCardValue, "操作的牌")
--                 local nShowStatus = GameLogic.SHOW_NULL
--                 local cbParam = msgTab.WeaveItemArray[i][j].cbParam
--                 if cbParam == GameLogic.WIK_GANERAL then
--                     if cbOperateData[1] == cbOperateData[2] then    --碰
--                         nShowStatus = GameLogic.SHOW_CHI
--                     else                                            --吃
--                         nShowStatus = GameLogic.WIK_LEFT
--                     end
--                     cbOperateData.cbCardNum = 3
--                     table.remove(cbOperateData.cbCardValue, 4)--去掉末尾0
--                 elseif cbParam == GameLogic.WIK_MING_GANG then
--                     nShowStatus = GameLogic.SHOW_MING_GANG
--                     cbOperateData.cbCardNum = 4
--                 elseif cbParam == GameLogic.WIK_FANG_GANG then
--                     nShowStatus = GameLogic.SHOW_FANG_GANG
--                     cbOperateData.cbCardNum = 4
--                 elseif cbParam == GameLogic.WIK_AN_GANG then
--                     nShowStatus = GameLogic.SHOW_AN_GANG
--                     cbOperateData.cbCardNum = 4
--                 end
--                 cbOperateData.cbType = nShowStatus
--                 -- local wProvideViewId = self:SwitchViewChairID(cmd_data.WeaveItemArray[i][j].wProvideUser)
--                 -- cbOperateData.wProvideViewId = wProvideViewId

--                 --排序
--                 table.sort(cbOperateData.cbCardValue, function (a, b)
--                     return a < b
--                 end)
--                 table.insert(tabActiveCardData, cbOperateData)
--             end
--             result.cbActiveCardData = tabActiveCardData
--         end

--         --插入
--         table.insert(resultList, result)
--     end

--     local ResultLayer =  VideoResultLayer:create(self)
--     if nil ~= ResultLayer then
--         print("显示结算框", ResultLayer)
--         --dump(resultList, "结算信息", 8)
--         ResultLayer:showLayer(resultList, msgTab.cbProvideCard)
--         return ResultLayer
--     end

--     return nil
-- end

return GameVideoReplay
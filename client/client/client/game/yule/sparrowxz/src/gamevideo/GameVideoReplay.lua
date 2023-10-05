
-- 包含录像处理, 单局结算详情界面

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.sparrowxz.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
--local GameResultLayer = appdf.req(module_pre .. ".views.layer.ResultLayer")
local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.ResultLayer")

local VideoResultLayer = appdf.req(module_pre .. ".gamevideo.GameVideoResultLayer")



local xzvideo_debug=true

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
        content = "血战麻将牌局回放,血战麻将游戏精彩刺激，一起来玩吧！",
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
    self.msgNO=self.msgNO or 0
    local datalen = pBuffer:getlen()
    --print("datalen:",datalen)
    local curlen = pBuffer:getcurlen()
    print("curlen:",curlen)
    if curlen < datalen then
        local sub = pBuffer:readword()
        self.msgNO=self.msgNO+1
        print("sub:",sub)
        if cmd.SUB_S_GAME_START==sub then
            --游戏开始
            local cmdtable = {}
            cmdtable.NO=self.msgNO --debug
            cmdtable.tt="SUB_S_GAME_START"
            cmdtable.msgsub = cmd.SUB_S_GAME_START
            cmdtable.startinfo = {}
            local tmp1 = ExternalFun.read_netdata(cmd.Video_GameStart, pBuffer)
            table.insert(cmdtable.startinfo, tmp1)
            local tmp2 = ExternalFun.read_netdata(cmd.Video_GameStart, pBuffer)
            table.insert(cmdtable.startinfo, tmp2)
            local tmp3 = ExternalFun.read_netdata(cmd.Video_GameStart, pBuffer)
            table.insert(cmdtable.startinfo, tmp3)
            local tmp4 = ExternalFun.read_netdata(cmd.Video_GameStart, pBuffer)
            table.insert(cmdtable.startinfo, tmp4)
            return cmdtable
        elseif cmd.SUB_S_CHANGE_CARD_RESULT==sub then --换三张结果
            local cmdtable = ExternalFun.read_netdata(cmd.Video_ChangeCardResult, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_CHANGE_CARD_RESULT
            cmdtable.NO=self.msgNO 
            cmdtable.tt="SUB_S_CHANGE_CARD_RESULT"
            return cmdtable
        elseif cmd.SUB_S_CALL_CARD==sub then
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_CallCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_CALL_CARD
            cmdtable.NO=self.msgNO 
             cmdtable.tt="SUB_S_CALL_CARD"
            return cmdtable
        elseif cmd.SUB_S_BANKER_INFO==sub then --选缺结果
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_BankerInfo, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_BANKER_INFO
            for i=1,cmd.GAME_PLAYER-1 do
                ExternalFun.read_netdata(cmd.CMD_S_BankerInfo, pBuffer)
            end
            cmdtable.NO=self.msgNO 
            cmdtable.tt="SUB_S_BANKER_INFO"
            return cmdtable
        elseif cmd.SUB_S_OUT_CARD == sub then
            -- 用户出牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_OutCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_OUT_CARD
            cmdtable.NO=self.msgNO 
             cmdtable.tt="SUB_S_OUT_CARD"
            return cmdtable
        elseif cmd.SUB_S_SEND_CARD == sub then
            -- 用户发牌
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_SendCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_SEND_CARD
            cmdtable.NO=self.msgNO 
            cmdtable.tt="SUB_S_SEND_CARD"
            return cmdtable

        elseif cmd.SUB_S_OPERATE_RESULT == sub then
            -- 操作结果
            local cmdtable = ExternalFun.read_netdata(cmd.Video_OperateResult, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_OPERATE_RESULT
            cmdtable.NO=self.msgNO 
            cmdtable.tt="SUB_S_OPERATE_RESULT"
            return cmdtable
        elseif cmd.SUB_S_CHI_HU==sub then
            local cmdtable = ExternalFun.read_netdata(cmd.Video_ChiHu, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_CHI_HU
            cmdtable.NO=self.msgNO 
            cmdtable.tt="SUB_S_CHI_HU"
            return cmdtable
        elseif cmd.SUB_S_GAME_END == sub then
            -- 游戏结果
            local cmdtable = ExternalFun.read_netdata(cmd.Video_GameEnd, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_GAME_END
            for i=1,4 do
                print("len:",cmdtable.cbCardCount[1][i])
            end
            cmdtable.NO=self.msgNO 
            cmdtable.tt="SUB_S_GAME_END"
            return cmdtable
        else 
            print("sub:",sub)
            assert(false)
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
    print("处理消息序号",msgTable.NO)
    LogAsset:getInstance():logData(cjson.encode("处理消息序号:"..msgTable.NO.."  "..msgTable.tt))
    if cmd.SUB_S_GAME_START == sub then
      
        -- 发牌消息
        print("回放 开始")
        return self:onGetVideoStart(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_CHANGE_CARD_RESULT==sub then--换完三张

         return self:onGetVideoChangeCard(gameLayer, msgTable, bForward, bBackward)

    elseif cmd.SUB_S_CALL_CARD==sub then
         return self:onGetVideoCallCard(gameLayer, msgTable, bForward, bBackward)--没作用

    elseif cmd.SUB_S_BANKER_INFO==sub then --都选完缺

         return self:onGetVideoBankerInfo(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_OUT_CARD == sub then
        -- 发牌
        print("回放 出牌")
        return self:onGetVideoOutcard(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_SEND_CARD == sub then
        -- 发牌
        print("回放 发牌")
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
 
    elseif cmd.SUB_S_OPERATE_RESULT == sub then
        -- 
        print("回放 操作")
        return self:onGetVideoOperateResult(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_CHI_HU==sub then
        print("回放 胡牌")
        return self:onGetVideoChiHu(gameLayer, msgTable, bForward, bBackward)
    elseif cmd.SUB_S_GAME_END == sub then
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
    local cachemsg=cjson.encode({"onGetVideoStart", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    -- 界面重置
    gameLayer:OnResetGameEngine()
    gameLayer._gameView.bIsVideo = true

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

    
        --头像隐藏缺门信息
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView.m_tabUserHead[viewid]:setShowCallCard(false)
            gameLayer._gameView:setShowHuSign(viewid,false)
        end

    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        -- 更新
       -- self:cacheUserCards(msg, gameLayer)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    gameLayer.leftCard = 108


    -- 约战房刷新信息
    if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("回放约战房")
        PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
        gameLayer._gameView._priView:onRefreshInfo()
    end

    local wViewBankerUser

    for k,v in pairs(msgTable.startinfo) do

        if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            if 1 == msgTable.nGameRound then
                PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
            end
            print("==========================================》》》》", PriRoom:getInstance().m_tabPriData.lIniScore)
        end

        local viewId = gameLayer:SwitchViewChairID(v.wChairID)
        print("转换视图 ==> ", viewId)

        -- 发牌
        gameLayer.wBankerUser = v.wBankerUser
        print("wBankerUser:",v.wBankerUser)
        wViewBankerUser = gameLayer:SwitchViewChairID(v.wBankerUser)
        print("wViewBankerUser:",wViewBankerUser)
        
        for i=1,cmd.MAX_COUNT do --手牌
            print("viewId"..viewId,"card"..i,v.cbCardData[1][i])
            if not (wViewBankerUser~=viewId and i==cmd.MAX_COUNT) then
                table.insert(gameLayer.cbHandCardData[viewId], v.cbCardData[1][i])
            end
        end

        LogAsset:getInstance():logData(cjson.encode("viewid "..viewId.." 开局收到牌:"..table.concat(gameLayer.cbHandCardData[viewId],",")),true)

    end
    print("wViewBankerUser2:",wViewBankerUser)
    gameLayer._gameView:videoGameStart(wViewBankerUser)
    return 90,true
end

function GameVideoReplay:onGetVideoChangeCard(gameLayer, msgTable, bForward, bBackward)
    local cachemsg=cjson.encode({"onGetVideoChangeCard", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    if bBackward then
        print("快退处理 onGetVideoChangeCard")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
         --头像隐藏缺门信息
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView.m_tabUserHead[viewid]:setShowCallCard(false)
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    gameLayer.leftCard=108 -13 *4 -1

    local cardLayer=gameLayer._gameView._cardLayer

    gameLayer._gameView:runAction(cc.Sequence:create(
        cc.CallFunc:create(function() 
                for i=1,cmd.GAME_PLAYER do
                    local viewid=gameLayer:SwitchViewChairID(i-1)
                    cardLayer:removeHandCards(viewid,msgTable.cbChangeCardDataBefore[i])
                    cardLayer:sortHandCard(viewid)
                end
            end),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create(function() 
                local changeType=msgTable.cbChangeType
                gameLayer._gameView:setShowChangeTypeSps(true,changeType<=0 and 1 or changeType)
                for i=1,cmd.GAME_PLAYER do
                    local viewid=gameLayer:SwitchViewChairID(i-1)
                    cardLayer:exchangeCard(msgTable.cbChangeCardDataAfter[i],viewid)
                end
            end)
        ))

   

    for i=1,cmd.GAME_PLAYER do

        local viewid=gameLayer:SwitchViewChairID(i-1)

         print(viewid.."出去牌",table.concat(msgTable.cbChangeCardDataBefore[i],","))
         print(viewid.."进来牌",table.concat(msgTable.cbChangeCardDataAfter[i],","))
        print(i.." before remove:",table.concat(gameLayer.cbHandCardData[viewid],","))


        LogAsset:getInstance():logData(cjson.encode("viewid "..viewid.."出去牌 "..table.concat(msgTable.cbChangeCardDataBefore[i],",")),true)
        LogAsset:getInstance():logData(cjson.encode("viewid "..viewid.."进来牌"..table.concat(msgTable.cbChangeCardDataAfter[i],",")),true)

        local nbef=#gameLayer.cbHandCardData[viewid]
        --移除
        for j=1,#msgTable.cbChangeCardDataBefore[i]  do
            local bFound=false
            for k=1,#gameLayer.cbHandCardData[viewid] do
                if gameLayer.cbHandCardData[viewid][k]==msgTable.cbChangeCardDataBefore[i][j] then
                    table.remove(gameLayer.cbHandCardData[viewid],k)
                    bFound=true
                    break
                end
            end
            assert(bFound)
        end

        print("#gameLayer.cbHandCardData[viewid],nbef",#gameLayer.cbHandCardData[viewid],nbef)
        assert(#gameLayer.cbHandCardData[viewid]+3==nbef)

        --添加
        for j=1,#msgTable.cbChangeCardDataAfter[i]  do

            table.insert(gameLayer.cbHandCardData[viewid],msgTable.cbChangeCardDataAfter[i][j])
        end
    end

    return 30,true

end


function GameVideoReplay:onGetVideoCallCard(gameLayer, msgTable, bForward, bBackward)
    local cachemsg=cjson.encode({"onGetVideoCallCard", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)
    if bBackward then
        print("快退处理 onGetVideoCallCard")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
      
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
         --头像隐藏缺门信息
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView.m_tabUserHead[viewid]:setShowCallCard(false)
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end
    return 1,true
end


function GameVideoReplay:onGetVideoBankerInfo(gameLayer, msgTable, bForward, bBackward)
    local cachemsg=cjson.encode({"onGetVideoBankerInfo", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    if bBackward then
        print("快退处理 onGetVideoBankerInfo")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
      
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
         --头像隐藏缺门信息
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView.m_tabUserHead[viewid]:setShowCallCard(false)
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    gameLayer.leftCard=108 -13 *4 -1
    gameLayer.callcards={}
    for i=1,cmd.GAME_PLAYER do
        local viewid=gameLayer:SwitchViewChairID(i-1)
        local t={[1]=1,[17]=2,[33]=3} --万 是  1  索  17  童  33 
        gameLayer.callcards[viewid]=t[ msgTable.cbCallCard[1][i] ]
        gameLayer._gameView.m_tabUserHead[viewid]:showCallCard(gameLayer.callcards[viewid])
    end
    gameLayer._gameView._cardLayer:setCallcardKindList(gameLayer.callcards)
    for i=1,cmd.GAME_PLAYER do
        gameLayer._gameView._cardLayer:sortHandCard(i)
    end

    return 20,true
end


-- 回放出牌

function GameVideoReplay:onGetVideoOutcard(gameLayer, msgTable, bForward, bBackward)
    local cachemsg=cjson.encode({"onGetVideoOutcard", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    if bBackward then
        print("快退处理 onGetVideoOutcard")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)

        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    local wViewId = gameLayer:SwitchViewChairID(msgTable.wOutCardUser)

     LogAsset:getInstance():logData(cjson.encode("viewid "..wViewId.." 出牌 "..msgTable.cbOutCardData),true)

    
    print("bBackward:",bBackward)
    print("carddataBeforeOut:",table.concat(gameLayer.cbHandCardData[wViewId],","))
    gameLayer._gameView._cardLayer:outCard(wViewId, msgTable.cbOutCardData)
    gameLayer:playCardDataSound(msgTable.wOutCardUser, msgTable.cbOutCardData)
 
    print("viewid"..wViewId,"出牌"..msgTable.cbOutCardData)
    --存储界面数据
    table.insert(gameLayer.cbOutCardData[wViewId], msgTable.cbOutCardData)
    local bFound=false
    for i=1,#gameLayer.cbHandCardData[wViewId] do
        if gameLayer.cbHandCardData[wViewId][i] == msgTable.cbOutCardData then
            table.remove(gameLayer.cbHandCardData[wViewId], i)
            bFound=true
            break
        end 
    end
    assert(bFound)
    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 20, true
end

-- 回放发牌
function GameVideoReplay:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    
    local cachemsg=cjson.encode({"onGetVideoSendCard", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    if bBackward then
        print("快退处理 onGetVideoSendCard")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    local viewId = gameLayer:SwitchViewChairID(msgTable.wCurrentUser)

     LogAsset:getInstance():logData(cjson.encode("viewid "..viewId.." 收到牌 "..msgTable.cbCardData),true)
    
    gameLayer._gameView:onUpdataLeftCard(-1)

    gameLayer.leftCard = gameLayer.leftCard -1

    print("gameSendCard:",viewId, msgTable.cbCardData)
    gameLayer._gameView:gameSendCard(viewId, msgTable.cbCardData)

    table.insert(gameLayer.cbHandCardData[viewId], msgTable.cbCardData)
    --gameLayer.leftCard = gameLayer.leftCard -1

    --print("转换视图 ==> ", viewId)
    -- 隐藏准备
    gameLayer._gameView.btStart:setVisible(false)

    return 20, true
end


-- 回放操作结果
function GameVideoReplay:onGetVideoOperateResult(gameLayer, msgTable, bForward, bBackward)
    
    local cachemsg=cjson.encode({"onGetVideoOperateResult", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    local bNoDelay = (bBackward or bForward)
    if bBackward then
        print("快退处理 onGetVideoOperateResult")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub
        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end
   
    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end
    
    gameLayer:playCardOperateSound(msgTable.wOperateUser,msgTable.cbOperateCode)

    local wOperateViewId = gameLayer:SwitchViewChairID(msgTable.wOperateUser)
    local wProvideViewId = gameLayer:SwitchViewChairID(msgTable.wProvideUser)

    local removeHandN=0
    local tagAvtiveCard={}
    tagAvtiveCard.cbType = GameLogic.SHOW_NULL
    if msgTable.cbOperateCode == GameLogic.WIK_PENG then
        tagAvtiveCard.cbType = GameLogic.SHOW_PENG
        tagAvtiveCard.cbCardNum = 3
        removeHandN=2
        local card=msgTable.cbOperateCard
        tagAvtiveCard.cbCardValue = {card,card,card}
    elseif msgTable.cbOperateCode == GameLogic.WIK_GANG then
        --判断杠类型
        if wOperateViewId ~= wProvideViewId then 
            tagAvtiveCard.cbType = GameLogic.SHOW_MING_GANG
            removeHandN=3
        else
            for i=1,#gameLayer._gameView._cardLayer.cbActiveCardData[wOperateViewId] do
                --查找是否已经碰
                local activeInfo = gameLayer._gameView._cardLayer.cbActiveCardData[wOperateViewId][i]
                if activeInfo.cbCardValue[1] == msgTable.cbOperateCard then --有碰
                    tagAvtiveCard.cbType = GameLogic.SHOW_BU_GANG --SHOW_BU_GANG --补杠的 wOperateViewId 一定等于wProvideViewId吗？
                end
                removeHandN=1
            end
            if tagAvtiveCard.cbType == GameLogic.SHOW_NULL then
                tagAvtiveCard.cbType = GameLogic.SHOW_AN_GANG  
                removeHandN=4
            end
        end
        tagAvtiveCard.cbCardNum = 4
        local card = msgTable.cbOperateCard
        tagAvtiveCard.cbCardValue = {card,card,card,card}
        print("操作的麻将信息", tagAvtiveCard.cbType, tagAvtiveCard.cbCardNum, tagAvtiveCard.cbCardValue[1])
    end

    print("msgTable type ",tagAvtiveCard.cbType)
    gameLayer._gameView._cardLayer:createActiveCard(wOperateViewId, tagAvtiveCard, wProvideViewId)

    if msgTable.cbOperateCode == GameLogic.WIK_PENG then
        gameLayer._gameView:showOperateAction(wOperateViewId,GameLogic.WIK_PENG+60)
    elseif msgTable.cbOperateCode == GameLogic.WIK_GANG then
        local charttype= tagAvtiveCard.cbType==GameLogic.SHOW_AN_GANG and cmd.CHARTTYPE.XIAYU_TYPE or cmd.CHARTTYPE.GUAFENG_TYPE
        gameLayer._gameView:showOperateAction(wOperateViewId,charttype)
    end

    if tagAvtiveCard.cbType == GameLogic.SHOW_BU_GANG then
        for i=1,#gameLayer.cbActivityCardData[wOperateViewId] do
            if gameLayer.cbActivityCardData[wOperateViewId][i].cbCardValue[1] == msgTable.cbOperateCard then
                table.remove(gameLayer.cbActivityCardData[wOperateViewId],i)
                break
            end
        end

    end

    local opeStr={[GameLogic.WIK_PENG]=" 碰牌 ",[GameLogic.SHOW_BU_GANG]=" 补杠 ",[GameLogic.SHOW_AN_GANG]=" 暗杠 ",[GameLogic.SHOW_MING_GANG]=" 明杠 "}

    LogAsset:getInstance():logData(cjson.encode("viewid "..wOperateViewId..(opeStr[tagAvtiveCard.cbType] or " 操作类型"..msgTable.cbOperateCode.." ")..msgTable.cbOperateCard),true)

    table.insert(gameLayer.cbActivityCardData[wOperateViewId], tagAvtiveCard)

    if  wOperateViewId ~= wProvideViewId then
        table.remove(gameLayer.cbOutCardData[wProvideViewId], #gameLayer.cbOutCardData[wProvideViewId])
    end

    print("before operate cards:",table.concat(gameLayer.cbHandCardData[wOperateViewId],","))

    --删除手牌
    for i=1,removeHandN do
        local bFound=false
        for j = #gameLayer.cbHandCardData[wOperateViewId], 1, -1 do
            if gameLayer.cbHandCardData[wOperateViewId][j] ==  msgTable.cbOperateCard then
                table.remove(gameLayer.cbHandCardData[wOperateViewId], j)
                bFound=true
                break
            end
        end
        assert(bFound)
    end

    print("after operate cards:",table.concat(gameLayer.cbHandCardData[wOperateViewId],","))

    return 30, true
end

function GameVideoReplay:onGetVideoChiHu(gameLayer, msgTable, bForward, bBackward)
    
    local cachemsg=cjson.encode({"onGetVideoChiHu", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    if bBackward then
        print("快退处理 onGetVideoChiHu")
        local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
        if not self:backwardCheck(msg, msgTable) then
            return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
        end
        
        -- 更新玩家牌
        self:backwardUpdateUserCards(msg, gameLayer)
        --隐藏胡牌标志
        for viewid=1,cmd.GAME_PLAYER do
            gameLayer._gameView:setShowHuSign(viewid,false)
        end
    else
        -- 存储数据
        local msg = {}
        msg.msgsub = msgTable.msgsub

        -- 三个玩家牌
        self:cacheUserCards(msg, gameLayer, bForward)

        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
    end

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    gameLayer:playCardOperateSound(msgTable.wChiHuUser,GameLogic.WIK_CHI_HU)
    local huViewId=gameLayer:SwitchViewChairID(msgTable.wChiHuUser)
    gameLayer._gameView:showOperateAction(huViewId,GameLogic.WIK_CHI_HU)
    return 30,true
end

-- 回放结算
function GameVideoReplay:onGetVideoGameConclude(gameLayer, msgTable, bForward, bBackward)
    
    local cachemsg=cjson.encode({"onGetVideoGameConclude", bForward, bBackward})
    LogAsset:getInstance():logData(cachemsg,true)

    if bBackward then
        print("快退处理 onGetVideoGameConclude")
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

    if xzvideo_debug then
        for ii=1,cmd.GAME_PLAYER do
            local t={"viewid "..ii,"gamelayercards",table.concat(gameLayer.cbHandCardData[ii],","),"cardlayercards",table.concat(gameLayer._gameView._cardLayer.cbCardData[ii],",")}
            LogAsset:getInstance():logData(cjson.encode(t),true)
        end
    end

    -- local jsonStr = cjson.encode(msgTable)
    -- LogAsset:getInstance():logData(jsonStr, true)

     --   gameLayer.bEnd=true
    gameLayer.leftUserViewId=gameLayer:SwitchViewChairID(msgTable.wLeftUser)
    gameLayer.myTagCharts=msgTable.tagHistoryChart[gameLayer:GetMeChairID()+1]
    print("OnSubGameEnd")
    for i=1,#gameLayer.myTagCharts do
        print("gameLayer.myTagCharts"..i..".charttype:", gameLayer.myTagCharts[i].charttype)
        gameLayer.myTagCharts[i].bEffectChairID=gameLayer.myTagCharts[i].bEffectChairID[1]
    end

    for chairId=1,cmd.GAME_PLAYER do
        local cbCardData={}
        print("msgTable.cbCardCount[1][viewId]: ",msgTable.cbCardCount[1][chairId])
        for i=1,msgTable.cbCardCount[1][chairId] do
            table.insert(cbCardData,msgTable.cbCardData[chairId][i])
        end
        local viewId=gameLayer:SwitchViewChairID(chairId-1)
        
        gameLayer._gameView._cardLayer:showUserTileMajong( viewId, cbCardData,gameLayer.tabCallCard[viewId])
        
        gameLayer.tabHupaiInfo[viewId].gameScore=msgTable.lGameScore[1][chairId]
    end
    
    gameLayer._gameView:showGameResult(gameLayer.tabHupaiInfo)
    gameLayer._gameView:stopAllActions()

    return 40,false
end

function GameVideoReplay:onGetPrivateRoundEnd( gameLayer, msgTable, bForward, bBackward )

    gameLayer._gameView._priView:onPriGameEnd()

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
    gameLayer.callcards=msg.callcards or {}

    --手牌排序
    for i=1,cmd.GAME_PLAYER do
        local viewid = gameLayer:SwitchViewChairID(i - 1)

        LogAsset:getInstance():logData(cjson.encode("backwardUpdateUserCards viewid "..viewid..":"..table.concat(userHandCards[viewid],",")),true)

        if #userHandCards[viewid] > 0 then
           userHandCards[viewid]=GameLogic.SortCardList(userHandCards[viewid],nil,gameLayer.callcards[viewid])
        end
    end

    local userOutCards = clone(msg.userOutCards)
    local userActivityCards = clone(msg.userActivityCards)

    gameLayer.cbHandCardData = userHandCards
    gameLayer.cbOutCardData = userOutCards
    gameLayer.cbActivityCardData = userActivityCards
    gameLayer.leftCard = msg.leftCard

    gameLayer._gameView:onUpdataLeftCard(msg.leftCard)

    
    gameLayer._gameView:updateCardsNode(userHandCards, userOutCards, userActivityCards, gameLayer.leftCard,gameLayer.callcards)
end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer, isForward)
    -- 三个玩家牌
    local userHandCards = {}
    local userOutCards = {}
    local userActivityCards = {}

    userHandCards = clone(gameLayer.cbHandCardData)

    userOutCards = clone(gameLayer.cbOutCardData)
    userActivityCards = clone(gameLayer.cbActivityCardData)

    msg.userHandCards = userHandCards
    msg.userOutCards = userOutCards
    msg.userActivityCards = userActivityCards
    msg.callcards = gameLayer.callcards or {}
    msg.leftCard = gameLayer.leftCard

    if isForward then
        gameLayer:stopAllActions()
        --gameLayer._gameView:onResetData()
        gameLayer._gameView:stopAllActions()

            --手牌排序
        for viewid=1,cmd.GAME_PLAYER do
            if #userHandCards[viewid] > 0 then
               userHandCards[viewid]=GameLogic.SortCardList(userHandCards[viewid],nil,msg.callcards[viewid])
            end
        end

        gameLayer._gameView:updateCardsNode(userHandCards, userOutCards, userActivityCards,  gameLayer.leftCard,msg.callcards)
    end

    if xzvideo_debug then
        for i=1,cmd.GAME_PLAYER do
            LogAsset:getInstance():logData(cjson.encode("cacheUserCards viewid "..i..": "..table.concat(userHandCards[i],",")),true)
        end
    end
end

-- 战绩详情
function GameVideoReplay:getVideoResultView( cmdlist )
    local msgTab = nil
    for k,v in pairs(cmdlist) do
        print("v.msgsub:",v.msgsub)
        if v.msgsub == cmd.SUB_S_GAME_END then --找到总结算
            msgTab = v
            break
        end
    end
    if type(msgTab) ~= "table" then
        print("结算消息包异常")
        print("type(msgTab):",type(msgTab))
        return false, "结算消息包异常"
    end

    local jsonStr = cjson.encode(msgTab)
    LogAsset:getInstance():logData(jsonStr, true)

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
      
        result.lScore = lScore
        --胡牌类型
        result.bHu = msgTab.dwChiHuRight[1][i]~=0

        result.cbCardData = {}
        --手牌
        for j = 1, msgTab.cbCardCount[1][i] do
            result.cbCardData[j] = msgTab.cbCardData[i][j]
        end


        result.huCard=msgTab.cbChiHuCardArray[1][i]
 
             --先设置碰杠出的牌
        if msgTab.cbWeaveCount[1][i] > 0 then
            local tabActiveCardData = {}
            for j = 1, msgTab.cbWeaveCount[1][i] do
                local cbOperateData = {} --此处为tagAvtiveCard
                local weaveItem=msgTab.WeaveItemArray[i][j]
                cbOperateData.cbCardValue = {weaveItem.cbCenterCard}
                        
                local nShowStatus = GameLogic.SHOW_NULL
                local cbParam = weaveItem.cbWeaveKind
                
                if cbParam == GameLogic.WIK_PENG then
                    nShowStatus = GameLogic.SHOW_PENG
                    cbOperateData.cbCardNum = 3
                elseif cbParam == GameLogic.WIK_GANG then
                    if weaveItem.cbPublicCard==0 then
                        nShowStatus = GameLogic.SHOW_AN_GANG 
                    else --==1
                        if weaveItem.wProvideUser==i-1 then
                            nShowStatus=GameLogic.SHOW_BU_GANG 
                        else
                            nShowStatus=GameLogic.SHOW_MING_GANG 
                        end
                    end
                    cbOperateData.cbCardNum = 4
                end
                cbOperateData.cbType = nShowStatus

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
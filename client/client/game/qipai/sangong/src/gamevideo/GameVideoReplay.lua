-- 录像处理
-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")
local PriGameLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.privateroom.PriGameLayer")

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
        content = cmd.GAME_NAME.."牌局回放,"..cmd.GAME_NAME.."游戏精彩刺激，一起来玩吧！",
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
--    dump(pBuffer,"GameVideoReplay:onReadMessage=================")
    local datalen = pBuffer:getlen()
--    print("data len ",datalen)
   
    local curlen = pBuffer:getcurlen()
    if curlen < datalen then
        local sub = pBuffer:readword()
        if 114 == sub then
--            print("回放->游戏开始")
            local t = --游戏开始
            {    
                {t = "word", k = "wPlayerCount"},                               --真实游戏人数
                {t = "word", k = "wGamePlayerCountRule"},                       --游戏规则人数，没有为最大值
                {t = "word", k = "wChairID"},                                   --用户椅子号
                {t = "word", k = "wRoomID"},                                    --房主椅子号
                {t = "bool", k = "bCallBanker"},					            --是否抢庄
                {t = "score", k = "lScore"},                                    --用户分数
                {t = "score", k = "lCellScore"}				                    --基础积分
            }
            local cmdtable = {}   
            cmdtable.msgsub = 114
            cmdtable.startinfo = {}

            local buffer = ExternalFun.read_netdata(t, pBuffer)
            table.insert(cmdtable.startinfo, buffer)
            if buffer.wPlayerCount>=2 then
                for i=1,buffer.wPlayerCount-1 do
                    buffer = ExternalFun.read_netdata(t, pBuffer)
                    table.insert(cmdtable.startinfo, buffer)
                end
            end
--            dump(cmdtable, "114")
            return cmdtable
        elseif sub == cmd.SUB_S_READY_BEGIN then 
--            print("SUB_S_READY_BEGIN")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_ReadyBein, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_READY_BEGIN
--            dump(cmdtable, "SUB_S_READY_BEGIN")
            return cmdtable
        elseif sub == cmd.SUB_S_CALL_BEGIN then 
--            print("SUB_S_CALL_BEGIN")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_CallBegin, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_CALL_BEGIN
--            dump(cmdtable, "SUB_S_CALL_BEGIN")
            return cmdtable
        elseif sub == cmd.SUB_S_CALL_BANKER then 
--            print("SUB_S_CALL_BANKER")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_CallBanker, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_CALL_BANKER
--            dump(cmdtable, "SUB_S_CALL_BANKER")
            return cmdtable
        elseif sub == cmd.SUB_S_ADD_BEGIN then 
--            print("SUB_S_ADD_BEGIN")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_AddBegin, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_ADD_BEGIN
--            dump(cmdtable, "SUB_S_ADD_BEGIN")
            return cmdtable
        elseif sub == cmd.SUB_S_ADD_SCORE then 
--            print("SUB_S_ADD_SCORE")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_AddScore, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_ADD_SCORE
--            dump(cmdtable, "SUB_S_ADD_SCORE")
            return cmdtable
        elseif sub == cmd.SUB_S_SEND_CARD then 
--            print("SUB_S_SEND_CARD")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_SendCard, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_SEND_CARD
--            dump(cmdtable, "SUB_S_SEND_CARD")
            return cmdtable
        elseif sub == cmd.SUB_S_OPEN_CARD then 
--            print("SUB_S_OPEN_CARD")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_Open_Card, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_OPEN_CARD
--            dump(cmdtable, "SUB_S_OPEN_CARD")
            return cmdtable
        elseif sub == cmd.SUB_S_GAME_END then 
--            print("SUB_S_GAME_END")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_GAME_END
--            dump(cmdtable, "SUB_S_GAME_END")
            return cmdtable
        elseif sub == cmd.SUB_S_RECORD then 
--            print("SUB_S_RECORD")
            local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_RECORD, pBuffer)
            cmdtable.msgsub = cmd.SUB_S_RECORD
--            dump(cmdtable, "SUB_S_RECORD")
            return cmdtable
        -- elseif sub == cmd.SUB_S_GAME_START
        --     cmdtable.msgsub = cmd.SUB_S_GAME_START
        --     return
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

    if sub == 114 then
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoData(gameLayer, msgTable, bForward, bBackward)
    -- elseif sub == cmd.SUB_S_GAME_START then
    --     return self:onGetVideoGameStart(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_READY_BEGIN then
--        dump(msgTable,"SUB_S_READY_BEGIN")
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoReadyBegin(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_CALL_BEGIN then 
--        dump(msgTable,"SUB_S_CALL_BEGIN")
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoCallBegin(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_CALL_BANKER then 
--        dump(msgTable,"SUB_S_CALL_BANKER")
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoCallBanker(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_ADD_BEGIN then 
--        dump(msgTable,"SUB_S_ADD_BEGIN")
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoAddBegin(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_ADD_SCORE then 
--        dump(msgTable,"SUB_S_ADD_SCORE")
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoAddScore(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_SEND_CARD then 
--        dump(msgTable,"SUB_S_SEND_CARD")
        GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msgTable
        return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    elseif sub == cmd.SUB_S_OPEN_CARD then 
--        dump(msgTable,"SUB_S_OPEN_CARD")
    elseif sub == cmd.SUB_S_GAME_END then
--        dump(msgTable,"SUB_S_GAME_END")
    elseif sub == cmd.SUB_S_RECORD then 
--        dump(msgTable,"SUB_S_RECORD")
    end
    return 10
end

-- 回放场景
function GameVideoReplay:onGetVideoScene(gameLayer, msgTable, bForward, bBackward)
    return 5, true   --第一个是每发一次的间隔，第二个是前进还是暂停
end

--获取回放基本信息
function GameVideoReplay:onGetVideoData(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        self:onClearTable(gameLayer)
    end
    self.m_wCurrentUser = gameLayer:GetMeChairID()
    self.bCallBanker = msgTable.startinfo[1].bCallBanker
    self.lCellScore = msgTable.startinfo[1].lCellScore
    self.bIsPrivateRoom = msgTable.bIsPrivateRoom
    self.nGameRound = msgTable.nGameRound
--    print(self.bCallBanker, self.lCellScore, self.bIsPrivateRoom, self.nGameRound, self.wRoomID)
    self.userData = { }
    for i = 1, cmd.GAME_PLAYER do
        if msgTable.startinfo[i] then
            self.userData[i] = msgTable.startinfo[i]
            if msgTable.startinfo[i].wChairID == msgTable.startinfo[i].wRoomID then
                self.wRoomID = msgTable.startinfo[i].wRoomID
            end
        end
    end
    gameLayer._gameView:SetRoomHolder(gameLayer:SwitchViewChairID(self.wRoomID))
end

-- 游戏开始
function GameVideoReplay:onGetVideoReadyBegin(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        self:onClearTable(gameLayer)
    end
end

-- function GameVideoReplay:onGetVideoGameStart(gameLayer, msgTable, bForward, bBackward)
--     if bBackward then
--         self:onClearTable(gameLayer)
--     end
--     gameLayer:onGameStart()
-- end

-- 抢庄开始
function GameVideoReplay:onGetVideoCallBegin(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        self:onClearTable(gameLayer)
    end
    gameLayer:PlaySound(cmd.RES .. "sound_res/GAME_START.wav")
    local bCallBanker = msgTable.bCallBanker
    if bCallBanker then
        gameLayer._gameView.btCallBanker:setVisible(true)
        gameLayer._gameView.btNoCallBanker:setVisible(true)
    end
    return 10, true
end

-- 玩家叫庄
function GameVideoReplay:onGetVideoCallBanker(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        self:onClearTable(gameLayer)
    end
    local bCallBanker = msgTable.bCallBanker
	local wCallBankerUser = msgTable.wCallBankerUser
        wCallBankerUser = gameLayer:SwitchViewChairID(wCallBankerUser)
    if bCallBanker then
        gameLayer._gameView.m_flagCallBanker[wCallBankerUser]:setSpriteFrame(cmd.GAME.."bg_callbanker.png")
    else
        gameLayer._gameView.m_flagCallBanker[wCallBankerUser]:setSpriteFrame(cmd.GAME.."bg_nocallbanker.png")
    end
    gameLayer._gameView.m_flagCallBanker[wCallBankerUser]:setVisible(true)
    if wCallBankerUser == cmd.MY_VIEWID then
        gameLayer._gameView.btCallBanker:setVisible(false)
        gameLayer._gameView.btNoCallBanker:setVisible(false)
    end
    return 10, true
end

-- 开始下注
function GameVideoReplay:onGetVideoAddBegin(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        self:onClearTable(gameLayer)
    end
    gameLayer._gameView.btCallBanker:setVisible(false)
    gameLayer._gameView.btNoCallBanker:setVisible(false)

    local bCallBanker = msgTable.bCallBanker
    self.wBankerUser = msgTable.wBankerUser
    self.lCellScore = msgTable.lCellScore

    if not bCallBanker then
        gameLayer:PlaySound(cmd.RES .. "sound_res/GAME_START.wav")
    end

    gameLayer._gameView:SetBanker(gameLayer:SwitchViewChairID(self.wBankerUser))
    for _, v in pairs(gameLayer._gameView.m_flagCallBanker) do
        v:setVisible(false)
    end

    -- 下注：庄家确定了之后，由其他玩家选择押注，下注时间为10秒，超过时间未下注的玩家，则按最低分数下注
    if gameLayer:SwitchViewChairID(self.m_wCurrentUser) ~= gameLayer:SwitchViewChairID(self.wBankerUser) then
        gameLayer._gameView.btAddScoreNum1:setString(""..1*self.lCellScore)
        gameLayer._gameView.btAddScoreNum2:setString(""..2*self.lCellScore)
        gameLayer._gameView.btAddScoreNum3:setString(""..3*self.lCellScore)
        gameLayer._gameView.btAddScoreNum4:setString(""..4*self.lCellScore)
        gameLayer._gameView.btAddScoreNum5:setString(""..5*self.lCellScore)
        gameLayer._gameView.nodeAddScore:setVisible(true)
    end
    return 10, true
end

-- 玩家下注
function GameVideoReplay:onGetVideoAddScore(gameLayer, msgTable, bForward, bBackward)
    if bBackward then
        self:onClearTable(gameLayer)
    end
    local wAddScoreUser = msgTable.wAddScoreUser
    local lAddScoreCount = msgTable.lAddScoreCount
    gameLayer._gameView:SetUserTableScore(gameLayer:SwitchViewChairID(wAddScoreUser), lAddScoreCount)
    if gameLayer:SwitchViewChairID(wAddScoreUser) == cmd.MY_VIEWID then
        gameLayer._gameView.nodeAddScore:setVisible(false)
    end
    return 10, true
end

-- 发牌
function GameVideoReplay:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
    gameLayer:PlaySound(cmd.RES .. "sound_res/SEND_CARD_BEGIN.wav")
    self.cbCardData = msgTable.cbCardData
    local delayCount = 1
    for index = 1, 3 do
        for i = 1, cmd.GAME_PLAYER do
            local chair = math.mod(self.wBankerUser + i - 1, cmd.GAME_PLAYER)
            if self.userData[chair+1] then
                local viewid = gameLayer:SwitchViewChairID(chair)
                self:SendCard(gameLayer, viewid, chair+1, index, delayCount * 0.1)
                delayCount = delayCount + 1
            end
        end
    end
    return 40, true
end

--发牌动画
function GameVideoReplay:SendCard(gameLayer,viewid,chair,index,fDelay)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	local fInterval = 0.1
    local ptCard = {cc.p(390, 490), cc.p(240, 330), cc.p(670, 95), cc.p(1020, 330), cc.p(870, 490), cc.p(630, 500)}
	local this = gameLayer._gameView
	local nodeCard = gameLayer._gameView.userCard[viewid]
	nodeCard.area:setVisible(true)

	local spriteCard = nodeCard.card[index]
	spriteCard:stopAllActions()
	spriteCard:setScale(1.0)
	spriteCard:setVisible(true)
	spriteCard:setSpriteFrame(cmd.GAME.."card_back.png")
	spriteCard:move(display.cx, display.cy + 170)
	spriteCard:runAction(
		cc.Sequence:create(
		    cc.DelayTime:create(fDelay),
		    cc.CallFunc:create(
			    function ()
				    this._scene:PlaySound(cmd.RES.."sound_res/CENTER_SEND_CARD.wav")
			    end
			),
			cc.Spawn:create(
				cc.ScaleTo:create(0.25,viewid==cmd.MY_VIEWID and 1.0 or 0.7),
				cc.MoveTo:create(0.25, cc.p(
					ptCard[viewid].x - (viewid==cmd.MY_VIEWID and 125 or 0) + (viewid==cmd.MY_VIEWID and (i==1 and 0 or 120) or 35)*(index- 1),ptCard[viewid].y))  --牌的间距
				),
            cc.DelayTime:create(0.5),
            cc.CallFunc:create(
			    function ()
                    if index == 3 then
                        gameLayer._gameView:SetUserCard(viewid, self.cbCardData[chair])
                        gameLayer:PlaySound(cmd.RES .. "sound_res/GAME_END.wav")
                    end
			    end
                )
			)
		)

end

-- 清理桌面玩家信息
function GameVideoReplay:onClearTable(gameLayer)
--    local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
--    if not self:backwardCheck(msg, msgTable) then
--        return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
--    end
    for i = 1, cmd.GAME_PLAYER do
        gameLayer._gameView.btCallBanker:setVisible(false)
        gameLayer._gameView.btNoCallBanker:setVisible(false)
        gameLayer._gameView.nodeAddScore:setVisible(false)
        gameLayer._gameView.userCard[i].area:setVisible(false)
        gameLayer._gameView.m_ScoreView[i].frame:setVisible(false)
        gameLayer._gameView.m_ScoreView[i].score:setVisible(false)
        gameLayer._gameView.m_BankerFlag:setVisible(false)
        gameLayer._gameView.m_flagCallBanker[i]:setVisible(false)
    end
    for i = 1, GameVideo:getInstance().m_nMessageIdx do
        self:onHandleMessage(gameLayer, GameVideo:getInstance().m_tabStepVideoMessage[i])
    end
end

return GameVideoReplay
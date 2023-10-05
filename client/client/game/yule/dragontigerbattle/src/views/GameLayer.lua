local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local Game_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")
local GameServer_CMD = appdf.req(appdf.HEADER_SRC.."CMD_GameServer")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.GameViewLayer")
local GameFrame = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.GameFrame")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")

function GameLayer:ctor(frameEngine, scene)	
	self._dataModle = GameFrame:create()
	--游戏是否处于空闲状态
	GameLayer.super.ctor(self, frameEngine, scene)
	GameLayer.super.OnInitGameEngine(self)

	-- for i=1, 20 do
	-- 	self:onGameMessageSimpleChart()
	-- end
end

--获取gamekind
function GameLayer:getGameKind()
    return Game_CMD.KIND_ID
end

--创建场景
function GameLayer:CreateView()
    return GameViewLayer:create(self)
        :addTo(self)
end

--网络处理
--------------------------
--场景消息
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
	self:resetUser()
	self.m_cbGameStatus = cbGameStatus
	if cbGameStatus == Game_CMD.GAME_SCENE_FREE then
		self:onGameSceneFree( dataBuffer )
	elseif cbGameStatus == Game_CMD.GAME_SCENE_JETTON or cbGameStatus == Game_CMD.GAME_SCENE_END then
		self:onGameScenePlaying( dataBuffer,  cbGameStatus)
	end
	-- self:dismissPopWait()
end

--刷新所有玩家信息
function GameLayer:resetUser()
	-- --先清理玩家信息
 --    for i = 1, Game_CMD.GAME_PLAYER do
 --        local wViewChairId = self:SwitchViewChairID(i-1)
 --        self._gameView:OnUpdateUserExit(wViewChairId)
 --    end

    --初始化已有玩家
    self._dataModle:removeAllUser()
    for i = 1, Game_CMD.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        if userItem then
       		self._dataModle:addUser(userItem)
        end
   end
end

-- 游戏数据
function GameLayer:onEventGameMessage(sub, dataBuffer)
	print(sub,"---------游戏数据-----sub------------------")
	if sub == Game_CMD.SUB_S_GAME_FREE then         
		--游戏空闲
		self.GameStatus = Game_CMD.GAME_SCENE_FREE
		self:onGameMessageFree(dataBuffer)
	elseif sub == Game_CMD.SUB_S_GAME_START then        
		--游戏开始
		if not self._gameView:isMeIsBanker() and self._gameView.myAllJettonScore == 0 then
			self.GameStatus = Game_CMD.GAME_SCENE_FREE
		else
			self.GameStatus = Game_CMD.SUB_S_CONTINU_JETTON
		end
		self:onGameMessageStart(dataBuffer)

	elseif sub == Game_CMD.SUB_S_PLACE_JETTON then     
		--用户下注
		if not self._gameView:isMeIsBanker() and self._gameView.myAllJettonScore == 0 then
			self.GameStatus = Game_CMD.GAME_SCENE_FREE
		else
			self.GameStatus = Game_CMD.SUB_S_CONTINU_JETTON
		end
		self:onGameMessagePlaceJetton(dataBuffer)
	elseif sub == Game_CMD.SUB_S_PLACE_JETTON_FAIL then         
		-- 下注失败
		self:onGameMessagePlaceJettonFail(dataBuffer)
	elseif sub == Game_CMD.SUB_S_CONTINU_JETTON then
		--续投
		self:onGameMessageReject(dataBuffer)
	elseif sub == Game_CMD.SUB_S_PLACE_JETTON_FAIL then
		self:onGameMessageRejectFaild(dataBuffer)
	elseif sub == Game_CMD.SUB_S_GAME_END then       
		--游戏结束 
		if self._gameView:isMeIsBanker() then
			self.GameStatus = Game_CMD.GAME_SCENE_JETTON
		else
			self.GameStatus = Game_CMD.GAME_SCENE_FREE
		end
		self:onGameMessageEnd(dataBuffer)
	elseif sub == Game_CMD.SUB_S_SEND_RECORD then   
			--游戏记录 
		-- self:onGameMessageSendRecord(dataBuffer)
	elseif sub == Game_CMD.SUB_S_OCCUPYSEAT then
		--占位结果
		self:onGameMessageOccupy(dataBuffer)
	elseif sub == Game_CMD.SUB_S_OCCUPYSEAT_FAIL then
		--占位失败
		self:onGameMessageOccupyFaild(dataBuffer)
	elseif sub == Game_CMD.SUB_S_UPDATE_OCCUPYSEAT then
		--更新占位
		self:updateGameMessageOccupy(dataBuffer)
	elseif sub == Game_CMD.SUB_S_OCCUPYSEAT_STANDUP then
		--占位起立
		self:onGameMessageOccupyLeave(dataBuffer)
	elseif sub == Game_CMD.SUB_S_SEND_SIMPLE_CHART then
		--简洁路单
		self:onGameMessageSimpleChart(dataBuffer)
	elseif sub == Game_CMD.SUB_S_SEND_ADVANCED_CHART then
		--高级路单
		self:onGameMessageAdvancedChart(dataBuffer)
	elseif sub == Game_CMD.SUB_S_APPLY_BANKER then
		--申请庄家
		self:onGameMessageApplyBanker(dataBuffer)
	elseif sub == Game_CMD.SUB_S_CANCEL_BANKER then
		--取消申请
		self:onGameMessageCancelBanker(dataBuffer)
	elseif sub == Game_CMD.SUB_S_CHANGE_BANKER then
		--切换庄家
		self:onGameMessageChageBanker(dataBuffer)
	elseif sub == Game_CMD.SUB_S_CHANGE_BANKER then
		--庄家非空闲时段下庄
		self:onGameMessageBankerQuit()		
	elseif sub == Game_CMD.SUB_S_SYSTIP then
		--系统提示
		self:onGameMessageNotice(dataBuffer)
	elseif sub == Game_CMD.SUB_S_STATISTICS_RECORD then 
		self:onGameLinePlayer(dataBuffer)
	end 
end

function GameLayer:onGameSceneFree(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_StatusFree, dataBuffer) 
	dump(cmd_table,"------------游戏空闲------------",6)
	self._gameView:onGameSceneFree(cmd_table)
	self:SetGameClock(self:GetMeChairID(), 1, cmd_table.cbTimeLeave)

	--等待上庄
	self._gameView:showWaitBnaker();
	--隐藏庄家
	self._gameView:hideBankerBtn()
end

function GameLayer:onGameScenePlaying(dataBuffer, cbGameStatus)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_StatusPlay, dataBuffer) 
	dump(cmd_table,"------------游戏状态------------",6)
	self._gameView:onGameScenePlaying(cmd_table, cbGameStatus)
	self:SetGameClock(self:GetMeChairID(), 1, cmd_table.cbTimeLeave)

	--隐藏庄家
	self._gameView:hideBankerBtn()
end

--游戏空闲
function GameLayer:onGameMessageFree(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_GameFree, dataBuffer)
	self._gameView:onGameFree(cmd_table)
	--空闲倒计时 
	self:SetGameClock(self:GetMeChairID(), 1, cmd_table.cbTimeLeave)
	self.m_cbGameStatus = 0

	--等待上庄
	self._gameView:showWaitBnaker();
end

-- --游戏开始
function GameLayer:onGameMessageStart(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_GameStart, dataBuffer)
	self._gameView:onGameStart(cmd_table)
	--游戏开始倒计时
	self:SetGameClock(self:GetMeChairID(), 1, cmd_table.cbTimeLeave)
	self.m_cbGameStatus = 0
	
	--隐藏等待上庄
	if self._gameView.waitShangZhuang then
        self._gameView.waitShangZhuang:setVisible(false)
    end
end

--游戏结束
function GameLayer:onGameMessageEnd(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_GameEnd, dataBuffer)
	-- dump(cmd_table.lUserAreaWinScore,"lUserAreaWinScore",12)
	self._gameView:onGameEnd(cmd_table)
	self:SetGameClock(self:GetMeChairID(), 1, cmd_table.cbTimeLeave)
	self.m_cbGameStatus = 0
end
-----------------------------------------------------------------
--网络处理
--申请上庄
function GameLayer:sendApplyBanker()
	local cmddata = CCmd_Data.create(0)
	self:SendData(Game_CMD.SUB_C_APPLY_BANKER, cmddata)
end

--申请下庄
function GameLayer:sendCancelApply()
    local cmddata = CCmd_Data:create(0)
    self:SendData(Game_CMD.SUB_C_CANCEL_BANKER, cmddata)
end

--申请上庄
function GameLayer:onGameMessageApplyBanker(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_ApplyBanker,dataBuffer)
    self._dataModle:addApplyUser(cmd_table.wApplyUser, false) 
    self._gameView:onApplyBanker(cmd_table)
end

--申请下庄
function GameLayer:onGameMessageCancelBanker(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_CancelBanker,dataBuffer)
	--从申请列表移除
    self._dataModle:removeApplyUser(cmd_table.wCancelUser)
    
    self._gameView:onCancelBanker(cmd_table)
end

--庄家非空闲下庄
function GameLayer:onGameMessageBankerQuit()
    self._gameView:onGameMessageBankerQuit()
end

--切换庄家
function GameLayer:onGameMessageChageBanker(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_ChangeBanker,dataBuffer)
	self._gameView:changeBanker(cmd_table)
end

--系统提示
function GameLayer:onGameMessageNotice(dataBuffer)
	local string = dataBuffer:readstring()
    showToast(self, string, 1)
end

--从庄家列表中移除庄家
function GameLayer:removeBankerFromBankerList(wCancelUser)
	self._dataModle:removeApplyUser(wCancelUser)
end

--发送下注
function GameLayer:SendPlaceJetton( jettonScore, jettonArea )
	-- body
	local cmddata = ExternalFun.create_netdata(Game_CMD.CMD_C_PlaceJetton)
	cmddata:pushbyte(jettonArea)
	cmddata:pushscore(jettonScore)
	self:SendData(Game_CMD.SUB_C_PLACE_JETTON, cmddata)
end

--用户下注
function GameLayer:onGameMessagePlaceJetton(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_PlaceJetton, dataBuffer)
	if self._gameView:isMeChair(cmd_table.wChairID) then
		self.m_cbGameStatus =Game_CMD.GAME_SCENE_JETTON
		self.GameStatus = Game_CMD.GAME_SCENE_JETTON
	end

	self._gameView:onPlaceJetton(cmd_table)
end

--下注失败
function GameLayer:onGameMessagePlaceJettonFail(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_PlaceJettonFail, dataBuffer)
	self._gameView:onPlaceJettonFail(cmd_table)
end

--发送续投
function GameLayer:sendContinueJetton()
	local cmddata = CCmd_Data.create(0)

	self:SendData(Game_CMD.SUB_C_CONTINUE_JETTON, cmddata)
end

--续投
function GameLayer:onGameMessageReject(dataBuffer)

	-- local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_ContiueJetton, dataBuffer)
	-- self._gameView:onGameMessageReject(cmd_table)

	--每局只能续投一次
	self._gameView.isReject = true;
    self._gameView:upDateRejettonButton();

end

--续投失败
function GameLayer:onGameMessageRejectFaild(dataBuffer)
	showToast(self, "重复下注失败", 1)
end

--发送占位
function GameLayer:sendOccupySeat(chair, index)
	local cmddata = ExternalFun.create_netdata(Game_CMD.CMD_C_OccupySeat)
	cmddata:pushword(chair)
	cmddata:pushbyte(index)
	self:SendData(Game_CMD.SUB_C_OCCUPYSEAT, cmddata)
end

--占位结果
function GameLayer:onGameMessageOccupy(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_OccupySeat, dataBuffer)
	self._gameView:onGameMessageOccupy(cmd_table)
end

--占位失败
function GameLayer:onGameMessageOccupyFaild(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_OccupySeat_Fail, dataBuffer)
	self._gameView:onGameMessageOccupyFaild(cmd_table)
end

--更新占位
function GameLayer:updateGameMessageOccupy(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_UpdateOccupySeat, dataBuffer)
	self._gameView:updateGameMessageOccupy(cmd_table)
end

--占位起立
function GameLayer:onGameMessageOccupyLeave(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_OccupySeatStandUp, dataBuffer)

	self._gameView:onGameMessageOccupyLeave(cmd_table)
end

--简洁路单
function GameLayer:onGameMessageSimpleChart(dataBuffer)
	self._gameView.simplePoint = {}
	local len = dataBuffer:getlen();
	len = len-1;
    local count = math.floor(len/2)

    if (len - count * 2) ~= 0 then
        return 
    end
  
    for i=1, count do
        local tmp = {}
        for j=1, 2 do
            tmp[j] = dataBuffer:readbyte()
        end
        if tmp[1] == 0 then
        	break
        end
        table.insert(self._gameView.simplePoint, tmp)
    end
    -- self._gameView:onGameMessageSimpleChart()


    local cbGameStatus =  dataBuffer:readbyte();
    -- if Game_CMD.GAME_SCENE_FREE == cbGameStatus then
    	self._gameView:onGameMessageSimpleChart()
    -- end
end

--高级路单
function GameLayer:onGameMessageAdvancedChart(dataBuffer)
	self._gameView.advancedPoint = {}
	local len = dataBuffer:getlen()
	len = len-1;
    local count = math.floor(len/2)

    if (len - count * 2) ~= 0 then
        return 
    end
  
    for i=1, count do
        local tmp = {}
        for j=1, 2 do
            tmp[j] = dataBuffer:readbyte()
        end
        if tmp[1] == 0 then
        	break
        end
        table.insert(self._gameView.advancedPoint, tmp)
    end
    -- self._gameView:onGameMessageAdvancedChart()

    local cbGameStatus =  dataBuffer:readbyte();
    -- f Game_CMD.GAME_SCENE_FREE == cbGameStatus then
    	self._gameView:onGameMessageAdvancedChart()
    -- end

    -- local isExist =cc.FileUtils:getInstance():isFileExist("text.txt")
    -- local writablePath =cc.FileUtils:getInstance():getWritablePath()
    -- local a = json.encode(self._gameView.advancedPoint)
    -- self:writefile("text.txt", a)
end

function GameLayer:writefile(path, content, mode)
	mode = mode or "w+b"
	local file = io.open(path, a)
	if file then
		if file:write(content) == nil then return false end
		io.close(file)
		return true
	else
		return false
	end
end

--申请取款
function GameLayer:sendTakeScore(lScore, szPassword )
    local cmddata = ExternalFun.create_netdata(GameServer_CMD.CMD_GR_C_TakeScoreRequest)
    cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE, GameServer_CMD.SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushbyte(GameServer_CMD.SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushscore(lScore)
    cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)
    cmddata:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)
    self:sendNetData(cmddata)
end

function GameLayer:sendSaveScore(lScore)
	local cmddata = ExternalFun.create_netdata(GameServer_CMD.CMD_GR_C_SaveScoreRequest)
	cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_SAVE_SCORE_REQUEST)
	cmddata:pushbyte(GameServer_CMD.SUB_GR_SAVE_SCORE_REQUEST)
	cmddata:pushscore(lScore)
	self:sendNetData(cmddata)
end


--请求银行信息
function GameLayer:sendRequestBankInfo()
    local cmddata = ExternalFun.create_netdata(GameServer_CMD.CMD_GR_C_QueryInsureInfoRequest)
    cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--银行消息
function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    self:dismissPopWait()
    if sub == GameServer_CMD.SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureSuccess, dataBuffer)
        self.bank_success = cmd_table

        self._gameView:onBankSuccess()
        self:sendRequestBankInfo()
    elseif sub == GameServer_CMD.SUB_GR_USER_INSURE_FAILURE then
        local cmd_table = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureFailure, dataBuffer)
        self.bank_fail = cmd_table

        self._gameView:onBankFailure()
    elseif sub == GameServer_CMD.SUB_GR_USER_INSURE_INFO then --银行资料
        local cmdtable = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureInfo, dataBuffer)
        self._gameView:onGetBankInfo(cmdtable)
    else
        -- print("unknow gamemessage sub is ==>"..sub)
    end
end

-- --更新积分
-- function GameLayer:onGameMessageChangeUserScore( dataBuffer )
--     local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_ChangeUserScore,dataBuffer)
-- 	self._gameView:refreshScore(cmd_table)
-- end
-- ---------------------------------------------------------
function GameLayer:getDataMgr()
	return self._dataModle
end

-- 重置框架
function GameLayer:OnResetGameEngine()
    self._dataModle:removeAllUser()
    self._dataModle:initUserList(self:getUserList())
    self._gameView:cleanGameData()
end

function GameLayer:getUserList()
    return self._gameFrame._UserList
end

-- ----------用户消息
--用户进入
function GameLayer:onEventUserEnter(wTableID, wChairID, useritem)
	--缓存用户
	self._dataModle:addUser(useritem)
	self._gameView:refreshUserList()
	-- self._gameView:onGetUserScore(useritem)
end

--用户状态
function GameLayer:onEventUserStatus(useritem, newstatus, oldstatus)
	if newstatus.cbUserStatus == yl.US_FREE or newstatus == yl.US_NULL then
		self._dataModle:removeUser(useritem)
		self._gameView:refreshUserList()
	else
		self._dataModle:updateUser(useritem)
		self._gameView:refreshUserList()
	end
end

--用户分数
function GameLayer:onEventUserScore(useritem)


	-- print("onEventUserScore ==== ====> ",userite)
	-- dump(useritem,"onEventUserScore",12)
	self._dataModle:updateUser(useritem)

	self._gameView:onGetUserScore(useritem)
end

function GameLayer:sendNetData( cmddata )
    return self._gameFrame:sendSocketData(cmddata)
end

function GameLayer:sendLinePalyer()
	local cmddata = CCmd_Data.create(0)
	self:SendData(Game_CMD.SUB_C_REQUEST_STATISTICS_RECORD, cmddata)
end

function GameLayer:onGameLinePlayer( cmddata )
	-- body
	

	local len = cmddata:getlen()
	if math.fmod( len, 18 )~= 0 then 
		-- print("onGameLinePlayer == ",cmddata:getlen())
		return 
	end
	local lineTab = {}

	for i=1,cmddata:getlen()/18 do
		local cmd_table = ExternalFun.read_netdata(Game_CMD.CMD_S_StatisticsRecord, cmddata)
		lineTab[#lineTab+1] = cmd_table
	end

	self._gameView:showUserList(lineTab)
end

--额外规则
function GameLayer:getGameRule(buffer)
    local strTable ={}
    local betNum = ExternalFun.bytesToInt(buffer, 2, 9);
    local bankerLimit = ExternalFun.bytesToInt(buffer, 10, 17);
    local bankerNum = ExternalFun.bytesToInt(buffer, 18, 21);
    
    table.insert(strTable,"个人下注限制:" .. betNum);
    table.insert(strTable,"上庄配置:" .. bankerLimit);
    table.insert(strTable,"上庄局数:" .. bankerNum);
    return strTable
end

-- --byte数组转int
-- function GameLayer:bytesToInt(buffer, size, index)
-- 	local value = 0;
-- 	for i = index, size do
-- 		-- 
-- 		if (i - index) == 0 then
-- 			value = value + buffer[i];
-- 		else
-- 			value = value + bit:_lshift(buffer[i],((i - index) * 8));
-- 		end
-- 	end
-- 	return value;
-- end

--退出桌子
function GameLayer:onExitTable()
   
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE  then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    self._gameFrame:StandUp(1)
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    --print("delay leave")
                    self:onExitRoom()
                end
                )
            )
        )
        return
    end

   self:onExitRoom()
end

--离开房间
function GameLayer:onExitRoom()
    self._scene:onKeyBack()
end

--退出
function GameLayer:onQueryExitGame()
    if self._queryDialog then
       return
    end
    local tip = self.m_cbGameStatus > 0 and "现在退出游戏会由系统托管代打哟，输了可不能怪他哦！" or "您要退出游戏么？"
   
    self._queryDialog = QueryDialog:create(tip, function(ok)
        if ok == true then
            --退出防作弊
            self._gameFrame:setEnterAntiCheatRoom(false)
            if  self.m_cbGameStatus > 0 then

                self._gameFrame:onCloseSocket()
                self:dismissPopWait()
                self:stopAllActions()
                self:KillGameClock()
                self._scene:onKeyBack()
                return
            end
            
            self:onExitTable()
        end
        self._queryDialog = nil
    end):setCanTouchOutside(false)
        :addTo(self)
end

return GameLayer
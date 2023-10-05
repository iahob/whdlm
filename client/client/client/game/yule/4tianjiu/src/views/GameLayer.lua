local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)

local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.views.layer.GameViewLayer")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.GameLogic")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")


local IDI_STARTGAME=201
local IDI_CALLBANKER=202
local IDI_SETCHIP=203
local IDI_RANGECARD=204

function GameLayer:ctor(frameEngine, scene)
	self.name="GameLayer"
	GameLayer.super.ctor(self, frameEngine, scene)
	self.myChairId=self:GetMeChairID()
	self.m_cbGameStatus=0;
	self.m_cbGameStatus2 = 0;

	self.m_tabUserItem = {};
	self.tabUserItemCache = {};
	
	print("self.myChairId:",self.myChairId)

	self.bIsDismiss = false
	self.m_bLookOn = false
    self.m_playingUser = {}
end

function GameLayer:CreateView()
	dump(GameViewLayer,"GameViewLayer",6)
    return GameViewLayer:create(self):addTo(self)
end

function GameLayer:OnResetGameEngine()
	GameLayer.super.OnResetGameEngine(self)
	self.m_cbGameStatus=0;

	self.m_cbGameStatus2 = 0;
	self.bEnd=false
	self.allCardCount=32

	self._gameView:ReSetGameData()
	
	self.bIsDismiss = false
end

function GameLayer:onPlayingUserInfo(userInfo)
    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
    dump(self.m_playingUser,"-----------self.m_playingUser-------",6)
end

function GameLayer:onQueryExitGame()
    local bNoGame = false --没有参与游戏

    for i=1,#self.m_playingUser do
        if self.m_playingUser[i] == GlobalUserItem.tabAccountInfo.dwUserID  then
            bNoGame = false
            break
        elseif i == #self.m_playingUser then
            bNoGame = true
        end
    end

    if self.m_bLookOn  or bNoGame then
        self:onExitTable()
        return
    end

    if PriRoom and true == GlobalUserItem.bPrivateRoom and (false == GlobalUserItem.bPrivateGoldRoom or GlobalUserItem.dwCurRoomGroupID ~= 0) then  --金币房卡可以直接退出
        PriRoom:getInstance():queryQuitGame(self.m_cbGameStatus)
    else

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
end

function GameLayer:getGameKind()
    return cmd.KIND_ID
end

function GameLayer:onGetSitUserNum()
    -- return table.nums(self._gameView.tabHeadSp)
    local num = 0
	for i = 1, cmd.GAME_PLAYER do
		local player = self._gameView.node_player[i];
	    local player_item = player:getChildByName("player_item");
		if player_item:isVisible() then
			num = num + 1
		end
	end
    return num
end

--倒计时
function GameLayer:OnEventGameClockInfo(chairId,time,clockId)
	-- print(chairId, time, clockId,"---------OnEventGameClockInfo--------")
	if chairId~=self:GetMeChairID() then
		return
	end
	if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
		return
	end

	if not self._gameView.bTrustee and time<=4 then
		ExternalFun.playSoundEffect("GAME_WARN.WAV")
	end

	--勋章房
    if time <= 1 then
        -- print(GlobalUserItem.dwCurRoomGroupID, self.m_cbGameStatus, PriRoom:getInstance().m_tabPriData.dwPlayCount,"------------超时等待-----------------")
        if PriRoom and GlobalUserItem.dwCurRoomGroupID ~= 0 and self.m_cbGameStatus == cmd.GAME_STATUS_FREE then
            if PriRoom:getInstance().m_tabPriData.dwPlayCount == 0 then
                self:onExitTable()
            --else
                --准备
                --self._gameView:OnButtonClickedEvent(GameViewLayer.BT_READY,nil);
            end
        end
    end
    
	if clockId==IDI_STARTGAME then
		if self._gameView.bTrustee and time<=math.max(self._gameView.cbTimeStartGame-1,1) then
			
		elseif time<=1  then 
			self._gameFrame:setEnterAntiCheatRoom(false);    --退出防作弊
            self:onExitTable()
		end
	elseif clockId==IDI_CALLBANKER then
		
	elseif clockId==IDI_SETCHIP then
		
	elseif clockId==IDI_RANGECARD then
		
	end
end

function GameLayer:SetGameClock(chair,id,time)
	if GlobalUserItem.bVideo then
		return 
	end
	 GameLayer.super.SetGameClock(self,chair,id,time)
	 -- 设置计时器
    -- local viewid = self:GetClockViewID()
    -- if viewid and viewid ~= yl.INVALID_CHAIR then
    --    	self._gameView:createHeadAnim(viewid)
    --    	self._gameView:setShowParticle(viewid,time,true)
    --    	local progress = self._gameView.m_TimeProgress[viewid]  
    -- 	for i=1,2 do
    -- 		if progress[i] then
	   --          progress[i]:setPercentage(100)
	   --          progress[i]:setVisible(true)
	   --          progress[i]:runAction(cc.Sequence:create(cc.ProgressTo:create(time, 0), cc.CallFunc:create(function()
	   --              progress[i]:setVisible(false)
	   --          end)))
	   --      end
    --     end
    -- end

end

function GameLayer:KillGameClock(notView)
	if GlobalUserItem.bVideo then
		return 
	end

    local viewid = self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
      --   local progress = self._gameView.m_TimeProgress[viewid] or {}
      --   for i=1,2 do
    		-- if progress[i] then
	     --        progress[i]:stopAllActions()
	     --        progress[i]:setVisible(false)
	     --    end
      --   end

      --   self._gameView:setShowParticle(viewid,0,false)
    end
    
    GameLayer.super.KillGameClock(self,notView)

end

--系统消息
function GameLayer:onSystemMessage( wType,szString )
    if wType == 501 or wType == 515 or wType == 59 then
        self:KillGameClock()
        print("游戏币不足消息", szString)
        GlobalUserItem.bWaitQuit = true
        self:onSubNoticeAddScore(szString)
    end
end

function GameLayer:onSubNoticeAddScore(szString)
    --设置准备按钮不可见
    self.m_bNoScore = true

    if self.m_bStartGame  or GlobalUserItem.dwCurRoomGroupID ~= 0 then
        local msg = szString or "你的游戏币不足，无法继续游戏"
        self.m_querydialog = QueryDialog:create(msg,function()
            if not self.m_bClubRoom then 
                    self:onExitTable()
            end 
        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)
    else
        self.m_bNoScore = true
        self.m_szScoreMsg = szString
    end
end

--根据chiarID获取显示位置（自己固定为3）
function GameLayer:SwitchViewChairID(chairId)
    local middlePosition = 2;
    local positionOffset = middlePosition - self:GetMeChairID();
    local PLAYER_MAX = cmd.GAME_PLAYER;
    return (chairId + positionOffset + PLAYER_MAX) % PLAYER_MAX + 1;
end

--根据显示位置获取chiarID
function GameLayer:getChairIDByPosition(pos)
    local middlePosition = 2;
    local positionOffset = middlePosition - self:GetMeChairID();
    local PLAYER_MAX = cmd.GAME_PLAYER;
    return (pos -1- positionOffset + PLAYER_MAX) % PLAYER_MAX;
end

function GameLayer:SwitchViewChairIDBeforeSceneMsg(chairId)
	self.myChairId=self:GetMeChairID()

	local viewid=cmd.MY_VIEWID +(chairId-self.myChairId)
	return viewid>0 and viewid or viewid+cmd.GAME_PLAYER
end

function GameLayer:getParentNode()
	return self._scene
end

------网络发送
--发送战绩
function GameLayer:sendZhanjiRequest(nBeg,nEnd)
	-- if not self.m_bOnGame then return end
	-- local cmd_data = CCmd_Data:create(8)
	-- cmd_data:pushint(nBeg)
	-- cmd_data:pushint(nEnd)
	-- self:SendData(cmd.SUB_C_QUERY_WINLOST, cmd_data)

	if not self.m_bOnGame then return end
	local cmd_data = CCmd_Data:create(0) 
	self:SendData(cmd.SUB_C_REQUEST_RCRecord, cmd_data)
end

--发送准备
function GameLayer:onStartGame(bReady)
	print(bReady,"----------发送准备------------")
	local myChairId = self:GetMeChairID();
	--重置游戏界面
	self._gameView:ReSetGameData();
	self._gameView:onStartBtnShow(false);
	if bReady then
		self:SendUserReady();
	end
end
function GameLayer:SendUserReady()
	GameLayer.super.SendUserReady(self);
	self:KillGameClock();
end

--发送托管
function GameLayer:sendTrustee(cbTrusteeMode)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_Trustee);
    cmd_data:pushbyte(cbTrusteeMode);
    return self:SendData(cmd.SUB_C_TRUSTEE, cmd_data);
end

--发送抢庄
function GameLayer:sendRobBanker(bRobBanker)
	self:KillGameClock();
	local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_Trustee);
	cmd_data:pushbool(bRobBanker);
	self:SendData(cmd.SUB_C_ROB_BANKER, cmd_data);
end

--发送下注
function GameLayer:sendChip(chipArr)
	local myChairId = self:GetMeChairID();
	dump(chipArr,"------发送下注-------",6)
	print(self.myChairId,myChairId,"----------self.myChairId---------")
	self:KillGameClock();
	local cmd_data = ExternalFun.create_netdata(24);
	cmd_data:pushword(myChairId);
	cmd_data:pushscore(chipArr[1]);
	cmd_data:pushscore(chipArr[2]);
	cmd_data:pushscore(chipArr[3]);
	self:SendData(cmd.SUB_C_CHIP, cmd_data);
end

--发送组合牌
function GameLayer:sendOpen(t1,t2) 
	self:KillGameClock();
	
	local cmd_data = CCmd_Data:create(4);
	cmd_data:pushbyte(t1[1]);
	cmd_data:pushbyte(t1[2]);
	cmd_data:pushbyte(t2[1]);
	cmd_data:pushbyte(t2[2]);
	self:SendData(cmd.SUB_C_OPEN_CARD, cmd_data);
end

--发送切锅
function GameLayer:sendChangeBanker(bChangeBanker)
	self:KillGameClock();
	local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_ChangeBanker);
	
	cmd_data:pushbool(bChangeBanker);
	self:SendData(cmd.SUB_C_CHNAGE_BANKER, cmd_data);
end

--发送结束
function GameLayer:sendShowScoreEnd()
	if self._gameView.bGameFree and self._gameView.bTrustee then
		return;
	end

	self:KillGameClock()
	local cmd_data = CCmd_Data:create();
	self:SendData(cmd.SUB_C_SHOW_SCORE_END, cmd_data);
end

--换桌
-- function GameLayer:onChangeDesk()
-- 	-- local myChairId = self:GetMeChairID();
-- 	-- local myViewId = self:SwitchViewChairID(myChairId);

-- 	-- self._gameView:ReSetGameData();
-- 	-- self._gameView:hideAllReady();

-- 	-- --清空玩家
-- 	-- for i = 1, cmd.GAME_PLAYER do
--  --        if  i ~= self:GetMeChairID()+1 then
--  --            self._gameFrame:QueryUserInfo(tableId, i-1);
--  --        end
--  --    end
-- 	-- self:KillGameClock()
-- 	-- local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_ChangeBanker)
	
-- 	-- cmd_data:pushbool(bChangeBanker)
-- 	-- self:SendData(cmd.SUB_C_CHNAGE_BANKER, cmd_data)
-- end

--换桌
function GameLayer:onChangeDesk()
    self._gameFrame:QueryChangeDesk();
end

------网络接收
--通用
function GameLayer:tagGameConfig(cmd_data)
	self._gameView.cbPlayMode = cmd_data.cbPlayMode;
	--游戏模式
    GlobalUserItem.bPrivateGoldRoom = self._gameView.cbPlayMode == 1;
-- print(GlobalUserItem.bPrivateGoldRoom,"-------------GlobalUserItem.bPrivateGoldRoom--------------")

    self._gameView.cbTimeStartGame = cmd_data.cbTimeStartGame;
    self._gameView.cbTimeRobBanker = cmd_data.cbTimeRobBanker;
    self._gameView.cbTimeChip = cmd_data.cbTimeChip;
    self._gameView.cbTimeOpenCard = cmd_data.cbTimeOpenCard;
    self._gameView.cbTimeChangeBanker = cmd_data.cbTimeChangeBanker;
    self._gameView.dBetScore = cmd_data.dBetScore[1];
    self._gameView.bJiaGuaPaiJiu = cmd_data.bJiaGuaPaiJiu;
    self._gameView.bDaPaiJiu = cmd_data.bDaPaiJiu;
    self._gameView.cbBankerMode = cmd_data.cbBankerMode;
    self._gameView.nChangeBankerCount = cmd_data.nChangeBankerCount;
    self._gameView.cbBankerOpenCardCount = cmd_data.cbBankerOpenCardCount;
    self._gameView.cbBetDaoShu = cmd_data.cbBetDaoShu;
    self._gameView.bAutoOpenCard = cmd_data.bAutoOpenCard;
    self._gameView.bSelectScore = cmd_data.bSelectScore;
    self._gameView.dGuoDiScore = cmd_data.dGuoDiScore;
    self._gameView.dMinBetScore = cmd_data.dMinBetScore;
    self._gameView.bHeadAllBet = cmd_data.bHeadAllBet;
    self._gameView.bZhaDan = cmd_data.bZhaDan;
    self._gameView.bDiJiuNiangNiang = cmd_data.bDiJiuNiangNiang;
    self._gameView.bGuiZi = cmd_data.bGuiZi;
    self._gameView.bTianJiuWang = cmd_data.bTianJiuWang;

    if self._gameView.bDaPaiJiu then
    	self._gameView.cardNumLimit = 4;
    else
    	self._gameView.cardNumLimit = 2;
    end
end

--场景消息
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
	print("场景数据:" .. cbGameStatus);
	local myUserItem = self:GetMeUserItem()
    self.m_bLookOn = false
    if myUserItem.cbUserStatus == yl.US_LOOKON then
        self.m_bLookOn = true
    end

    --先清理玩家信息
    for i = 1, cmd.GAME_PLAYER do
        local wViewChairId = self:SwitchViewChairID(i-1);
        self._gameView:resetUser(wViewChairId);
    end
    --初始化已有玩家
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        dump(userItem,"------初始化已有玩家------",6)
        local wViewChairId = self:SwitchViewChairID(i-1)
        self._gameView:showUserInfo(userItem)
        self.m_tabUserItem[i] = userItem
        self.tabUserItemCache[i] = userItem;
        
        print("onEventGameScene viewid is==============> ", wViewChairId)

        if userItem and (not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom) and wViewChairId==cmd.MY_VIEWID then
            self.lInitScore= userItem.lScore
        end
    end   
    -- local tableId = self._gameFrame:GetTableID();
    -- if not self:GetMeChairID() then
    --     return;
    -- end
    -- for i = 1, cmd.GAME_PLAYER do
    --     if  i ~= self:GetMeChairID()+1 then
    --         self._gameFrame:QueryUserInfo(tableId, i-1);
    --     end
    -- end

    self:KillGameClock();
    --0.重置本局数据
    self._gameView.bGameFree = false;
    self._gameView:ReSetGameData();
    self.cbPlayStatus = {0,0,0,0};            --玩家游戏状态
    --背景音乐
	ExternalFun.playBackgroudAudio("background.mp3")

    self.m_cbGameStatus = cbGameStatus;
    self.m_cbGameStatus2 = cbGameStatus;

    if cbGameStatus == cmd.GAME_STATUS_FREE then   --空闲状态
    	local data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree,dataBuffer);
    	dump(data,"游戏空闲状态-->",6);

    	self:tagGameConfig(data.GameConfig);
		self:onGameSceneFree(data);
		self._gameView.bGameFree = true;
		self._gameView:showWaitLayer(false);

	elseif cbGameStatus == cmd.GAME_STATUS_ROBBANKER then  --抢庄状态
		local data = ExternalFun.read_netdata(cmd.CMD_S_StatusRobBanker,dataBuffer);
		dump(data,"抢庄状态-->",6);
		
		self:tagGameConfig(data.GameConfig);
		self:onGameSceneRobBanker(data);
		self._gameView:showWaitLayer(true);
	elseif cbGameStatus == cmd.GAME_STATUS_CHIP then      --下注状态
		local data = ExternalFun.read_netdata(cmd.CMD_S_StatusChip,dataBuffer);
		dump(data,"下注状态-->",6);

		self:tagGameConfig(data.GameConfig);
		self:onGameSceneChip(data);
		self._gameView:showWaitLayer(true);
	elseif cbGameStatus == cmd.GAME_SCENE_OPEN_CARD then      --开牌状态
		local data = ExternalFun.read_netdata(cmd.CMD_S_StatusOpenCard,dataBuffer);
		dump(data,"开牌状态-->",6);

		self:tagGameConfig(data.GameConfig);
		self:onGameSceneOpenCard(data);
		self._gameView:showWaitLayer(true);
	elseif cbGameStatus == cmd.GAME_GAME_END then            --结束状态
		local data = ExternalFun.read_netdata(cmd.CMD_S_StatusGameEnd,dataBuffer);
		dump(data,"结束状态-->",6);

		self:tagGameConfig(data.GameConfig);
		self:onGameSceneGameEnd(data);
		self._gameView:showWaitLayer(true);
	end

	-- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end

    if GlobalUserItem.bPrivateGoldRoom then
        if self._gameView._priView then
            self._gameView._priView:onRefreshInfo()
        end
        --设置房主
        -- if self._gameView.tabHeadSp[cmd.MY_VIEWID] then
        -- 	self._gameView.tabHeadSp[cmd.MY_VIEWID]:setRoomerState(false)
        -- end
        
    end

    if GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom then 
        self.m_bClubRoom = true
    end
    self._gameView:showSitBtn(self.m_bLookOn)

	self:dismissPopWait()
end

--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    print("显示开始按钮",dwWaitTime)
    self._gameView:showSitBtn(dwWaitTime~=0 and true or false,dwWaitTime)
end


function GameLayer:resetUserItem()
	
end

--回放玩家信息显示
function GameLayer:resetVideoUserItem()
	
end

function GameLayer:onSubVideoGameStart(data)
	
end

--空闲状态
function GameLayer:onGameSceneFree(data)
	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.cbTrusteeMode = data.cbTrusteeMode[1][myChairId+1];
	self._gameView.dBankerMoney = data.dBankerMoney;

	self:trusteeUiFresh();
	self._gameView:showdGuoDiScore(data.dBankerMoney);
	self._gameView:hideAllButtons();

	if self:GetMeUserItem().cbUserStatus < yl.US_READY then
		self._gameView:onStartBtnShow(true);

		if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom  or GlobalUserItem.dwCurRoomGroupID ~= 0 then
	        -- 设置倒计时
	        if not self.m_bLookOn then
	            self:SetGameClock(self:GetMeChairID(),cmd.IDI_STARTGAME,self._gameView.cbTimeStartGame);
	        end
	    end  

		--倒计时
		self._gameView:onStartHeadAnim(myChairId, self._gameView.cbTimeStartGame, self._gameView.cbTimeStartGame);
	end
end

--抢庄状态
function GameLayer:onGameSceneRobBanker(data)
	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.cbTrusteeMode = data.cbTrusteeMode[1][myChairId+1];
	self._gameView.cbTimeRemain = data.cbTimeRemain;
	self.cbPlayStatus = data.cbPlayStatus[1];
	self._gameView.bRobBanker = data.bRobBanker[1];
	self._gameView.dBankerMoney = data.dBankerMoney;

	local bHaveRobBanker = data.bHaveRobBanker[1];
	
	self:trusteeUiFresh();
	self._gameView:showdGuoDiScore(data.dBankerMoney);
	self._gameView:hideAllButtons();

	for i = 1, cmd.GAME_PLAYER do
		--抢庄结果
		if self.cbPlayStatus[i]==cmd.PS_SITTING then
			if bHaveRobBanker[i] == false then
				--抢庄中
				self._gameView:showPlayerTip(i-1, cmd.actRob);
				--倒计时
				self._gameView:onStartHeadAnim(i-1, self._gameView.cbTimeRemain, self._gameView.cbTimeRobBanker);
				self:SetGameClock(self:GetMeChairID(),cmd.IDI_CALLBANKER,self._gameView.cbTimeRemain);
			else
				self._gameView:hidePlayerTip(i-1);
				self._gameView:showRobResult(i-1, data.bRobBanker);
			end
		end
	end

	if self.cbPlayStatus[myChairId+1] == cmd.PS_SITTING and bHaveRobBanker[myChairId+1] == false then
		self._gameView:onShowRobBtn(true);
	end
end

--下注状态
function GameLayer:onGameSceneChip(data)
	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.cbTrusteeMode = data.cbTrusteeMode[1][myChairId+1];
	self._gameView.cbTimeRemain = data.cbTimeRemain;
	self.cbPlayStatus = data.cbPlayStatus[1];
	self.wBankerUser = data.wBankerUser;
	self._gameView.dBankerMoney = data.dBankerMoney;
	
	local bHaveChip = data.bHaveChip[1];
	local dUserChip = data.dUserChip;

	self:trusteeUiFresh();
	self._gameView:showdGuoDiScore(data.dBankerMoney);
	self._gameView:hideAllButtons();

	--显示庄家
	self._gameView:onShowRobBtn(false);
	self._gameView:showBanker(data.wBankerUser);

	for i = 1, cmd.GAME_PLAYER do
		--下注
		if self.cbPlayStatus[i]==cmd.PS_SITTING then
			if bHaveChip[i] == false then
				--下注中
				self._gameView:showPlayerTip(i-1, cmd.actChip);
				--倒计时
				self._gameView:onStartHeadAnim(i-1, self._gameView.cbTimeRemain, self._gameView.cbTimeChip);
				self:SetGameClock(self:GetMeChairID(),cmd.IDI_SETCHIP,self._gameView.cbTimeRemain);
			else
				self._gameView:hidePlayerTip(i-1);
				self._gameView:showUserChip(i-1, dUserChip[i]);
			end
		end
	end

	if self.cbPlayStatus[myChairId +1] == cmd.PS_SITTING and bHaveChip[myChairId +1] == false and data.wBankerUser ~= myChairId then
		self._gameView:showChipBtns(true);
	end
end

--开牌状态
function GameLayer:onGameSceneOpenCard(data)
	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.cbTrusteeMode = data.cbTrusteeMode[1][myChairId+1];
	self._gameView.cbTimeRemain = data.cbTimeRemain;
	self.cbPlayStatus = data.cbPlayStatus[1];
	self.wBankerUser = data.wBankerUser;
	self._gameView.dBankerMoney = data.dBankerMoney;
	self._gameView.myCardData = data.cbHandCard[1];
	self._gameView.cbAutoHandCard = data.cbAutoHandCard[1];
	self._gameView.cbBankerHandCard = data.cbBankerHandCard[1];

	local dUserChip = data.dUserChip;
	local bHaveOpenCard = data.bHaveOpenCard[1];

	self:trusteeUiFresh();
	self._gameView:showdGuoDiScore(data.dBankerMoney);
	self._gameView:hideAllButtons();
	self._gameView:getCardHeapArr(data.nUsedCardNum);

	--庄家
	self._gameView:setBankerUser(data.wBankerUser, false);

	self:stopAllActions();
	for i = 1, cmd.GAME_PLAYER do
		local viewId = self:SwitchViewChairID(i-1)
		if self.cbPlayStatus[i]==cmd.PS_SITTING then
			--已下注
			self._gameView:showUserChip(i-1, dUserChip[i]);
			--创建牌
			self._gameView:createHandCard(viewId);

			--组牌
			if bHaveOpenCard[i] == false then
				--组牌中
				self._gameView:showPlayerTip(i-1, cmd.actCombine);
				--倒计时
				self._gameView:onStartHeadAnim(i-1, self._gameView.cbTimeRemain, self._gameView.cbTimeOpenCard);
				self:SetGameClock(self:GetMeChairID(),cmd.IDI_RANGECARD,self._gameView.cbTimeRemain);
			else
				--组牌完成
				self._gameView:hidePlayerTip(i-1);
				self._gameView:showZupaiFinished(i-1, true);

				self._gameView:showOperateButton(false);
				self._gameView:showZupaiResult(viewId);
			end
		end
	end

	--组牌按钮
	if self.cbPlayStatus[myChairId +1] == cmd.PS_SITTING then
		self._gameView:showOperateButton(not bHaveOpenCard[myChairId+1]);
	end
end

--结束状态
function GameLayer:onGameSceneGameEnd(data)
	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.cbTrusteeMode = data.cbTrusteeMode[1][myChairId+1];
	self._gameView.cbTimeRemain = data.cbTimeRemain;
	self.cbPlayStatus = data.cbPlayStatus[1];
	self.wBankerUser = data.wBankerUser;
	self._gameView.dBankerMoney = data.dBankerMoney;

	local bChangeBanker = data.bChangeBanker;
	local cbOpenCard = data.cbOpenCard;
	local bCompareInfo = data.bCompareInfo[1];
	local nWinScoreInfo = data.nWinScoreInfo[1];
	local dGameTax = data.dGameTax[1];
	local dGameScore = data.dGameScore[1];
	local dUserChip = data.dUserChip;
	
	self._gameView.myCardData = cbOpenCard[myChairId +1];
	self._gameView.cbBankerHandCard = cbOpenCard[self.wBankerUser +1];

	self:trusteeUiFresh();
	self._gameView:hideAllButtons();
	self._gameView:getCardHeapArr(data.nUsedCardNum);

	--庄家
	self._gameView:setBankerUser(data.wBankerUser, false);

	--结束
	self:stopAllActions();
	self:runAction(cc.Sequence:create(
        cc.CallFunc:create( function()
            for i = 1, cmd.GAME_PLAYER do
				local viewId = self:SwitchViewChairID(i-1);

				if self.cbPlayStatus[i]==cmd.PS_SITTING then
					--1.已下注
					self._gameView:showUserChip(i-1, dUserChip[i]);
					--2.创建牌
					self._gameView:createHandCard(viewId);
					--3.显示牌
					self._gameView:showHandCard(viewId, cbOpenCard[i]);
				end
			end
            --4.飞金币
            self._gameView:playCoinFlyAnimation(dGameScore);
        end ),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            for i = 1, cmd.GAME_PLAYER do
				if self.cbPlayStatus[i]==cmd.PS_SITTING then
					--5.飘分显示
					self._gameView:showPlayerWinScore(i-1, dGameScore[i])
					--6.输赢显示
					self._gameView:showEndResult(i-1, dGameScore[i]);
				end
			end
            
        end),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
            --7.切锅
			if bChangeBanker then
				self._gameView:onShowQieguoBtn(true);
			end
        end),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
            --8.发送结束
            self:sendShowScoreEnd();
        end)
    ));
end

--游戏消息
function GameLayer:onEventGameMessage(sub, dataBuffer)
	self.bReceivedGameMsg=true
	if cmd.SUB_S_QUERY_RESULT~=sub then
		-- self.m_cbGameStatus = sub
		self.m_cbGameStatus2 = sub;
	end
	if sub==cmd.SUB_S_GAME_START then --游戏开始
		self:onSubGameStart(dataBuffer);
		self.m_cbGameStatus = cmd.GAME_STATUS_ROBBANKER;
	elseif sub==cmd.SUB_S_ROB_BANKER then --玩家抢庄
		self:onSubRobBanker(dataBuffer);
	elseif sub==cmd.SUB_S_ROB_RESULT then --抢庄结果
		self:onSubRobResult(dataBuffer);
	elseif sub==cmd.SUB_S_CHIP_RESULT then --下注结果
		self:onSubChipResult(dataBuffer);
	elseif sub==cmd.SUB_S_START_OPEN_CARD then --玩家开牌
		self:onSubOpen(dataBuffer);
	elseif sub==cmd.SUB_S_OPEN_CARD then --开牌结果
		self:onSubOpenResult(dataBuffer);
	elseif sub==cmd.SUB_S_GAME_END then --游戏结束
		self:onSubGameEnd(dataBuffer);
		self.m_cbGameStatus=cmd.GAME_STATUS_FREE;
	elseif sub==cmd.SUB_S_TRUSTEE then            --玩家托管
        self:onSubTrustee(dataBuffer);
    elseif sub==cmd.SUB_S_CHNAGE_BANKER then       --切换庄家
        self:onSubChangeBanker(dataBuffer);
    elseif sub==cmd.SUB_S_GAME_FREE then           --空闲状态
        self:onSubGameFree(dataBuffer);
        self.m_cbGameStatus = cmd.GAME_STATUS_FREE;
	elseif sub==cmd.SUB_S_WINLOSE_INFO then --
		local data=ExternalFun.read_netdata(cmd.CMD_S_WinLoseInfo,dataBuffer);
		self:onSubWinLoseInfo(data);
	elseif sub==cmd.SUB_S_QUERY_RESULT then  --
		print("dataBuffer:len",dataBuffer:getlen());

		local data=ExternalFun.read_netdata(cmd.CMD_S_QueryResult,dataBuffer);
		self:onSubZhanjiQueryResult(data);
	elseif sub==cmd.SUB_S_RECORD then    --
		
		self:onSubRecord(dataBuffer);
	end
end


function GameLayer:OnUpdataClockView()
	self._gameView:updateClockView(self._ClockTime,self._ClockViewChair,self._ClockID)
end

--战绩
function GameLayer:onSubRecord(dataBuffer)
	local data=ExternalFun.read_netdata(cmd.CMD_S_RECORD,dataBuffer);
	dump(data,"onSubRecord",6)

	self.recordData = data;
	self._gameView:refreshZhanjiView(self.recordData);

	-- self.zhanjiRecord = {}
	-- for i=1,data.nCount do
	-- 	self.zhanjiRecord[i]={}
	-- 	for j=1,cmd.GAME_PLAYER do
	-- 		local viewid=self:SwitchViewChairID(j-1)
	-- 		self.zhanjiRecord[i][viewid]=data.lDetailScore[j][i]
	-- 	end
	-- end

	-- 
end

function GameLayer:onSubWinLoseInfo(data)
	-- self.winInfo=data.WinInfo
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
	local data =ExternalFun.read_netdata(cmd.CMD_S_GameStart,dataBuffer);
	dump(data,"------onSubGameStart-----",6);
	
	self._gameView.bGameFree = false;
	self._gameView:onStartBtnShow(false);

	--清理
	self._gameView:ReSetGameData();
	self.cbPlayStatus = {0,0,0,0};            --玩家游戏状态
	self._gameView:hideAllReady();

	if not self.m_bLookOn then 
        self._gameView:showSitBtn(false)
    end

    -- --更新玩家
    -- for i = 1, cmd.GAME_PLAYER do
    --     local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
    --     local wViewChairId = self:SwitchViewChairID(i-1)
    --     self._gameView:showUserInfo(userItem)
    --     self.m_tabUserItem[i] = userItem
    --     self.tabUserItemCache[i] = userItem;

    --     if userItem and (not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom) and wViewChairId==cmd.MY_VIEWID then
    --         self.lInitScore= userItem.lScore
    --     end
    -- end   

	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);
	
	self.cbPlayStatus = data.cbPlayStatus[1];
	self.wBankerUser = data.wBankerUser;
	self._gameView.dBankerMoney = data.dBankerMoney;

	self._gameView:showdGuoDiScore(data.dBankerMoney);

	if data.wBankerUser ~= yl.INVALID_CHAIR then
		--有庄家
		self._gameView:showBanker(data.wBankerUser);
		if self.wBankerUser ~= myChairId and myViewId == cmd.MY_VIEWID then
			self._gameView:showChipBtns(true);
		end

		--下注状态
		for i = 1, cmd.GAME_PLAYER do
			if self.cbPlayStatus[i] == cmd.PS_SITTING then
				self._gameView:showPlayerTip(i-1, cmd.actChip);
				--倒计时
				self._gameView:onStartHeadAnim(i-1, self._gameView.cbTimeChip+5, self._gameView.cbTimeChip+5);
				self:SetGameClock(self:GetMeChairID(),cmd.IDI_SETCHIP,self._gameView.cbTimeChip+5);
			end
		end
	else
		--抢庄
		for i = 1, cmd.GAME_PLAYER do
			if self.cbPlayStatus[i] == cmd.PS_SITTING then
				self._gameView:showPlayerTip(i-1, cmd.actRob);
				--倒计时
				self._gameView:onStartHeadAnim(i-1, self._gameView.cbTimeRobBanker, self._gameView.cbTimeRobBanker);
				self:SetGameClock(self:GetMeChairID(),cmd.IDI_CALLBANKER,self._gameView.cbTimeRobBanker);
			end
			
			if myViewId == cmd.MY_VIEWID then
				self._gameView:onShowRobBtn(true);
			end
		end
	end

	ExternalFun.playSoundEffect(cmd.GAME_START);

	-- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
        	PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
            self._gameView._priView:onRefreshInfo()
        end
    end

    if GlobalUserItem.bPrivateGoldRoom then
        if self._gameView._priView then
            self._gameView._priView:onRefreshInfo()
        end
        -- self._gameView.tabHeadSp[cmd.MY_VIEWID]:setRoomerState(false)
    end

end

--玩家抢庄
function GameLayer:onSubRobBanker(dataBuffer) 
	local data = ExternalFun.read_netdata(cmd.CMD_S_RobBanker,dataBuffer);
	dump(data,"------onSubRobBanker-----",6);

	local myChairId = self:GetMeChairID();
	-- local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.bRobBanker[data.wRobUser + 1] = data.bRobBanker;

	if data.wRobUser == myChairId then
		self._gameView:onShowRobBtn(false);
	end

	if data.wRobUser == yl.INVALID_WORD then
		self._gameView:onShowRobBtn(true);
		if self.cbPlayStatus[i] == cmd.PS_SITTING then
			self._gameView:showPlayerTip(i-1, cmd.actRob);
		end
	else
		self._gameView:hidePlayerTip(data.wRobUser);
		self._gameView:showRobResult(data.wRobUser, data.bRobBanker);

		self._gameView:onStopHeadAnim(data.wRobUser);
	end
end

--抢庄结果
function GameLayer:onSubRobResult(dataBuffer)
	local data = ExternalFun.read_netdata(cmd.CMD_S_RobBankerResult,dataBuffer)
	dump(data,"------onSubRobResult-----",6);

	self.wBankerUser = data.wBankerUser;
	self._gameView.dBankerMoney = data.dBankerMoney;

	self._gameView:showdGuoDiScore(data.dBankerMoney);
	self._gameView:setBankerUser(data.wBankerUser,true);
	self._gameView:showRobResult(nil, nil);
end

--下注结果
function GameLayer:onSubChipResult(dataBuffer)
	local data = ExternalFun.read_netdata(cmd.CMD_S_ChipResult,dataBuffer)
	dump(data,"-----------onSubChipResult---------------",6)

	local myChairId = self:GetMeChairID();
	local dChip = data.dChip[1];
	self._gameView:showUserChip(data.wChipUser, dChip);
	self._gameView:hidePlayerTip(data.wChipUser);
	self._gameView:onStopHeadAnim(data.wChipUser);

	--隐藏下注按钮
	if data.wChipUser == myChairId then
		self._gameView:showChipBtns(false);
	end

	ExternalFun.playSoundEffect(cmd.MOVE_GOLD);
end

--玩家发牌
function GameLayer:onSubOpen(dataBuffer)
	local data = ExternalFun.read_netdata(cmd.CMD_S_StartOpenCard,dataBuffer)
	dump(data,"-----------onSubOpen---------------",6)

	local nUsedCardNum = data.nUsedCardNum;
	local cbHandCard = data.cbHandCard[1];
	local cbAutoHandCard = data.cbAutoHandCard[1];
	local wSiceCount = data.wSiceCount;

	self._gameView.cbBankerHandCard = data.cbBankerHandCard[1];
	self._gameView.myCardData = data.cbHandCard[1];
	self._gameView.cbAutoHandCard = data.cbAutoHandCard[1];

	local startChairId = self.wBankerUser;
	self._gameView:getCardHeapArr(nUsedCardNum);

	--打骰子
	local n = math.pow(2,8);
	local sice = {1, 1};
	sice[1] = math.floor(wSiceCount/n);
	sice[2] = wSiceCount%n;

	local num = 0;
	for j=1, cmd.GAME_PLAYER do
		if self.cbPlayStatus[j] == cmd.PS_SITTING then
			num = num + 1;
		end
	end

	local siceNum = sice[1]+sice[2];
	local index = siceNum % num;
	local num2 = 0;  
	for j=1, cmd.GAME_PLAYER do
		if self.cbPlayStatus[j] == cmd.PS_SITTING then
			num2 = num2 + 1;
			if num2 == index then
				startChairId = j;
			end
		end
	end

	self:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.5),
        cc.CallFunc:create(function()
            --打色子
            self._gameView:diceAnimation(sice);
        end),
        cc.DelayTime:create(3.5),
        cc.CallFunc:create(function()
            --发牌
            self._gameView:sendCards(startChairId);
        end)
    ));

    ExternalFun.playSoundEffect(cmd.OUT_CARD);
end

--开牌结果
function GameLayer:onSubOpenResult(dataBuffer)
	local data = ExternalFun.read_netdata(cmd.CMD_S_Open_Result,dataBuffer)
	dump(data, "------------onSubOpenResult------------", 6)

	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	if data.wOpenUser == myChairId then
		if self._gameView.bTrustee then
			self._gameView:ontipEvent();
		end

		self._gameView:showOperateButton(false);
	end
	
	local viewId = self:SwitchViewChairID(data.wOpenUser);
	self._gameView:showZupaiResult(viewId);

	self._gameView:hidePlayerTip(data.wOpenUser);
	self._gameView:showZupaiFinished(data.wOpenUser, true);
	self._gameView:onStopHeadAnim(data.wOpenUser);
end

function GameLayer:onSubGameEnd(dataBuffer)
	local data = ExternalFun.read_netdata(cmd.CMD_S_GameEnd,dataBuffer)
	dump(data, "------------onSubGameEnd------------", 6)

	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	local cbEndMode = data.cbEndMode;
	local bChangeBanker = data.bChangeBanker;
	local cbOpenCard = data.cbOpenCard;
	local bCompareInfo = data.bCompareInfo[1];
	local nWinScoreInfo = data.nWinScoreInfo[1];
	local dGameTax = data.dGameTax[1];
	local dGameScore = data.dGameScore[1];

	self.bIsDismiss = true;
	for i = 1, cmd.GAME_PLAYER do
        if dGameScore[i] ~= 0 then
            self.bIsDismiss = false;
        end
    end

	self._gameView.dBankerMoney = data.dBankerMoney;
	
	self._gameView:showdGuoDiScore(data.dBankerMoney);
	--重置
	self._gameView.bGameEnd = true;
	for i = 1, cmd.GAME_PLAYER do
		self._gameView:hidePlayerTip(i-1);
		self._gameView:showZupaiFinished(i-1, false);
	end
	self._gameView:hideAllButtons();


	--解散重置
	if cbOpenCard[1][1] == 0 and cbOpenCard[2][1] == 0 and cbOpenCard[3][1] == 0 and cbOpenCard[4][1] == 0 then
		self._gameView:ReSetGameData();
		return;
	end

	--结束
	self:runAction(cc.Sequence:create(
		cc.DelayTime:create(0.5),
        cc.CallFunc:create( function()
			--1.翻牌
			self._gameView:cardFanPaiAnimation(cbOpenCard);
            
        end ),
        cc.DelayTime:create(0.5 + 3.2),
        cc.CallFunc:create( function()
        	--2.飞金币
        	self._gameView:playCoinFlyAnimation(dGameScore);
        end ),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
            for i = 1, cmd.GAME_PLAYER do
				if self.cbPlayStatus[i]==cmd.PS_SITTING then
					--3.飘分显示
					self._gameView:showPlayerWinScore(i-1, dGameScore[i])
					--4.输赢显示
					self._gameView:showEndResult(i-1, dGameScore[i]);
				end
			end
        end),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
        	--5.切锅
			if self.wBankerUser == myChairId and bChangeBanker then
				self._gameView:onShowQieguoBtn(true);
			end
        end),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
            --6.发送结束
            self:sendShowScoreEnd();
        end)
    ));

end

--切换庄家
function GameLayer:onSubChangeBanker(dataBuffer)
    --消息处理
    local data = ExternalFun.read_netdata(cmd.CMD_S_ChangeBanker, dataBuffer);
    dump(data, "------------onSubChangeBanker------------", 6);

    self._gameView:showdGuoDiScore(data.dBankerMoney);

    if data.wBankerUser == yl.INVALID_WORD or data.wBankerUser == -1 then
    	showToast(self,"玩家下庄",2);
    	self._gameView:hideAllBanker();
    elseif data.wBankerUser == self.wBankerUser then
    	showToast(self,"庄家不变",2);
    else
    	self._gameView:exchangeBankerAnimation();
    end
end

--空闲状态
function GameLayer:onSubGameFree(dataBuffer)
    --消息处理

    local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	self._gameView.bGameFree = true;
	--准备
	self._gameView:onStartBtnShow(true);
	--倒计时
	self._gameView:onStartHeadAnim(myChairId, self._gameView.cbTimeStartGame, self._gameView.cbTimeStartGame);
	if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom then
        -- 设置倒计时
        self:SetGameClock(self:GetMeChairID(),cmd.IDI_STARTGAME,self._gameView.cbTimeStartGame);
    end 

	--自动准备
	if self._gameView.bTrustee and myViewId == cmd.MY_VIEWID then
		self:runAction(cc.Sequence:create(
	        cc.DelayTime:create(3.0),
	        cc.CallFunc:create( function()
	        	if self._gameView.bGameFree then
	        		self:onStartGame(true);
	        	end
	        end)
	    ));
	end
end

--用户托管
function GameLayer:onSubTrustee(dataBuffer)
    local data = ExternalFun.read_netdata(cmd.CMD_S_Trustee, dataBuffer);
    dump(data, "用户托管", 6);

    local myChairId = self:GetMeChairID();
    if data.wChairID == myChairId then
    	self._gameView.cbTrusteeMode = data.cbTrusteeMode;
    	self._gameView.TopUINode:changeMaxOrMinBet();
    	if self._gameView.cbTrusteeMode == 0 then
    		self._gameView.bTrustee = false;
    	else
    		self._gameView.bTrustee = true;
    	end

    	if self._gameView.bTrustee and self._gameView.bGameFree then
    		self:onStartGame(true);
    	end
    end
end

function GameLayer:trusteeUiFresh()
	local myChairId = self:GetMeChairID();
	local myViewId = self:SwitchViewChairID(myChairId);

	if myViewId == cmd.MY_VIEWID then
		self._gameView.TopUINode:changeMaxOrMinBet();
		if self._gameView.cbTrusteeMode == 0 then
			self._gameView.bTrustee = false;
		else
			self._gameView.bTrustee = true;
		end
	end
end

function GameLayer:onSubChip(data)

	self.chipMsgTime=currentTime()
	if self.wBankerUser~=self.myChairId and self.cbPlayStatus[self.myChairId+1]==cmd.PS_SITTING  then
		self._gameView:setShowChipBtns(true,data.lChipData[1])		
	end
	
	if not self.bReceivedRobMsg then
		self:setChipClock(0)	--非庄家才收到该消息
		self._gameView:showActionSps(cmd.actChip)
	end
end

--玩家进入
function GameLayer:onEventUserEnter(wTableID,wChairID,userItem)
    print("add user " .. userItem.wChairID .. "; nick " .. userItem.szNickName);
    if wTableID ~= self:GetMeUserItem().wTableID and not GlobalUserItem.bVideo then
        return
    end

    -- if not userItem then
    -- end

    if userItem then
        self.m_tabUserItem[userItem.wChairID+1] = clone(userItem);
        self.tabUserItemCache[userItem.wChairID+1] = userItem;
    end

    if userItem == self:GetMeUserItem() then
        if PriRoom and GlobalUserItem.bPrivateRoom then
            if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == userItem.dwUserID then
                self._gameView:showFangzhuIcon(cmd.MY_VIEWID);
            end
            self._gameView:updateScore(userItem.lScore, userItem.wChairID);
        end
        -- if not GlobalUserItem.bVideo then
        --     return
        -- end
    end
    self._gameView:showUserInfo(userItem);

    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo();
        end
    end
end

--玩家状态改变
function GameLayer:onEventUserStatus(userItem,newstatus,oldstatus)
    print("change user " .. userItem.wChairID .. "; nick " .. userItem.szNickName);
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then
        print("删除")
        if (oldstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            return;
        end

        if PriRoom and GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount >0 and not GlobalUserItem.bPrivateGoldRoom then
            return;
        end
        self._gameView:deleteUserInfo(userItem, newstatus, oldstatus);
    else
        --刷新用户信息
        -- self._dataModle:updateUser(userItem);
        print("刷新用户信息")
        if (newstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            return;
        end
        print(newstatus.wTableID,self:GetMeUserItem().wTableID,"刷新用户信息")
        if userItem then
            self.m_tabUserItem[userItem.wChairID+1] = clone(userItem);
            self.tabUserItemCache[userItem.wChairID+1] = userItem;
        end
        --刷新用户信息
        if userItem == self:GetMeUserItem() then
            if PriRoom and GlobalUserItem.bPrivateRoom then
                if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == userItem.dwUserID then
                    self._gameView:showFangzhuIcon(cmd.MY_VIEWID);
                end
                self._gameView:updateScore(userItem.lScore, userItem.wChairID);
            end
            -- return
        end
        self._gameView:showUserInfo(userItem);
    end
end

--分数刷新
function GameLayer:onEventUserScore(userItem)
    if userItem.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end
    local pos = self:SwitchViewChairID(userItem.wChairID);
    self._gameView:updateScore(userItem.lScore, userItem.wChairID);
end

--退出桌子
function GameLayer:onExitTable()
    print("onExitTable()")
    if self.m_querydialog then
        return
    end
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    self._gameFrame:StandUp(1)
                    print("11111111111111")
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    print("delay leave")
                    print("222222222222222222")
                    self:onExitRoom()
                end
                )
            )
        )
        return
    end

   self:onExitRoom()
end

function GameLayer:onExitRoom()
	
    self._gameFrame:onCloseSocket()
    self:stopAllActions()
    self:KillGameClock()
    self:dismissPopWait()
    --self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
    self._scene:onKeyBack()
end

--用户聊天
function GameLayer:onUserChat(chatData, wChairId)
	local viewid = self:SwitchViewChairID(wChairId);
	print("玩家聊天", chatData.szChatString, viewid, wChairId)
    if chatData and wChairId then
        local viewId = self:SwitchViewChairID(wChairId);
        self._gameView:onUserChat(viewId,chatData.szChatString);
    end
end

--用户表情
function GameLayer:onUserExpression(expression, wChairId)
    print("用户表情=================")
    if expression and wChairId then
        local viewId = self:SwitchViewChairID(wChairId);
        if viewId and viewId ~= yl.INVALID_CHAIR then
            self._gameView:onUserExpression(viewId, expression.wItemIndex)
        end
    end
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( userItem, filepath )
    local viewid = self:SwitchViewChairID(userItem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, true)
    end
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( userItem, filepath )
    local viewid = self:SwitchViewChairID(userItem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, false)
    end
end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode("share/ShareLayer.csb"):addTo(self)
            local TAG_WXSHARE = 101
            local TAG_CYCLESHARE = 102
            local MASK_PANEL = 103
            -- 按钮事件
            local touchFunC = function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    local tag = ref:getTag()
                    local target = nil
                    if TAG_WXSHARE == tag then
                        target = yl.ThirdParty.WECHAT
                    elseif TAG_CYCLESHARE == tag then
                        target = yl.ThirdParty.WECHAT_CIRCLE
                    elseif MASK_PANEL == tag then
                        
                    end
                    if nil ~= target then
                        MultiPlatform:getInstance():shareToTarget(
                            target, 
                            function(isok)

                            end,
                            "截图分享", 
                            "分享我的游戏截图", 
                            yl.HTTP_URL, 
                            imagepath, 
                            "true"
                        )
                    end
                    shareLayer:removeFromParent()
                end
            end
            -- 微信按钮
            local btn = shareLayer:getChildByName("btn_wxshare")
            btn:setTag(TAG_WXSHARE)
            btn:addTouchEventListener( touchFunC )
            -- 朋友圈按钮
            btn = shareLayer:getChildByName("btn_cycleshare")
            btn:setTag(TAG_CYCLESHARE)
            btn:addTouchEventListener( touchFunC )
            -- 屏蔽层
            local panel = shareLayer:getChildByName("share_panel")
            panel:setTag(MASK_PANEL)
            panel:addTouchEventListener( touchFunC )
        else
            print("no image")
        end
    end
end


--获取用户数据
function GameLayer:getUserInfoByChairID(wchairid)
    -- for k,v in pairs(self.tabUserItemCache) do
    --     if v.wChairID == wchairid then
    --         return v
    --     end
    -- end
    -- return nil
     return self.tabUserItemCache[wchairid+1];
end

--获取用户数据
function GameLayer:getUserInfoByUserID(dwUserID)
    for k,v in pairs(self.tabUserItemCache) do
        if v.dwUserID == dwUserID then
            return v
        end
    end
    return nil
end

function GameLayer:getGameRule(buffer)
    dump(buffer, "玩法", 5)
    if nil == buffer then
        return;
    end

    local strTable ={};
    --游戏模式(玩法)
    local gameMode = buffer[4];
    local zadan = buffer[5];
    local guizi = buffer[6];
    local dijiuniangniang = buffer[7];
    local tianjiuwang = buffer[8];
    local dao = buffer[9];
    local bankerType = buffer[10];
    local bankerShow = buffer[11];

    local ruleStr = "";
    if gameMode == 1 then
    	ruleStr = "大牌九";
    elseif gameMode == 2 then
    	ruleStr = "小牌九";
    elseif gameMode == 3 then
    	ruleStr = "加锅牌九";
    end
    table.insert(strTable,ruleStr);

    if zadan ~= 0 then
    	ruleStr = "炸弹";
    else
    	ruleStr = "";
    end
    table.insert(strTable,ruleStr);

    if guizi ~= 0 then
    	ruleStr = "鬼子";
    else
    	ruleStr = "";
    end
    table.insert(strTable,ruleStr);

    if dijiuniangniang ~= 0 then
    	ruleStr = "地九娘娘";
    else
    	ruleStr = "";
    end
    table.insert(strTable,ruleStr);

    if tianjiuwang ~= 0 then
    	ruleStr = "天九王";
    else
    	ruleStr = "";
    end
    table.insert(strTable,ruleStr);

    if dao == 2 then
    	ruleStr = "2道";
    elseif dao == 3 then
    	ruleStr = "3道";
    end
    table.insert(strTable,ruleStr);

    if bankerType == 0 then
    	ruleStr = "轮庄";
    elseif bankerType == 1 then
    	ruleStr = "抢庄";
    elseif bankerType == 2 then
    	ruleStr = "霸王庄";
    end
    table.insert(strTable,ruleStr);

    if bankerShow == 0 then
    	ruleStr = "不亮";
    elseif bankerShow == 2 then
    	ruleStr = "亮2张";
    elseif bankerShow == 3 then
    	ruleStr = "亮3张";
    end
    table.insert(strTable,ruleStr);
        
    return strTable
end
return GameLayer
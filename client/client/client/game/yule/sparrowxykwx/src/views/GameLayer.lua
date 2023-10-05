local initpath = string.format("%syule.%s.src.init", appdf.GAME_SRC, "sparrowxykwx")
require(initpath)
local GameLayer = class("GameLayer", require(appdf.CLIENT_SRC.."gamemodel.GameModel"))
-- local appdf.GAME_SRC.."yule.sparrowxykwx.src."  = appdf.GAME_SRC.."yule.sparrowxykwx.src."
local cmd = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")
local GameLogic = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.GameLogic")
local GameViewLayer = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.GameViewLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ExternalFun =  require(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = require(appdf.EXTERNAL_SRC .. "MultiPlatform")

function GameLayer:ctor(frameEngine, scene)
    GameLayer.super.ctor(self, frameEngine, scene)
end

function GameLayer:CreateView()
    return GameViewLayer:create(self):addTo(self)
end

function GameLayer:getParentNode()
    return self._scene
end

--设置私有房的层级
function GameLayer:addPrivateGameLayer( layer )
    if nil == layer then
        return
    end
    self._gameView:addChild(layer, 40)
end

function GameLayer:getVideoLayerConfig()
    return
    {
        pos = cc.p(667, 200),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 80
    }
end

--回放层级控制
function GameLayer:addVideoControlLayer(controlLayer)
	if nil == controlLayer then
        return
    end
    self:addChild(controlLayer, 100)
end

function GameLayer:OnInitGameEngine()
	self.lCellScore = 0
	self.cbTimeOutCard = 0
	self.cbTimeOperateCard = 0
	self.cbTimeStartGame = 0
	self.cbTimeWaitEnd = 0
	self.wCurrentUser = yl.INVALID_CHAIR
	self.wBankerUser = yl.INVALID_CHAIR

	self.cbGender = {0, 0, 0, 0}
	self.bListening = false
	self.bTrustee = false
	self.m_bOnGame = false

	self.isMyProvideUser = false  --我是操作提供者，这样不进入托管



	self.myChirID = yl.INVALID_CHAIR
	-- self.cbListenPromptOutCard = {}
	-- self.cbListenCardList = {}
	-- self.cbHuFanList = {}

	self.cbOutCardData = {}
	self.cbHuCardCount = {}
	self.cbHuCardData = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}}
	self.cbHuCardRemainingCount = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}}
	self.cbHuFan = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}}

	self.cbActionMask = nil
	self.bSendCardFinsh = true
	self.lDetailScore = {}
	self.m_userRecord = {}
    self._ting_data = {}

             self.m_bGameStart = false

             self.m_bGameEnd = false

	--规则
	self.cbPlayerCount = cmd.GAME_PLAYER
	self.cbMaCount = 0
	self.cbMagicMode = 0
	self.bQiangGangHu = false
	self.bHuQiDui = false
	self.bHaveZiCard = false
	self.bNoMagicDouble = false

             self.m_bLookOn = false

            self.m_cbPlayStatus = {0,0,0}

            self.m_playingUser = {}

	-----回放
	--手牌
	self.cbHandCardData = {{}, {}, {}, {}}
	self.cbOutCardData = {{}, {}, {}, {}}
	self.cbActivityCardData = {{}, {}, {}, {}}
	--鬼牌
	self.cbMagicCard = {}
	self.leftCard = 0
	self.cbAllCardCount = cmd.MAX_REPERTORY
	self.cbLeftCardCount = 0
	--房卡需要
	self.m_sparrowUserItem = {}
	self.wRoomHostViewId = 0

	self.isPriOver = false

            self.endLayer = nil

    --是否已关闭总结算
    self.isCloseGameEnd = false

    --亮倒状态
    self.cbHearStatus = {0,0,0,0}
    self._handCards = {}
    self.kouPaiList = {{}, {}, {}, {}}
    self._ban_out_list = {}
end

function GameLayer:OnResetGameEngine()
    GameLayer.super.OnResetGameEngine(self)
    self:stopAllActions()
    self._ban_out_list = {}
    self._gameView:onResetData()
    self.bListening = false
	self.nGameSceneLimit = false
	self.bTrustee = false
	self.cbAppearCardData = {} 		--已出现的牌
	self.bMoPaiStatus = false
	self.cbActionMask = nil

	self.isMyProvideUser = false
	-----回放
	--手牌
	self.cbHandCardData = {{}, {}, {}, {}}
	self.cbOutCardData = {{}, {}, {}, {}}
	self.cbActivityCardData = {{}, {}, {}, {}}

	--鬼牌
	self.cbMagicCard = {}
	self.leftCard = 0

    --亮倒状态
    self.cbHearStatus = {0,0,0,0}
    self._handCards = {}
    self.kouPaiList = {{}, {}, {}, {}}
    self._ban_out_list = {}

    self._can_ting_list = {}
    self._hu_list = {}
    self._can_kou_list = {}
end

local function table_filter(tbl, filter, fileterNum)
    table.sort(tbl)
    local map = {}
    for k,v in ipairs(filter) do
        map[v] = 3
    end
    local newTbl = {}
    for k,v in ipairs(tbl) do
        if not map[v] then
            table.insert(newTbl, v)
        else
            if map[v] <= 0 then
                table.insert(newTbl, v)
                map[v] = map[v] - 1
            end
        end

    end
    return newTbl
end

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID
end

function GameLayer:onExitRoom()
    self._gameFrame:onCloseSocket()
    self:stopAllActions()
    self:KillGameClock()
    self:dismissPopWait()
    --self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
    self._scene:onKeyBack()
    self.m_userRecord = {}
end

--换位操作
function GameLayer:onChangeDesk()
    self._gameFrame:QueryChangeDesk()
end


--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    self._gameView:showSitBtn(dwWaitTime ~= 0 and true or false,dwWaitTime)
end


function GameLayer:onPlayingUserInfo(userInfo)

    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
    dump(self.m_playingUser,"==========onPlayingUserInfo===========")
end



--系统消息
function GameLayer:onSystemMessage( wType,szString )

        local msg = szString or ""
        self.m_querydialog = QueryDialog:create(msg,function()

            if not self.m_bClubRoom then 
                    self:onExitTable()
            end 

        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)

end


-- 椅子号转视图位置,注意椅子号从0~nChairCount-1,返回的视图位置从1~nChairCount
function GameLayer:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = self:getPlayerNum()
    local myChairID = yl.INVALID_CHAIR
    if self:GetMeChairID() ~= yl.INVALID_CHAIR then
    	myChairID = self:GetMeChairID()
    else
    	myChairID = self.myChirID
    end

    myChairID = myChairID + 1
    chair = chair + 1

    if nChairCount == 3 then
        local right = myChairID + 1
        if right > nChairCount then
            right = 1
        end

        local left = myChairID - 1
        if left <= 0  then
            left = nChairCount
        end


         -- print("获取转换后的位置ID nChairCount chair, myChairID, right, top, left",nChairCount, chair, myChairID, right, top, left)
         if chair == myChairID then
            return cmd.MY_VIEWID
         elseif chair == right then
            return cmd.RIGHT_VIEWID
         elseif chair == left then
            return cmd.LEFT_VIEWID
         -- elseif chair == top then
         --     return cmd.TOP_VIEWID
         end
    else
        if chair == myChairID then
            return 1
        else
            if chair > myChairID then
                return cmd.RIGHT_VIEWID
            else
                return cmd.LEFT_VIEWID
            end
        end
    end
end

function GameLayer:getRoomHostViewId()
	return self.wRoomHostViewId
end

function GameLayer:getUserInfoByChairID(chairId)
	return self.m_sparrowUserItem[chairId]
end

function GameLayer:getSitUserInfoByChairID(chairId)
	for i=1, 4 do --查找在座位上的玩家
		if nil ~= self._gameView.m_UserItem[i] and self.m_sparrowUserItem[chairId] and self._gameView.m_UserItem[i].dwUserID == self.m_sparrowUserItem[chairId].dwUserID then
			return self.m_sparrowUserItem[chairId]
		end
	end
	return nil
end

function GameLayer:getMaCount()
	print("返回码数", self.cbMaCount)
	return self.cbMaCount
end

function GameLayer:onGetSitUserNum()
	local num = 0
	for i = 1, cmd.GAME_PLAYER do
		if nil ~= self._gameView.m_tabUserHead[i] then
			num = num + 1
		end
	end
    return num
end

function GameLayer:onEnterTransitionFinish()
    GameLayer.super.onEnterTransitionFinish(self)
end

-- 计时器响应
function GameLayer:OnEventGameClockInfo(chair,time,clockId)
    local selfChairID = self:GetMeChairID()
    local meChairId = self:SwitchViewChairID(selfChairID)

	--指针指向
    self._gameView:OnUpdataClockPointView(chair, selfChairID)
    --设置转盘时间
    self._gameView:OnUpdataClockTime(time)

    if self._gameView.bIsVideo then
    	self._gameView:OnUpdataClockTime(0)
    end
    -- if (GlobalUserItem.bPrivateRoom and self.playMode~=1) or self._gameView.bIsVideo then
    if (GlobalUserItem.bPrivateRoom or self._gameView.bIsVideo) and not GlobalUserItem.bPrivateGoldRoom then
        return
    end
    -- body
    -- print("更新中间指针， 位置， 时间", clockId, chair)

    if time <= 0 and self.m_bLookOn then 
        return 
    end

    if clockId == cmd.IDI_START_GAME then
    	--托管
    	--超时
		if time <= 0 then

                                      if self.m_bClubRoom then
                                            if PriRoom:getInstance().m_tabPriData.dwPlayCount==0 then
                                                self:onExitTable()
                                            end
                                      else
                                            self._gameFrame:setEnterAntiCheatRoom(false)--退出防作弊，如果有的话
                                            self:onExitTable()
                                      end

		elseif time <= 5 then
    		      self:PlaySound(cmd.RES_PATH.."sound/GAME_WARN.wav")
		end
    elseif clockId == cmd.IDI_PIAO_CARD then   

                if time <= 0 then
                    
                elseif time <= 5 then
                          self:PlaySound(cmd.RES_PATH.."sound/GAME_WARN.wav")
                end

    elseif clockId == cmd.IDI_OUT_CARD then
    	if chair == meChairId then
    		--托管.
    		--超时
    		if time <= 0 and not self.bTrustee then
				--self:sendUserTrustee()
				self._gameView._cardLayer:setMyCardTouchEnabled(false)
				self._gameView:ShowGameBtn(GameLogic.WIK_NULL)
    		elseif time <= 5 then
    			self:PlaySound(cmd.RES_PATH.."sound/GAME_WARN.wav")
    		end
    	end
    elseif clockId == cmd.IDI_OPERATE_CARD then
    	--if chair == meChairId then
    		if self.isMyProvideUser then
    			return
    		end
    		--超时
    		if time <= 0 and not self.bTrustee then
    			--放弃，进入托管,隐藏操作按钮
    			self._gameView:ShowGameBtn(GameLogic.WIK_NULL)
    			--self:sendUserTrustee()
    			self._gameView._cardLayer:setMyCardTouchEnabled(false)
    		elseif time <= 5 then
    			self:PlaySound(cmd.RES_PATH.."sound/GAME_WARN.wav")
    		end
    	--end
    end
end

--退出桌子
function GameLayer:onExitTable()
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
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


--用户聊天
function GameLayer:onUserChat(chat, wChairId)
	print("玩家聊天", chat.szChatString)
    self._gameView:onUserChat(chat, self:SwitchViewChairID(wChairId))
end

--用户表情
function GameLayer:onUserExpression(expression, wChairId)
    self._gameView:onUserExpression(expression, self:SwitchViewChairID(wChairId))
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, true)
    end
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, false)
    end
end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    print("GameLayer:onEventUserStatus=====<sid,nick,newstatus>", useritem.wChairID, useritem.szNickName, newstatus.cbUserStatus)
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then

            if self.isPriOver and  GlobalUserItem.bPrivateRoom then
                return
            end

	    if (oldstatus.wTableID ~= self:GetMeUserItem().wTableID) then
	        return
	    end

        if yl.INVALID_CHAIR ==  useritem.wChairID then

        	-- print("查找人数",#self._gameView.m_UserItem)
        	for i=1, cmd.GAME_PLAYER do
                local seatObj = self._gameView.m_UserItem[i]
	        	if seatObj and self._gameView.m_UserItem[i].dwUserID == useritem.dwUserID then
	        		print("查找",#self._gameView.m_UserItem, useritem.szNickName, self._gameView.m_UserItem[i].szNickName)
	        		self._gameView:OnUpdateUserExit(i)
	        		self._gameView:showUserState(i, false)
	        	end
        	end

            for i=1,cmd.GAME_PLAYER do
                local userObj = self._gameView.m_UserItem[i]
                if userObj and userObj.dwUserID == useritem.dwUserID then
                    -- print("查找",#self._gameView.m_UserItem, useritem.szNickName, self._gameView.m_UserItem[i].szNickName)
                    self._gameView:OnUpdateUserExit(i)
                    self._gameView:showUserState(i, false)
                end
            end

        else
        	local wViewChairId = self:SwitchViewChairID(useritem.wChairID)
        	self._gameView:OnUpdateUserExit(wViewChairId)
        	-- print("删除", wViewChairId)

        	self._gameView:showUserState(wViewChairId, false)
        end

    else

    	if (newstatus.wTableID ~= self:GetMeUserItem().wTableID)  or newstatus.cbUserStatus == yl.US_LOOKON then
	        return
	end
	    local wViewChairId = self:SwitchViewChairID(useritem.wChairID)
	    if newstatus.cbUserStatus == yl.US_READY then
            self._gameView:showUserState(wViewChairId, true)
        end
        self.m_sparrowUserItem[useritem.wChairID +1] = useritem
        --刷新用户信息
        -- if useritem == self:GetMeUserItem() then
        --     return
        -- end
        --先判断是否是换桌
        print("更新新玩家", wViewChairId, useritem)
    	self._gameView:OnUpdateUser(wViewChairId, useritem)
    end

    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInviteBtn then
            self._gameView._priView:onRefreshInviteBtn()
        end
    end


end

--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)
    -- print("GameLayer:onEventUserEnter===>", tableid, chairid)
    -- dump(useritem, "#GameLayer:onEventUserEnter===>")

    if tableid ~= self:GetMeUserItem().wTableID and not GlobalUserItem.bVideo then
       return
    end

    if  useritem.cbUserStatus == yl.US_LOOKON then 
        
        return
    end

    -- dump(useritem, "#GameLayer:onEventUserEnter=====")
  	--刷新用户信息
    local wViewChairId = self:SwitchViewChairID(useritem.wChairID)
    self.m_sparrowUserItem[useritem.wChairID +1] = useritem
    if useritem.cbUserStatus == yl.US_READY then
        self._gameView:showUserState(wViewChairId, true)
    end
    self._gameView:OnUpdateUser(wViewChairId, useritem)

end

--用户分数
function GameLayer:onEventUserScore( item )
    if item.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end
    local wViewChairId = self:SwitchViewChairID(item.wChairID)
    self._gameView:OnUpdateUser(wViewChairId, item)
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        if exit then
            local shareLayer = cc.CSLoader:createNode(cmd.RES_PATH.."ShareLayer.csb"):addTo(self)
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

--查找可以杠的牌
function GameLayer:findUserGangCard(cbCardData)
	local card = {}
	for i=1,#cbCardData do
		local cardNum = 0
		local cardValue = cbCardData[i]
		for j=i,#cbCardData do
			if cardValue == cbCardData[j] then
				cardNum = cardNum +1
			end
		end
		if 4 == cardNum then
			table.insert(card, cardValue)
		end
	end
	return card
end

function GameLayer:getGameRule(buffer)

        local strTable ={}
        --游戏模式(玩法)
        local piao = buffer[4]
        local maMode= buffer[5]
        local ma = buffer[6]
        local wanfa = buffer[7]

        local piaoStr = {"不漂","漂一次","每次要漂"}
        table.insert(strTable,piaoStr[piao+1])

        local wanfaStr = {"全频道","半频道"}
        table.insert(strTable,wanfaStr[wanfa+1])

        local maModeStr = {"不买马","自摸买马","亮倒自摸买马"}
        table.insert(strTable,maModeStr[maMode+1])
        if maMode ~= 0 then 
            local maStr = {"买1马","买6马","买一送一"}
            table.insert(strTable,maStr[ma])
        end

    return strTable
end



-- 场景消息
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
    -- print("GameLayer:onEventGameScene==", cbGameStatus, cmd.GAME_SCENE_FREE, cmd.GAME_SCENE_PLAY)
        if self.m_bOnGame then
        print("GameLayer:onEventGameScene self.m_bOnGame false return")
            return
        end

        self.m_cbGameStatus = cbGameStatus
        self.m_bOnGame = true

        local myUserItem = self:GetMeUserItem()
        if myUserItem.cbUserStatus ~= yl.US_LOOKON then 
            self.m_bLookOn = false
        else
            self.m_bLookOn = true
        end

    --先清理玩家信息
    for i = 1, cmd.GAME_PLAYER do
        local wViewChairId = self:SwitchViewChairID(i-1)
        self._gameView:OnUpdateUserExit(wViewChairId)
    end

    --初始化已有玩家
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        if nil ~= userItem then
            local wViewChairId = self:SwitchViewChairID(i-1)
            -- print("SwitchViewChairID======",i-1,wViewChairId)
            self.m_sparrowUserItem[i] = userItem
            if userItem.cbUserStatus == yl.US_READY then
        		self._gameView:showUserState(wViewChairId, true)
        	else
        		self._gameView:showUserState(wViewChairId, false)
    		end
            self._gameView:OnUpdateUser(wViewChairId, userItem)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(wViewChairId, userItem, false)
            end
        end
    end

    --回放
	self._gameView.bIsVideo = false
    if unit_test and testcase_protocol then
        self.super:onEventGameScene(cbGameStatus, dataBuffer)
    end

	-- if cbGameStatus == cmd.GAME_SCENE_FREE then
	-- 	self:onGameSceneFree(dataBuffer)
	-- elseif cbGameStatus == cmd.GAME_SCENE_PLAY or cbGameStatus == 102 then
	-- 	self:onGameScenePlay(dataBuffer)
	-- else
	-- 	print("\ndefault\n")
	-- 	return false
	-- end

            if cbGameStatus == cmd.GS_MJ_FREE then
                self:onGameSceneFree(dataBuffer)
            elseif cbGameStatus == cmd.GS_MJ_PLAY then
                self:onGameScenePlay(dataBuffer)
            elseif cbGameStatus == cmd.GS_MJ_PIAO then
                self:onGameScenePiao(dataBuffer)
            else
                print("\ndefault\n")
                return false
            end

	self.myChirID = self:GetMeChairID()

            -- 刷新房卡
            if PriRoom and GlobalUserItem.bPrivateRoom then
                if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
                    self._gameView._priView:onRefreshInfo()
                end
                if self.playMode and self.playMode==1 then
                    -- self._gameView._priView:setGameInfoBtn()
                    for i = 1, cmd.GAME_PLAYER do
                        if self._gameView.m_tabUserHead[i] then
                            self._gameView.m_tabUserHead[i]:showRoomHolder(false)
                        end
                    end
                end
            end

            if GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom then 
                self.m_bClubRoom = true
            end

            self._gameView:showSitBtn(self.m_bLookOn)

            self:onSendQueryRecord()

	self:dismissPopWait()

	return true
end

function GameLayer:onGameSceneFree(dataBuffer)
		local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)

		self.lCellScore = cmd_data.lCellScore
        self.playMode = cmd_data.cbPlayMode
        GlobalUserItem.bPrivateGoldRoom = self.playMode == 1 or false
		--历史积分
        dump(cmd_data, "------------空闲状态--------------",6)
		--规则
        self.cbPlayerCount = cmd_data.cbPlayerCount or cmd.GAME_PLAYER
        self.cbMaCount = cmd_data.cbMaCount
        self.cbMagicMode = cmd_data.cbMagicMode
        self.bQiangGangHu = cmd_data.bQiangGangHu
        self.bHuQiDui = cmd_data.bHuQiDui
        self.bHaveZiCard = cmd_data.bHaveZiCard
        self.bNoMagicDouble = cmd_data.bNoMagicDouble
        self.cbAllCardCount = cmd_data.cbAllCardCount

        self.cbMaMode = cmd_data.cbMaMode
        self.cbPiaoMode = cmd_data.cbPiaoMode
        self.cbPlayerCount = cmd_data.cbPlayerCount
        self.cbRule = cmd_data.cbRule

        self.cbTimeOutCard = cmd_data.cbTimeOutCard or cmd.TIME_OUT_CARD_FAST
        self.cbTimeOperateCard = cmd_data.cbTimeOperate or cmd.OPERATE_TIME
        self.cbTimeStartGame = cmd_data.cbTimeStart or cmd.TIME_START_GAME
        self.cbTimeWaitEnd = cmd_data.cbTimeWaitEnd or cmd.OPERATE_TIME
        self.cbTimeCallPiao = cmd_data.cbTimeCallPiao or 30



        local piaoMode, channel, maMode, maNum = cmd_data.cbPiaoMode, cmd_data.cbRule, cmd_data.cbMaMode, cmd_data.cbMaCount
		--设置信息
		self._gameView:onshowRule(piaoMode, channel, maMode, maNum)
		--设置牌数
		self._gameView:onUpdataLeftCard(self.cbAllCardCount)

		--防作弊不显示
		if not GlobalUserItem.isAntiCheat() then
			--判断我的状态
			local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), self:GetMeChairID())
			if nil ~= userItem then
				if userItem.cbUserStatus < yl.US_READY then
					self._gameView.btStart:setVisible(true)
				end
			end
		end


		if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
                                self._gameView:setClockVisible(false)

		else
			local wMyChairId = self:GetMeChairID()
                                      self._gameView:OnUpdataClockTime(self.cbTimeStartGame)
			self:SetGameClock(self:SwitchViewChairID(wMyChairId), cmd.IDI_START_GAME, self.cbTimeStartGame)
		end
end


function GameLayer:onGameScenePiao(dataBuffer)
        local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusPiao, dataBuffer)

        self.lCellScore = cmd_data.lCellScore
        self.playMode = cmd_data.cbPlayMode
        GlobalUserItem.bPrivateGoldRoom = self.playMode == 1 or false
        --历史积分
        dump(cmd_data, "------------叫漂状态--------------",6)
        --规则
        self.cbPlayerCount = cmd_data.cbPlayerCount or cmd.GAME_PLAYER
        self.cbMaCount = cmd_data.cbMaCount
        self.cbMagicMode = cmd_data.cbMagicMode
        self.bQiangGangHu = cmd_data.bQiangGangHu
        self.bHuQiDui = cmd_data.bHuQiDui
        self.bHaveZiCard = cmd_data.bHaveZiCard
        self.bNoMagicDouble = cmd_data.bNoMagicDouble
        self.cbAllCardCount = cmd_data.cbAllCardCount

        self.cbMaMode = cmd_data.cbMaMode
        self.cbPiaoMode = cmd_data.cbPiaoMode
        self.cbPlayerCount = cmd_data.cbPlayerCount
        self.cbRule = cmd_data.cbRule

        self.m_bGameStart = true

        self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1])

        self.cbTimeOutCard = cmd_data.cbTimeOutCard or cmd.TIME_OUT_CARD_FAST
        self.cbTimeOperateCard = cmd_data.cbTimeOperate or cmd.OPERATE_TIME
        self.cbTimeStartGame = cmd_data.cbTimeStart or cmd.TIME_START_GAME
        self.cbTimeWaitEnd = cmd_data.cbTimeWaitEnd or cmd.OPERATE_TIME
        self.cbTimeCallPiao = cmd_data.cbTimeCallPiao or 30


        --托管判断
        for i = 1, cmd.GAME_PLAYER do
            local viewId = self:SwitchViewChairID(i - 1)
            if viewId == cmd.MY_VIEWID then
                self.bTrustee = cmd_data.bTrustee[1][i]
                if self.bTrustee then --显示托管界面
                    self._gameView:setTrustShadowVisible(true)
                end
            end
        end

        local piaoMode, channel, maMode, maNum = cmd_data.cbPiaoMode, cmd_data.cbRule, cmd_data.cbMaMode, cmd_data.cbMaCount
        --设置信息
        self._gameView:onshowRule(piaoMode, channel, maMode, maNum)
        --设置牌数
        self._gameView:onUpdataLeftCard(self.cbAllCardCount)

        self._gameView.btStart:setVisible(false)

        local wMyChairId = self:GetMeChairID()

        local myPiao = cmd_data.bCallPiao[1][wMyChairId+1]

        if self.m_cbPlayStatus[wMyChairId+1] ==1 and not myPiao and not self.m_bLookOn then 
            self._gameView:setPiaoVisible(true)

            if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
                self._gameView:setClockVisible(false)
            else
                self._gameView:setClockVisible(true)
                self._gameView:OnUpdataClockTime(self.cbTimeCallPiao)
                self:SetGameClock(self:SwitchViewChairID(wMyChairId), cmd.IDI_PIAO_CARD, self.cbTimeCallPiao)
            end
        end 


end


function GameLayer:onGameScenePlay(dataBuffer)
    print("GameLayer:onGameScenePlay==")
    self:OnResetGameEngine()

	--准备不显示
	for i=1,cmd.GAME_PLAYER do
		self._gameView:showUserState(i, false)
	end

	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
	-- dump(cmd_data.WeaveItemArray, "WeaveItemArray")
	-- dump(cmd_data.cbWeaveCount, "cbWeaveCount")
	-- dump(cmd_data.cbCardData, "cbCardData")
	-- dump(cmd_data.cbMagicIndex, "cbMagicIndex")

    dump(cmd_data, "------------游戏状态--------------",6)

	self.lCellScore = cmd_data.lCellScore
    self.cbPlayerCount = cmd_data.cbPlayerCount or cmd.GAME_PLAYER
    self.cbPiaoMode = cmd_data.cbPiaoMode
    self.cbMaMode = cmd_data.cbMaMode       --0不买马，1自摸买马，2亮倒自摸买马
    self.cbMaCount = cmd_data.cbMaCount     --0无，1买1马，2买6马，3买1送1
    self.cbRule = cmd_data.cbRule           --0全频道，1半频道

    self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1])

    --游戏变量
    self.wBankerUser = cmd_data.wBankerUser
    self.wCurrentUser = cmd_data.wCurrentUser

    --状态变量
    self.cbActionCard = cmd_data.cbActionCard
    self.cbActionMask = cmd_data.cbActionMask
    self.cbHearStatus = cmd_data.cbHearStatus[1]
    self.cbLeftCardCount = cmd_data.cbLeftCardCount
    self.bTrustee = cmd_data.bTrustee

    --出牌信息
    self.wOutCardUser = cmd_data.wOutCardUser
    self.cbOutCardData = cmd_data.cbOutCardData
    self.cbDiscardCount = cmd_data.cbDiscardCount
    self.cbDiscardCard = cmd_data.cbDiscardCard

    --扑克信息
    self.cbCardCount = cmd_data.cbCardCount
    self.cbCardData = cmd_data.cbCardData
    self.cbSendCardData = cmd_data.cbSendCardData
    self.cbCardIndex = cmd_data.cbCardIndex
    self.cbLinstenCardData = cmd_data.cbLinstenCardData

    --组合扑克
    self.cbWeaveCount = cmd_data.cbWeaveCount
    self.WeaveItemArray = cmd_data.WeaveItemArray

    --牌堆信息
    self.wHeapHead = cmd_data.wHeapHead
    self.wHeapTail = cmd_data.wHeapTail
    self.cbHeapCardInfo = cmd_data.cbHeapCardInfo

    --胡牌信息
    self.cbHuCardCount = cmd_data.cbHuCardCount
    self.cbHuCardData = cmd_data.cbHuCardData
    --叫漂情况
    self.cbUserPiao = cmd_data.cbUserPiao
    --房主
    self.wTableOwnerID = cmd_data.wTableOwnerID

    self.cbTimeOutCard = cmd_data.cbTimeOutCard or cmd.TIME_OUT_CARD_FAST
    self.cbTimeOperateCard = cmd_data.cbTimeOperate or cmd.OPERATE_TIME
    self.cbTimeStartGame = cmd_data.cbTimeStart or cmd.TIME_START_GAME
    self.cbTimeWaitEnd = cmd.OPERATE_TIME

    self.cbTimeCallPiao = cmd_data.cbTimeCallPiao or 30

    self.playMode = cmd_data.cbPlayMode or 1
    GlobalUserItem.bPrivateGoldRoom = self.playMode == 1 or false

	--设置癞子
	-- self._gameView._cardLayer.cbMagicData = cmd_data.cbMagicIndex[1]
	-- self._gameView:onUpdataMagicCard(cmd_data.cbMagicIndex[1])
	--规则

	-- self.cbMagicMode = cmd_data.cbMagicMode
	-- self.bQiangGangHu = cmd_data.bQiangGangHu
	-- self.bHuQiDui = cmd_data.bHuQiDui
	-- self.bHaveZiCard = cmd_data.bHaveZiCard
	-- self.bNoMagicDouble = cmd_data.bNoMagicDouble

	self.cbAllCardCount = self.cbCardCount
	self.cbLeftCardCount = self.cbLeftCardCount
	self._gameView.m_nAllCard = self.cbAllCardCount

	--庄家self.cbMagicData
	local wViewBankerUser = self:SwitchViewChairID(self.wBankerUser)
	self._gameView.m_tabUserHead[wViewBankerUser]:showBank(true)

    local piaoMode, channel, maMode, maNum = cmd_data.cbPiaoMode, cmd_data.cbRule, cmd_data.cbMaMode, cmd_data.cbMaCount

	--设置信息
	self._gameView:onshowRule(piaoMode, channel, maMode, maNum)
	--剩余牌数
	self._gameView:onUpdataLeftCard(self.cbLeftCardCount)


    ---------------------------------------------哪些牌不能出
    for idx,cardList in pairs(cmd_data.cbHuCardData) do
        local viewid = self:SwitchViewChairID(idx-1)
        local huValues = {}
        if viewid ~= cmd.MY_VIEWID then
            for _,cardValue in pairs(cardList) do
                if cardValue > 0 then
                    self._ban_out_list[cardValue] = true
                    table.insert(huValues, cardValue)
                end
            end
        else
            for _,cardValue in pairs(cardList) do
                if cardValue > 0 then
                    table.insert(huValues, cardValue)
                end
            end
        end

        if next(huValues) then
            self._gameView:showHuTipStatic(viewid, huValues)
        end
    end
    dump(self._ban_out_list, "#self._ban_out_list")
    self._gameView._cardLayer:setBanOutList(self._ban_out_list)
    ---------------------------------------------哪些牌不能出

    local playerNum = self:getPlayerNum()
	--先设置已经出的牌
	for i=1,playerNum do
		local wViewChairId = self:SwitchViewChairID(i - 1)
		if cmd_data.cbWeaveCount[1][i] > 0 then
			for j = 1, cmd_data.cbWeaveCount[1][i] do
				local cbOperateData = {} --此处为tagAvtiveCard
				cbOperateData.cbCardValue = cmd_data.WeaveItemArray[i][j].cbCardData[1]
				-- dump(cbOperateData.cbCardValue, "#已经出的牌")
				local nShowStatus = GameLogic.SHOW_NULL
                --[[
				local cbParam = cmd_data.WeaveItemArray[i][j].cbParam
				if cbParam == GameLogic.WIK_GANERAL then
					if cbOperateData[1] == cbOperateData[2] then 	--碰
						nShowStatus = GameLogic.SHOW_PENG
					else 											--吃
						nShowStatus = GameLogic.SHOW_CHI
					end
					cbOperateData.cbCardNum = 3
                    ]]
                                                    --卡五星没有吃
                                                    local cbParam = cmd_data.WeaveItemArray[i][j].cbWeaveKind
                                                    if cbParam == GameLogic.WIK_PENG then
                                                        nShowStatus = GameLogic.SHOW_PENG
                                                        cbOperateData.cbCardNum = 3
				elseif cbParam == GameLogic.WIK_MING_GANG or cbParam == GameLogic.WIK_GANG then
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

				self._gameView._cardLayer:createActiveCardReEnter(wViewChairId, cbOperateData)
			end
		end
	end

	--设置手牌
	local viewCardCount = {}
	for i = 1, playerNum do
		local viewId = self:SwitchViewChairID(i - 1)
		viewCardCount[viewId] = cmd_data.cbCardCount[1][i]

	end
	local cbHandCardData = {}
	for i = 1, viewCardCount[cmd.MY_VIEWID] do
		local data = cmd_data.cbCardData[1][i]
		table.insert(cbHandCardData, data)
	end
	local cbSendCard = cmd_data.cbSendCardData
	if cbSendCard > 0 and self.wCurrentUser == self:GetMeChairID() then
		for i = 1, #cbHandCardData do
			if cbHandCardData[i] == cbSendCard then
				table.remove(cbHandCardData, i)				--把刚抓的牌放在最后
				break
			end
		end
		table.insert(cbHandCardData, cbSendCard)
	end

	-- dump(cbHandCardData, "#我的手牌")
	for i = 1, playerNum do
                          if self.m_cbPlayStatus[i] == 1 then 
                                      local viewId = self:SwitchViewChairID(i - 1)
                		if viewId == cmd.MY_VIEWID then
                			self._gameView._cardLayer:createHandCard(viewId, cbHandCardData, viewCardCount[viewId])
                		else
                			self._gameView._cardLayer:createHandCard(viewId, nil, viewCardCount[viewId])
                		end
                          end
	end

	--设置已经出的牌
	for i = 1, playerNum do
                          if self.m_cbPlayStatus[i] == 1 then 
                		local viewId = self:SwitchViewChairID(i - 1)
                		local cbOutCardData = {}
                		for j = 1, cmd_data.cbDiscardCount[1][i] do
                			--已出的牌
                			cbOutCardData[j] = cmd_data.cbDiscardCard[i][j]
                		end
                		self._gameView._cardLayer:createOutCard(viewId, cbOutCardData, cmd_data.cbDiscardCount[1][i])
                          end
	end

	--托管判断
	for i = 1, playerNum do
        local viewId = self:SwitchViewChairID(i - 1)
		--self._gameView:setUserTrustee(viewId, cmd_data.bTrustee[1][i])
		if viewId == cmd.MY_VIEWID then
			self.bTrustee = cmd_data.bTrustee[1][i]
			if self.bTrustee then --显示托管界面
				self._gameView:setTrustShadowVisible(true)
			end
		end
	end

	--刚出的牌
	if cmd_data.cbOutCardData and cmd_data.cbOutCardData > 0 then
		local wOutUserViewId = self:SwitchViewChairID(cmd_data.wOutCardUser)
		self._gameView:showOutCard(wOutUserViewId, cmd_data.cbOutCardData, true)
	end

    -- print("计时器倒计时=======", self.cbTimeOutCard)
	--计时器
    if self.wCurrentUser ~= yl.INVALID_CHAIR then
        self:SetGameClock(self:SwitchViewChairID(self.wCurrentUser), cmd.IDI_OUT_CARD, self.cbTimeOutCard)
    else
        if cmd_data.cbActionMask > 0 then
            local selfChairID = self:GetMeChairID()
            self:SetGameClock(self:SwitchViewChairID(selfChairID), cmd.IDI_OUT_CARD, self.cbTimeOutCard)
        end
    end

	--允许操作操作
	if self.wCurrentUser == self:GetMeChairID() then
		self._gameView._cardLayer:setMyCardTouchEnabled(true)
	end

	--操作提示
	print("断线重连玩家碰杠信息", self.bTrustee, cmd_data.cbActionMask)
	if not self.bTrustee then
		--如果我可以操作，显示操作栏
		if GameLogic.WIK_NULL ~=  cmd_data.cbActionMask then
			self._gameView:ShowGameBtn(cmd_data.cbActionMask)
			self._gameView.cbActionCard = cmd_data.cbActionCard
			self.cbActionMask = cmd_data.cbActionMask
			self.cbActionCard = cmd_data.cbActionCard
		end
	else
		-- self.bTrustee = cmd_data.bTrustee
		self._gameView:setTrustShadowVisible(self.bTrustee)
	end

    ---------------------------------------------亮倒处理
    ---------------------------------------------找到自己已经杠了的牌
    local alreadyGangCards = {}
    local mineChairId = self:GetMeChairID()
    local weaveItemList = cmd_data.WeaveItemArray[mineChairId+1]
    for _,item in pairs(weaveItemList) do
        local value = item.cbCardData[1][1]
        if value > 0 then
            if item.cbWeaveKind == GameLogic.WIK_MING_GANG or
                item.cbWeaveKind == GameLogic.WIK_FANG_GANG or
                item.cbWeaveKind == GameLogic.WIK_GANG or
                item.cbWeaveKind == GameLogic.WIK_AN_GANG then
                alreadyGangCards[value] = true
            end
        end
    end
    -- dump(alreadyGangCards, "#alreadyGangCards===")
    ---------------------------------------------找到自己已经杠了的牌

    for playerIdx = 1, playerNum do
        local viewid = self:SwitchViewChairID(playerIdx-1)
        local handCardsData = {}
        for k,v in pairs(self.cbCardIndex[playerIdx]) do
            for _=1,v do
                table.insert(handCardsData, GameLogic.LocalCardData[k])
            end
        end

        -- dump(handCardsData, "#handCardsData==")
        local kouPai = cmd_data.cbLinstenCardData[playerIdx]
        --要去掉已经杠了的扣牌

        ---------------------------------------------判断有没有扣牌,如果有扣牌,就取得扣牌数据
        if kouPai and next(kouPai) then
            local mapKouPai = {}
            local isInMap = {}
            for k,v in pairs(kouPai) do
                if v>0 and not isInMap[v] and not alreadyGangCards[v] then
                    table.insert(mapKouPai, v)
                    isInMap[v] = true
                end
            end

            self._gameView._cardLayer:createKouPai(viewid, mapKouPai)
        end
        ---------------------------------------------判断有没有扣牌,如果有扣牌,就取得扣牌数据

        ---------------------------------------------亮倒的牌

        -- dump(handCardsData, "#before filter handCardsData===")
        local status = self.cbHearStatus[playerIdx]
        if status and status ~= 0 then --已经亮倒
            if next(kouPai) then
                handCardsData = table_filter(handCardsData, kouPai, 0)
            end
            -- dump(handCardsData, "#showUserTileMajong handCardsData===")
            self._gameView._cardLayer:showUserTileMajong(viewid, handCardsData)
            self._gameView:showShowOutTip(viewid)
        end
        ---------------------------------------------亮倒的牌
    end
    ---------------------------------------------亮倒处理

    self._gameView._cardLayer:setBanListGray()
    --dump(self.m_sparrowUserItem, "#m_sparrowUserItem==")
end

-- 游戏消息
function GameLayer:onEventGameMessage(sub, dataBuffer)
    if unit_test and testcase_protocol then
        testcase_protocol:onEventGameMessage_kwx(sub, dataBuffer)
    end

	if sub == cmd.SUB_S_GAME_START then 					--游戏开始
        self.m_cbGameStatus = cmd.GAME_SCENE_PLAY
		return self:onSubGameStart(dataBuffer)
	elseif sub == cmd.SUB_S_OUT_CARD then 					--用户出牌
		return self:onSubOutCard(dataBuffer)
	elseif sub == cmd.SUB_S_SEND_CARD then 					--发送扑克
		return self:onSubSendCard(dataBuffer)
	elseif sub == cmd.SUB_S_OPERATE_NOTIFY then 			--操作提示
		return self:onSubOperateNotify(dataBuffer)
	elseif sub == cmd.SUB_S_HU_CARD then 					--胡牌提示
		return self:onSubHuNotify(dataBuffer)
	elseif sub == cmd.SUB_S_OPERATE_RESULT then 			--操作命令
		return self:onSubOperateResult(dataBuffer)
	elseif sub == cmd.SUB_S_LISTEN then 				--用户亮倒
		return self:onSubListenCard(dataBuffer)
	-- elseif sub == cmd.SUB_S_TING_DATA then 					--听牌数据
	-- 	return self:onSubListenCardData(dataBuffer)
	elseif sub == cmd.SUB_S_TRUSTEE then 					--用户托管
		return self:onSubTrustee(dataBuffer)
	elseif sub == cmd.SUB_S_GAME_CONCLUDE then 				--游戏结束
        self.m_cbGameStatus = cmd.GAME_SCENE_FREE
		return self:onSubGameConclude(dataBuffer)
	elseif sub == cmd.SUB_S_RECORD then 					--游戏记录
		return self:onSubGameRecord(dataBuffer)
	elseif sub == cmd.SUB_S_SET_BASESCORE then 				--设置基数
		self.lCellScore = dataBuffer:readint()
		return true
    elseif sub == cmd.SUB_S_CALLPIAO then                     --开始叫漂
        self.m_cbGameStatus = cmd.GAME_SCENE_PLAY
        self:onUserBeginPiao(dataBuffer)
        return true
    elseif sub == cmd.SUB_S_PIAO then                     --叫漂

        self:onUserCallPiao(dataBuffer)
        return true
    elseif sub == cmd.SUB_S_TING_DATA_NOTIFY then
        self:onListenDataNotify(dataBuffer)
        return true
    elseif sub == cmd.SUB_S_GANG then
        self:onSubGang(dataBuffer)
        return true
	else
		assert(false, "GameLayer:onEventGameMessage invalid sub", sub)
	end

	return false
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)

            if not self.m_bGameStart then 
                    self:OnResetGameEngine()
                    self._gameView:showSitBtn(false)
                    self._gameView.btStart:setVisible(false)

                    -- 刷新房卡
                    if PriRoom and GlobalUserItem.bPrivateRoom then
                        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
                            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
                            self._gameView._priView:onRefreshInfo()
                        end
                    end
             end


            self.m_bGameStart = true

	self.m_cbGameStatus = cmd.GAME_SCENE_PLAY

	--准备不显示
	for i=1,cmd.GAME_PLAYER do
		self._gameView:showUserState(i, false)
                        --self._gameView:setTrustShadowVisible(false)
	end

	self.myChirID = self:GetMeChairID()
	-- print("游戏开始")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
	 dump(cmd_data, "#CMD_S_GameStart===",6)
            self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1])

	self.wBankerUser = cmd_data.wBankerUser
	local wViewBankerUser = self:SwitchViewChairID(self.wBankerUser)
	self._gameView.m_tabUserHead[wViewBankerUser]:showBank(true)



	local cbCardCount = {0, 0, 0, 0}
	for i = 1, cmd.GAME_PLAYER do
		local userItem = self._gameFrame:getTableUserItem(self:GetMeTableID(), i - 1)
		self.m_sparrowUserItem[i] = userItem
		local wViewChairId = self:SwitchViewChairID(i - 1)
		self._gameView:OnUpdateUser(wViewChairId, userItem)
		if userItem then
			
			cbCardCount[wViewChairId] = 13
			if wViewChairId == wViewBankerUser then
				cbCardCount[wViewChairId] = cbCardCount[wViewChairId] + 1
			end
		end
	end

	if self.wBankerUser ~= self:GetMeChairID() then
		cmd_data.cbCardData[1][cmd.MAX_COUNT] = nil
	end
    local myCardData = {}
    for i=1, cmd.MAX_COUNT do
        if 0 ~= cmd_data.cbCardData[1][i] then
            myCardData[i] = cmd_data.cbCardData[1][i]
        end
    end
    table.sort(myCardData, function (a, b)
            return a < b
        end)

	--设置癞子
	--self._gameView._cardLayer.cbMagicData = cmd_data.cbMagicIndex[1]

	--开始发牌
    -- self._gameView:gameStart(wViewBankerUser, myCardData, cbCardCount, cmd_data.cbUserAction, cmd_data.cbMagicIndex[1])
	self._gameView:gameStart(wViewBankerUser, myCardData, cbCardCount, cmd_data.cbUserAction, {})
	--设置时间
	self:SetGameClock(wViewBankerUser, cmd.IDI_OUT_CARD, self.cbTimeOutCard +8) --加的8为发牌时间

	self.wCurrentUser = cmd_data.wBankerUser
	self.bMoPaiStatus = true
	self.bSendCardFinsh = false
	self:PlaySound(cmd.RES_PATH.."sound/GAME_START.wav")

end

--用户出牌
function GameLayer:onSubOutCard(dataBuffer)
	-- print("用户出牌")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_OutCard, dataBuffer)
	dump(cmd_data, "#CMD_S_OutCard==")

	local wViewId = self:SwitchViewChairID(cmd_data.wOutCardUser)
	self._gameView:gameOutCard(wViewId, cmd_data.cbOutCardData, cmd_data.bSysOut)

	self:PlaySound(cmd.RES_PATH.."sound/OUT_CARD.wav")
	self:playCardDataSound(cmd_data.wOutCardUser, cmd_data.cbOutCardData)

    -------------------------------------如果亮倒了,还要打掉一张
    --协议设计有点尴尬,亮倒,通知服务端扣了哪些牌,然后再通知一次,打掉哪张
    --导致亮倒返回多了一张牌(最后要打的牌)
    --这里刷新的时候如果亮倒了,就删掉那张多余的牌
    --还要过滤亮倒的牌不能打
    if self._shouldOutOne then
        local handCardsData = self._handCards[wViewId]
        for k,v in pairs(handCardsData) do
            if v == cmd_data.cbOutCardData then
                table.remove(handCardsData, k)
                break
            end
        end
        --去掉扣牌
        local kouPai = self.kouPaiList[wViewId]
        if next(kouPai) then
            handCardsData = table_filter(handCardsData, kouPai, 3)
        end

        -- dump(handCardsData, "#GameLayer:onSubOutCard ==> handCardsData==")
        self._gameView._cardLayer:showUserTileMajong(wViewId, handCardsData)
        self._handCards[wViewId] = handCardsData

        self._shouldOutOne = nil
    end
    -------------------------------------如果亮倒了,还要打掉一张

    --移除胡牌提示
    self:setHuTipLayerVisible(false)

	return true
end

--发送扑克(抓牌)
function GameLayer:onSubSendCard(dataBuffer)
	-- print("发送扑克")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_SendCard, dataBuffer)
	 dump(cmd_data, "#GameLayer:onSubSendCard==")
	self.isMyProvideUser = false

	self.wCurrentUser = cmd_data.wCurrentUser
	local wCurrentViewId = self:SwitchViewChairID(self.wCurrentUser)
	self._gameView:gameSendCard(wCurrentViewId, cmd_data.cbCardData)

	self:PlaySound(cmd.RES_PATH.."sound/SEND_CARD.wav")

	self:SetGameClock(wCurrentViewId, cmd.IDI_OUT_CARD, self.cbTimeOutCard)

    local selfChairID = self:GetMeChairID()
    local isShowOut = self.cbHearStatus[selfChairID+1] ~= 0


    -- dump(self.cbHearStatus, "#self.cbHearStatus====")
	--如果我可以操作，显示操作栏
    -- print("GameLayer:onSubSendCard=====", cmd_data.cbActionMask, self.wCurrentUser, selfChairID, self.bTrustee, isShowOut)
	if GameLogic.WIK_NULL ~= cmd_data.cbActionMask and
        self.wCurrentUser == selfChairID and
        not self.bTrustee and
        not isShowOut   then

		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5), cc.CallFunc:create(function ()
			self._gameView:ShowGameBtn(cmd_data.cbActionMask)
	end)))
		--self._gameView:ShowGameBtn(cmd_data.cbActionMask)
		self._gameView.cbActionCard = cmd_data.cbCardData
		self.cbActionMask = cmd_data.cbActionMask
		self.cbActionCard = cmd_data.cbCardData
	end

    local chairId = self:GetMeChairID()
    local status = self.cbHearStatus[chairId+1]
    local isTingOut = status and status ~= 0 --亮倒

    if not isTingOut then
        --轮到自己操作,将不能打打牌置灰色
        self._gameView._cardLayer:setBanListGray()
    end

        
	return true
end

function GameLayer:isTingOut(chairid)
    local status = self.cbHearStatus[chairid+1]
    local isTingOut = status and status ~= 0 --亮倒
    return isTingOut
end

--操作提示
function GameLayer:onSubOperateNotify(dataBuffer)
	print("操作提示")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_OperateNotify, dataBuffer)

    --没有托管才显示
    if not self.bTrustee then
        self._gameView:ShowGameBtn(cmd_data.cbActionMask)
    else
        self._gameView:ShowGameBtn(GameLogic.WIK_NULL)
    end
	self._gameView.cbActionCard = cmd_data.cbActionCard

	self.cbActionMask = cmd_data.cbActionMask
	self.cbActionCard = cmd_data.cbActionCard

	local wProvideUserViewId = self:SwitchViewChairID(cmd_data.wProvideUser)
	self:SetGameClock(wProvideUserViewId, cmd.IDI_OPERATE_CARD, self.cbTimeOutCard)
    print("GameLayer:onSubOperateNotify===============", wProvideUserViewId, cmd.MY_VIEWID)
    --轮到谁来操作
	if wProvideUserViewId == cmd.MY_VIEWID then
		self.isMyProvideUser = true
	end

    local chairId = self:GetMeChairID()
    local status = self.cbHearStatus[chairId+1]
    local isTingOut = status and status ~= 0 --亮倒

    if not isTingOut then
        --轮到自己操作,将不能打打牌置灰色
        self._gameView._cardLayer:setBanListGray()
    end
	return true
end

--胡牌提示
function GameLayer:onSubHuNotify(dataBuffer)
	print("胡牌数据")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_HuCard, dataBuffer)
	-- --胡牌用户位置
	-- local wViewChairId = self:SwitchViewChairID(cmd_data.wListenUser)
	-- --胡牌数据
	-- self.cbHuCardList = {}
	-- for i = 1, cmd_data.cbHuCardCount do
	-- 	self.cbHuCardList[wViewChairId][i] = cmd_data.cbHuCardData[1][i]
	-- end

    --亮倒胡口牌禁止出
    --除非手上都是别人要胡的牌
    self._ban_out_list = self._ban_out_list or {}

    local huList = cmd_data.cbHuCardData[1]
    local chairId = cmd_data.wUser
    local viewid = self:SwitchViewChairID(chairId)

    local huValues = {}
    for k,v in pairs(huList) do
        if v > 0 then
            self._ban_out_list[v] = true
            table.insert(huValues, v)
        end
    end

    self._gameView._cardLayer:setBanOutList(self._ban_out_list)
    self._gameView:showHuTipStatic(viewid, huValues)
	return true
end

function GameLayer:getBanOutList()
    return self._ban_out_list
end

function GameLayer:findPenCard(viewId, cardValue)
    if cardValue and cardValue>0 then
        local cardDatas = self._gameView._cardLayer.cbActiveCardData[viewId]
        for k,v in pairs(cardDatas) do
            if v.cbCardValue[1] == cardValue then
                return true
            end
        end
    end
    return false
end

--操作结果
function GameLayer:onSubOperateResult(dataBuffer)
	print("操作结果")

	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_OperateResult, dataBuffer)
	-- dump(cmd_data, "CMD_S_OperateResult")
	if cmd_data.cbOperateCode == GameLogic.WIK_NULL then
		assert(false, "没有操作也会进来？")
		return true
	end
	self.isMyProvideUser = false

	local wOperateViewId = self:SwitchViewChairID(cmd_data.wOperateUser)
	local wProvideViewId = self:SwitchViewChairID(cmd_data.wProvideUser)

    --别人操作了，移除操作栏
    --亮倒了如果自动碰,杠,也要移除操作按钮
    -- if wOperateViewId ~= cmd.MY_VIEWID then
        self._gameView:ShowGameBtn(GameLogic.WIK_NULL)
    -- end

	local tagAvtiveCard = {}
	tagAvtiveCard.cbType = GameLogic.SHOW_NULL
	print("操作的麻将玩家", wOperateViewId, wProvideViewId)
	if cmd_data.cbOperateCode == GameLogic.WIK_GANG then
        --dump(self._gameView._cardLayer.cbActiveCardData[wOperateViewId], "#查找已经碰杠的牌==")
        --print("查找已经碰杠的牌====牌值", cmd_data.cbOperateCard[1][1] )

        local isPen = self:findPenCard(wOperateViewId, cmd_data.cbOperateCard[1][1])
        -- print("查找已经碰杠的牌====结果:", isPen)
		--判断杠类型
		if wOperateViewId == wProvideViewId and not isPen then  --暗杠
            tagAvtiveCard.cbType = GameLogic.SHOW_AN_GANG
        else
            -- dump(self._gameView._cardLayer.cbActiveCardData[wOperateViewId], "#查找已经碰杠的牌==")
            for i=1,#self._gameView._cardLayer.cbActiveCardData[wOperateViewId] do
                --查找是否已经碰
                local activeInfo = self._gameView._cardLayer.cbActiveCardData[wOperateViewId][i]
                if activeInfo.cbCardValue[1] == cmd_data.cbOperateCard[1][1] then --有碰
                    tagAvtiveCard.cbType = GameLogic.SHOW_MING_GANG
                end
            end
            if tagAvtiveCard.cbType == GameLogic.SHOW_NULL then
                tagAvtiveCard.cbType = GameLogic.SHOW_FANG_GANG
            end
        end
		tagAvtiveCard.cbCardNum = 4
		tagAvtiveCard.cbCardValue = cmd_data.cbOperateCard[1]
		--再加一个
		tagAvtiveCard.cbCardValue[4] = cmd_data.cbOperateCard[1][1]
		print("操作的麻将信息", tagAvtiveCard.cbType, tagAvtiveCard.cbCardNum, tagAvtiveCard.cbCardValue[1])
	end

	if cmd_data.cbOperateCode == GameLogic.WIK_PENG then
		tagAvtiveCard.cbType = GameLogic.SHOW_PENG
		tagAvtiveCard.cbCardNum = 3
		tagAvtiveCard.cbCardValue = cmd_data.cbOperateCard[1]
		--再加一个
		tagAvtiveCard.cbCardValue[3] = cmd_data.cbOperateCard[1][1]
	end
	-- 显示操作动作
	local jsonStr = cjson.encode(tagAvtiveCard)
    LogAsset:getInstance():logData(jsonStr, true)
	self._gameView._cardLayer:createActiveCard(wOperateViewId, tagAvtiveCard, wProvideViewId)

    --如果是亮倒了的玩家,要把扣住的牌刷一下

	self._gameView:showOperateAction(wOperateViewId, cmd_data.cbOperateCode)
	self:playCardOperateSound(cmd_data.wOperateUser, false, cmd_data.cbOperateCode)

	self:SetGameClock(wOperateViewId, cmd.IDI_OUT_CARD, self.cbTimeOutCard)

    --碰/杠听牌
    if cmd_data.wOperateUser == self:GetMeChairID() and bit:_and(cmd_data.cbActionMask, GameLogic.WIK_LISTEN) ~= 0 then
        self._gameView:ShowGameBtn(GameLogic.WIK_LISTEN)
    end
    if cmd_data.wOperateUser == self:GetMeChairID() and cmd_data.cbOperateCode == GameLogic.WIK_PENG then
        self._gameView._cardLayer:sortHandCardOutCardMode()
    end

    --亮倒的了的玩家要删除手上摸到的杠牌
    local status = self.cbHearStatus[cmd_data.wOperateUser+1]
    local isTingOut = status and status ~= 0 --亮倒
    if cmd_data.wOperateUser == self:GetMeChairID() and isTingOut then
        --直接删除这张牌,不用等动画,
        --由于可能删错牌(有相同的牌,无法区分哪张是发到手的牌,而且还分站着和亮倒的牌)直接刷新一次亮倒牌
        -- self:removeMoPai(viewid)
        self._gameView._cardLayer:sortHandCard(cmd.MY_VIEWID)
    end

	return true
end

--用户亮倒
function GameLayer:onSubListenCard(dataBuffer)
	print("用户亮倒")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Listen, dataBuffer)
    local cbSelectCount = cmd_data.cbSelectCount --亮倒了几张
    local cbSelectCard = cmd_data.cbSelectCard[1] --亮到了哪些牌
    local sid = cmd_data.wChairId
    local viewid = self:SwitchViewChairID(sid)
    local cardIdxList = cmd_data.cbCardIndex[1]

    local cards = {}
    for i=1,cbSelectCount,3 do
        local cardvalue = cbSelectCard[i]
        if cardvalue and cardvalue>0 then
            table.insert(cards, cardvalue)
        end
    end

    --------------------------------------------------保存亮倒玩家的手牌,亮倒还要打一张
    local handCardsData = {}
    for k,v in pairs(cardIdxList) do
        for i=1,v do
            table.insert(handCardsData, GameLogic.LocalCardData[k])
        end
    end

    self.kouPaiList[viewid] = clone(cards)
    self._handCards = {}
    self._handCards[viewid] = handCardsData
    --------------------------------------------------保存亮倒玩家的手牌,亮倒还要打一张

    self.cbHearStatus[sid+1] = 3 --标识该玩家听牌了
    self._shouldOutOne = true
    self._gameView._cardLayer:createKouPai(viewid, cards) --创建扣倒牌
    self._gameView:showShowOutTip(viewid)
	return true
end

--用户听牌数据
function GameLayer:onSubListenCardData(dataBuffer)
	print("听牌提示=================================================================")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_TING_DATA, dataBuffer)
	--dump(cmd_data, "CMD_S_TING_DATA")
	self._gameView._cardLayer.cbListenList = {}
	for i = 1, cmd_data.cbOutCardCount do
		self._gameView._cardLayer.cbListenList[i] = cmd_data.cbOutCardData[1][i]
		print("玩家听牌需要打的字", self._gameView._cardLayer.cbListenList[i])
		self._gameView._cardLayer.cbHuCard[i] = {}
		self._gameView._cardLayer.cbHuCardLeft[i] = {}
		self._gameView._cardLayer.cbHuCardFan[i] = {}
		for j = 1, cmd_data.cbHuCardCount[1][i] do
			self._gameView._cardLayer.cbHuCard[i][j] = cmd_data.cbHuCardData[i][j]
			self._gameView._cardLayer.cbHuCardLeft[i][j] = cmd_data.cbHuCardRemainingCount[i][j]
			self._gameView._cardLayer.cbHuCardFan[i][j] = cmd_data.cbHuFan[i][j]
			print("玩家打完可以胡的字，剩余数，番数", self._gameView._cardLayer.cbHuCard[i][j], self._gameView._cardLayer.cbHuCardLeft[i][j], self._gameView._cardLayer.cbHuCardFan[i][j])
		end
	end
	return true
end

--用户托管
function GameLayer:onSubTrustee(dataBuffer)
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Trustee, dataBuffer)

             dump(cmd_data,"+++++++++++++用户托管",6)

	if cmd_data.wChairID == self:GetMeChairID() then
		self.bTrustee = cmd_data.bTrustee

                if self.bTrustee then
                    self._gameView:setTrustShadowVisible(true)
        	       self._gameView:ShowGameBtn(GameLogic.WIK_NULL)
                    print("托管了")
                else
                    self._gameView:setTrustShadowVisible(false)
                    print("取消托管了")
                end
	end

	return true
end

function GameLayer:getPlayerNum()
    -- local tid = self:GetMeTableID()
    -- local count = 0
    -- for i = 1, cmd.GAME_PLAYER do
    --     local item = self._gameFrame:getTableUserItem(self:GetMeTableID(), i - 1)
    --     if item then
    --         count = count + 1
    --     end
    -- end
    -- return count
    return self.cbPlayerCount
end

--回放需要设置人数
function GameLayer:setPlayerNum(num)
    self.cbPlayerCount = math.max(math.min(cmd.GAME_PLAYER, num), 2)
end

--游戏结束
function GameLayer:onSubGameConclude(dataBuffer)
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
	-- local jsonStr = cjson.encode(cmd_data)
 --   	LogAsset:getInstance():logData(jsonStr, true)
             self.m_bGameStart = false
             self.m_bGameEnd = true

	local bMeWin = false
	--先显示翻马界面
	--获取胡牌玩家
	local HuView = 0
	local HuChair = yl.INVALID_CHAIR

	for i = 1, cmd.GAME_PLAYER do
		local viewid = self:SwitchViewChairID(i - 1)
		if cmd_data.lGameScore[1][i] > 0 then
			HuView = viewid
			HuChair = i - 1

			--显示胡牌动画
			self._gameView:showOperateAction(viewid, GameLogic.WIK_CHI_HU)
                                      self:playCardOperateSound(HuChair, false, GameLogic.WIK_CHI_HU)
		end
	end

    local isWaste = cmd_data.wProvideUser == yl.INVALID_CHAIR
    local playerNum = self:getPlayerNum() --cmd.GAME_PLAYER
	--如果没人胡，则是荒庄
	-- if 0 ==  HuView and cmd_data.wFleeUser == yl.INVALID_CHAIR then
	-- 	--显示荒庄
	-- 	self._gameView:showNoWin(true)
 --        --桌子没坐满
 --        --显示玩家手牌
	-- 	for i = 1,playerNum do
	-- 		--手牌
	-- 		local cbCardData = {}
	-- 		local viewid = self:SwitchViewChairID(i - 1)
	-- 		for j = 1, cmd_data.cbCardCount[1][i] do
	-- 			cbCardData[j] = cmd_data.cbHandCardData[i][j]
	-- 		end
	-- 		--显示玩家手牌
	-- 		self._gameView._cardLayer:showUserTileMajong( viewid, cbCardData)
	-- 		--杠碰牌显示
	-- 		self._gameView._cardLayer:tileActiveCard(viewid)
	-- 	end
 --        if GlobalUserItem.bPrivateRoom and self.playMode~=1 then
 --            self._gameView.userPoint:setVisible(false)
 --        else
 --            self:SetGameClock(self:SwitchViewChairID(self:GetMeChairID()), cmd.IDI_START_GAME, self.cbTimeStartGame)
 --        end
	-- 	return
	-- end

    if isWaste then
        local dataList = {}
        for i = 1, playerNum do
            if self.m_cbPlayStatus[i] == 1 then
                local item = self.m_sparrowUserItem[i]
                local data = {item.szNickName or "", cmd_data.lGangScore[1][i]}
                table.insert(dataList, data )
            end
        end

        local uiWaste = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiWaste")
        local wasteObj = uiWaste:ctor(self._gameView)
        wasteObj:refresh(dataList)
        wasteObj:setCallbackAfterDestroy(function() 
            self.endLayer = nil
            self._gameView:showFinalResult() 
            self.m_bGameEnd = false
            end)
        self.endLayer = uiWaste
    end

        if GlobalUserItem.bPrivateRoom then
            if PriRoom:getInstance().m_tabPriData.dwPlayCount < PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then
                print("荒庄开始游戏======")
                if not self._gameView.objFinalReport then
                    self._gameView.btStart:setVisible(true)
                else
                    self._gameView.btStart:setVisible(false)
                end
            else
                self._gameView.btStart:setVisible(false)
            end
        else
            self._gameView.btStart:setVisible(true)
        end

	local maNum = cmd_data.cbMaCount
	local validMaNum = cmd_data.cbMaFan
	local maData = {}
	for i = 1, maNum do
		maData[i] = cmd_data.cbMaData[1][i]
	end
	local validMaData = {}
	for i=1,validMaNum do
		validMaData[i] = cmd_data.cbZhongMaData[1][i]
	end

	local resultList = {}
	for i = 1, playerNum do
		local viewid = self:SwitchViewChairID(i - 1)
                          if self.m_cbPlayStatus[i] == 1 then 
                		local lScore = cmd_data.lGameScore[1][i]
                		local lHuScore = cmd_data.lPiaoScore[1][i]--cmd_data.lHuScore[1][i]
                		if lHuScore > 0 and viewid == cmd.MY_VIEWID then
                			bMeWin = true
                		end

                		local result = {}
                		result.userItem = self._gameFrame:getTableUserItem(self:GetMeTableID(), i - 1)

                		if nil == result.userItem then
                			result.userItem = self._gameView.m_tabUserHead[viewid].m_userItem
                		end
                		result.lScore = lScore
                		result.lHuScore = lHuScore
                		result.lGangScore = cmd_data.lGangScore[1][i]
                		result.lMaScore = cmd_data.lMaScore[1][i]
                		result.cbChHuKind = cmd_data.dwChiHuKind[1][i] --cmd_data.cbChiHuKind[1][i]
                		--胡牌类型
                		result.dwChiHuRight = {}
                		for j=1,cmd.MAX_RIGHT_COUNT do
                                                local dwChiHuRightPlayer = cmd_data.dwChiHuRight[i] or {}
                			result.dwChiHuRight[j] = dwChiHuRightPlayer[j] or 0
                		end

                		result.cbCardData = {}
                		--手牌
                		for j = 1, cmd_data.cbCardCount[1][i] do
                			result.cbCardData[j] = cmd_data.cbCardData[i][j]  --cmd_data.cbHandCardData[i][j]
                		end

                                        --没胡牌的不用
                                        if HuChair == self:GetMeChairID() then
                                            --只需要13张,末尾再补一张牌
                                            local whichOne = table.keyof(result.cbCardData, cmd_data.cbProvideCard)
                                            table.remove(result.cbCardData, whichOne)

                                    		--如果是我自摸
                                    		if HuChair == i-1 and 0 ~= cmd_data.cbProvideCard then
                                    			table.insert(result.cbCardData, cmd_data.cbProvideCard)
                                    		end
                                        end

                                        --删除发牌之后的延迟操作按钮显示(胡,弃牌)
                                        self:stopAllActions()
                                        --清发牌
                                        self._gameView:stopAllActions()
                                        --清发牌
                                        self._gameView._cardLayer:clearSendCardChildren()
                                        --删除扣牌
                                        self._gameView._cardLayer:removeKouPai()
                                        --删掉发的牌
                                        self._gameView._cardLayer:clearSendCardChildren()
                                		--显示玩家手牌
                		self._gameView._cardLayer:showUserTileMajong( viewid, result.cbCardData )
                		--杠碰牌显示
                		self._gameView._cardLayer:tileActiveCard(viewid)

                                        --先设置碰杠出的牌
                                        if cmd_data.cbWeaveItemCount[1][i] > 0 then
                                            local tabActiveCardData = {}
                                            for j = 1, cmd_data.cbWeaveItemCount[1][i] do
                                                local cbOperateData = {} --此处为tagAvtiveCard
                                                cbOperateData.cbCardValue = cmd_data.WeaveItemArray[i][j].cbCardData[1]
                                                -- dump(cbOperateData.cbCardValue, "#已经出的牌")
                                                local nShowStatus = GameLogic.SHOW_NULL
                                                local cbParam = cmd_data.WeaveItemArray[i][j].cbParam
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
                                                local wProvideViewId = self:SwitchViewChairID(cmd_data.WeaveItemArray[i][j].wProvideUser)
                                                cbOperateData.wProvideViewId = wProvideViewId

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
	end

	--显示结算框
	local delayTime = maNum *1 + cmd.OPERATE_TIME
	if cmd_data.wFleeUser ~= yl.INVALID_CHAIR then
		delayTime = 0
	end

    local function showResultCallback()
        print("买马回调===================called!!!")
        self:showResultLayer(cmd_data)
    end

    if 0 ~= HuView and maNum > 0 then
        -- self._gameView:showMoMaAction(self.wBankerUser, HuChair, HuView, maNum, maData, validMaNum, validMaData, self.cbTimeWaitEnd, showResultCallback)
        self._gameView:showMoMaAction(self.wBankerUser, HuChair, HuView, maNum, maData, validMaNum, validMaData, nil, showResultCallback)
    else
        if not isWaste then
            showResultCallback()
        end
    end
	--播放音效
	if bMeWin then
		self:PlaySound(cmd.RES_PATH.."sound/ZIMO_WIN.wav")
	else
		self:PlaySound(cmd.RES_PATH.."sound/ZIMO_LOSE.wav")
	end

    self.bTrustee = false
    self._gameView:setTrustShadowVisible(false)
    self._gameView:ShowGameBtn(GameLogic.WIK_NULL)

	if GlobalUserItem.bPrivateRoom and self.playMode~=1 then
		self._gameView:setClockVisible(false)
	else
		self:SetGameClock(self:SwitchViewChairID(self:GetMeChairID()), cmd.IDI_START_GAME, self.cbTimeStartGame)
	end

	return true
end

--显示胡牌提示
function GameLayer:setHuTipLayerVisible(visible, cardValue)
    if visible then
        local huList = self:getHuListData()
        -- dump(huList, "#huList==")
        -- print("cardValue==", cardValue)
        local data = huList[cardValue]
        local refreshDataList = {}

        if data then
            for k,v in ipairs(data.huList) do
                table.insert(refreshDataList, {v, data.leftCountList[k], data.multiList[k]})
            end

            -- dump(refreshDataList, "#refreshDataList==")
            self._gameView:showHuTipLayer(refreshDataList)
        end
    else
        self._gameView:removeHuTipLayer()
    end
end

function GameLayer:showResultLayer(cmd_data)
    local parent = nil
    local dataList = {}
    local playerNum = self:getPlayerNum()
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
    local huStrType = cmd_data.dwChiHuRight[1] --胡牌类型


    dataList.desCardType = ""
    dataList.maPai = {}
    for k,v in ipairs(cbMaData) do
        if v > 0 then
            table.insert(dataList.maPai, v)
        end
    end

    for playerIdx=1,playerNum do

        if self.m_cbPlayStatus[playerIdx] == 1 then 
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
            playerData.isSelf = (playerIdx-1) == self:GetMeChairID()

            if playerData.isHuPai then
                playerData.cardTypeStr = self:getCardTypeStr(huStrType[playerIdx])
            end

            local item = self.m_sparrowUserItem[playerIdx]
            playerData.useritem = clone(item)
            table.insert(dataList, playerData)
        end
    end

    self._gameView:showResultLayer(parent, dataList)
end

function GameLayer:getCardTypeStr(typeValue)
    local kwxCardType = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.kwxCardType")
    local isQingYiSe = bit.band(GameLogic.CHR_QING_YI_SE, typeValue) ~= 0
    local tittle = ""

    for k,v in ipairs(kwxCardType) do
        if v.type ~= GameLogic.CHR_QING_YI_SE and bit.band(v.type, typeValue) ~= 0 then
            if isQingYiSe then
                tittle = string.format("%s 清一色%sx%s", tittle, v.typeName, v.fan)
            else
                tittle = string.format("%s %sx%s", tittle, v.typeName, v.fan)
            end
        end
    end

    if string.len(tittle) <= 0 and isQingYiSe then
        for k,v in ipairs(kwxCardType) do
            if bit.band(v.type, GameLogic.CHR_QING_YI_SE) ~= 0 then
                return string.format("%sx%s", v.typeName, v.fan)
            end
        end
    end

    return tittle
end

function GameLayer:onUserBeginPiao(dataBuffer)

    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_BeginPiao, dataBuffer)

    self:OnResetGameEngine()
    self.m_bGameStart = true
    self._gameView.btStart:setVisible(false)
    self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1])

    if GlobalUserItem.bPrivateRoom and self.playMode~=1 then
        self._gameView:setClockVisible(false)
    else
        self._gameView:setClockVisible(true)
        self._gameView:OnUpdataClockTime(self.cbTimeCallPiao)
        self:SetGameClock(self:SwitchViewChairID(self:GetMeChairID()), cmd.IDI_PIAO_CARD, self.cbTimeCallPiao)
    end

    --self._gameView:setClockVisible(false)
    self._gameView:showSitBtn(false)
    if not self.m_bLookOn then 
        self._gameView:setPiaoVisible(true)
    end
    --准备不显示
    for i=1,cmd.GAME_PLAYER do
        self._gameView:showUserState(i, false)
        self._gameView:setTrustShadowVisible(false)
    end

    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
            self._gameView._priView:onRefreshInfo()
        end
    end

end


function GameLayer:onUserCallPiao(dataBuffer)

    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_CallPiao, dataBuffer)

    local wMyChairId = self:GetMeChairID()
    if wMyChairId == cmd_data.wUser then 
        self._gameView:setPiaoVisible(false)
    end

end


--自己听牌
function GameLayer:onListenDataNotify(dataBuffer)
    local ting_data = ExternalFun.read_netdata(cmd.SUB_S_TING_DATA, dataBuffer)
    self._ting_data = ting_data
    self._can_ting_list = {} --出哪几张可以听牌
    for k,v in pairs(ting_data.cbOutCardData[1]) do
        if v > 0 then
            self._can_ting_list[v] = true
        end
    end
    --dump(self._can_ting_list,"==============888888888888")
    --胡牌列表(打哪张,胡哪些牌,剩余多少张,多少番)
    self._hu_list = {}
    for idx,outCardValue in ipairs(ting_data.cbOutCardData[1]) do
        if outCardValue > 0 then
            local huList = {}
            local leftCountList = {}
            local multiList = {}
            local remainList = ting_data.cbHuCardRemainingCount[idx]
            local cbHuFanList = ting_data.cbHuFan[idx]

            for huIdx,value in ipairs(ting_data.cbHuCardData[idx]) do
                if value > 0 then
                    table.insert(huList, value)
                    table.insert(leftCountList, remainList[huIdx])
                    table.insert(multiList, cbHuFanList[huIdx])
                end
            end
            local data = {}
            data.huList = huList
            data.leftCountList = leftCountList
            data.multiList = multiList
            self._hu_list[outCardValue] = data
        end
    end

    --哪些牌能扣倒(扣倒逻辑跟胡牌逻辑相互关联,扣倒的牌不能影响胡牌)
    --扣倒的牌就不能再打了,只能选择其他的胡口
    self._can_kou_list = {}
    for idx,v in ipairs(ting_data.cbSelectCardData[1]) do
        if v > 0 then
            self._can_kou_list[v] = true
        end
    end
end

function GameLayer:onSubGang(dataBuffer)
    local gangData = ExternalFun.read_netdata(cmd.CMD_S_GangCard, dataBuffer)
    local sid = gangData.wChairId
    local gangScore = gangData.cbGangScore[1]

    for i=1,cmd.GAME_PLAYER do
        -- local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        -- if userItem then
            local wViewChairId = self:SwitchViewChairID(i-1)
            local userItem = self._gameView:getUserItemByViewId(wViewChairId)
            if userItem then
                userItem.lScore = userItem.lScore + gangScore[i]
                self._gameView:OnUpdateUser(wViewChairId, userItem)
            end
        -- end
    end
end

--自己可以听哪些牌
function GameLayer:getTingData()
    return self._can_ting_list
end
--自己听哪些牌(剩多少张,多少番)
function GameLayer:getHuListData()
    return self._hu_list
end

--自己能扣哪些牌
function GameLayer:getCanKouListData()
    return self._can_kou_list
end

function GameLayer:grabRule(ruleStruct)
    local piaoStr = {"不漂", "叫漂", "叫漂"}
    local maModeStr = {"不买马", "自摸买马", "亮倒自摸买马"}
    local maNumStr = {"买1马", "买6马", "买一送一"}
    local chnnl = {"全频道", "半频道"}

    local ruleText
    if ruleStruct.cbMaMode and ruleStruct.cbMaMode > 0 then
        ruleText = string.format("%s %s %s", piaoStr[ruleStruct.cbPiaoMode+1], maModeStr[ruleStruct.cbMaMode+1], maNumStr[ruleStruct.cbMaCount], chnnl[ruleStruct.cbRule+1])
    else
        ruleText = string.format("%s %s %s", piaoStr[ruleStruct.cbPiaoMode+1], maModeStr[ruleStruct.cbMaMode+1], chnnl[ruleStruct.cbRule+1])
    end

    return ruleText
end


--游戏记录（房卡）
function GameLayer:onSubGameRecord(dataBuffer)
	print("游戏记录")
	local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RecordInfo, dataBuffer)
	-- dump(cmd_data, "#CMD_S_Record")
	    --全部成绩

	self.m_userRecord = {}
	local nInningsCount = cmd_data.nCount
	for i = 1, cmd.GAME_PLAYER do
		self.m_userRecord[i] = {}
                          self.m_userRecord[i].cbZiMo = cmd_data.cbZiMo[1][i]
                          self.m_userRecord[i].cbDianPao = cmd_data.cbDianPao[1][i]
		self.m_userRecord[i].cbHuCount = cmd_data.cbJiePao[1][i]
		self.m_userRecord[i].cbMingGang = cmd_data.cbMingGang[1][i]
		self.m_userRecord[i].cbAnGang = cmd_data.cbAnGang[1][i]
        -- self.m_userRecord[i].cbMaCount = cmd_data.cbMaCount[1][i]
		self.m_userRecord[i].lAllScore = cmd_data.lAllScore[1][i]
	end

    local playerNum = self:getPlayerNum()
    local detailScore = cmd_data.lDetailScore
    local playerScoreList = {}
    for roundIdx=1,nInningsCount do
        local data = {}
        for playerIdx=1,playerNum do
            table.insert(data, detailScore[playerIdx][roundIdx])
        end
        table.insert(playerScoreList, data)
    end
    self._gameView:updataUserInfo()
    self._gameView:updataScoreList(playerScoreList)
end

--*****************************    普通函数     *********************************--
--播放麻将数据音效（哪张）
function GameLayer:playCardDataSound(chairID, cbCardData)
	local strGender = ""
	if self._gameFrame:getTableUserItem(self:GetMeTableID(), chairID) and self._gameFrame:getTableUserItem(self:GetMeTableID(), chairID).cbGender == 1 then
		strGender = "1"
	else
		strGender = "0"
	end
	local color = {"0_", "1_", "2_", "3_"}
	local nCardColor = math.floor(cbCardData/16) + 1
	local nValue = math.mod(cbCardData, 16)

	local strFile = cmd.RES_PATH.."sound/"..strGender.."/"..color[nCardColor]..nValue..".wav"
	self:PlaySound(strFile)
end
--播放麻将操作音效
function GameLayer:playCardOperateSound(chairID, bTail, operateCode)
	assert(operateCode ~= GameLogic.WIK_NULL)

	local strGender = ""
	if self._gameFrame:getTableUserItem(self:GetMeTableID(), chairID)  and self._gameFrame:getTableUserItem(self:GetMeTableID(), chairID).cbGender == 1 then
		strGender = "1"
	else
		strGender = "0"
	end
	local strName = ""
	if bTail then
		strName = "REPLACE.wav"
	else
		if operateCode >= GameLogic.WIK_CHI_HU then
			strName = "action_64.wav"
		elseif operateCode == GameLogic.WIK_LISTEN then
			strName = "TING.wav"
		elseif operateCode == GameLogic.WIK_GANG then
			strName = "action_16.wav"
		elseif operateCode == GameLogic.WIK_PENG then
			strName = "action_8.wav"
		elseif operateCode <= GameLogic.WIK_RIGHT then
			strName = "action_1.wav"
		end
	end
	local strFile = cmd.RES_PATH.."sound/"..strGender.."/"..strName
	self:PlaySound(strFile)
end
--播放随机聊天音效
function GameLayer:playRandomSound(viewId)
	local strGender = ""
	if self.cbGender[viewId] == 1 then
		strGender = "BOY"
	else
		strGender = "GIRL"
	end
	local nRand = math.random(25) - 1
	if nRand <= 6 then
		local num = 6603000 + nRand
		local strName = num..".wav"
		local strFile = cmd.RES_PATH.."sound/PhraseVoice/"..strGender.."/"..strName
		self:PlaySound(strFile)
	end
end

function GameLayer:getDetailScore()
	return self.m_userRecord
end

function GameLayer:getListenPromptOutCard()
	return self.cbListenPromptOutCard
end

function GameLayer:getListenPromptHuCard(cbOutCard)
	if not cbOutCard then
		return nil
	end

	for i = 1, #self.cbListenPromptOutCard do
		if self.cbListenPromptOutCard[i] == cbOutCard then
			assert(#self.cbListenCardList > 0 and self.cbListenCardList[i] and #self.cbListenCardList[i] > 0)
			return self.cbListenCardList[i]
		end
	end

	return nil
end

--*****************************    发送消息     *********************************--
--开始游戏
function GameLayer:sendGameStart()
             if not self.m_bLookOn then 
	       self:SendUserReady()
             end 
	self:OnResetGameEngine()
end

--发送请求流水
function GameLayer:onSendQueryRecord()
    print("发送请求流水:onSendQueryRecord==")
    local dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_RECORD, dataBuffer)
end

--出牌
function GameLayer:sendOutCard(card)
	-- body
	print("发送出牌：", card)

	local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_OutCard)
	cmd_data:pushbyte(card)
	return self:SendData(cmd.SUB_C_OUT_CARD, cmd_data)
end
--操作扑克
function GameLayer:sendOperateCard(cbOperateCode, cbOperateCard)
    -- dump(cbOperateCard, "#GameLayer:sendOperateCard====================")
	print("发送操作提示：", cbOperateCode, table.concat(cbOperateCard, ","))
	assert(type(cbOperateCard) == "table")

	--听牌数据置空
    local cmd_data = CCmd_Data:create(4)
	cmd_data:pushbyte(cbOperateCode)
	for i = 1, 3 do
		cmd_data:pushbyte(cbOperateCard[i])
	end
	--dump(cmd_data, "operate")
	self:SendData(cmd.SUB_C_OPERATE_CARD, cmd_data)
end

--用户听牌
function GameLayer:sendUserListenCard(bListen)
	local cmd_data = CCmd_Data:create(1)
	cmd_data:pushbool(bListen)
	self:SendData(cmd.SUB_C_LISTEN_CARD, cmd_data)
end

--用户托管
function GameLayer:sendUserTrustee()
	local cmd_data = CCmd_Data:create(1)
	cmd_data:pushbool(not self.bTrustee)
	self:SendData(cmd.SUB_C_TRUSTEE, cmd_data)
end

--发送扑克
function GameLayer:sendControlCard(cbControlGameCount, cbCardCount, wBankerUser, cbCardData)
	local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_SendCard)
	cmd_data:pushbyte(cbControlGameCount)
	cmd_data:pushbyte(cbCardCount)
	cmd_data:pushword(wBankerUser)
	for i = 1, #cbCardData do
		cmd_data:pushbyte(cbCardData[i])
	end
	self:SendData(cmd.SUB_C_SEND_CARD, cmd_data)
end

function GameLayer:sendUserPiao(type)
    assert(type==0 or type==1 or type==2, "Invalid type!!")
    local cmd_data = CCmd_Data:create(1)
    cmd_data:pushbyte(type)
    self:SendData(cmd.SUB_C_CALLPIAO, cmd_data)
end

--亮倒扣牌
function GameLayer:sendUserShowOut(maskCards)
    local cmd_data = CCmd_Data:create(16)
    -- local cid = self:GetMeChairID()
    -- cmd_data:pushbyte(cid)
    cmd_data:pushbyte(1)
    for k,cardValue in ipairs(maskCards) do
        cmd_data:pushbyte(cardValue)
        cmd_data:pushbyte(cardValue)
        cmd_data:pushbyte(cardValue)
    end
    for i=#maskCards*3+1, cmd.MAX_COUNT do
        cmd_data:pushbyte(0)
    end

    --dump(maskCards, "#GameLayer:sendUserShowOut==")
    cmd_data:pushbyte(#maskCards*3)
    return self:SendData(cmd.SUB_C_LISTEN, cmd_data)
end

return GameLayer
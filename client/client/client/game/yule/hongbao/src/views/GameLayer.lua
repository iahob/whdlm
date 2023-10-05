--
-- Author: Tang
-- Date: 2016-10-11 17:22:09
--
local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.hongbao.src";
require("cocos.init")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = module_pre .. ".models.CMD_Game"
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"

local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local g_var = ExternalFun.req_var
local GameFrame = appdf.req(module_pre .. ".models.GameFrame")
local GameServer_CMD = appdf.req(appdf.HEADER_SRC.."CMD_GameServer")


function GameLayer:ctor( frameEngine,scene )

    print("GameLayer:ctor...........................................")
    ExternalFun.registerNodeEvent(self)
    self._dataModle = GameFrame:create()    
    GameLayer.super.ctor(self,frameEngine,scene)

    self.m_DispatchBagList = {}
    self.m_getUser = {}
    self.m_UserList = {}
    self.meGetBag = false

    self.myScore = 0    --记录玩家进入时的金币
    

     self._gameFrame:QueryUserInfo( self:GetMeUserItem().wTableID,yl.INVALID_CHAIR)
end

--创建场景
function GameLayer:CreateView()
     self._gameView = GameViewLayer:create(self)
     self:addChild(self._gameView)
     return self._gameView
end

function GameLayer:resetData()
    self.m_getUser = {}
    self.meGetBag = false
end

function GameLayer:getParentNode( )
    return self._scene
end

function GameLayer:getFrame( )
    return self._gameFrame
end

function GameLayer:getUserList()
    local userList = {}
    local id  = self._gameFrame:GetTableID() + 1
    if self._gameFrame._tableUserList[id]  then
        for k,userid in pairs(self._gameFrame._tableUserList[id]) do
            if userid then
                table.insert(userList,clone(self._gameFrame._UserList[userid]))
            end
        end
    end
    return userList
end

function GameLayer:sendNetData( cmddata )
    return self:getFrame():sendSocketData(cmddata)
end

function GameLayer:getDataMgr( )
    return self._dataModle
end

-- -- 根据椅子号获取玩家信息
-- function GameFrame:getUserByChair(userList,wchair)
--     local item

--     --dump(userList, "useritem is ======================== >", 6)
--     for k,v in pairs(userList) do
--         local userItem = v
    
--         if userItem.wChairID == wchair then

--             item = userItem
--             return item
--         end
--     end

--     return item
-- end

function GameLayer:logData(msg)
    if nil ~= self._scene.logData then
        self._scene:logData(msg)
    end
end

function GameLayer:readOnly(t)
    local _table = {}
    local mt = {
        __index = t,
        __newindex = function()
            error(" the table is read only ")
        end
    }
    setmetatable(_table, mt)
    return _table
  end

---------------------------------------------------------------------------------------
function GameLayer:onExit()
    print("gameLayer onExit()...................................")
    self:KillGameClock()
    self:dismissPopWait()
end

--退出桌子
function GameLayer:onExitTable()
    self:KillGameClock()
    self:showPopWait()

    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:runAction(cc.Sequence:create(
            cc.DelayTime:create(2),
            cc.CallFunc:create(
                function ()   
                    print("-----------onExitTable-----7777777777777-------------------")
                    self._gameFrame:StandUp(1)
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    --强制离开游戏(针对长时间收不到服务器消息的情况)
                    print("delay leave")
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
    self:getFrame():onCloseSocket()

    self._scene:onKeyBack()    
end

------网络发送
--发红包
function GameLayer:sendUserBet( lScore, lThunderPoint )
    print("发红包sendUserBet",lScore, lThunderPoint)
    if self.myScore/self.nRatio < lScore then
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, "金币不足", 1)
        end
        return
    end
    local cmddata = ExternalFun.create_netdata(g_var(cmd).CMD_C_DispatchBag)
    cmddata:pushscore(lScore)
    cmddata:pushscore(lThunderPoint)

    self:SendData(g_var(cmd).SUB_C_DISPATCH_BAG, cmddata)
end

--抢红包
function GameLayer:sendRobBanker()
    local lBagKey = self.m_lBagKey
    
    if lBagKey and lBagKey ~= 0 then
        local cmddata = ExternalFun.create_netdata(g_var(cmd).CMD_C_GetBag)
        cmddata:pushscore(lBagKey)
        self:SendData(g_var(cmd).SUB_C_GET_BAG, cmddata)
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

--申请存款
function GameLayer:sendSaveScore(lScore)
    --操作记录
    print("sendSaveScore lScore ==",lScore)
    local buffer = ExternalFun.create_netdata(GameServer_CMD.CMD_GR_C_SaveScoreRequest)
    buffer:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushbyte(GameServer_CMD.SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushscore(lScore)
    buffer:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)
    self:sendNetData(buffer)
end

--请求银行信息
function GameLayer:sendRequestBankInfo()
    local cmddata = CCmd_Data:create(67)
    cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    local str = md5(GlobalUserItem.szPassword) or "d1fd5495e7b727081497cfce780b6456"
    cmddata:pushstring(str,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end


function GameLayer:OnEventGameClockInfo(chair,clocktime,clockID)
    if nil ~= self._gameView  and self._gameView.UpdataClockTime then
        self._gameView:UpdataClockTime(clocktime)
    end
end

-- 设置计时器
function GameLayer:SetGameClock(chair,id,time,viewtype)

    GameLayer.super.SetGameClock(self,chair,id,time)
    -- if nil ~= self._gameView and nil ~= self._gameView.createClockView then
    --     self.m_cbLeftTime = time
    --     self._gameView:createClockView(time,viewtype)
    -- end
end
-------------------------------------------------------------------------------------
--银行 
function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    print(sub)
    self:dismissPopWait()

    if sub == g_var(game_cmd).SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureSuccess, dataBuffer)
        self.bank_success = cmd_table

        self._gameView:onBankSuccess()
        self:sendRequestBankInfo()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_FAILURE then

        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureFailure, dataBuffer)
        self.bank_fail = cmd_table

        self._gameView:onBankFailure()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_INFO then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureInfo, dataBuffer)
        self._gameView:onBankUserInfo(cmd_table)

    else
        print("unknow gamemessage sub is ==>"..sub)
    end
end

-------------------------------------------------------------------------------------场景消息

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
    print("场景数据:" .. cbGameStatus)

    self:KillGameClock()

    self._info = {}
dump(self._gameFrame._UserList,"self._gameFrame._UserList")


    -- self._gameView._bankerView:reloadData()

    --申请按钮状态
    -- self._gameView:SetApplyStatus(GameViewLayer.unApply)
    
    self._gameView.m_cbGameStatus = cbGameStatus;
    self.m_cbGameStatus = cbGameStatus;
	if cbGameStatus == g_var(cmd).GS_GAME_FREE	then                        --空闲状态
        self:onEventGameSceneStatus(dataBuffer);
	elseif cbGameStatus == g_var(cmd).GS_GAME_PLAY 	then
        self:onEventGameSceneStatus(dataBuffer);
	end
    self:dismissPopWait()
end

function GameLayer:onEventGameSceneFree(buffer)    --空闲 

end

function GameLayer:onEventGameSceneStatus(buffer)   

    self:resetData()
    self._gameView:resetGameView()
    self.m_UserList = self:getUserList()--{}--self._gameFrame._tableUserList[self._gameFrame:GetTableID() + 1]

    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_StatusPlay, buffer)
    dump(cmd_table, "--------------onEventGameSceneStatus--------------", 6)

    --断线重连 清空数据
    self.m_DispatchBagList = {}
    self._gameView:bankerViewReload()
    self._playTime = cmd_table.cbPlayTime
    self._playPlayerCount = cmd_table.cbPlayerCount
    self._dBagTimes = cmd_table.dBagTimes
    self.nRatio = cmd_table.nRatio
    print("self._playTime,self._playPlayerCount == ",self._playTime,self._playPlayerCount)
    self._gameView:setRoomInfo(self._playPlayerCount, self._dBagTimes, self.nRatio)

    self._bagMinScore = cmd_table.lScoreRangeMin
    self._bagMaxScore = cmd_table.lScoreRangeMax
    self._bagCondition = cmd_table.lDispatchCondition
    -- k = "lExChangeRadio"
    self.m_wBankerUser = cmd_table.wBankerUser
    
    self.m_lBagKey = cmd_table.lBagKey
    self.m_currBagScore = cmd_table.lScore

    --刷新玩家信息
    local userItem = self:GetMeUserItem()
    self.myScore = userItem.lScore
    self._gameView:updateScore(userItem.lScore)
    self._gameView:initSelfHead(userItem)

    if cmd_table.wBankerUser == 65535 then
        return
    end


    self:KillGameClock()
    self:SetGameClock(self:GetMeChairID(), 1, cmd_table.cbLeftTime,1)

    self._gameView:updateBankerInfo(cmd_table)

    for k, chairID in pairs(cmd_table.wGetUser[1]) do
        if chairID >= 0 and chairID < 65535 then
            local item = self:getUserByChair(chairID)
            table.insert(self.m_getUser, item)
            if chairID == self:GetMeChairID() then
                self._gameView:showGetBagUser(item, cmd_table.cbLeftCount, cmd_table.dBagDetail)
                -- self._gameView:showBagOpen(cmd_table.dBagDetail)
            else
                self._gameView:showGetBagUser(item, cmd_table.cbLeftCount)
            end
        end
    end
end


--------------------------------------------------------------------------------------- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)  

    if self.m_bLeaveGame or nil == self._gameView then
        return
    end 

	if sub == g_var(cmd).SUB_S_START then  --游戏开始
		self:onSubGameStart(dataBuffer)
    elseif sub == g_var(cmd).SUB_S_DISPATCH_BAG then --派发红包
        self:onSubGameDispatchBag(dataBuffer)
    elseif sub == g_var(cmd).SUB_S_CANCEL_BAG then --取消红包
        self:onSubCancelBag(dataBuffer)
    elseif sub == g_var(cmd).SUB_S_GET_BAG then --抢红包
        self:onSubGameGetBag(dataBuffer)
    elseif sub == g_var(cmd).SUB_S_GET_BAG_FAIL then --抢包失败
        self:onSubGetBagFail(dataBuffer)
    elseif sub == g_var(cmd).SUB_S_END then --抢包结束 
        self:onSubGameEnd(dataBuffer)
	else

		print("unknow gamemessage sub is ==>"..sub)
	end
end

function GameLayer:onSubGameStart(dataBuffer) --游戏开始
    self:resetData()
    self._gameView:resetGameView()

    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_GameStart, dataBuffer)
    print("===============================游戏开始")

    self.m_wBankerUser = cmd_table.wBankerUser
    self.m_lBagKey = cmd_table.lBagKey -- 红包标志
    self.m_currBagScore = cmd_table.lScore

    for k,v in pairs(self.m_DispatchBagList) do
        if v.lBagKey == self.m_lBagKey then
            table.remove(self.m_DispatchBagList, k)
            break
        end
    end

    self._gameView:bankerViewReload()

    self:KillGameClock()
    self:SetGameClock(self:GetMeChairID(), 1, self._playTime,1)


    self._gameView:startGame(cmd_table)

end

function GameLayer:onSubGameDispatchBag( dataBuffer ) --用户发红包
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_DispatchBag, dataBuffer)
    dump(cmd_table,"用户发红包")

    if self:GetMeChairID() == cmd_table.wDispatchUser then
        -- 改变玩家分数
        self.myScore = self.myScore - cmd_table.lScore*self.nRatio
        print("==============================onSubGameDispatchBag",self.myScore)
        self._gameView:updateScore(self.myScore)
        self._gameView:showSendBagSuccess()
        -- local runScene = cc.Director:getInstance():getRunningScene()
        -- if nil ~= runScene then
        --     showToast(runScene, "埋雷成功", 1)
        -- end
    end

    local item = self:getUserByChair(cmd_table.wDispatchUser)
    local bankerItem ={
                        szNickName = item.szNickName,
                        lScore = cmd_table.lScore,
                        lBagKey = cmd_table.lBagKey
                    } 

    table.insert(self.m_DispatchBagList, bankerItem)
    table.sort( self.m_DispatchBagList, function(a,b) return a.lScore > b.lScore end )
    dump(self.m_DispatchBagList,"发红包的")
    self._gameView:bankerViewReload()
end

function GameLayer:onSubCancelBag( dataBuffer ) --删除离开用户发的红包
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_CancelBag, dataBuffer)

    for k, v in pairs(self.m_DispatchBagList) do
        if v.lBagKey == cmd_table.lBagKey then
            table.remove(self.m_DispatchBagList, k)
            break
        end
    end
    dump(self.m_DispatchBagList,"onSubCancelBag")
    self._gameView:bankerViewReload()
end


function GameLayer:onSubGameGetBag( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_GetBag, dataBuffer)
    dump(cmd_table,"onSubGameGetBag")
    local item = self:getUserByChair(cmd_table.wGetUser)
    if cmd_table.wGetUser == self:GetMeChairID() then
        self.meGetBag = true
    end
    self._gameView:showGetBagUser(item, cmd_table.cbLeftCount, cmd_table.fBagScore, cmd_table.fUserScore)
    table.insert(self.m_getUser, item)
    dump(self.m_getUser, "self.m_getUser")

end

function GameLayer:onSubGetBagFail(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_GetBag_Fail, dataBuffer)
    local text = {
        "分数不够赔付，不能抢包",
        "红包已被抢完",
        "重复抢包"
    }

    if self:GetMeChairID() == cmd_table.wGetUser then
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, text[cmd_table.cbReason] or "抢包失败", 2)
        end
    end
end

function GameLayer:onSubGameEnd( dataBuffer ) --游戏结束
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_GameEnd, dataBuffer)
    print("-------------游戏结束---------------")
    dump(cmd_table)
    -- dump(self.m_getUser,"self.m_getUser")

    if self.m_wBankerUser == self:GetMeChairID() then
        -- 改变玩家分数
        local remain = self.m_currBagScore
        -- for k,v in pairs(cmd_table.dBagScore[1]) do
        --     if not self.m_getUser[k] then break end
        --     if cmd_table.bThunderPoint[1][k] == false and self.m_getUser[k].wChairID ~= self.m_wBankerUser then
        --         remain = remain - v
        --         print("remain , v",remain , v)
        --     elseif cmd_table.bThunderPoint[1][k] == true and self.m_getUser[k].wChairID == self.m_wBankerUser then
        --         self.myScore = self.myScore + (cmd_table.dUserScore[1][k] + v)*self.nRatio
        --     end
        -- end
        -- print("onSubGameEnd self.myScore ", self.myScore,cmd_table.fBankerScore , remain )
        local fBankerScore = cmd_table.fBankerScore > 0 and cmd_table.fBankerScore or 0 

        self.myScore = self.myScore + (cmd_table.fBankerScore + remain)*self.nRatio
        print("self.myScore ", self.myScore )
        
        self._gameView:updateScore(self.myScore)--tonumber(string.format("%0.2f",self.myScore)))
    else
        for i = 1 , 10 do
            if self.m_getUser[i] ~= nil then

                if self.m_getUser[i].wChairID == self:GetMeChairID() then
                    print("i ==",i)
                    dump(cmd_table.dUserScore)
                    self.myScore = self.myScore + cmd_table.dUserScore[1][i]*self.nRatio
                    self._gameView:updateScore(self.myScore)--tonumber(string.format("%0.2f",self.myScore)))
                end
            end
        end
    end

    self._gameView:showEndInfo(cmd_table.fBankerScore,cmd_table.dBagScore[1], cmd_table.bThunderPoint[1], cmd_table.dUserScore[1])
    self.m_lBagKey = nil

end



--用户进入
function GameLayer:onEventUserEnter( wTableID,wChairID,useritem )
    print("add user " .. useritem.wChairID .. "; nick " .. useritem.szNickName)
    --缓存用户
    -- self._dataModle:insertUserList(useritem)
    if useritem.dwUserID == self:GetMeUserItem().dwUserID then
        -- self._gameView:updateScore(useritem)
        self._gameView:initSelfHead(useritem)
    end


    for k,v in ipairs(self.m_UserList) do
        if v.dwUserID == useritem.dwUserID then
            self.m_UserList[k] = useritem
            return
        end
    end
    table.insert(self.m_UserList, useritem)
end

--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    print("change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName)
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then
        -- self._dataModle:removeUserList(useritem)
        if useritem.dwUserID == self:GetMeUserItem().dwUserID then
            -- self._gameView:updateScore(useritem.lScore)
            self._gameView:initSelfHead(useritem)
        end

        for k,v in ipairs(self.m_UserList) do
            if v.dwUserID == useritem.dwUserID then
                table.remove(self.m_UserList, k)
            end
        end
    else
        --刷新用户信息
        -- self._dataModle:insertUserList(useritem)
        if useritem.dwUserID == self:GetMeUserItem().dwUserID then
            -- self._gameView:updateScore(useritem.lScore)
            self._gameView:initSelfHead(useritem)
        end

        for k,v in ipairs(self.m_UserList) do
            if v.dwUserID == useritem.dwUserID then
                self.m_UserList[k] = useritem
                return
            end
        end
        table.insert(self.m_UserList, useritem)
    end    
end

--用户分数
function GameLayer:onEventUserScore( item )
    print("用户分数  更新")
    dump(item, "onEventUserScore", 6)
    if item.dwUserID == self:GetMeUserItem().dwUserID then
        -- self._gameView:updateScore(item)
    end

    for k,v in ipairs(self.m_UserList) do
        if v.dwUserID == item.dwUserID then
            self.m_UserList[k].lScore = item.lScore
            return
        end
    end
end

--用户聊天
function GameLayer:onUserChat(chat, wChairId)

    -- if wChairId== self:GetMeUserItem().wChairID then  --过滤自己
    --     return
    -- end

    local useritem = self:getUserByChair(wChairId)
    if not useritem then
        return
    end

    self._gameView:userChat( wChairId, chat.szChatString)
end

--用户表情
function GameLayer:onUserExpression(expression, wChairId)

    -- if wChairId== self:GetMeUserItem().wChairID then  --过滤自己
    --     return
    -- end

    local useritem = self:getUserByChair(wChairId)
    if not useritem then
        return
    end
    
    self._gameView:onUserExpression(expression, wChairId)
end

function GameLayer:getUserByChair(wChairId)
    local item
    for k,v in pairs(self:getUserList()) do
        local userItem = v
    
        if userItem.wChairID == wChairId then

            item = userItem
            return item
        end
    end

    return item
end


return GameLayer
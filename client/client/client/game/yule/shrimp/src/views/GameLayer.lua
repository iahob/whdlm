--                            _ooOoo_  
--                           o8888888o  
--                           88" . "88  
--                           (| -_- |)  
--                            O\ = /O  
--                        ____/`---'\____  
--                      .   ' \\| |// `.  
--                      / \\||| : |||// \  
--                     / _||||| -:- |||||- \  
--                       | | \\\ - /// | |  
--                     | \_| ''\---/'' | |  
--                      \ .-\__ `-` ___/-. /  
--                   ___`. .' /--.--\ `. . __  
--                ."" '< `.___\_<|>_/___.' >'"".  
--               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
--                 \ \ `-. \_ __\ /__ _/ .-` / /  
--         ======`-.____`-.___\_____/___.-`____.-'======  
--                            `=---='  
--  
--         .............................................  
--                  佛祖保佑             永无BUG 
local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)

local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.GameViewLayer")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.GameViewLayer")
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local g_var = ExternalFun.req_var
local ControlLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.control.ControlLayer")
local RoomInfo = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.RoomInfo")


function GameLayer:onExit()
    self:KillGameClock()
    self:dismissPopWait()
    GameLayer.super.onExit(self)
end

-- 座位号
function GameLayer:setMyChairId( data )
    -- body
    local chid =  self:GetMeChairID()
    for i=1,#data do
        if data[i] == self:GetMeChairID() then 
            self.selfSeatIndex  = i
        end
    end
end

function GameLayer:getSeatId( chairID )
    -- body
    for k,v in pairs(self.gameUserChair) do
        if v == chairID then 
            return k
        end
    end

    return nil

end
 
--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)

    if self.isPriOver then
        return 
    end

    -- print("newstatus.cbUserStatus = ",newstatus.cbUserStatus)
    -- dump(useritem,"useritem",12)
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then
        -- 离开
        for i=1, cmd.GAME_PLAYER do
            if self.m_GameUserItem[i] and self.m_GameUserItem[i].dwUserID == useritem.dwUserID then
                    self.m_GameUserItem[i] = nil
            end
        end
        -- dump(useritem,"onEventUserStatus hide useritem",12)
        self.gameView.avatrPanel.hide(useritem,self.gameUserChair)
    else

        -- if GlobalUserItem.bPrivateRoom then
        --     return 
        -- end

        --print("改变状态")
        if (newstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            --print("tableID is error")
            return
        end
        self.m_GameUserItem[useritem.wChairID+1] = clone(useritem)
        -- dump(useritem,"onEventUserStatus useritem",12)
        self.gameView:updataDeskChair(useritem,self.gameUserChair)

    end    
end

--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)

    print("the table id is ================>"..tableid.." chairid==>"..chairid)

    if self.isPriOver then
        return 
    end

    -- ---用户信息
    if (tableid ~= self:GetMeUserItem().wTableID and not GlobalUserItem.bVideo) then
        return
    end

    if useritem then
        self.m_GameUserItem[useritem.wChairID+1] = clone(useritem)
    end

    self.gameView:updataDeskChair(useritem,self.gameUserChair)


    if self.gameStatus == cmd.GAME_STATUS_FREE then 
         self.gameView:showPreparation()
    end

end

--用户分数  g
function GameLayer:onEventUserScore( item )
    if item.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end
    -- dump(item,"onEventUserScore useritem",12)
    -- print("onEventUserScore ===================")
    for k,v in pairs(self.m_GameUserItem) do
        if v then
            if v.dwGameID == item.dwGameID then 
                -- dump(item,"用户分数",12)
                self.m_GameUserItem[k] = item
            end
        end
    end

    self.gameView:updataDeskChair(item,self.gameUserChair)
 
    -- self.gameView.avatrPanel.hide(item,self.gameUserChair)
    -- self._gameView:updateScore(self:SwitchViewChairID(item.wChairID))
end


--玩家服务器座位号转化为客户端座位号
function GameLayer:getLocalIndexForServerIndex(serverIndex)
    if serverIndex == nil then
        return -1
    end
    local selfSeatIndex = self.selfSeatIndex
    if selfSeatIndex == nil then
        return -1
    end
    -- (最大座位数 - 自己的真实座位号（当前玩家的服务器座位号） + 需要转换的真实座位号（玩家的服务器座位号）) % 最大座位数 + 1
    return ((11 - selfSeatIndex + serverIndex ) % 11 + 1)
end

--退出桌子
function GameLayer:onExitTable()
    self:KillGameClock()
    self:onExitRoom()
end

--离开房间
function GameLayer:onExitRoom()
    self._scene:onKeyBack()
end


--系统消息
function GameLayer:onSystemMessage( wType,szString )
    -- dump(wType,"wType",12)
    -- dump(szString,"szString",12)
    if wType == 1027 then 
        showToast(self._scene,szString,1)
        -- os.execute("pause")
    end

    if wType == 515 then 
        showToast(self._scene,szString,2)
    end
end


function GameLayer:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = 5
    --print("椅子数目", nChairCount)
    local nChairID = self:GetMeChairID()
    if nChairID==yl.INVALID_CHAIR then 
        nChairID=self.myChair
    end
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        viewid = math.mod(chair + math.floor(nChairCount * 3/2) - nChairID, nChairCount) + 1
    end
    return viewid
end
-- 初始化界面
function GameLayer:ctor(frameEngine,scene)
    GameLayer.super.ctor(self,frameEngine,scene)
    -- self:CreateView()
    self.isPriOver = false
    self.bankerid = nil
    self.isMyBanker = false
    self.gameUserChair = {}
    self.m_GameUserItem = {}
    self.gameStatus = cmd.GAME_STATUS_FREE
    -- self.m_cbGameStatus = self.gameStatus
    self.m_cbGameStatus = cmd.GAME_STATUS_FREE
    self.m_tabPrivateRoomConfig = {}

    self.bIsDoubleSice = false
    self.gameEndData = {}
    self.lApplyBankerCondition = 0
    self.bankerList = {}
    self.oneGameCount = true
    self.allCountData = {}
    self.isGameEnd = false
end

-- 创建场景
function GameLayer:CreateView()
    cc.FileUtils:getInstance():addSearchPath(device.writablePath..cmd.RES,true)
    self.gameView = GameViewLayer:create(self)
    self:addChild(self.gameView)

    -- 准备
    self:SendUserReady()

    self.bankerList = {}
    print("CreateView")
    return self.gameView
    
end

function GameLayer:getParentNode()
    return self._scene
end


-- 初始化游戏数据
function GameLayer:OnInitGameEngine()
    self.m_GameUserItem = {}
end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()

end

-- 设置计时器
function GameLayer:SetGameClock(chair,id,time)
    
end

-- 关闭计时器
function GameLayer:KillGameClock(notView)
    
end

--获得当前正在玩的玩家数量
function GameLayer:getPlayingNum()
    
end

-- 时钟处理
function GameLayer:OnEventGameClockInfo(chair,time,clockid)
     --房卡不托管
end

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)


    print("onEventGameScene == "..cbGameStatus)

    -- dump(dataBuffer,"CMD_S_StatusJetton",12)
    self.m_GameUserItem = {}
    
    --初始化已有玩家  
    local cmd_data = {}

    if cbGameStatus == cmd.GAME_STATUS_FREE then
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
        print("GAME_STATUS_FREE")
        self.gameStatus = cmd.GAME_STATUS_FREE
    elseif cbGameStatus == cmd.GAME_STATUS_PLAY then 
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusJetton, dataBuffer)
        print("CMD_S_StatusJetton")
        -- dump(cmd_data,"CMD_S_StatusJetton",12)
        self.gameStatus = cmd.GAME_STATUS_PLAY
        self.gameView:setDcieAniOver(false)
    elseif cbGameStatus == cmd.GAME_STATUS_END then
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusEnd, dataBuffer)
        print( "CMD_S_StatusEnd")
        self.gameStatus = cmd.GAME_STATUS_END
        
    end 
    self.m_cbGameStatus = cmd.GAME_STATUS_FREE
    dump(cmd_data,"cmd_data",12)
    if cmd_data.bIsPersonalRoom then 
        if cmd_data.bIsGoldPersonalRoom  then 
            GlobalUserItem.bPrivateGoldRoom = true
        else
            GlobalUserItem.bPrivateGoldRoom = false
        end
    end
    self.gameView:OnResetGameEngine()
    self.bankerid = cmd_data.wBankerUser
    self.isMyBanker = self.bankerid == self:GetMeChairID()
    if self.gameStatus ~= cmd.GAME_STATUS_END then
        self.countBankerId = self.bankerid
    end
    self.bankerList[#self.bankerList+1] = cmd_data.wBankerUser

    self.lApplyBankerCondition = cmd_data.lApplyBankerCondition
    self.bIsDoubleSice = cmd_data.bIsDoubleSice
    self.lAreaLimitScore = cmd_data.lAreaLimitScore

    self.lUserMaxScore = cmd_data.lUserMaxScore
    self.lAreaLimitScore = cmd_data.lAreaLimitScore

    self.m_GameUserItem[self:GetMeChairID()+1] = clone(self:GetMeUserItem()) 
    
    local tableId = self._gameFrame:GetTableID()
    for i = 1, cmd.GAME_PLAYER do
        if i ~= self:GetMeChairID()+1 then
            self._gameFrame:QueryUserInfo(tableId, i-1)
        end
    end

    self.gameUserChair = cmd_data.wDeskChairID[1]
    self.gameView:setGameModle()
    self.gameView.avatrPanel.removeAllHead()
    self.gameView:removeAllChip()
    self.gameView:updataDeskChair(clone(self:GetMeUserItem()),self.gameUserChair)
    self.gameView.avatrPanel.show(12,clone(self:GetMeUserItem()))
    self.gameView:showLastSice(cmd_data.cbLastSice[1])
    self.gameView:showBtnNum(cmd_data)
    self.gameView:showTimes(cmd_data.cbTimeLeave)
    self.gameView:intiBetlimit()

    self.gameView.notBanker:setVisible(false)
    if self.gameStatus == cmd.GAME_STATUS_FREE then 
        self.gameView:showPreparation()
        if self.bankerid >= 255 then 
            self.gameView.notBanker:setVisible(true)
        end
    elseif self.gameStatus == cmd.GAME_STATUS_PLAY then 
        self.gameView.betNumberLayer.showAllBetInfo(cmd_data.lAreaJettonScore[1],cmd_data.lUserBetScore[1])
    end
    -- self.m_cbGameStatus = self.gameStatus
    self.gameView:setBankerOperationpanel()
    self.gameView:hideOperateLayer()
    self:onRefreshInfo()
    self.gameView.btn_control:setVisible(cmd_data.bGameCheatUser)



    print("self:GetMeChairID ",self:GetMeChairID())
    dump(cmd_data,"onEventGameScene",12)


    self.m_tabPrivateRoomConfig.bIsDoubleSice = cmd_data.bIsDoubleSice
    self.m_tabPrivateRoomConfig.lUserMaxScore = cmd_data.lUserMaxScore
    self.m_tabPrivateRoomConfig.lAreaLimitScore = cmd_data.lAreaLimitScore
    self.m_tabPrivateRoomConfig.bIsPersonalRoom = cmd_data.bIsPersonalRoom
    self.m_tabPrivateRoomConfig.bIsGoldPersonalRoom = cmd_data.bIsGoldPersonalRoom  

    if cbGameStatus == cmd.GAME_STATUS_PLAY then
        self._gameView:setTableBtnIsOk(false)
    else
        self._gameView:setTableBtnIsOk(true)
    end
        
end

function GameLayer:onRefreshInfo(index)
    index = index or 0
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self.gameView._priView and nil ~= self.gameView._priView.onRefreshInfo then
            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + index
            self.gameView._priView:onRefreshInfo()
        end
    end
end

-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)
    print("onEventGameMessage == "..sub)

    if sub == cmd.SUB_S_GAME_FREE then 
        self:onSubGameFree(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_START then 
        self:onSubGameStart(dataBuffer)
    elseif sub == cmd.SUB_S_PLACE_JETTON then 
        self:onSubGamePlaceJetton(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_END then
        self:onSubGameEnd(dataBuffer)
    elseif sub == cmd.SUB_S_SEND_RECORD then
        self:onSubSendRecord(dataBuffer)
    elseif sub == cmd.SUB_S_PLACE_JETTON_FAIL then 
        self:onSubGamePlaceJettonFall(dataBuffer)
    elseif sub == cmd.SUB_S_CANCEL_BANKER then
        self:onSubCancelBanker(dataBuffer)
    elseif sub == cmd.SUB_S_APPLY_BANKER then 
        self:onSubApplyBanker(dataBuffer)
    elseif sub == cmd.SUB_S_CHANGE_BANKER then 
        self:onSubChangeBanker(dataBuffer)
    elseif sub == cmd.SUB_S_USER_CHAIR then
        self:onSubUserChair(dataBuffer)
    elseif sub == cmd.SUB_S_BET_LIMIT then 
        self:onSubBetLimit(dataBuffer)
    elseif sub == cmd.SUB_S_USER_BET_INFO then 
        self:onSubUserBetInfo(dataBuffer)
    elseif sub == cmd.SUB_S_BANKER_YAO then
        self:onSubBankerYao(dataBuffer)
    elseif sub == cmd.SUB_S_BANKER_OPEN then
        self:onSubBankerOpen(dataBuffer)
    elseif sub == cmd.SUB_S_SCORE then
        self:onSubScore(dataBuffer)
    elseif sub == cmd.SUB_S_CONTROL then
        self:onSubControl(dataBuffer)
    elseif sub == cmd.S_CR_FAILURE then
        self:onSubCrFailuer(dataBuffer)
    elseif sub == cmd.S_CR_SET_SUCCESS then
        self:onSubCrSetSuccess(dataBuffer)
    elseif sub == cmd.S_CR_CANCEL_SUCCESS then
        self:onSubCancerSuccess(dataBuffer)
    elseif sub == cmd.SUB_S_USER_RESULT then 
        self:onSubUserResult(dataBuffer)
    elseif sub == cmd.SUB_S_UPDATE then 
        self:onSubUpdata(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_RECORD then 
        self:onSubGameGoldRecord(dataBuffer)
    end

    if self.gameStatus ~= cmd.GAME_STATUS_FREE then 
        self.oneGameCount = false
    end

end

function GameLayer:onSubGameFree(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameFree,dataBuffer)
    -- dump(cmd_data, "SUB_S_GAME_FREE", 10)
    self.gameView:setGameFree()
    self.gameStatus = cmd.GAME_STATUS_FREE
    self.m_cbGameStatus = self.gameStatus
    self.gameView.betNumberLayer.initBetNum()
    self.gameView:showTimes(cmd_data.cbTimeLeave)
    if self.bankerid >= 255 then 
        self.gameView.notBanker:setVisible(true)
    else
        self.gameView.notBanker:setVisible(false)
    end
    if not self.oneGameCount then
        self.gameView:waitGameSpine()
    end
    self.isGameEnd = false
    self._gameView:setTableBtnIsOk(true)
    
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameStart,dataBuffer)
    -- dump(cmd_data, "游戏开始", 10)
    self.bankerid = cmd_data.wBankerUser
    self._gameView:setTableBtnIsOk(false)
    self.isMyBanker = self.bankerid == self:GetMeChairID()

    self.gameStatus = cmd.GAME_STATUS_PLAY
    -- self.m_cbGameStatus = self.gameStatus

    self.gameView:initWanPanel()
    self.gameView:startGameSpine()
    self.gameView.betNumberLayer.initBetNum()
    self.gameView:showTimes(cmd_data.cbTimeLeave)

    self:onRefreshInfo(1)
    self.countBankerId = self.bankerid
    self.gameView.notBanker:setVisible(false)
    self.isGameEnd = false     
end

--用户下注
function GameLayer:onSubGamePlaceJetton( dataBuffer )
    -- self._gameView:setTableBtnIsOk(true)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_PlaceJetton,dataBuffer)
    dump(cmd_data, "用户下注", 10)
    self.gameView:showPlayerBet(cmd_data)

end

--游戏结算
function GameLayer:onSubGameEnd(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameEnd,dataBuffer)
    -- dump(cmd_data,"CMD_S_GameEnd",12)
    self.gameStatus = cmd.GAME_STATUS_END
    self.m_cbGameStatus = cmd.GAME_STATUS_FREE

    self.gameEndData = cmd_data
    self.gameView:showTimes(cmd_data.cbTimeLeave)
    self.gameView:gameEndSpine()
    -- self._gameView:setTableBtnIsOk(true)
    -- print("self.isGameEnd gameend = ",self.isGameEnd)
    self.isGameEnd = true
end

--游戏记录
function GameLayer:onSubSendRecord( dataBuffer )
    local len = dataBuffer:getlen();
    local siceLen = 3
    local recordcount = math.floor(len / siceLen);
    if len - recordcount * siceLen ~= 0 then
        print("record_len_error" .. len);
        return
    end
    --游戏记录
    local game_record = {}
    --读取记录列表
    for i=1,recordcount do
        if nil == dataBuffer then
            break
        end
        local serverrecord = {}
        serverrecord[#serverrecord+1] = dataBuffer:readbyte()
        serverrecord[#serverrecord+1] = dataBuffer:readbyte()
        serverrecord[#serverrecord+1] = dataBuffer:readbyte()
        game_record[#game_record+1] = serverrecord
    end
    -- dump(game_record,"111",12)
    self.gameView:showRecordLayer(game_record)
end

--下注失败
function GameLayer:onSubGamePlaceJettonFall(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_PlaceJettonFail,dataBuffer)
    dump(cmd_data, "下注失败", 10)    
    -- self.gameView:showBetError(cmd_data)

    if cmd_data.wPlaceUser == self:GetMeChairID() and cmd_data.cbReason then
        showToast(self,cmd.BETERROR[cmd_data.cbReason],1)
    end

end

--取消申请
function GameLayer:onSubCancelBanker( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_CancelBanker,dataBuffer)
    dump(cmd_data, "取消申请", 10)


    for i=#self.bankerList,1,-1 do
        if self.bankerList[i] == cmd_data.wCancelUser and cmd_data.wCancelUser ~= self.bankerid  then 
            table.remove(self.bankerList,i)
        end
    end

    if cmd_data.wCancelUser == self:GetMeChairID() then 
        if not cmd_data.bIsClickCancelBanker then 
            showToast(self._scene,"下庄成功",1)
            for i=#self.bankerList,1,-1 do
                if self.bankerList[i] == cmd_data.wCancelUser then 
                    table.remove(self.bankerList,i)
                end
            end
        else
            showToast(self._scene,"下庄申请成功",1)
        end
    end

    self.gameView:initBankerLiet(self.bankerList)
end

--申请庄家
function GameLayer:onSubApplyBanker(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_ApplyBanker,dataBuffer)
    dump(cmd_data, "申请庄家", 10)


    for k,v in pairs(self.bankerList) do
        if v == cmd_data.wApplyUser then 
            return 
        end
    end
    self.bankerList[#self.bankerList+1] = cmd_data.wApplyUser
    self.gameView:initBankerLiet(self.bankerList)
end

--切换庄家 
function GameLayer:onSubChangeBanker(dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_ChangeBanker,dataBuffer)
    -- dump(cmd_data, "切换庄家", 10)
    
    for i=#self.bankerList,1,-1 do
        if self.bankerList[i] == self.bankerid then 
            table.remove(self.bankerList,i)
        end
    end

    self.bankerid = cmd_data.wBankerUser
    self.isMyBanker = self.bankerid == self:GetMeChairID()
    self.gameUserChair = cmd_data.wUserChair[1]
    self.gameView:updataDeskBanker(self.m_GameUserItem,cmd_data.wUserChair[1])
    self.gameView:setBankerOperationpanel()

    if self.bankerid < 65535 then
        self.countBankerId = self.bankerid
    end

    if self.gameStatus == cmd.GAME_STATUS_FREE then 
        if self.bankerid >= 255 then 
            self.gameView.notBanker:setVisible(true)
            self.gameView:removeChildByName("WaitGameSpine")
        else
            self.gameView.notBanker:setVisible(false)
        end
    end

    self.gameView:initBankerLiet(self.bankerList)
end

--玩家位置
function GameLayer:onSubUserChair( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_UserChair,dataBuffer)
    dump(cmd_data, "玩家位置", 10)
    -- self.m_GameUserItem = {}
    self.gameUserChair = cmd_data.wUserChair[1]
    -- dump(self.m_GameUserItem,"玩家列表",12)
    self.gameView:updataDeskBanker(self.m_GameUserItem,self.gameUserChair)
end

--更新下注限制
function GameLayer:onSubBetLimit(dataBuffer)
    -- body
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Current_Limit,dataBuffer)
    self.gameView:showSpecialBet(cmd_data)
end

--下注信息
function GameLayer:onSubUserBetInfo(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Current_UserBetInfo,dataBuffer)
    self.gameView:intBetInfo(cmd_data)
end

--庄家摇骰子
function GameLayer:onSubBankerYao(dataBuffer)
    self.gameView:wanYao()
end

function GameLayer:onSubBankerOpen( dataBuffer )
    self.gameView:wanKai()
end

function GameLayer:onSubScore(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Score,dataBuffer)
    dump(cmd_data,"CMD_S_Score",12)
    self.allCountData = cmd_data
end

function GameLayer:onSubControl(dataBuffer)
    -- body
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_ControlReturns,dataBuffer)
    dump(cmd_data,"SUB_S_CONTROL",12)
    showToast(self._scene,"设置成功",1)    
end

function GameLayer:onSubCrFailuer(dataBuffer)
    showToast(self._scene,"失败",1)
end

function GameLayer:onSubCrSetSuccess(dataBuffer)
    showToast(self._scene,"设置成功",1)
end

function GameLayer:onSubCancerSuccess(dataBuffer)
    showToast(self._scene,"取消成功",1)
end

function GameLayer:onSubUserResult(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RequestQueryResult,dataBuffer)
    dump(cmd_data,"SUB_S_USER_RESULT",12)
    -- self.gameView:showTextQuery(cmd_data)
    local layer = self:getChildByName("ControlLayer")
    if layer then 
        layer:showTextQuery(cmd_data)
    end
end

function GameLayer:onSubUpdata(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_UpdateStorage,dataBuffer)
    -- dump(cmd_data,"SUB_S_CONTROL",12)
    -- self.gameView:showTextQuery(cmd_data)
    local layer = self:getChildByName("ControlLayer")
    if layer and cmd_data.cbReqType == 1 then 
        layer:updataInventory(cmd_data)
    elseif not layer and cmd_data.cbReqType == 1 then 
        local l = ControlLayer:create(self)
        l:setName("ControlLayer")
        l:updataInventory(cmd_data)
        self:addChild(l)
    end

    if cmd_data.cbReqType == 2 then 
        showToast(self._scene,"保存成功",1)
    end
end
    
--金币流水               
function GameLayer:onSubGameGoldRecord(dataBuffer)

    local cmd_data = {}
    if  self.m_tabPrivateRoomConfig.bIsGoldPersonalRoom then 
        local tag = {
            {t = "int",k = "nUserPlayCount"},
            {t = "score",k = "lScore",l = {30}},
        }
        cmd_data = ExternalFun.read_netdata(tag,dataBuffer)
    else
        
        local tag = {
            {t = "int",k = "nUserPlayCount"},
            {t = "score",k = "lScore",l = {64,64,64,64,64,64,64,64,64,64,64},}
        }
        cmd_data = ExternalFun.read_netdata(tag,dataBuffer)

    end

    dump(cmd_data,"cmd_data",12)

    local l = RoomInfo:create(self,cmd_data)
    self._gameView:addChild(l,40)
end

--发送查询金币流水
function GameLayer:onSendGameGoldRecord()
    local cmddata = ExternalFun.create_netdata()
    cmddata:pushword(0)
    self:SendData(cmd.SUB_C_GAME_RECORD, cmddata)
end

function GameLayer:OnFlushCardFinish()
end


function GameLayer:onSubNoticeAddScore(szString)
    --设置准备按钮不可见
end


--发送准备
function GameLayer:onStartGame(bReady)
   
end

--自动比牌
function GameLayer:onAutoCompareCard()

end

--比牌操作
function GameLayer:onCompareCard()
 
end

function  GameLayer:OnCompareChoose(index)
   
end

function GameLayer:onUserChat(chatinfo,sendchair)
    -- dump(chatinfo,"chatinfo",12)
    -- dump(sendchair,"sendchair",12)
    if chatinfo and sendchair then
        -- local viewid = self:SwitchViewChairID(sendchair)
        local viewid = nil
        for k,v in pairs(self.gameUserChair) do
            if v == sendchair then 
                viewid = k
            end
        end

        if viewid and viewid ~= yl.INVALID_CHAIR then
            self.gameView:ShowUserChat(viewid, chatinfo.szChatString)
        end
    end
end

function GameLayer:onUserExpression(expression,sendchair)
    if expression and sendchair then

        local viewid = nil
        for k,v in pairs(self.gameUserChair) do
            if v == sendchair then 
                viewid = k
            end
        end

        if viewid and viewid ~= yl.INVALID_CHAIR then
            self.gameView:ShowUserExpression(viewid, expression.wItemIndex)
        end

    end
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = nil
    for k,v in pairs(self.gameUserChair) do
        if v == useritem.wChairID then 
            viewid = k
        end
    end

    if viewid and viewid ~= yl.INVALID_CHAIR then
        self.gameView:onUserVoiceStart(viewid)
    end
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = nil
    for k,v in pairs(self.gameUserChair) do
        if v == useritem.wChairID then 
            viewid = k
        end
    end
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self.gameView:onUserVoiceEnded(viewid)
    end
end

--获得加密Key
function GameLayer:getAesKey()
    --将数组转成字符串
end

-------------------------------------------------------新大厅增加函数--------------------------------------------
--游戏记录（房卡）
function GameLayer:onSubGameRecord(dataBuffer)
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
   
end

--申请上庄
function GameLayer:sendApplyBanker()
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_APPLY_BANKER, cmddata)
end

--取消上庄
function GameLayer:sendCancelApplyBanker()
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_CANCEL_BANKER, cmddata)
end

--用户下注
function GameLayer:sendGameBetInfo(betType,betArea,betNum,betOldArea)
    local chid =  self:GetMeChairID()
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_PlaceJetton)
    cmddata:pushword(chid)
    cmddata:pushbyte(betType)
    cmddata:pushbyte(betArea)
    cmddata:pushscore(betNum)
    cmddata:pushbyte(betOldArea)

    self:SendData(cmd.SUB_C_PLACE_JETTON, cmddata)
end

-- 查询豹子
function GameLayer:checkTheBaoZi()
    -- body
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_QURARY_BAOZI_BET, cmddata)
end

-- 查询连串
function GameLayer:checkTheLianChuan()
    -- body
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_QURARY_LCHUAN_BET, cmddata)
end

-- 查询挪单
function GameLayer:checkTheOrder()
    -- body
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_QURARY_ARERA_BET, cmddata)
end

-- 查询挪串
function GameLayer:checkTheNuoChuan()
    -- body
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_QURARY_CHUAN_BET, cmddata)
end

-- 查询本轮玩家信息
function GameLayer:checkTheBetting()
    -- body
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_QURARY_USER_BET, cmddata)
end

function GameLayer:getHistoryRecord()
    -- body
    local cmddata = CCmd_Data:create()
    self:SendData(cmd.SUB_C_SEND_RECORD, cmddata)

end

function GameLayer:sendBankerYao()
    if self.isMyBanker then
        local cmddata = CCmd_Data:create()
        self:SendData(cmd.SUB_C_CANCEL_BANKER_YAO, cmddata)
    end
end

function GameLayer:sendBankerOpen()
    if self.isMyBanker then 
        local cmddata = CCmd_Data:create()
        self:SendData(cmd.SUB_C_CANCEL_BANKER_OPEN, cmddata)
    end
end

function GameLayer:getDiceIndex()
    -- body
    if self.bIsDoubleSice then 
        return 2
    else
        return 3
    end

    return 2
end

-- 庄家发送开始
function GameLayer:sendGameStart(dataBuffer)
    local cmddata = CCmd_Data:create()
    self:SendData(12, cmddata)
    print("=============== ",cmd.SUB_C_CANCEL_BANKER_OPEN)
end

function GameLayer:getUserInfoByChairID(wChairID)
    -- body
    for k,v in pairs(self.m_GameUserItem) do
        if v then 
            if v.wChairID == wChairID then
                return v
            end
        end
    end
    return nil
end

--申请取款
function GameLayer:sendTakeScore( lScore,szPassword )
    local cmddata = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_TakeScoreRequest)
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE, g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushscore(lScore)
    cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)

    self._gameFrame:sendSocketData(cmddata)
end

--请求银行信息
function GameLayer:sendRequestBankInfo()
    local cmddata = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_QueryInsureInfoRequest)
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE,g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)

    self._gameFrame:sendSocketData(cmddata)
end

function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    self:dismissPopWait()
    if sub == g_var(game_cmd).SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureSuccess, dataBuffer)
        dump(cmd_table,"self.bank_success",12)
        self.bank_success = cmd_table

        self.gameView:onBankSuccess()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_FAILURE then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureFailure, dataBuffer)
        self.bank_fail = cmd_table
        dump(cmd_table,"self.bank_fail",12)
        self.gameView:onBankFailure()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_INFO then --银行资料
        local cmdtable = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureInfo, dataBuffer)
        dump(cmdtable, "cmdtable", 6)
        
        self.gameView:onGetBankInfo(cmdtable)
    else
        print("unknow gamemessage sub is ==>"..sub)
    end
end

function GameLayer:dismissResult()
    print("dismissResult")
    self.gameView:dismissResult()
end

function GameLayer:addPrivateGameLayer( layer )
    if nil == layer then
        return
    end
    self._gameView:addChild(layer, 39)
end

function GameLayer:sendContro(table_id,cbControlAppType,cbCtrlType,bIsBankerWin,scie,cbControlTimes)
    -- local chid =  self:GetMeChairID()
    print("table_id         = ",table_id)
    print("cbControlAppType = ",cbControlAppType)
    print("cbCtrlType       = ",cbCtrlType)
    print("bIsBankerWin     = ",bIsBankerWin)
    print("cbControlTimes   = ",cbControlTimes)
    dump(scie,"scie",12)

    local cmddata = ExternalFun.create_netdata()
    cmddata:pushint(table_id)
    cmddata:pushbyte(cbControlAppType)
    cmddata:pushbyte(cbCtrlType)
    cmddata:pushbyte(bIsBankerWin)
    cmddata:pushbyte(scie[1])
    cmddata:pushbyte(scie[2])
    cmddata:pushbyte(scie[3])
    cmddata:pushbyte(cbControlTimes)


    self:SendData(cmd.SUB_C_CONTROL, cmddata)
end

function GameLayer:sendUserQuery(userId)

    local cmddata = ExternalFun.create_netdata()
    cmddata:pushint(userId)
    self:SendData(cmd.SUB_C_QUERY_USER, cmddata)
end

function GameLayer:sendUpDataInventory(tab)
    local cmddata = ExternalFun.create_netdata()
    cmddata:pushbyte(tab.cbReqType)
    cmddata:pushscore(tab.lStorageStart)
    cmddata:pushscore(tab.lStorageDeduct)
    cmddata:pushscore(tab.lStorageCurrent)
    cmddata:pushscore(tab.lStorageMax1)
    cmddata:pushscore(tab.lStorageMul1)
    cmddata:pushscore(tab.lStorageMax2)
    cmddata:pushscore(tab.lStorageMul2)
    self:SendData(cmd.SUB_C_UPDATE, cmddata)
end


function GameLayer:getGameRule(buffer)
    dump(buffer,"'buffer",12)
    -- ClubFrame:onSubGetTableList 获取到数据全都为0
    local strTable = {}
    local gameMode = buffer[4] 
    if gameMode == 1 then 
        strTable[#strTable+1] = "二骰玩法"
    else
        strTable[#strTable+1] = "三骰玩法"
    end
    
    strTable[#strTable+1] = "区域封顶:"..(buffer[5]*1000)
    
    return strTable
end


function GameLayer:formatScoreTextEx(score)
    local scorestr = ExternalFun.formatScore(score)
    if score < 10000 then
        return scorestr
    end

    if score < 100000000 then
        scorestr = string.format("%d万", score / 10000)
        return scorestr
    end
    scorestr = string.format("%d亿", score / 100000000)
    return scorestr
end

function GameLayer:getRoomConfig( ... )
    -- body
    return self.m_tabPrivateRoomConfig
end

--换位操作
function GameLayer:onChangeDesk()
    print("onChangeDesk")
    self._gameFrame:QueryChangeDesk()
end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

return GameLayer
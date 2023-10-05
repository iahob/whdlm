local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.line.src";
require("cocos.init")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = module_pre .. ".models.CMD_Game"
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local g_var = ExternalFun.req_var
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
--local GameFrame = appdf.req(module_pre .. ".models.GameFrame")

local scheduler = cc.Director:getInstance():getScheduler()

function GameLayer:getParentNode( )
    return self._scene
end

function GameLayer:getFrame( )
    return self._gameFrame
end

function GameLayer:sendNetData( cmddata )
    return self:getFrame():sendSocketData(cmddata)
end

function GameLayer:ctor( frameEngine,scene )
    ExternalFun.registerNodeEvent(self)
    self.m_bLeaveGame = false
    GameLayer.super.ctor(self,frameEngine,scene)
    --self._gameFrame:QueryUserInfo( self:GetMeUserItem().wTableID,yl.INVALID_CHAIR)

end

--创建场景
function GameLayer:CreateView()
     self._gameView = GameViewLayer:create(self)
     self:addChild(self._gameView,0,2001)
     return self._gameView
end

function GameLayer:OnInitGameEngine()
    GameLayer.super.OnInitGameEngine(self)

    self.m_gamedata = {}        --游戏数据

    self.m_egglist = {}         --癞子

    self.m_taskComplete = {0,0,0}

    self.m_multipleIndex = 0    --得分最大
    self.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
    self.m_winScore = 0         --本局得分
    self.m_taskScore = 0        --任务得分

    self.m_lUserCoins           = 0         --金币
    self.m_lUserBeans           = 0         --豆子

    self.m_waittime = 0
    self.m_scheduleAuto = nil 

    self.tGonggao = {}

    self.m_bIsAuto              = false        --控制自动开始按钮
    self.m_bIsItemMove          = false     --动画是否滚动的标示

    self.m_plusindex            = 1


    self.bStopNew = nil
----------------------------------------------------
    self.m_bIsLeave             = false     --是否离开游戏
    self.m_bIsPlayed            = false       --是否玩过游戏

    --游戏逻辑操作

    --self.m_bYafenIndexNow       = 0         --发送服务器时的压分索引

    --是否立刻停止
    self.isAtOnceStop = false;
end

function GameLayer:resetData()
    --GameLayer.super.resetData(self)

    self.m_gamedata = {}        --游戏数据

    self.m_egglist = {}         --癞子

    self.m_taskComplete = {0,0,0}

    self.m_multipleIndex = 0    --得分最大
    self.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
    self.m_winScore = 0         --本局得分
    self.m_taskScore = 0        --任务得分

    self.m_lUserCoins           = 0         --金币
    self.m_lUserBeans           = 0         --豆子

    self.m_waittime = 0
    self.m_scheduleAuto = nil 

    self.tGonggao = {}

    self.m_bIsAuto              = false        --控制自动开始按钮
    self.m_bIsItemMove          = false     --动画是否滚动的标示

    self.m_plusindex            = 1

    self.bStopNew = nil
----------------------------------------------------
    self.m_bIsLeave             = false     --是否离开游戏
    self.m_bIsPlayed            = false       --是否玩过游戏

    --游戏逻辑操作

    --self.m_bYafenIndexNow       = 0         --发送服务器时的压分索引

    --是否立刻停止
    self.isAtOnceStop = false;

end

-- 重置游戏数据

--------------------------------------------------------------------------------------
function GameLayer:onExit()

    if self.m_scheduleAuto then
        scheduler:unscheduleScriptEntry(self.m_scheduleAuto)
        self.m_scheduleAuto = nil
        self.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
    end

    print("gameLayer onExit()...................................")
    self:KillGameClock()
    self:dismissPopWait()
end

--退出桌子
function GameLayer:onExitTable()
    if self.m_querydialog then
        return
    end
    self:KillGameClock()
    self:showPopWait()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:runAction(cc.Sequence:create(
            cc.DelayTime:create(2),
            cc.CallFunc:create(
                function ()   
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

--系统消息
function GameLayer:onSystemMessage( wType,szString )
    print("wType, szString",wType,szString)
    if wType == 515 then  --515 当玩家没钱时候
       self.m_querydialog = QueryDialog:create(szString,function()
        
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
                            print("delay leave")
                            self:onExitRoom()
                        end
                        )
                    )
                )
                return
            end
           self:onExitRoom()

        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)
    end
end
-- -------------------------------------------------------------------------------------
-- -------------------------------------------------------------------------------------场景消息
--银行
function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    self:dismissPopWait()
    if sub == g_var(game_cmd).SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureSuccess, dataBuffer)
        self.bank_success = cmd_table

        self._gameView:onBankSuccess()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_FAILURE then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureFailure, dataBuffer)
        self.bank_fail = cmd_table

        self._gameView:onBankFailure()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_INFO then --银行资料
        local cmdtable = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureInfo, dataBuffer)

        self._gameView:onGetBankInfo(cmdtable)
    else
        print("unknow gamemessage sub is ==>"..sub)
    end
end

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
    print("场景数据:" .. cbGameStatus)
    --self._gameView:removeAction()
    self:KillGameClock()
    self:onEventGameSceneFree(dataBuffer);

    self:dismissPopWait()
end

function GameLayer:OnResetGameEngine()
    print("OnResetGameEngine")
    --self._gameView.againFlag = true
    GameLayer.super.OnResetGameEngine(self)

    if self.m_gamestatus == g_var(cmd).GAME_SCENE_FREE then 
        --self:resetData()
    --elseif self.m_gamestatus == g_var(cmd).GAME_SCENE_SLOW then
        --print("不用处理")
    elseif self.m_gamestatus == g_var(cmd).GAME_SCENE_PLAY  or self.m_gamestatus == g_var(cmd).GAME_SCENE_SLOW  then
        --self:resetData()
        --停止定时器
        if self._gameView then
            for i=1,3 do
                if self._gameView.m_rolllist[i].m_scheduleUpdate then
                    local scheduler = cc.Director:getInstance():getScheduler()
                    scheduler:unscheduleScriptEntry(self._gameView.m_rolllist[i].m_scheduleUpdate)
                    self._gameView.m_rolllist[i].m_scheduleUpdate = nil
                end
                self._gameView.m_rolllist[i]:removeFromParent()
            end
            --重新初始化场景
            self._gameView:initRolllist()
            self.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
            
            self._gameView:changeStartButton(false)
            --如果是托管，变成不是托管
            if self.m_bIsAuto then
                self:onAutoStart()
            end
        end

        self._gameView:gameBeginClearShow()
    end
    if not self.bIsStop then
        --self:SendUserReady()
    end 
    if self.m_scheduleAuto then
        scheduler:unscheduleScriptEntry(self.m_scheduleAuto)
        self.m_scheduleAuto = nil
        self.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
    end
end


--场景消息
function GameLayer:onEventGameSceneFree(buffer)    --空闲 
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_Status, buffer)
    dump(cmd_table)
    --初始化数据
    self.m_gamedata = cmd_table

    self._gameView:updatePlusScore()
    self._gameView:updatePoolScore()

    local useritem = self:GetMeUserItem()
    --dump(useritem)
    self.m_lUserCoins = useritem.lScore
    self.m_lUserBeans = useritem.dBeans
    self._gameView:refreshGameScore()

    --if not self.m_pReWardLayer then
    self._gameView:initReWardLayer()
    --end

    self:SendUserReady()
end

-----------------------------------------------------------------------------------
-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)
    --print("onEventGameMessage sub :",sub)
    if sub == g_var(cmd).SUB_S_ELEMENT_STOP then               --元素信息 101
        print("SUB_S_ELEMENT_STOP")
        LogAsset:getInstance():logData("SUB_S_ELEMENT_STOP "..currentTime())
        print("元素信息")
        self:onSubElementStop(dataBuffer)
        self:SendUserReady()          
    elseif sub == g_var(cmd).SUB_S_LOTTERY_POOL then           --彩池数量 102
        --print("彩池数量")
        self:onSubLotteryPool(dataBuffer)                      
    elseif sub == g_var(cmd).SUB_S_TASK_INFO then              --任务信息 103
        print("任务信息")
        self:onSubTaskInfo(dataBuffer)                       
    elseif sub == g_var(cmd).SUB_S_AWARD_INFO then             --中奖信息 104
        print("中奖信息") 
        self:onSubAwardInfo(dataBuffer)
    else
        print("unknow gamemessage sub is "..sub)
    end
end
--元素信息
function GameLayer:onSubElementStop( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_ElementStop, dataBuffer)
    LogAsset:getInstance():logData("onSubElementStop")
    LogAsset:getInstance():logData(cjson.encode(cmd_table))
    --dump(cmd_table)
    self.bIsStop = true  
    self.m_egglist              = {} 

    --中奖线
    self.m_lineMultiple = cmd_table.bPlayWinLine[1]
    --分数
    self.m_lUserCoins = self.m_lUserCoins + cmd_table.lPlayWinScore + cmd_table.nPlayTaskScore

    --癞子计算
    if GameLogic:DeformCount(cmd_table.nElementIndex)  > 0 then
        local pointLine = {
            {{x=1,y=1},{x=2,y=1},{x=3,y=1}},
            {{x=1,y=2},{x=2,y=2},{x=3,y=2}},
            {{x=1,y=3},{x=2,y=3},{x=3,y=3}},
            {{x=1,y=1},{x=1,y=2},{x=1,y=3}},
            {{x=2,y=1},{x=2,y=2},{x=2,y=3}},
            {{x=3,y=1},{x=3,y=2},{x=3,y=3}},
            {{x=3,y=1},{x=2,y=2},{x=1,y=3}},
            {{x=3,y=3},{x=2,y=2},{x=1,y=1}},
        }
        --遍历线判断
        for index=1,g_var(cmd).LINE_INDEX_MAX do
            --获取数值
            local nLineElement = {}
            for nLineElemenIndex=1,3 do
                local posx = pointLine[index][nLineElemenIndex].x
                local posy = pointLine[index][nLineElemenIndex].y

                nLineElement[nLineElemenIndex] = cmd_table.nElementIndex[posx][posy]
            end
            --判断数值
            if GameLogic:ThreeLineDeform(nLineElement) == true  then
                for nLineElemenIndex=1,3 do
                    local posx = pointLine[index][nLineElemenIndex].x
                    local posy = pointLine[index][nLineElemenIndex].y

                    if nLineElement[nLineElemenIndex] ~= cmd_table.nElementIndex[posx][posy] then
                        --self.m_egglist
                        -- print("改变的",nLineElement[nLineElemenIndex] )
                        -- print("原始",cmd_table.nElementIndex[posx][posy])
                        table.insert(self.m_egglist,nLineElement[nLineElemenIndex])
                    end         
                end
            end
        end
    end
    print("@@@@@@@蛋@@@@@@@@@")
    dump(self.m_egglist)

    self.m_multipleIndex = cmd_table.nMultipleIndex
    self.m_gamestatus = g_var(cmd).GAME_SCENE_SLOW
    self.m_winScore = cmd_table.lPlayWinScore
    self.m_taskScore = cmd_table.nPlayTaskScore
    self._gameView.m_rolllist[1]:setListData(cmd_table.nElementIndex[1][1],cmd_table.nElementIndex[1][2],cmd_table.nElementIndex[1][3])
    self._gameView.m_rolllist[2]:setListData(cmd_table.nElementIndex[2][1],cmd_table.nElementIndex[2][2],cmd_table.nElementIndex[2][3])
    self._gameView.m_rolllist[3]:setListData(cmd_table.nElementIndex[3][1],cmd_table.nElementIndex[3][2],cmd_table.nElementIndex[3][3])

    self.bStopNew = cmd_table.bStopNew
    
    -- 托管时
    if self.m_bIsAuto then
        self._gameView:onGameStop(false)
    else
        if self.isAtOnceStop then
            self._gameView:onGameStop(true)
        else
            self._gameView:onGameStop(false)
        end
        
    end

end
--彩池数量
function GameLayer:onSubLotteryPool( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_LotteryPool, dataBuffer)
    self.m_gamedata.nRoomLotteryPool = cmd_table.nRoomLotteryPool
    self._gameView:updatePoolScore()
    --dump(cmd_table)
end
--
function GameLayer:onSubTaskInfo( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_TaskInfo, dataBuffer)
    self.m_gamedata.nTaskProgress = cmd_table.nTaskProgress
    self._gameView.m_pReWardLayer:updateRewardList()
    --dump(cmd_table)
end
--中奖信息
function GameLayer:onSubAwardInfo( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(g_var(cmd).CMD_S_AwardInfo, dataBuffer)
    --dump(cmd_table)
    self._gameView:addGongGao(clone(cmd_table))
end

--------------------------------------------
function GameLayer:onGameStart()
    if self.m_gamestatus ~= g_var(cmd).GAME_SCENE_FREE then
        print("游戏状态 ", self.m_gamestatus)
        return
    end
    --清空
    self._gameView:gameBeginClearShow()
    --判断下注
    local costScore = self.m_gamedata.nBetScore[1][self.m_plusindex]*g_var(cmd).LINE_INDEX_MAX

    if  self.m_lUserCoins < costScore then
        showToast(self,"金币不足",1)
        if self.m_bIsAuto then
            --调用托管的函数
            self._gameView:onButtonClickedEvent(self._gameView.m_btnAuto:getTag(),self._gameView.m_btnAuto)
        end
        return
    end

    self.bIsStop  = false
    self.m_waittime = 0             --等待时间

    self.m_bIsItemMove = true       --item move
    --刷新分数
    self.m_lUserCoins = self.m_lUserCoins - costScore
    self._gameView:refreshGameScore()

    --音效
    ExternalFun.playSoundEffect("StartLogo.wav")
    self._gameView:showTouBiAni()

    self:sendMessageGameBegin()

    self.m_gamestatus = g_var(cmd).GAME_SCENE_PLAY

    --转动
    for index=1,3 do
        self._gameView.m_rolllist[index]:setStatus(0)
    end

    --改变开始按钮 开始变为停止
    self._gameView:changeStartButton(true)
end
--停止游戏
function GameLayer:onGameStop()
    if self.m_gamestatus == g_var(cmd).GAME_SCENE_PLAY then
        self:sendMessageGameStop()
        self.m_waittime = 0
    else
        self._gameView:stopAllActions()
        for i=1,3 do
            self._gameView.m_rolllist[i]:setStatus(2)
        end
        --self.m_lUserCoins = self.m_lUserCoins + self.m_winScore + self.m_taskScore
        self._gameView:gameEndScoreDeal()
        self.m_waittime = 0
    end
    self._gameView:changeStartButton(false)
end

function GameLayer:autoPlayUpdate( dalta )
    --print("dalta",dalta)
    if not self.m_bIsAuto then
        if self.m_scheduleAuto then
            scheduler:unscheduleScriptEntry(self.m_scheduleAuto)
            self.m_scheduleAuto = nil
            --self.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
        end
        return
    end
    print("self.m_gamestatus",self.m_gamestatus)
    if self.m_gamestatus == g_var(cmd).GAME_SCENE_FREE then

        if self.m_winScore <= 0 then
            self.m_waittime = self.m_waittime + dalta
            --print("self.m_waittime",self.m_waittime)
            if self.m_waittime > 1.8 then
                --print("m_winScore<=0",self.m_winScore)

                self._gameView:onButtonClickedEvent(self._gameView.m_btnStart:getTag(),self._gameView.m_btnStart)
                self.m_waittime = 0
            end
        else
            self.m_waittime = self.m_waittime + dalta
            --print("self.m_waittime",self.m_waittime)
            if self.m_waittime > 8 then
                self._gameView:onButtonClickedEvent(self._gameView.m_btnStart:getTag(),self._gameView.m_btnStart)
                self.m_waittime = 0
            end
        end
    end
end
--托管
function GameLayer:onAutoStart( )
    --
    self.m_bIsAuto = not self.m_bIsAuto
    if self.m_bIsAuto == true then
        print("self.m_gamestatus",self.m_gamestatus)
        if self.m_gamestatus == g_var(cmd).GAME_SCENE_FREE  then
            local costScore = self.m_gamedata.nBetScore[1][self.m_plusindex]*g_var(cmd).LINE_INDEX_MAX
            if  self.m_lUserCoins < costScore then
                showToast(self,"金币不足",1)
                return
            end

            self._gameView:changeStartButton(true)
            self._gameView:onButtonClickedEvent(self._gameView.m_btnStart:getTag(),self._gameView.m_btnStart)
            self.m_waittime = 0
        end
        if self.m_scheduleAuto then
            scheduler:unscheduleScriptEntry(self.m_scheduleAuto)
            self.m_scheduleAuto = nil
        end
        self.m_scheduleAuto = scheduler:scheduleScriptFunc(handler(self,GameLayer.autoPlayUpdate), 0, false)
        --self._gameView:changeStartButton2(self.m_bIsAuto)
    end
    self._gameView:updateAutoBtn(self.m_bIsAuto)
end
--加注
function GameLayer:onAddScore()
    print("加注")
    if self.m_plusindex < g_var(cmd).BET_INDEX_MAX  then
        self.m_plusindex = self.m_plusindex + 1
        self._gameView:updatePlusScore();
    end
end
--减注
function GameLayer:onSubScore()
    print("减注")
    if self.m_plusindex > 1 then
        self.m_plusindex = self.m_plusindex - 1
        self._gameView:updatePlusScore();
    end
end
--------------消息-----------------
function GameLayer:sendMessageGameBegin(  )
    --发送数据
    local dataBuffer = CCmd_Data:create(12)
    local bBetScore = self.m_gamedata.nBetScore[1][self.m_plusindex]
    dataBuffer:pushint(bBetScore)
    for i=1,g_var(cmd).LINE_INDEX_MAX do
        local nBetLine = true
        dataBuffer:pushbool(nBetLine)
    end
    self:SendData(g_var(cmd).SUB_C_START,dataBuffer)
     print("sendMessageGameBegin")
   
     LogAsset:getInstance():logData("sendMessageGameBegin "..currentTime())
end
--停止消息
function GameLayer:sendMessageGameStop()
    print("sendMessageGameStop")
    LogAsset:getInstance():logData("sendMessageGameStop "..currentTime())
    local dataBuffer = CCmd_Data:create(1)
    self:SendData(g_var(cmd).SUB_C_STOP,dataBuffer)
end

--申请取款
function GameLayer:sendTakeScore( lScore,szPassword )
    local cmddata = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_TakeScoreRequest)
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE, g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushscore(lScore)
    cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)
    cmddata:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--请求银行信息
function GameLayer:sendRequestBankInfo()
    local cmddata = CCmd_Data:create(67)
    print("g_var(game_cmd).MDM_GR_INSURE",g_var(game_cmd).MDM_GR_INSURE)
    print("g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO",g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE,g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    local str = md5(GlobalUserItem.szPassword) or "d1fd5495e7b727081497cfce780b6456"
    cmddata:pushstring(str,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--申请存款
function GameLayer:onSaveScore(lScore)
    --操作记录
    local buffer = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_SaveScoreRequest)
    buffer:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE,g_var(game_cmd).SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushbyte(g_var(game_cmd).SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushscore(lScore)
    buffer:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)
    self:sendNetData(buffer)
    --if not self.sendNetData(buffer) then
        --showToast(self,"发送存款失败！",2)
    --end
end

return GameLayer
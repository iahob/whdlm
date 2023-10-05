local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)
local QueryDialog   = appdf.req("app.views.layer.other.QueryDialog")
local module_pre = "game.yule.linefruit.src";
require("cocos.init")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."yule.linefruit.src.models.CMD_Game")
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

    self.beginLock = true
    self.nGiveAwayCount = 0
    self.historyRecord = {}
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
    self.m_gamestatus = cmd.GAME_SCENE_FREE
    self.m_winScore = 0         --本局得分
    self.m_taskScore = 0        --任务得分

    self.m_lUserCoins           = 0         --金币
--    self.m_lUserBeans           = 0         --豆子

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
end

function GameLayer:resetData()
    --GameLayer.super.resetData(self)

    self.m_gamedata = {}        --游戏数据

    self.m_egglist = {}         --癞子

    self.m_taskComplete = {0,0,0}

    self.m_multipleIndex = 0    --得分最大
    self.m_gamestatus = cmd.GAME_SCENE_FREE
    self.m_winScore = 0         --本局得分
    self.m_taskScore = 0        --任务得分

    self.m_lUserCoins           = 0         --金币
--    self.m_lUserBeans           = 0         --豆子

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

end

-- 重置游戏数据

--------------------------------------------------------------------------------------
function GameLayer:onExit()

    if self.m_scheduleAuto then
        scheduler:unscheduleScriptEntry(self.m_scheduleAuto)
        self.m_scheduleAuto = nil
        self.m_gamestatus = cmd.GAME_SCENE_FREE
    end
    if self.m_scheduleUpdate then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_scheduleUpdate)
        self.m_scheduleUpdate = nil
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
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
--            cc.DelayTime:create(2),
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
    print("wType，szString",wType,szString)
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
        self:sendRequestBankInfo()
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
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
    self:onGamePause()
--    self._gameView:onButtonClickedEvent(self._gameView.m_btnStop:getTag(),self._gameView.m_btnStop)

    print("场景数据:" .. cbGameStatus)
    -- self._gameView:removeAction()
    self:KillGameClock()
    if cbGameStatus == cmd.GAME_SCENE_FREE then
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
        dump(cmd_table, "cmd.CMD_S_StatusFree")
        -- 初始化数据
        self.m_gamedata = cmd_table
    elseif cbGameStatus == cmd.GAME_SCENE_PLAY then
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
        dump(cmd_table, "cmd.CMD_S_StatusPlay")
        -- 初始化数据
        self.m_gamedata = cmd_table
        self.nGiveAwayCount = cmd_table.nGiveAwayCount
        self._gameView:onGiveAwayCount(self.nGiveAwayCount)
    end

    self._gameView:updatePlusScore()

    local useritem = self:GetMeUserItem()
    -- dump(useritem)
    self.m_lUserCoins = useritem.lScore
    self.m_lUserName = useritem.szNickName
    self._gameView:refreshGameScore()

    self._gameView:initButton()

        --是否免费游戏
    if self.nGiveAwayCount > 0 then
        self._gameView:setAllButtonEnabled(false)
    else
        self._gameView:setAllButtonEnabled(true)
    end

    self:SendUserReady()
    self:dismissPopWait()
end

-----------------------------------------------------------------------------------
-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)
--    print("onEventGameMessage sub :",sub)
    if sub == cmd.SUB_S_GAME_START then                 --结束信息
        print("结束信息")
        self:onSubElementStop(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_CONCLUDE then          --结束游戏
        print("结束游戏")
        self:onSubGameConclude(dataBuffer)    
        self:SendUserReady()
    elseif sub == cmd.SUB_S_GAME_LOTTERY then           --彩池数量
--        print("彩池数量")
        self:onSubLotteryPool(dataBuffer) 
    elseif sub == cmd.SUB_S_GAME_HISTORY_RECORD then    --大奖记录
        print("大奖记录")
        self:onSubHistoryRecord(dataBuffer)
    elseif sub == cmd.SUB_S_SUPERRABBIT_START then      --兔子游戏开始
        print("兔子游戏开始")
        self:onSubRabbitStart(dataBuffer)                       
    elseif sub == cmd.SUB_S_SUPERRABBIT_END then        --兔子游戏结束
        print("兔子游戏结束") 
        self:onSubRabbitEnd(dataBuffer)
    else
        print("unknow gamemessage sub is "..sub)
    end
end

function GameLayer:onSubRabbitEnd(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SupperRabbitEnd, dataBuffer)
    dump(cmd_table,"GameLayer:onSubRabbitEnd")
    self.rabbitLayer:showEndResult(cmd_table.lCurSuperRabbitScore)
    self._gameView.sendEnd = true

    --兔子游戏结束置空
    self.rabbitStartInfo = nil;
    -- 刷新分数
    self.m_taskScore = cmd_table.lCurSuperRabbitScore;
    self._gameView:updateTotalScore();
    self.m_lUserCoins = self.m_lUserCoins + cmd_table.lCurSuperRabbitScore - self.m_winScore;
    self._gameView:refreshGameScore()

    self:sendMessageGameStop()
end

function GameLayer:onSubRabbitStart(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SupperRabbitStart, dataBuffer)
    dump(cmd_table,"GameLayer:onSubRabbitStart")
    self.rabbitStartInfo = clone(cmd_table)
    self._gameView:gameEnd()
end

function GameLayer:onSubHistoryRecord( dataBuffer )
--    print(dataBuffer:getlen()) --dataBuffer的长度/cmd.CMD_S_HistoryRecord的长度 108
    local n = math.floor(dataBuffer:getlen()/108)
    for i = 1,n do
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_HistoryRecord, dataBuffer)
--        dump(cmd_table,"GameLayer:CMD_S_HistoryRecord")
        table.insert(self.historyRecord,cmd_table)
    end
end

function GameLayer:onSubGameConclude( dataBuffer )
    self.beginLock = true
end

--元素信息
function GameLayer:onSubElementStop( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
    self.bIsStop = true  
    self.m_egglist              = {} 
--    cmd_table.cbItemInfo = {{1,2,4,4,5},{0,2,8,7,9},{5,0,6,8,0}}
--    cmd_table.bPlayWinLine[1] = {true,true,true,true,true,true,true,true}
    dump(cmd_table,"GameLayer:onSubElementStop")


    self.cbItemInfo = clone(cmd_table.cbItemInfo)
    --中奖线
    self.m_lineMultiple = clone(cmd_table.lLineScore[1])
    self.cbItemType = clone(cmd_table.cbItemType[1])
    self.nGiveAwayCount = clone(cmd_table.nGiveAwayCount)
    if self.nGiveAwayCount > 0 then
        self.m_gamedata.cbLineCount = self._gameView.lineNum
        self.m_gamedata.cbBetIndex = self._gameView.scoreIndex - 1
    end

    --癞子计算
--    if GameLogic:DeformCount(cmd_table.nElementIndex)  > 0 then
--        local pointLine = {
--            {{x=1,y=1},{x=2,y=1},{x=3,y=1}},
--            {{x=1,y=2},{x=2,y=2},{x=3,y=2}},
--            {{x=1,y=3},{x=2,y=3},{x=3,y=3}},
--            {{x=1,y=1},{x=1,y=2},{x=1,y=3}},
--            {{x=2,y=1},{x=2,y=2},{x=2,y=3}},
--            {{x=3,y=1},{x=3,y=2},{x=3,y=3}},
--            {{x=3,y=1},{x=2,y=2},{x=1,y=3}},
--            {{x=3,y=3},{x=2,y=2},{x=1,y=1}},
--        }
--        --遍历线判断
--        for index=1,cmd.LINE_INDEX_MAX do
--            --获取数值
--            local nLineElement = {}
--            for nLineElemenIndex=1,3 do
--                local posx = pointLine[index][nLineElemenIndex].x
--                local posy = pointLine[index][nLineElemenIndex].y

--                nLineElement[nLineElemenIndex] = cmd_table.nElementIndex[posx][posy]
--            end
--            --判断数值
--            if GameLogic:ThreeLineDeform(nLineElement) == true  then
--                for nLineElemenIndex=1,3 do
--                    local posx = pointLine[index][nLineElemenIndex].x
--                    local posy = pointLine[index][nLineElemenIndex].y

--                    if nLineElement[nLineElemenIndex] ~= cmd_table.nElementIndex[posx][posy] then
--                        --self.m_egglist
--                        -- print("改变的",nLineElement[nLineElemenIndex] )
--                        -- print("原始",cmd_table.nElementIndex[posx][posy])
--                        table.insert(self.m_egglist,nLineElement[nLineElemenIndex])
--                    end         
--                end
--            end
--        end
--    end
--    print("@@@@@@@蛋@@@@@@@@@")
--    dump(self.m_egglist)

    self.m_multipleIndex = 100
    self.m_gamestatus = cmd.GAME_SCENE_SLOW
    self.m_taskScore = 0
    self._gameView.m_rolllist[1]:setListData(cmd_table.cbItemInfo[1][1],cmd_table.cbItemInfo[2][1],cmd_table.cbItemInfo[3][1])
    self._gameView.m_rolllist[2]:setListData(cmd_table.cbItemInfo[1][2],cmd_table.cbItemInfo[2][2],cmd_table.cbItemInfo[3][2])
    self._gameView.m_rolllist[3]:setListData(cmd_table.cbItemInfo[1][3],cmd_table.cbItemInfo[2][3],cmd_table.cbItemInfo[3][3])
    self._gameView.m_rolllist[4]:setListData(cmd_table.cbItemInfo[1][4],cmd_table.cbItemInfo[2][4],cmd_table.cbItemInfo[3][4])
    self._gameView.m_rolllist[5]:setListData(cmd_table.cbItemInfo[1][5],cmd_table.cbItemInfo[2][5],cmd_table.cbItemInfo[3][5])

    self.bStopNew = false
    self._gameView:onGameStop(self.bStopNew)

end
--彩池数量
function GameLayer:onSubLotteryPool( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_Lottery, dataBuffer)
    self.m_gamedata.lLotteryScore = cmd_table.lLotteryScore
--    dump(cmd_table,"GameLayer:onSubLotteryPool")
    self._gameView:updatePoolScore()
end
--
function GameLayer:onSubTaskInfo( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_TaskInfo, dataBuffer)
    self.m_gamedata.nTaskProgress = cmd_table.nTaskProgress
    self._gameView.m_pReWardLayer:updateRewardList()
    --dump(cmd_table)
end
--中奖信息
function GameLayer:onSubAwardInfo( dataBuffer )
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_AwardInfo, dataBuffer)
    --dump(cmd_table)
    self._gameView:addGongGao(clone(cmd_table))
end

--------------------------------------------
function GameLayer:onGameStart()
    if self.m_gamestatus ~= cmd.GAME_SCENE_FREE then
        print("游戏状态 ", self.m_gamestatus)
        return
    end
    --清空
    self._gameView:gameBeginClearShow()
    --判断下注
    local costScore = (self.m_gamedata.lCellScore * self.m_gamedata.lBetScore[1][self._gameView.scoreIndex]) * self._gameView.lineNum
    if self.m_lUserCoins < costScore then
        showToast(self,"金币不足",1)
        if self._gameView.autoBegin then
            self._gameView.autoBegin = false
            --调用托管的函数
           -- self._gameView:onButtonClickedEvent(self._gameView.m_btnAuto:getTag(),self._gameView.m_btnAuto)

            --重置按钮状态
            self._gameView.playing = false
            self._gameView.m_btnAuto:loadTextureNormal(cmd.RES.."game/bt_auto_0.png")
            self._gameView:initButton()
        end
        return
    end

    self.bIsStop  = false
    self.m_waittime = 0             --等待时间

    self.m_bIsItemMove = true       --item move
     
    self:sendMessageGameBegin()
    self.m_gamestatus = cmd.GAME_SCENE_PLAY

    if self.nGiveAwayCount == 0  then
        --刷新分数
        self.m_lUserCoins = self.m_lUserCoins - costScore
        self._gameView:refreshGameScore()
    elseif self.nGiveAwayCount > 0  then
        self.nGiveAwayCount = self.nGiveAwayCount - 1
        self._gameView:onGiveAwayCount(self.nGiveAwayCount)
    end

    --转动
    for index=1,cmd.X_COUNT do
        self._gameView.m_rolllist[index]:setStatus(0)
    end
    --改变开始按钮 开始变为停止
    self._gameView:changeStartButton(true)

    if self._gameView.autoBegin then
--        self._gameView.m_btnStart:loadTextures("game/bt_start_2.png", "game/bt_start_0.png", "game/bt_start_3.png", ccui.TextureResType.plistType)
--        self._gameView.m_btnStop:loadTextures("game/bt_start_2.png", "game/bt_start_2.png", "game/bt_start_3.png", ccui.TextureResType.plistType)
--        self._gameView.autotips2:setVisible(true)
        self._gameView.Button_line_add:setEnabled(false)
        self._gameView.Button_score_add:setEnabled(false)
    else
--        self._gameView.autotips2:setVisible(false)
    end

    self.m_soundindex = 0
    self.slow = false
    local function update(dt)
        self:update(dt)
    end
--    if self.m_scheduleUpdate == nil then
--        self.m_scheduleUpdate = cc.Director:getInstance():getScheduler():scheduleScriptFunc(update, 0.02, false)
--    end
end

function GameLayer:update(time)
    self.m_soundindex = self.m_soundindex + 1
    if self.slow then
        self.m_soundindex = self.m_soundindex % 9
    else
        self.m_soundindex = self.m_soundindex % 3
    end
    if self.m_soundindex == 0 then
        ExternalFun.playSoundEffect("ElementMove.wav")
    end
end

--停止游戏
function GameLayer:onGameStop()
--    if self.m_gamestatus == cmd.GAME_SCENE_PLAY then
--        self.m_waittime = 0
--    else
        self._gameView.autoBegin = false
        self._gameView.Button_line_add:setEnabled(true)
        self._gameView.Button_score_add:setEnabled(true)
        self._gameView:initButton()

        self._gameView:stopAllActions()
        for i = 1, cmd.X_COUNT do
            self._gameView.m_rolllist[i]:setStatus(2)
        end
        -- self.m_lUserCoins = self.m_lUserCoins + self.m_winScore + self.m_taskScore
        self._gameView:gameEndScoreDeal()
        self.m_waittime = 0
        self._gameView:changeStartButton(false)
--    end
    self:sendMessageGameStop()
end

--暂停游戏
function GameLayer:onGamePause()
    for i = 1, cmd.X_COUNT do
        self._gameView.m_rolllist[i]:setStatus(2)
    end
    self.m_gamestatus = cmd.GAME_SCENE_FREE
    
    self._gameView:stopAllActions()
    self._gameView.autoBegin = false
    self._gameView.playing = false
    self._gameView.sendEnd = true
    self._gameView.Button_line_add:setEnabled(true)
    self._gameView.Button_score_add:setEnabled(true)
    self._gameView.Button_line_sub:setEnabled(true)
    self._gameView.Button_score_sub:setEnabled(true)
    
    self._gameView.Button_check:setEnabled(true)
    self._gameView.Button_max:setEnabled(true)
    self._gameView.m_btnAuto:loadTextureNormal(cmd.RES.."game/bt_auto_0.png")

    self._gameView:initButton()

    if self.m_scheduleUpdate ~= nil then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_scheduleUpdate)
        self.m_scheduleUpdate = nil
    end
    self.m_waittime = 0
    self:sendMessageGameStop()
end

function GameLayer:autoPlayUpdate( dalta )
    --print("dalta",dalta)
    if not self.m_bIsAuto then
        if self.m_scheduleAuto then
            scheduler:unscheduleScriptEntry(self.m_scheduleAuto)
            self.m_scheduleAuto = nil
            --self.m_gamestatus = cmd.GAME_SCENE_FREE
        end
        return
    end
    print("self.m_gamestatus",self.m_gamestatus)
    if self.m_gamestatus == cmd.GAME_SCENE_FREE then

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

--------------消息-----------------
function GameLayer:sendMessageGameBegin()
    dump(self:GetMeUserItem(), "self:GetMeUserItem()")
    print("self.nGiveAwayCount",self.nGiveAwayCount)
    if self.beginLock then
        if self.nGiveAwayCount > 0 then
            local cmd_data = CCmd_Data:create(0)
            self:SendData(cmd.SUB_C_GIVE_AWAY_START, cmd_data)
            print("发送开111")
        else
            -- 发送数据
            local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_GameStart)
            cmd_data:pushbyte(self._gameView.lineNum)
            cmd_data:pushbyte(self._gameView.scoreIndex - 1)
            self:SendData(cmd.SUB_C_GAME_START, cmd_data)
            print("发送开222",self._gameView.lineNum ,self._gameView.scoreIndex - 1)
        end
        print("发送消息")
        self.beginLock = false
    end
end

--停止消息
function GameLayer:sendMessageGameStop()
    if self._gameView.sendEnd then
        if not self.beginLock and self.nGiveAwayCount == 0 then
            local cmd_data = CCmd_Data:create(0)
            self:SendData(cmd.SUB_C_GAME_CONCLUDE, cmd_data)
        end
        self.beginLock = true
        if self._gameView then
            self._gameView.sendEnd = false
        end
    end
end

--兔子游戏开始
function GameLayer:sendMessageRabbitStart()
    local cmd_data = CCmd_Data:create(0)
    print("GameLayer:sendMessageRabbitStart")
    self:SendData(cmd.SUB_C_SUPERRABBIT_START, cmd_data)
end

--兔子游戏结束
function GameLayer:sendMessageRabbitEnd(score)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_SuperRabbitEnd)
    print("GameLayer:sendMessageRabbitEnd",score)
    cmd_data:pushint(score)
    self:SendData(cmd.SUB_C_SUPERRABBIT_END, cmd_data)
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
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE,g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    local str = md5(GlobalUserItem.szPassword) or "d1fd5495e7b727081497cfce780b6456"
    cmddata:pushstring(str,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--申请存款
function GameLayer:onSaveScore(lScore)
    print("存钱 GameLayer:onSaveScore",lScore)
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
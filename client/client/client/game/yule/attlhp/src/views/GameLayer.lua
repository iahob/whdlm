local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.attlhp.src";
require("cocos.init")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(module_pre..".models.CMD_Game")
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local GameLogic = module_pre .. ".models.GameLogic";
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local g_var = ExternalFun.req_var
local GameFrame = appdf.req(module_pre .. ".models.GameFrame")


function GameLayer:ctor(frameEngine,scene)
    ExternalFun.registerNodeEvent(self)
    self.m_bLeaveGame = false

    GameLayer.super.ctor(self,frameEngine,scene)

    self.m_cbGameStatus=0
end

--创建场景
function GameLayer:CreateView()
    return GameViewLayer:create(self)
        :addTo(self)
end

function GameLayer:getParentNode( )
    return self._scene
end

function GameLayer:getFrame( )
    return self._gameFrame
end

function GameLayer:getUserList(  )
    return self._gameFrame._UserList
end

function GameLayer:sendNetData(cmddata)
    return self:getFrame():sendSocketData(cmddata)
end


function GameLayer:logData(msg)
    if nil ~= self._scene.logData then
        self._scene:logData(msg)
    end
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

--银行消息
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


---------------------------------------------------------------------------------------
------继承函数

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID;
end

-- function GameLayer:onExit()
--     self:KillGameClock()
--     self:dismissPopWait()
--     GameLayer.super.onExit(self)
-- end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()
    self.handCards = {}
    self.wGuessCount = 0
    self.guessCardData = nil
    self.bguessBig = false
end

--强行起立、退出(用户切换到后台断网处理)
function GameLayer:standUpAndQuit()
    -- self:sendCancelOccupy()
    GameLayer.super.standUpAndQuit(self)
end

--退出桌子
function GameLayer:onExitTable()
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        -- self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    -- self:sendCancelOccupy()
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

-- 计时器响应
function GameLayer:OnEventGameClockInfo(chair,time,clockId)
    -- if nil ~= self._gameView and nil ~= self._gameView.updateClock then
    --     self._gameView:updateClock(clockId, time)
    -- end
end

-- 设置计时器
function GameLayer:SetGameClock(id,time)
    self:KillGameClock();
    local chairId = self:GetMeChairID();
    GameLayer.super.SetGameClock(self, chairId, id, time);

    --倒计时重置开始
    self._gameView:resertShowCutTimeLabel();
end

------网络发送
--玩家下注
function GameLayer:sendPlaceJetton(bet)
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_CurrentBet);
    cmddata:pushint(bet);

    self:SendData(cmd.SUB_C_CURRENT_BET, cmddata);
end

function GameLayer:sendSwitchCard()
    local cmddata = CCmd_Data:create() -- ExternalFun.create_netdata(cmd.CMD_C_SwitchCard);
    local count  = 0
    dump(self._gameView.cardHeldStatus,"self._gameView.cardHeldStatus")
    for i = 1, 5 do
        cmddata:pushbool(self._gameView.cardHeldStatus[i])
        if self._gameView.cardHeldStatus[i] then
            count = count +1
        end
    end
    cmddata:pushbyte(count)
    cmddata:pushbool(true)
    for i = 1, 5 do
        if not self._gameView.cardHeldStatus[i] then
            cmddata:pushbyte(self.cbFirstCardArray[i])
            print(" self.handCards[i]",i,self.cbFirstCardArray[i])
        else
            cmddata:pushbyte(self.cbSecondCardArray[i])
            print(" self.handCards[i]",i,self.cbSecondCardArray[i])

        end
    end

    self:SendData(cmd.SUB_C_SWITCH_CARD, cmddata);
end


function GameLayer:sendGameEnd(bet)
    -- assert(bet>0)
    local cmddata = CCmd_Data:create()
    cmddata:pushword(self:GetMeChairID())
    cmddata:pushint(bet)
    for i = 1, 5 do
        cmddata:pushbyte(self.handCards[i])
    end
    self:SendData(cmd.SUB_C_GAME_END, cmddata);
end

function GameLayer:sendLuckyTime()
    local cmddata = CCmd_Data:create(0)

    self:SendData(cmd.SUB_C_LUCKYTIME, cmddata);
end

function GameLayer:sendGuess(index, cardData)
    local cmddata = CCmd_Data:create()
    cmddata:pushword(index)
    cmddata:pushword(self.wGuessCount)
    cmddata:pushbool(self.bguessBig)
    cmddata:pushbyte(cardData)

    self:SendData(cmd.SUB_C_GUESS, cmddata);
end

function GameLayer:sendClickGuess()
    local cmddata = CCmd_Data:create(0)

    self:SendData(cmd.SUB_C_GAME_CLICK_GUESS, cmddata);
end

function GameLayer:sendGameAuto(bauto)
    local cmddata = CCmd_Data:create()
    cmddata:pushbool(bauto)
    self:SendData(cmd.SUB_C_GAME_AUTO, cmddata);
end

------网络接收

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
    print("场景数据:" .. cbGameStatus);
    

    self:KillGameClock();
    --0.重置本局数据
    -- self._gameView:ReSetGameData();

    --初始化个人信息
    self._gameView:initSelfInfo();

    self:OnResetGameEngine()


    local cmd_data = nil;
    local currentGameStatus = cmd.GAME_SCENE_FREE;
    self.m_cbGameStatus=0;
    --空闲场景
    print(cbGameStatus == cmd.GAME_SCENE_FREE)
    if cbGameStatus == cmd.GAME_SCENE_FREE   then                        --空闲状态
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusGameFree, dataBuffer);
        dump(cmd_data,"空闲状态",6);
        self._gameView:setBounsInfo(cmd_data.bonus)
        self.recordInfo = cmd_data.recordInfo   

        self.lBetItem = cmd_data.lBetItem[1]
        -- self.lBet = cmd_data.lBet
        self._gameView:setBetNum(1)

        self.lExchangeRadio = cmd_data.lExchangeRadio -- 兑换比例
        self._gameView:updateScore( self:GetMeUserItem().lScore )

    elseif cbGameStatus == cmd.GAME_STATUS_PLAY then  --下注状态、结束状态
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusGamePlay, dataBuffer);
        dump(cmd_data,"游戏中",6);
        self.cbFirstCardArray = cmd_data.cbFirstCardData[1]
        self.cbSecondCardArray = cmd_data.cbSecondCardData[1]
        self.recordInfo = cmd_data.recordInfo

        self._gameView.auto = cmd_data.bAuto
        self.lExchangeRadio = cmd_data.lExchangeRadio -- 兑换比例
        self._gameView:updateScore( self:GetMeUserItem().lScore )

        -- 筹码配置
        self.lBetItem = cmd_data.lBetItem[1]
        self.lBet = cmd_data.lBet
        self._gameView:setBetNum(1)

        if cmd_data.balreadySwitch then
            for i = 1, 5 do
                if cmd_data.bSwitchFlag[i] then
                    table.insert(self.handCards, self.cbSecondCardArray[i])
                else
                    table.insert(self.handCards, self.cbFirstCardArray[i])
                end
            end
            self._gameView.currStatus = self._gameView.SWITCH
        else
            self.handCards = clone(self.cbFirstCardArray)
            self._gameView.currStatus = self._gameView.SWITCH
        end

        self._gameView:setBtnStatus()
        self._gameView:setHandCard(self.handCards)
        self._gameView:setBounsInfo(cmd_data.bonus)

    elseif cbGameStatus == cmd.GS_GAME_LUCKYTIME then
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusLuckyTime, dataBuffer);
        dump(cmd_data,"luckyTime",6);

    -- {k = "lbureauCount",       t = "int"},                         --游戏局数
    -- {k = "lExchangeRadio",        t = "int"},                          --兑换比例
    -- {k = "lBet",               t = "int"},                          --压注
    -- {k = "bLuckyTimePause",        t = "bool"},                          --暂停标识
    -- {k = "guessRecord",        t = "table", d = cmd.GUESSRECORD, l = {6}},   --猜大小记录
    -- {k = "cbGuessCardResultRecord",  t = "byte", l = {6}},                  --猜大小牌记录
    -- {k = "wCurrentGuessCount",        t = "word"},                          --猜大小局数
    -- {k = "cbLuckyCardData",        t = "byte", l = {13}},                          --LuckyCard
    -- {k = "cbHandCardData",        t = "byte", l = {cmd.MAX_CARD_COUNT}},           --手牌扑克

    
    -- {k = "recordInfo",    t = "table", d = cmd.RECORD_INFO},                         --游戏局数
    -- {k = "wTableID",       t = "word"},                         --桌子ID
    -- {k = "szServerName",       t = "string", s =cmd.SERVER_LEN},    
    -- {k = "lBetItem",       t = "score", l = {10}},  -- 筹码配置

        self.cbLuckyCardData = cmd_data.cbLuckyCardData[1]

        self.lExchangeRadio = cmd_data.lExchangeRadio -- 兑换比例
        self._gameView:updateScore( self:GetMeUserItem().lScore )

        self.wGuessCount = cmd_data.wCurrentGuessCount
        self.handCards = cmd_data.cbHandCardData[1]

        --猜大小记录
        self.cbGuessCardResultRecord = cmd_data.cbGuessCardResultRecord[1]

        -- 筹码配置
        self.lBetItem = cmd_data.lBetItem[1]
        self.lBet = cmd_data.lBet
        self._gameView:setBetNum(1)
        for k,v in pairs(self.lBetItem) do
            if v == self.lBet then
                self._gameView:setBetNum(k)
            end
        end

        self._gameView:setBounsInfo(cmd_data.bonus)

        self._gameView:showLuckyTime()
    end


    --8.播放背景音乐
    -- ExternalFun.playBackgroudAudio(cmd.SOUND_GameSceneBGM);
end

----------------------------------------------------
-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)  
    if self.m_bLeaveGame or nil == self._gameView then
        return
    end 
    print("游戏消息---sub", sub);
	if sub == cmd.SUB_S_GAME_FREE then   --游戏空闲
        -- print("游戏空闲");
        self.m_cbGameStatus = cmd.GAME_SCENE_FREE;
		self:onSubGameFree(dataBuffer);
	elseif sub == cmd.SUB_S_GAME_START then --游戏开始
        -- print("游戏开始");

		self:onSubGameStart(dataBuffer);
	elseif sub == cmd.SUB_S_SWITCH_CARD then --转换扑克
        self:onSubSwitchCard(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_CONCLUDE then --结算
        self:onSubGameConclude(dataBuffer)

    elseif sub == cmd.SUB_S_LUCKYTIME then --SUB_S_LUCKYTIME
        self:onSubGameLuckyTime(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_GUSS_INDEX then --结算
        self:onSubGameGuessCard(dataBuffer)
	else
		-- print("unknow gamemessage sub is ==>"..sub)
	end
end

--游戏空闲
function GameLayer:onSubGameFree(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameFree, dataBuffer);
    dump(cmd_table,"游戏空闲",6);
    
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    self:OnResetGameEngine()
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart,dataBuffer);
    dump(cmd_table,"游戏开始",6)
    self.cbFirstCardArray = cmd_table.cbFirstCardArray[1]
    self.cbSecondCardArray = cmd_table.cbSecondCardArray[1]
    self._gameView:sendCards(self.cbFirstCardArray)
    self.handCards = clone(cmd_table.cbFirstCardArray[1])
    -- self._gameView:sendCards(cmd_table.cbFirstCardArray[1])
end

--转换扑克
function GameLayer:onSubSwitchCard(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SwitchCard, dataBuffer);
    dump(cmd_table,"转换扑克",6);
    self.bGuess = cmd_table.bGuess
    local flag = cmd_table.switchCard.bSwitchFlag[1]
    self.handCards = {}
    for i = 1, 5 do
        if flag[i] then
            table.insert(self.handCards, self.cbSecondCardArray[i])
        else
            table.insert(self.handCards, self.cbFirstCardArray[i])
        end
    end
    self._gameView:switchCards(cmd_table.switchCard.bSwitchFlag[1], self.bGuess)
    dump(self.handCards,"handCards")
end

--结算
function GameLayer:onSubGameConclude(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameConclude, dataBuffer);
    dump(cmd_table,"结算",6);

    if tolua.cast(self._gameView.luckLayer, "cc.Node") then
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1),
            cc.CallFunc:create( function() self._gameView:showResult(cmd_table.lUserScore)end)))
    else
        self._gameView:showResult(cmd_table.lUserScore)
    end
    self._gameView:setBounsInfo(cmd_table.bonus)
    self.recordInfo = cmd_table.recordInfo
end

function GameLayer:onSubGameLuckyTime(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_LuckyTime, dataBuffer);
    dump(cmd_table,"进入lucky")
    self.cbLuckyCardData = cmd_table.cbLuckyCardData[1]
    --猜大小记录
    self.cbGuessCardResultRecord = cmd_table.cbGuessCardResultRecord[1]
    self._gameView:showLuckyTime()
end

function GameLayer:onSubGameGuessCard(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_CLICK_Guess, dataBuffer);
    dump(cmd_table,"guss",6);
    local index = cmd_table.cbIndex
    self.guessCardData = self.cbLuckyCardData[cmd_table.cbIndex + 1]
    self:sendGuess(index, self.guessCardData)
    print(self.guessCardData,"self.guessCardData")
    self._gameView:GuessResult(self.guessCardData)
end


function GameLayer:onEventUserEnter(wTableID,wChairID,useritem)
    print("add user " .. useritem.wChairID .. "; nick " .. useritem.szNickName);
    --缓存用户
end

function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    print("change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName);
  
end

function GameLayer:onEventUserScore(item)
    if item.dwUserID == self:GetMeUserItem().dwUserID then
        self._gameView:updateScore(item.lScore)
    end
end

---------------------------------------------------------------------------------------
return GameLayer
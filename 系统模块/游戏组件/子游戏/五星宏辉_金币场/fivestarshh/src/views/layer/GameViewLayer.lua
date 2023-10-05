local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.fivestarshh.src"

--external
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"

local cmd = appdf.req(module_pre..".models.CMD_Game")

local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local QueryDialog   = require("app.views.layer.other.QueryDialog")

local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local HelpLayer = appdf.req(module_pre .. ".views.layer.HelpLayer")
local TrendInfoLayer = appdf.req(module_pre .. ".views.layer.TrendInfoLayer")
local BankerListLayer = appdf.req(module_pre .. ".views.layer.BankerListLayer")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local CardSprite = appdf.req(module_pre..".views.layer.CardSprite")
local BankLayer = appdf.req(module_pre..".views.layer.BankLayer")
local RewardLayer = appdf.req(module_pre..".views.layer.RewardLayer")


GameViewLayer.TAG_START				= 100
local enumTable = 
{
	"BT_Exit",               -- 退出游戏
	"BT_Menu",               -- 菜单
	"BT_LuDan",             -- 路单
	"BT_CutGaiLv",        -- 切换概率
	"BT_CutJuShu",            -- 切换局数
	"BT_Help",               -- 帮助
	"BT_Set",                -- 设置
	"BT_ExchangeBet",         -- 筹码切换
    "BT_JETTONSCORE_0",      --下注按钮
    "BT_JETTONSCORE_1",
    "BT_JETTONSCORE_2",
    "BT_JETTONSCORE_3",
    "BT_JETTONSCORE_4",
    "BT_APPLY_BANKER",
    "BT_BANK",
	"BT_CLEAR",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(GameViewLayer.TAG_START, enumTable);

--筹码对应的数值
local ChoumaType =
{
	[1] = 100,
	[2] = 10000,
	[3] = 100000,
	[4]	= 1000000,
	[5] = 5000000
}

local enumApply =
{
    "kCancelState",  -- 取消申请
    "kApplyState",   -- 申请上庄
    "kApplyedState"  -- 申请下庄
}

GameViewLayer._apply_state = ExternalFun.declarEnumWithTable(0, enumApply);
local APPLY_STATE = GameViewLayer._apply_state;

--默认选中的筹码
local DEFAULT_BET = 1
--筹码运行时间
local BET_ANITIME = 0.2

function GameViewLayer:ctor(scene)
	--注册node事件
	ExternalFun.registerNodeEvent(self)
	
	self._scene = scene

	self:loadRes();
	self:gameDataInit();

	--初始化csb界面
	self:initCsbRes();
	--初始化通用动作
	-- self:initAction();
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    if self.m_bankLayer then
  --  这个暂时屏蔽      self.m_bankLayer:refreshBankRate(bankinfo.wRevenueTake, bankinfo.lBankOPButtonArray[1])
    end
end

--移除银行
function GameViewLayer:removeBankLayer()
    if self.m_bankLayer then
        self.m_bankLayer:removeFromParent()
        self.m_bankLayer = nil
    end
end

function GameViewLayer:getParentNode(  )
	return self._scene
end

function GameViewLayer:getMeUserItem(  )
	if nil ~= GlobalUserItem.dwUserID then
		return self:getDataMgr():getUidUserList()[GlobalUserItem.dwUserID];
	end
	return nil;
end

function GameViewLayer:isMeChair( wchair )
	local useritem = self:getDataMgr():getChairUserList()[wchair + 1];
	if nil == useritem then
		return false
	else 
		return useritem.dwUserID == GlobalUserItem.dwUserID
	end
end

function GameViewLayer:getDataMgr( )
	return self:getParentNode():getDataMgr()
end

--银行操作成功
function GameViewLayer:onBankSuccess( )
    local bank_success = self:getParentNode().bank_success
    if nil == bank_success then
        return
    end
    
    GlobalUserItem.lUserScore= bank_success.lUserScore
    GlobalUserItem.lUserInsure = bank_success.lUserInsure

    if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
        self.m_bankLayer:refreshBankScore()
    end
    showToast(self, bank_success.szDescribrString, 2)
end

--银行操作失败
function GameViewLayer:onBankFailure( )
    local bank_fail = self:getParentNode().bank_fail
    if nil == bank_fail then
        return
    end
    print("银行操作失败",bank_fail.szDescribeString)
    showToast(self, bank_fail.szDescribeString, 2)
end

function GameViewLayer:loadRes()
	--加载卡牌纹理
	-- cc.Director:getInstance():getTextureCache():addImage("game/card.png");

    display.loadSpriteFrames(cmd.RES.."game/game.plist",cmd.RES.."game/game.png")
end

---------------------------------------------------------------------------------------
--界面初始化
function GameViewLayer:initCsbRes(  )
	local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameLayer.csb", self);
	self.m_rootLayer = rootLayer

	self._csbNode = csbNode;

	-- 初始化ui
	self:initUi(csbNode);
end

function GameViewLayer:gameDataInit()
    self.m_hasJetton = false
	--游戏状态
    self.m_cbGameStatus = cmd.GAME_SCENE_FREE;
    --剩余时间
    self.cbTimeLeave = 0;

    --显示分数
    self.m_showScore =  0;

    -- 玩家资产
    self.m_PlayerMoney = 0;
    -- 玩家总输赢
    self.lUserAllWinLose = 0;

    --单局个人下注总数限制
    self.lUserLimitScore = 0;
    --下注总数限制
    self.lAreaLimitScore = 0;

    -- 当前单次下注筹码数
    self.m_CurrentBetNum = 0;

    --倒计时Layout
    self.m_timeLayout = nil;

    -- 玩家本轮的总下注数
    self.m_CurrentJettonCount = {};
    self.m_CurrentJettonCount[cmd.AREA_FANG] = 0;
    self.m_CurrentJettonCount[cmd.AREA_MEI] = 0;
    self.m_CurrentJettonCount[cmd.AREA_HONG] = 0;
    self.m_CurrentJettonCount[cmd.AREA_HEI] = 0;
    self.m_CurrentJettonCount[cmd.AREA_KING] = 0;
    --本轮全局总下注数
    self.m_CurrentAllJettonCount = {};
    self.m_CurrentAllJettonCount[cmd.AREA_FANG] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_MEI] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_HONG] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_HEI] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_KING] = 0;
    
    -- 下注切换按钮
    self.BetNumButtons = {};

    --玩家椅子号
    self.m_UserChairID = self._scene:GetMeChairID();

    --玩家本轮赢分
    self.m_PlayerWinScore = 0;
    self.m_PlayerAllWinScore = 0;
    --下注值索引
    self.betIndex = 0;
    --下注值数组
    self.lChipCount = {1000,10000,100000,100000,100000};

    --大路单
    self.cbLuDanBigCount = 0;
    self.cbLuDanBigData = {};

    --小路单
    self.cbLuDanSmallCount = 0;
    self.cbLuDanSmallData = {};

    --最后一个路单记录位置
    self.lastRecordPosX = 0;
    self.lastRecordPosY = 0;

    --局数、花色
    self.nGameCount = 0;
    self.nOpenColorCount = {};

    --开奖卡牌
    self.m_openRewardCard = nil;
    -- 是否游戏结束
    self.isEndGame = false;
    --大奖广播
    self.m_infoList = {};
    self.isHornMoving = false;

    -- self.fireSpin = nil;
    --是否显示局数
    self.isShowJuShu = true;

    self.waitStationLayer = nil;
    self.waitCurTime = 0;

    --上庄列表层
    self.m_applyListLayer = nil;
    --当前庄家
    self.m_wBankerUser = 255;
    --当前庄家分数
    self.m_lBankerScore = 0;
    --申请状态
    self.m_enApplyState = APPLY_STATE.kCancelState;
    --申请上庄限制
    self.m_llCondition = 0;
    --是否显示庄家
    -- self.bIsAllowApplyBanker = false;
    --庄家赢分
    self.m_lBankerWinScore = 0;
    --庄家局数
    self.cbBankerTime = 0;
    --是否已申请下庄
    self.bIsClickCancelBanker = false;

end

--重置本局数据
function GameViewLayer:ReSetGameData()
    self.m_hasJetton = false
    -- 玩家本轮的总下注数
    self.m_CurrentJettonCount = {};
    self.m_CurrentJettonCount[cmd.AREA_FANG] = 0;
    self.m_CurrentJettonCount[cmd.AREA_MEI] = 0;
    self.m_CurrentJettonCount[cmd.AREA_HONG] = 0;
    self.m_CurrentJettonCount[cmd.AREA_HEI] = 0;
    self.m_CurrentJettonCount[cmd.AREA_KING] = 0;
    --本轮全局总下注数
    self.m_CurrentAllJettonCount = {};
    self.m_CurrentAllJettonCount[cmd.AREA_FANG] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_MEI] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_HONG] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_HEI] = 0;
    self.m_CurrentAllJettonCount[cmd.AREA_KING] = 0;

    --玩家本轮赢分
    self.m_PlayerWinScore = 0;

    --最后一个路单记录位置
    self.lastRecordPosX = 0;
    self.lastRecordPosY = 0;

    --开奖卡牌
    -- self.m_openRewardCard = {};
    -- 是否游戏结束
    self.isEndGame = false;

    --大奖广播
    -- self.m_infoList = {};
    self.isHornMoving = false;

    self.waitStationLayer = nil;
    
    --各区域总下注
    self:initAreaTotalJettonLabel();
    --各区域我的下注
    self:initAreaUserJettonLabel();

    if self.rewardLayer then
        self.rewardLayer:setVisible(false);
    end

	-- self.node_menu:stopAllActions();
	-- self.btn_menu:stopAllActions();
	self:stopAllActions();
	
	self.node_effect:removeAllChildren();
    self.node_gold:removeAllChildren();
    self.node_bet_effect:removeAllChildren();
    if self.bet_time then
        self.bet_time:setVisible(true);
    end
end

--初始化按钮
function GameViewLayer:initUi(csbNode)
	self.bg = csbNode:getChildByName("bg");

	--按钮列表
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender);
		end
	end	

	-- 按钮
	local btn_back = csbNode:getChildByName("btn_back");
	btn_back:setTag(TAG_ENUM.BT_Exit);
	btn_back:addTouchEventListener(btnEvent);

	local btn_menu = csbNode:getChildByName("btn_menu");
	btn_menu:setTag(TAG_ENUM.BT_Menu);
	btn_menu:addTouchEventListener(btnEvent);
	self.btn_menu = btn_menu;

	local node_menu = csbNode:getChildByName("node_menu");
	self.node_menu = node_menu;
	-- local menu_bg = node_menu:getChildByName("menu_bg");

	--桌子操作按钮屏蔽层
	local callbackShield = function(ref)
        self:onMenuAction(false)
        self.menuIsUp = false;
	end
	self.layoutShield = csbNode:getChildByName("image_touch");
	self.layoutShield:setVisible(true);
	self.layoutShield:setTouchEnabled(false);
	self.layoutShield:addClickEventListener(callbackShield);

	node_menu:setVisible(false);
	self:onMenuAction(false);

	local btn_back = node_menu:getChildByName("btn_back");
	btn_back:setTag(TAG_ENUM.BT_Exit);
	btn_back:addTouchEventListener(btnEvent);
    btn_back:setVisible(false);

	local btn_help = node_menu:getChildByName("btn_help");
	btn_help:setTag(TAG_ENUM.BT_Help);
	btn_help:addTouchEventListener(btnEvent);

	local btn_set = node_menu:getChildByName("btn_set");
	btn_set:setTag(TAG_ENUM.BT_Set);
	btn_set:addTouchEventListener(btnEvent);

    local btn_bank = csbNode:getChildByName("btn_bank");
    btn_bank:setTag(TAG_ENUM.BT_BANK);
    btn_bank:addTouchEventListener(btnEvent);

    -- local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    -- if not enableBank then
        -- btn_bank:setVisible(false)
        -- -- menu_bg:setContentSize(74,200)
    -- end
    
    --路单
    local btn_record = csbNode:getChildByName("btn_record");
    btn_record:setTag(TAG_ENUM.BT_LuDan);
    btn_record:addTouchEventListener(btnEvent);
    
    local btn_clear = csbNode:getChildByName("button_xuya");
    btn_clear:setTag(TAG_ENUM.BT_CLEAR);
    btn_clear:addTouchEventListener(btnEvent);
--    -- 是否可上庄
    local node_banker = csbNode:getChildByName("node_banker");
    self.node_banker = node_banker;
    local banker_name = node_banker:getChildByName("banker_name");
    local banker_gold = node_banker:getChildByName("banker_gold");
    local icon_gold = node_banker:getChildByName("icon_gold");
    self.banker_name = banker_name;
    self.banker_gold = banker_gold;
    self.icon_gold = icon_gold;
    local btn_apply_banker = node_banker:getChildByName("btn_apply_banker");
    btn_apply_banker:setTag(TAG_ENUM.BT_APPLY_BANKER);
    btn_apply_banker:addTouchEventListener(btnEvent);


    local node_layer = csbNode:getChildByName("node_layer");
    self.node_layer = node_layer;

    --各区域总下注
    self:initAreaTotalJettonLabel();

    --各区域我的下注
    self:initAreaUserJettonLabel();

    --下注
    local node_bet_btn = csbNode:getChildByName("node_bet_btn");
    --筹码切换
    local btn_exchange_bet = node_bet_btn:getChildByName("btn_exchange_bet");
    btn_exchange_bet:setTag(TAG_ENUM.BT_ExchangeBet);
    btn_exchange_bet:addTouchEventListener(btnEvent);

    local cur_bet_num = node_bet_btn:getChildByName("bottom"):getChildByName("Text_2");
    self.cur_bet_num = cur_bet_num;
    self.cur_bet_num:setString("1000")
    -- local node_bet_btn = csbNode:getChildByName("node_bet_btn");
    for i = 1, 5 do
        local btn_bet = node_bet_btn:getChildByName("btn_bet_" .. i);
        btn_bet:setTag(TAG_ENUM.BT_JETTONSCORE_0 + i - 1);
        btn_bet:addTouchEventListener(btnEvent);

        self.BetNumButtons[i] = btn_bet;
    end

    --路单
    local node_record = csbNode:getChildByName("node_record");
    self.node_record = node_record;
    local node_ludan = csbNode:getChildByName("node_ludan");
    self.node_ludan = node_ludan;
    --切换到概率
    local btn_jushu = node_ludan:getChildByName("btn_jushu");
    btn_jushu:setTag(TAG_ENUM.BT_CutJuShu);
    btn_jushu:addTouchEventListener(btnEvent);
    --切换到局数
    local btn_gailv = node_ludan:getChildByName("btn_gailv");
    btn_gailv:setTag(TAG_ENUM.BT_CutGaiLv);
    btn_gailv:addTouchEventListener(btnEvent);
    btn_gailv:setVisible(false);
    --动画效果
	self.node_effect = csbNode:getChildByName("node_effect");

    self.node_bet_effect = csbNode:getChildByName("node_bet_effect");

	self.node_gold = csbNode:getChildByName("node_gold");
    --
	self.node_left = csbNode:getChildByName("node_left");
    local card_type = self.node_left:getChildByName("card_type");
    card_type:setVisible(false);
    self.node_card = self.node_left:getChildByName("node_card");
    self.card_record = csbNode:getChildByName("card_record");
    self.card_record:setVisible(false);

    self.node_fire = self.node_left:getChildByName("node_fire");
    self.node_cut_time = self.node_left:getChildByName("node_cut_time");

    self.m_timeLayout = self.node_left:getChildByName("node_time");
    self.m_timeLayout:setVisible(false);
    self.icon_bet = self.m_timeLayout:getChildByName("icon_bet");
    self.bet_time = self.m_timeLayout:getChildByName("bet_time");

    --初始化庄家
   -- self:initBankerInfo();

    local node_bet_result = csbNode:getChildByName("node_bet_result");
    local total_cur_num = node_bet_result:getChildByName("total_cur_num");
    self.total_cur_num = total_cur_num;
    local total_win_num = node_bet_result:getChildByName("total_win_num");
    local total_lose_num = node_bet_result:getChildByName("total_lose_num");
    self.total_lose_num = total_lose_num;
    self.total_win_num = total_win_num;
    self.total_lose_num:setVisible(false);

	-- self:initSelfInfo();
    local circle_num = self.node_left:getChildByName("circle_num");
    local game_num = self.node_left:getChildByName("game_num");
    circle_num:setString("");
    game_num:setString("");

    --广播背景框
    self.hornBottom = csbNode:getChildByName("bottom_2");
    self.hornBottom:setVisible(false);

    self:freshUserAllWinLose();

	-- 卡牌创建
	self:openRewardCardCreate();

end
--申请上庄
function GameViewLayer:onApplyBanker(cmd_table)
    if self:isMeChair(cmd_table.wApplyUser) == true then
        self.m_enApplyState = APPLY_STATE.kApplyState;
    end

    self:refreshApplyList();
end

--切换庄家
function GameViewLayer:onChangeBanker(cmd_table)
    --上一个庄家是自己，且当前庄家不是自己，标记自己的状态
    if self.m_wBankerUser ~= cmd_table.wBankerUser and self:isMeChair(self.m_wBankerUser) then
        self.m_enApplyState = APPLY_STATE.kCancelState;
    end
    self.m_wBankerUser = cmd_table.wBankerUser;
    self.m_lBankerScore = cmd_table.lBankerScore;
    --self:resetBankerInfo();

    --自己上庄
    if self:isMeChair(cmd_table.wBankerUser) == true then
        showToast(self, "上庄成功!", 2);
        self.m_enApplyState = APPLY_STATE.kApplyedState;
    end
end

--取消申请
function GameViewLayer:onGetCancelBanker(cmd_table)
    if self:isMeChair(cmd_table.wCancelUser) == true then
        self.m_enApplyState = APPLY_STATE.kCancelState;
    end

    self:refreshApplyList();
end

--抢庄条件
function GameViewLayer:onGetApplyBankerCondition(lApplyBankerCondition)
    self.m_llCondition = lApplyBankerCondition;
end

--是否显示抢庄
function GameViewLayer:onIsShowApplyBanker(bIsAllowApplyBanker)
    self.bIsAllowApplyBanker = bIsAllowApplyBanker;

    self.node_banker:setVisible(self.bIsAllowApplyBanker);
end

--初始化庄家信息
function GameViewLayer:initBankerInfo()
    self.banker_name:setString("无人坐庄");
    self.banker_gold:setString("");
    self.icon_gold:setVisible(false);
end

--刷新庄家信息
function GameViewLayer:resetBankerInfo()
    -- local userItem = self:getDataMgr():getChairUserList()[self.m_wBankerUser+1];
    -- if nil ~= userItem then
        -- local name = string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 26));
        -- self.banker_name:setString(name);

        -- local bankerstr = self:formatScoreText(self.m_lBankerScore);
        -- self.banker_gold:setString(bankerstr);
        -- self.icon_gold:setVisible(true);
    -- else
        -- self.banker_name:setString("系统坐庄");
        -- local bankerstr = self:formatScoreText(self.m_lBankerScore);
        -- self.banker_gold:setString("");
        -- self.icon_gold:setVisible(false);
    -- end
end

--刷新庄家分数
function GameViewLayer:resetBankerScore()
    -- local gold_num = self.node_banker:getChildByName("gold_num");
    -- local bankerstr = ExternalFun.formatScoreText(self.m_lBankerScore);
    if self.m_lBankerScore > 0 then
        local bankerstr = self:formatScoreText(self.m_lBankerScore);
        self.banker_gold:setString(bankerstr);
    end
end

--刷新申请上庄列表
function GameViewLayer:refreshApplyList(  )
    if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
        local userList = self:getDataMgr():getApplyBankerUserList()     
        self.m_applyListLayer:refreshList(userList)
    end
end

--刷新申请列表按钮状态
function GameViewLayer:refreshApplyBtnState(  )
    if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
        self.m_applyListLayer:refreshBtnState();
    end
end

function GameViewLayer:getApplyState(  )
    return self.m_enApplyState;
end

function GameViewLayer:getApplyCondition(  )
    return self.m_llCondition;
end

--获取能否上庄
function GameViewLayer:getApplyable(  )
    local userItem = self:getMeUserItem();
    if nil ~= userItem then
        return userItem.lScore > self.m_llCondition;
    else
        return false;
    end
end

--获取能否取消上庄
function GameViewLayer:getCancelable(  )
    -- return self.m_cbGameStatus == cmd.GAME_SCENE_FREE;
    return true;
end

--上庄状态
function GameViewLayer:applyBanker( state )
    print("-----上庄状态-----", state, APPLY_STATE.kCancelState, APPLY_STATE.kApplyState, APPLY_STATE.kApplyedState)
    if state == APPLY_STATE.kCancelState then
        self._scene:sendCancelApply(); 
    elseif state == APPLY_STATE.kApplyState then
        self._scene:sendApplyBanker();
    elseif state == APPLY_STATE.kApplyedState then
        self._scene:sendCancelApply();    
    end
end

------------------------------------------------
--设置当前游戏状态
function GameViewLayer:SetCurrentGameStatus(gameStatus)
    --根据状态替换状态提示Label
    self:SetSystemTip(false, gameStatus)

    self.m_cbGameStatus = gameStatus;

    if cmd.GAME_SCENE_FREE == gameStatus and nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible()  and self:isMeChair(self.m_wBankerUser) then
        showToast(self,"现在是空闲时间，可以进行银行操作",1)
    end
end

--定时器调用更新倒计时
function GameViewLayer:OnUpdataClockView(viewId, cbTime)
    cbTime = cbTime;
	self.bet_time:setString(cbTime);

    if cbTime == 5 then
        -- 倒计时
        self.bet_time:setVisible(false);
        self:PlayCutTimeAnim();
    end

    if cbTime < 6 and cbTime ~= 0 then
        ExternalFun.playSoundEffect(cmd.EFFECT_Countdown);
    end
end

function GameViewLayer:resertShowCutTimeLabel()
    if self.bet_time then
        self.node_cut_time:removeAllChildren();
        self.bet_time:setVisible(true);
    end 
end

--筹码按钮选中状态 手动切换
function GameViewLayer:SwitchBetNumToggle(toggleIndex)
    --所有按钮置灰
    for i=1,#self.BetNumButtons do
        self.BetNumButtons[i]:setOpacity(200);
    end

    if toggleIndex ~= 0 then
        self.BetNumButtons[toggleIndex]:setOpacity(255);
    end
end

--启用/禁用 游戏按钮
function GameViewLayer:SwitchButtonEnabled(isEnable)
    --筹码切换按钮
    for i=1,#self.BetNumButtons do
        if isEnable then
        	isEnable = self:CheckPlayerMoneyForBetNumBtn() and self:CheckMaxScoreForBetNumBtn(i);
        else
        end

        --当庄时不能下注
        local myChairId = self._scene:GetMeChairID();

        if self.m_wBankerUser == myChairId then
            print("SwitchButtonEnabled 1 ")
            print("self.m_wBankerUser="..self.m_wBankerUser.."myChairId="..myChairId)
            self.BetNumButtons[i]:setEnabled(false);
            self:PlayJettonBtnAnim(self.BetNumButtons[i], true);
        else
            print("SwitchButtonEnabled 2 ")
            self.BetNumButtons[i]:setEnabled(isEnable);
            self:PlayJettonBtnAnim(self.BetNumButtons[i], not isEnable);
        end
    end
end

--判断玩家资产是否足够启用筹码按钮
function GameViewLayer:CheckPlayerMoneyForBetNumBtn()
    local isEnable = false
    if self.m_PlayerMoney >= self.m_CurrentBetNum then
        isEnable = true
    end

    print("self.m_PlayerMoney="..self.m_PlayerMoney.."self.m_CurrentBetNum="..self.m_CurrentBetNum)
    return isEnable
end

--根据最大下注限制 判断 是否要启用筹码按钮
function GameViewLayer:CheckMaxScoreForBetNumBtn(betNumBtnIndex)
    local isEnable = false

    local playerJettonTotal = 0;
    for i = 1, 5 do
        local index = self:getIndexByViewId(i);
        playerJettonTotal = playerJettonTotal + self.m_CurrentJettonCount[index-1];
    end

    -- local playerJettonTotal = self.m_CurrentJettonCount[cmd.RED_INDEX] + self.m_CurrentJettonCount[cmd.BLACK_INDEX] + self.m_CurrentJettonCount[cmd.POKER_TYPE_INDEX]
    


    local availableBetNum = self.lUserLimitScore - playerJettonTotal;
    if availableBetNum >= self.m_CurrentBetNum then
        isEnable = true
    -- elseif self.m_CurrentBetNum == BetNumButtonValue[betNumBtnIndex] then
    --     --当前选择的单次下注数 大于 剩余单局可下注 时，清空当前单次下注数
    --     self:UpdateCurrentBetNum(0);
    --     --取消按钮Toggle选中
    --     self:SwitchBetNumToggle(0);
    end

    print("availableBetNum="..availableBetNum.."self.m_CurrentBetNum="..self.m_CurrentBetNum)

    return isEnable
end

--初始化下注限制
function GameViewLayer:InitUserMaxJetton(userMaxScore, areaLimitScore)
    self.lUserLimitScore = userMaxScore;
    self.lAreaLimitScore = areaLimitScore;
end

--初始化玩家已下注的Label
function GameViewLayer:InitUserAndAllJettonLabel(allBet, playerBet)
    for i=1,5 do
        local index = self:getIndexByViewId(i);

        local userAreaScore = playerBet[index];
        --更新玩家区域总下注Label
        self:SetAreaUserJettonLabel(i, userAreaScore);

        local areaTotalScore = allBet[index];
        --更新总下注Label

        self:SetAreaTotalJettonLabel(i, areaTotalScore);
    end
end

--用户下注点击事件
function GameViewLayer:OnPlayerJettonClick(areaIndex, ref)
    --下注音效
    ExternalFun.playSoundEffect(cmd.EFFECT_Jetton);
    print("1111111111")
    if self.m_cbGameStatus ~= cmd.GAME_SCENE_BET or self.m_CurrentBetNum == 0 then return end
    print("222222222")
    --当庄时不能下注
    local myChairId = self._scene:GetMeChairID();
    if self.m_wBankerUser == myChairId then
        showToast(self, "庄家不能下注 ", 2);
        return;
    end
     print("3333333333")
    local index = self:getIndexByViewId(areaIndex);

    --每个区域限制不一样，所以客户端不做限制。服务端会限制
    self._scene:sendPlaceJetton(index -1, self.m_CurrentBetNum);

    
end

--用户下注返回
function GameViewLayer:PlayerJetton(jettonData, isSelf)
    local areaIndex = jettonData.cbArea;
    local jettonScore = jettonData.lBetScore;

    local viewId = self:getViewIdByIndex(areaIndex);

    if isSelf then
        self.m_hasJetton = true
        --玩家下注值累加
        local userTotalJetton = self.m_CurrentJettonCount[areaIndex] + tonumber(jettonScore);
        if userTotalJetton > 0 then
            self.m_CurrentJettonCount[areaIndex] = tonumber(userTotalJetton);
        end
        self:SetAreaUserJettonLabel(viewId, userTotalJetton);
        --手动刷新玩家金币
        local currentUserMoney = self.m_PlayerMoney - tonumber(jettonScore);
        self:UpdateUserMoney(currentUserMoney);
    else
        --其他玩家下注
        -- startPos = PlayerJettonChoumaPos[OtherPlayer];
    end

    --全局下注总数累加
    local allTotalJetton = self.m_CurrentAllJettonCount[areaIndex] + tonumber(jettonScore);
    if allTotalJetton > 0 then
        self.m_CurrentAllJettonCount[areaIndex] = tonumber(allTotalJetton);
    end
    self:SetAreaTotalJettonLabel(viewId, allTotalJetton);
    --判断是否启用筹码按钮
    for i=1,#self.BetNumButtons do
        local isEnabled = self:CheckPlayerMoneyForBetNumBtn(i) and self:CheckMaxScoreForBetNumBtn(i);
        --庄家不能下注
        local myChairId = self._scene:GetMeChairID();
        if self.m_wBankerUser == myChairId then
            self.BetNumButtons[i]:setEnabled(false);
        else
            self.BetNumButtons[i]:setEnabled(isEnabled);
        end
    end
end

--更新系统提示
function GameViewLayer:SetSystemTip(isTime, gameStatus)
	self.m_timeLayout:setVisible(true);
	local bet_icon = self.m_timeLayout:getChildByName("bet_icon");
    local title_icon = self.m_timeLayout:getChildByName("title_icon");

    local frame = nil;
    if gameStatus == cmd.GAME_SCENE_FREE then
        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("title_free_2.png")
    elseif gameStatus == cmd.GAME_SCENE_BET then
        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("title_8.png")
    elseif gameStatus == cmd.GAME_SCENE_END then
        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("title_end_2.png")
    end
    if nil ~= frame then
        self.icon_bet:setSpriteFrame(frame);
    end
end

--刷新自己信息
function GameViewLayer:initSelfInfo()
    --自己
	local roleGoldNum = self._scene._gameFrame:GetMeUserItem().lScore --0921self:formatScoreText(GlobalUserItem.tabAccountInfo.lUserScore);
	self.total_cur_num:setString(roleGoldNum);

	--刷新金币
    local my_gold  = self._scene._gameFrame:GetMeUserItem().lScore; --0921self:getMeUserItem().lScore;
    self:UpdateUserMoney(my_gold);
end

--刷新玩家输赢
function GameViewLayer:freshUserAllWinLose()
    self.m_PlayerAllWinScore = self.m_PlayerWinScore + self.m_PlayerAllWinScore
    local resultNum = self:formatScoreText(self.m_PlayerAllWinScore );
    if self.m_PlayerAllWinScore and self.m_PlayerAllWinScore >= 0 then
        self.total_win_num:setVisible(true);
        self.total_lose_num:setVisible(false);
        self.total_win_num:setString(resultNum);
        self.total_lose_num:setString("." .. resultNum);
    else
        self.total_win_num:setVisible(false);
        self.total_lose_num:setVisible(true);
        self.total_win_num:setString(resultNum);
        self.total_lose_num:setString("." .. resultNum);
    end
end

--系统字分数 0、1、2 代表 不显示、万、亿
function GameViewLayer:formatScoreText(score)
    if not score then
        return;
    end
	local scorestr = ExternalFun.formatScore(score)
    return scorestr;
	-- if score < 10000 then
	-- 	return scorestr, 0;
	-- end

	-- if score < 100000000 then
	-- 	scorestr = string.format("%.1f万", score / 10000)
	-- 	return scorestr, 1;
	-- end
	-- scorestr = string.format("%.1f亿", score / 100000000)
	-- return scorestr, 2;
end

--刷新自己信息
function GameViewLayer:UpdateUserMoney(userMoney)
	self.m_PlayerMoney = tonumber(userMoney);
	
	local selfScore = self:formatScoreText(self.m_PlayerMoney);
    self.total_cur_num:setString(selfScore);

    --刷新银行金币
    --0921GlobalUserItem.tabAccountInfo.lUserScore = self.m_PlayerMoney;
    self._scene._gameFrame:GetMeUserItem().lScore = self.m_PlayerMoney--0921
    if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
        self.m_bankLayer:refreshBankScore()
    end

end

--刷新用户分数
function GameViewLayer:onGetUserScore( useritem )
    --自己
    --0921if useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
    if useritem.dwUserID == GlobalUserItem.dwUserID then
        self.m_showScore = useritem.lScore
        self:UpdateUserMoney(useritem.lScore);
    end
end

--单次下注筹码数量切换按钮事件
function GameViewLayer:OnBetNumButtonClick(tag,ref)
    -- local currentBetNum = BetNumButtonValue[tag];
    -- self:UpdateCurrentBetNum(currentBetNum);
    -- self:SwitchBetNumToggle(tag);
    --下注区域、下注值
end

--修改单次下注筹码数量
function GameViewLayer:UpdateCurrentBetNum(betNum)

    print("!!!!!!!!!!!UpdateCurrentBetNum self.m_CurrentBetNum="..betNum)
    self.m_CurrentBetNum = tonumber(betNum);
end

--更新玩家区域总下注Label
function GameViewLayer:SetAreaUserJettonLabel(areaIndex, totalNum)
    local node_bet_result = self._csbNode:getChildByName("node_bet_result");
    local selfbet_num = node_bet_result:getChildByName("self_bet_" .. areaIndex);
    selfbet_num:setString(totalNum);
    selfbet_num:setVisible(true);
end

--更新区域总下注Label
function GameViewLayer:SetAreaTotalJettonLabel(areaIndex, totalNum)
    local node_bet_result = self._csbNode:getChildByName("node_bet_result");
    local totalbet_num = node_bet_result:getChildByName("total_bet_" .. areaIndex);
    totalbet_num:setString(0);
    totalbet_num:setString(totalNum);
    totalbet_num:setVisible(true);
end

--隐藏区域总下注Label
function GameViewLayer:initAreaTotalJettonLabel()
    local node_bet_result = self._csbNode:getChildByName("node_bet_result");
	for i = 1, 5 do
		local totalbet_num = node_bet_result:getChildByName("total_bet_" .. i);
		totalbet_num:setString(0);
	end
end

--隐藏玩家区域总下注Label
function GameViewLayer:initAreaUserJettonLabel()
    local node_bet_result = self._csbNode:getChildByName("node_bet_result");
    for i = 1, 5 do
        local selfbet_num = node_bet_result:getChildByName("self_bet_" .. i);
        selfbet_num:setString(0);
    end
end

--下注筹码切换
function GameViewLayer:onExchangeBet(isResert)
    --切换筹码音效
    ExternalFun.playSoundEffect(cmd.EFFECT_ExchangeChip);

    if isResert then
        self.betIndex = 0;
    end

    if self.betIndex >= 5 then
        self.betIndex = 1;
    else
        self.betIndex = self.betIndex + 1;
    end

    --保存上次选择下注额度值索引
    if isResert then
        local index = cc.UserDefault:getInstance():getIntegerForKey("fivestarshh_betIndex",0);
        if index >0 then
            self.betIndex = index;
        end
    end
    cc.UserDefault:getInstance():setIntegerForKey("fivestarshh_betIndex", self.betIndex)

    dump(self.lChipCount,"@@@@@@@self.lChipCount=")
    print("@@@@@@@@@@@@@betIndex = "..self.betIndex)
    local betNum = self.lChipCount[self.betIndex];
    print("@@@@@@@@@@@betNum = "..betNum)


 --   self.cur_bet_num:setString("." .. betNum);

    --当前下注值
    
    if betNum > 1000 then
    local num = betNum/10000
         self.cur_bet_num:setString("x"..num.."W");
    else
          self.cur_bet_num:setString("x" .. betNum); 
    end
   self:UpdateCurrentBetNum(betNum); 
    --刷新下注按钮
    if self.m_cbGameStatus == cmd.GAME_SCENE_BET then
        print(" onExchangeBet 1")
        --启用游戏按钮
        self:SwitchButtonEnabled(true);
    else
        print(" onExchangeBet 2")
        self:SwitchButtonEnabled(false);
    end
end

--概率、局数切换显示
function GameViewLayer:freshExchangeRecordShow()
    local node_ludan = self.node_ludan;
    local btn_jushu = node_ludan:getChildByName("btn_jushu");
    local btn_gailv = node_ludan:getChildByName("btn_gailv");

    if not self.isShowJuShu then -- 是否切换到概率
        btn_jushu:setVisible(true);
        btn_gailv:setVisible(false);
    else
        btn_jushu:setVisible(false);
        btn_gailv:setVisible(true);
    end

    local record_num = node_ludan:getChildByName("record_num_1");
    if not self.isShowJuShu then -- 是否切换到概率
        record_num:setString("100%");
        -- record_num:setScale(0.8);
    else
        record_num:setString(self.nGameCount);
        -- record_num:setScale(1.0);
    end
    local ludanData = self.cbLuDanBigData;
    local colorType = {0,0,0,0,0,0}
    for i = 1, #ludanData do
        local cardValue = ludanData[i];
        if cardValue > 0 then
            local cardType = self:getCardColorByCard(cardValue);
            colorType[cardType] = colorType[cardType]+1
        end
    end

    for i = 1, 5 do
        local record_num = node_ludan:getChildByName("record_num_" .. i+1);
        local index = self:getIndexByViewId(i);
        local openColorCount = colorType[index];
        
        local per = 0;
        if 0 ~= self.nGameCount then
            per = (openColorCount / self.nGameCount) * 100;
        end

        if not self.isShowJuShu then -- 是否切换到概率
            record_num:setString(string.format("%.1f%%", per));
            -- record_num:setScale(0.8);
        else
           record_num:setString(colorType[i]);
            -- record_num:setScale(1.0);
        end
    end
end

--更新筹码下注信息  
function GameViewLayer:updatePlusScore()
    -- if self._scene.m_gamedata.lCellScore and self._scene.m_gamedata.lBetScore[1] and #self._scene.m_gamedata.lBetScore[1] > 0 then
    --     local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
    --     self.artScore:setString(score)
    --     self.artTotalScore:setString(score * self.lineNum)
    -- end
end

-- 倒计时
function GameViewLayer:onUpdateTime()
    if self.wait_time_lab then
        self.wait_time_lab:setString(self.waitCurTime);
    end

    self.waitCurTime = self.waitCurTime - 1;

    if self.waitCurTime < 0 then
        self.waitStationLayer:setVisible(false);
        if self._timeLessUpdate then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeLessUpdate);
            self._timeLessUpdate = nil;
        end
    end
end

-----------------------------------------------------------------------------------------------
function GameViewLayer:onResetView()
    self:stopAllActions();
end

function GameViewLayer:onExit()
    self:onResetView();
end

---------------------------------------------------------------------------------
function GameViewLayer:onButtonClickedEvent(tag,ref)
    ExternalFun.playClickEffect();
    if tag == TAG_ENUM.BT_Exit then
        --当庄退出做逃跑处理
        local myChairId = self._scene:GetMeChairID();
        if self.m_wBankerUser == myChairId and self.m_cbGameStatus ~= cmd.GAME_SCENE_FREE then
            -- self:getParentNode():onExitRoom();
            self._scene.m_cbGameStatus = cmd.GAME_SCENE_BET;
            self:getParentNode():onQueryExitGame();
        else
            self:getParentNode():onQueryExitGame();
        end
        
    elseif tag == TAG_ENUM.BT_Menu then
        if self.menuIsUp then
            self:onMenuAction(false)
            self.menuIsUp = false;
        else
            self:onMenuAction(true)
            self.menuIsUp = true;
        end
    elseif tag == TAG_ENUM.BT_LuDan then         --路单
      ---  self._scene:sendQuraryRecord();
      -- self:onTrendInfoLayer();
    elseif tag == TAG_ENUM.BT_CLEAR then         --路单
        -- self._scene:sendQuraryRecord();
               --玩家下注值累加
        local userTotalJetton = 0
        for areaIndex = 0, 4 do
            userTotalJetton = self.m_CurrentJettonCount[areaIndex]+userTotalJetton
            self.m_CurrentAllJettonCount[areaIndex] =  self.m_CurrentAllJettonCount[areaIndex]-self.m_CurrentJettonCount[areaIndex]
            self.m_CurrentJettonCount[areaIndex]=0
            self:SetAreaUserJettonLabel(areaIndex+1, 0)           
            self:SetAreaTotalJettonLabel(areaIndex+1, self.m_CurrentAllJettonCount[areaIndex]);
        end

        --手动刷新玩家金币
        local currentUserMoney = self.m_PlayerMoney + userTotalJetton;
        self:UpdateUserMoney(currentUserMoney);
       self._scene:sendClearJetton()
    elseif tag == TAG_ENUM.BT_CutGaiLv then     -- 概率切换
        self.isShowJuShu = false;
        self:freshExchangeRecordShow()
    elseif tag == TAG_ENUM.BT_CutJuShu then     -- 局数切换
        self.isShowJuShu = true;
        self:freshExchangeRecordShow()
    elseif tag == TAG_ENUM.BT_ExchangeBet then       -- 筹码切换
       self:onExchangeBet()
    elseif tag == TAG_ENUM.BT_Help then
        self:onButtonClickedEvent(TAG_ENUM.BT_Menu);
        self:onHelpLayer();
    elseif tag == TAG_ENUM.BT_Set then
        self:onButtonClickedEvent(TAG_ENUM.BT_Menu);
        self:onSettingLayer();
    elseif tag == TAG_ENUM.BT_APPLY_BANKER then     -- 上庄列表
      --  self:onBankerListLayer();
      self:onTrendInfoLayer();
    elseif tag >= TAG_ENUM.BT_JETTONSCORE_0 and tag <= TAG_ENUM.BT_JETTONSCORE_4 then
        print("点击下注！！！！！！！！！！！！")
        self:OnPlayerJettonClick(ref:getTag()-TAG_ENUM.BT_JETTONSCORE_0+1, ref);

        -- --大奖广播
        -- for i=1, 1 do
        --     local cmd_table = {}
        --     cmd_table.lScore = 500*i;
        --     cmd_table.szNickName = "玩家--" .. i .. "--";
        --     cmd_table.bAI = false;

        --     if nil ~= cmd_table then
        --         --广播
        --         self:isShowTipsBottom(true);
        --         self:ShowAwardTip(cmd_table, 1);
        --     end
        -- end
    elseif tag == TAG_ENUM.BT_BANK then
        --银行
        if 0 == GlobalUserItem.cbInsureEnabled then
            showToast(self, "初次使用，请先开通银行！", 1)
             return 
        end

        self:onBankLayer()
    else
        showToast(self,"功能尚未开放！",1)
    end
end

--播放玩家赢分动画
function GameViewLayer:PlayPlayerWinAnim(selfWinScore, isBigWin)
	--获胜音效
	ExternalFun.playSoundEffect(cmd.EFFECT_Bigwin);

    if self.rewardLayer then
        self.rewardLayer:removeFromParent()
    end

    self.rewardLayer = RewardLayer:create();
    self.rewardLayer.m_parent = self;
    self.rewardLayer:playGoldEfeect()
    self.node_layer:addChild(self.rewardLayer,8);
end

--飞花色
function GameViewLayer:flyCurGameRecord(cardValue)
    --飞花色音效
    ExternalFun.playSoundEffect(cmd.EFFECT_FlyFlower);

    local beginX = 456;
    local beginY = 590;
    local disX = 100;
    local disY = 48;
    local posX = 32;
    local posX1 = 14;

    local cardType = self:getCardColorByCard(cardValue);
    self.card_record:setVisible(true);
    self.card_record:setPosition(cc.p(255, 387));
    self.card_record:setScale(1.0);
    --花色
    local str1 = string.format("card/card_type_%d_1.png", cardType);
    self.card_record:setTexture(str1);

    local ludanNum = self.cbLuDanSmallCount;
    local ludanData = self.cbLuDanSmallData;

    local cbLuDanSmallCount = ludanNum + 1;
    local cbLuDanSmallData = ludanData;

    local scale = 4.0;
    local scale1 = 1.5;
    local scale2 = 0.2;

    local recordPosX = beginX;
    local recordPosY = beginY;
    local lineNum = 0;
    local rowNum = 0;
    if ludanNum == cmd.LU_DAN_COUNT_SMALL then
        --路单已满时清理
        self.node_record:removeAllChildren();
        cbLuDanSmallCount = 1;
        cbLuDanSmallData[1] = cardValue;
    else
        lineNum = math.mod(cbLuDanSmallCount-1,8);
        rowNum = math.floor((cbLuDanSmallCount-1)/8);
        cbLuDanSmallData[cbLuDanSmallCount] = cardValue;
    end
    --移动到位置
    if cardType == cmd.TYPE_5 or cardType == cmd.TYPE_6 then
        recordPosX = beginX + lineNum * disX + posX1;
        recordPosY = beginY - rowNum * disY;
        self.card_record:setScale(scale);
        scale1 = 1.5 * scale;
        scale2 = 0.2 * scale;
    else
        recordPosX = beginX + lineNum * disX;
        recordPosY = beginY - rowNum * disY;
    end

    local delTime = 0.1;
    local moveTime = 1.0;
    --关闭结算弹窗
    if self.rewardLayer then
        self.rewardLayer:setVisible(false);
    end
    self.card_record:runAction(cc.Sequence:create(
        cc.Spawn:create(cc.MoveBy:create(0.3,cc.p(0,10)), cc.ScaleTo:create(0.3, scale1)),
        cc.Spawn:create(cc.MoveTo:create(moveTime,cc.p(recordPosX,recordPosY)), cc.ScaleTo:create(moveTime, scale2)),
        cc.DelayTime:create(delTime),
        cc.CallFunc:create( function()
            --路单刷新
            self.card_record:setVisible(false);
            self:showRecordResult(cbLuDanSmallCount, cbLuDanSmallData)
        end )
    ));
end

--飞金币
function GameViewLayer:flyCurGameGold(winScore)
    --飞金币音效
    self.node_gold:removeAllChildren();
    local num = cc.LabelAtlas:create(string.format("%d", winScore),"fonts/font_result_num.png",38,50,string.byte("."));
    num:setAnchorPoint(1.0,0.5);
    num:setPosition(1004, 427);
    self.node_gold:addChild(num);

    local delTime = 0.1;
    local moveTime = 1.0;
    num:runAction(cc.Sequence:create(
        cc.ScaleTo:create(0.2, 2.0),
        cc.Spawn:create(cc.MoveTo:create(moveTime,cc.p(346,160)), cc.ScaleTo:create(moveTime, 0.5)),
        cc.DelayTime:create(delTime),
        cc.CallFunc:create( function()
            num:removeFromParent();
            --刷新总输赢
            local resultNum = self:formatScoreText(winScore);
            if self.lUserAllWinLose then
                -- self.lUserAllWinLose = self.lUserAllWinLose + resultNum;
                --只显示本局输赢
                self.lUserAllWinLose = resultNum;
                self:freshUserAllWinLose();
            end
            
        end )
    ));
end

--播放开始下注、停止下注、休息一下
function GameViewLayer:PlayAnimByGameState(gameType)
	--spin
	local vsSpin = sp.SkeletonAnimation:create("spine/startbet/skeleton.json","spine/startbet/skeleton.atlas",1);
    vsSpin:setPosition(cc.p(667 + 100, 375 +85));
    if 1 == gameType then
        vsSpin:setSkin("xiuxi");
    elseif 2 == gameType then
        ExternalFun.playSoundEffect(cmd.EFFECT_StartJetton);
        vsSpin:setSkin("kaishi");
    elseif 3 == gameType then
        ExternalFun.playSoundEffect(cmd.EFFECT_StopJetton);
        vsSpin:setSkin("tingzhi");
    end 
    
    vsSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
	-- vsSpin:registerSpineEventHandler(function (event)
	-- 	-- 八卦图替换
	-- 	self.m_rolllist[index]:variableImageChnage()
		
 --  	end, sp.EventType.ANIMATION_END)
    self.node_effect:addChild(vsSpin);

    self:runAction(cc.Sequence:create(
		cc.DelayTime:create(3.0),
		cc.CallFunc:create( function()
			vsSpin:removeAllChildren();
	        vsSpin:removeFromParent();
	    end )
	));

end

--播放开牌后闪烁动画
function GameViewLayer:PlayOpenCardBlinkAnim()
    --spin
    local vsSpin = sp.SkeletonAnimation:create("spine/blink/skeleton.json","spine/blink/skeleton.atlas",1);
    vsSpin:setPosition(cc.p(249, 436));
    vsSpin:setAnimation(0, "animation", true);    --true是指循环播放walk动作
    self.node_effect:addChild(vsSpin);

    self:runAction(cc.Sequence:create(
        cc.DelayTime:create(3.0),
        cc.CallFunc:create( function()
            -- vsSpin:removeAllChildren();
            -- vsSpin:removeFromParent();
        end )
    ));
end

--播放下注按钮闪烁
function GameViewLayer:PlayJettonBtnAnim(betBtn, isHide)
	--spin
    if isHide then
        self.node_bet_effect:removeAllChildren();
    elseif not isHide then
        -- self.node_bet_effect:removeAllChildren();
        local betBtnSpin = sp.SkeletonAnimation:create("spine/betcicle/skeleton.json","spine/betcicle/skeleton.atlas",1);
        betBtnSpin:setPosition(cc.p(betBtn:getPositionX(), betBtn:getPositionY() +12));
        betBtnSpin:setAnimation(0, "animation", true);    --true是指循环播放walk动作
        self.node_bet_effect:addChild(betBtnSpin);
    end
end

--播放倒计时动画
function GameViewLayer:PlayCutTimeAnim()
	--spin
    self.node_cut_time:removeAllChildren();
	local cutTimeSpin = sp.SkeletonAnimation:create("spine/cuttime/skeleton.json","spine/cuttime/skeleton.atlas",1);
    cutTimeSpin:setPosition(cc.p(184, 335));
    cutTimeSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    self.node_cut_time:addChild(cutTimeSpin);
end

--播放结算动画
function GameViewLayer:PlayGameEndAnim()
    --spin
    local endSpin = sp.SkeletonAnimation:create("spine/end/skeleton.json","spine/end/skeleton.atlas",1);
    endSpin:setPosition(cc.p(815, 435));
    endSpin:setAnimation(0, "animation", true);    --true是指循环播放walk动作
    self.node_effect:addChild(endSpin);
end

--播放未开牌火动画
function GameViewLayer:PlayCloseCardAnim(isHide)
    --spin
    if isHide then
        self.node_fire:removeAllChildren();
    else
        self.node_fire:removeAllChildren();
        local fireSpin = sp.SkeletonAnimation:create("spine/fire/skeleton.json","spine/fire/skeleton.atlas",1);
        fireSpin:setPosition(cc.p(244, 460));
        fireSpin:setAnimation(0, "animation", true);    --true是指循环播放walk动作
        self.node_fire:addChild(fireSpin);
    end
end

--播放飞花前光动画
function GameViewLayer:PlayFlyFlowerLightAnim(cbCurrentOpenCard)
    --spin
    local lightSpin = sp.SkeletonAnimation:create("spine/light/skeleton.json","spine/light/skeleton.atlas",1);
    lightSpin:setPosition(cc.p(240, 452));
    lightSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    self.node_effect:addChild(lightSpin);

    self:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.6),
        cc.CallFunc:create( function()
            --飞花动画
            self:flyCurGameRecord(cbCurrentOpenCard);
        end),
        cc.DelayTime:create(0.5),
        cc.CallFunc:create( function()
            lightSpin:removeAllChildren();
            lightSpin:removeFromParent();
        end)
    ));
end

--播放开牌前动画
function GameViewLayer:PlayOpenCardPreAnim(cbCurrentOpenCard, isBlink)
    --spin
    local lightSpin = sp.SkeletonAnimation:create("spine/openpre/skeleton.json","spine/openpre/skeleton.atlas",1);
    lightSpin:setPosition(cc.p(235, 470));
    lightSpin:setAnimation(0, "animation", true);    --true是指循环播放walk动作
    self.node_effect:addChild(lightSpin);

    self:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.5),
        cc.CallFunc:create( function()
            --开牌
            self:cardOpenRewardAni(cbCurrentOpenCard, isBlink);
        end),
        cc.DelayTime:create(0.5),
        cc.CallFunc:create( function()
            lightSpin:removeAllChildren();
            lightSpin:removeFromParent();
        end)
    ));
end

-- 菜单上拉下拉
function GameViewLayer:onMenuAction(isDown)
	if isDown then
		self.node_menu:runAction(cc.Sequence:create(
			cc.CallFunc:create( function()
		        self.node_menu:setVisible(true);
		    end),
			cc.MoveTo:create(0.2,cc.p(0,0))
		));
        self.btn_menu:loadTextureNormal("btn_menu_2.png", ccui.TextureResType.plistType);
        self.btn_menu:loadTextureNormal("btn_menu_2.png", ccui.TextureResType.plistType);
        self.btn_menu:loadTextureNormal("btn_menu_2.png", ccui.TextureResType.plistType);
	else
		self.node_menu:runAction(cc.Sequence:create(
			cc.MoveTo:create(0.2,cc.p(0,200)),
			cc.CallFunc:create( function()
		        self.node_menu:setVisible(false);
		    end)
		));
        self.btn_menu:loadTextureNormal("btn_menu_1.png", ccui.TextureResType.plistType);
        self.btn_menu:loadTextureNormal("btn_menu_1.png", ccui.TextureResType.plistType);
        self.btn_menu:loadTextureNormal("btn_menu_1.png", ccui.TextureResType.plistType);
	end

	self.layoutShield:setTouchEnabled(isDown)
end

-- 银行
function GameViewLayer:onBankLayer()
	local bank = BankLayer:create(self);
    self.m_bankLayer = bank;
	bank.m_parent = self;
	self.node_layer:addChild(bank,10);

    self._scene:sendRequestBankInfo()
end

-- 帮助
function GameViewLayer:onHelpLayer()
    local help = HelpLayer:create();
    help.m_parent = self;
    self.node_layer:addChild(help,10);
end

-- 设置
function GameViewLayer:onSettingLayer()
	local set = SettingLayer:create();
	set.m_parent = self;
	self.node_layer:addChild(set,10);
end

-- 上庄列表
function GameViewLayer:onBankerListLayer()
    if self.m_applyListLayer == nil then
        self.m_applyListLayer = BankerListLayer:create();
        self.m_applyListLayer.m_parent = self;
        self.node_layer:addChild(self.m_applyListLayer,10);
    end
    local userList = self:getDataMgr():getApplyBankerUserList();     
    self.m_applyListLayer:refreshList(userList);

    self.m_applyListLayer:freshApplyBankerLimit(self.m_llCondition);
end

-- 路单走势
function GameViewLayer:onTrendInfoLayer()
    self.trendInfoLayer = TrendInfoLayer:create();
    self.trendInfoLayer.m_parent = self;
    self.trendInfoLayer:trendInfoFresh()
    self.node_layer:addChild(self.trendInfoLayer,10);
end

-- 路单走势刷新
function GameViewLayer:onTrendInfoFresh()
    if self.trendInfoLayer then
        self.trendInfoLayer:trendInfoFresh();
    end
end

-- 输赢路单结果
function GameViewLayer:showRecordResult(cbLuDanSmallCount, cbLuDanSmallData)
    local ludanNum = cbLuDanSmallCount;
    local ludanData = cbLuDanSmallData;

    local beginX = 456;
    local beginY = 590;

    local disX = 100;
    local disY = 48;
    local posX = 32;
    local posX1 = 14;

    self.node_record:removeAllChildren();

    local lastRecordPosX = 0;
    local lastRecordPosY = 0;
    for i = 1, ludanNum do
        local cardValue = ludanData[i];
        local cardType = self:getCardColorByCard(cardValue);

        local lineNum = math.mod(i-1,8);
        local rowNum = math.floor((i-1)/8);

        if cardType ~= nil then
            --花色
            local cardColor =nil;
            local str = string.format("record_type_%d.png", cardType)
            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
            if nil ~= frame then
                cardColor = cc.Sprite:createWithSpriteFrame(frame);
            end
            -- cardColor:setPosition(beginX + lineNum * disX, beginY - rowNum * disY);

            self.node_record:addChild(cardColor);

            lastRecordPosX = beginX + lineNum * disX;
            lastRecordPosY = beginY - rowNum * disY;

            self.lastRecordPosX = lastRecordPosX;
            self.lastRecordPosY = lastRecordPosY;
            cardColor:setPosition(lastRecordPosX, lastRecordPosY);
            if cardType == cmd.TYPE_5 or cardType == cmd.TYPE_6 then
                lastRecordPosX = beginX + lineNum * disX + posX1;
                lastRecordPosY = beginY - rowNum * disY;
                self.lastRecordPosX = lastRecordPosX;
                self.lastRecordPosY = lastRecordPosY;
                cardColor:setPosition(lastRecordPosX, lastRecordPosY);
            else
                --牌值
                local cardIcon = cc.Sprite:create("card/" .. cmd.ServerCardValueSprite[cardValue] .. ".png");
                cardIcon:setPosition(beginX + lineNum * disX + posX, beginY - rowNum * disY);
                cardIcon:setScale(0.45);
                self.node_record:addChild(cardIcon);
            end


        end 
    end
end

--牌显示
-- 卡牌创建
function GameViewLayer:openRewardCardCreate()
    --翻到人音效
    ExternalFun.playSoundEffect(cmd.EFFECT_OpenCard2);
    self.node_card:removeAllChildren();
    local card = CardSprite:createCard(self);
    card:setPosition(cc.p(244,452));
    self.node_card:addChild(card);
    self.m_openRewardCard = card;
end

--显示卡牌背面
function GameViewLayer:showCardBack()
    self.m_openRewardCard:showCardBack(true);
end

--换牌
function GameViewLayer:randomShowCardBack()
    self.m_openRewardCard:randomShowCardBack();
end

--轮数、局数
function GameViewLayer:freshCicleAndGame()
    local circle_num = self.node_left:getChildByName("circle_num");
    local game_num = self.node_left:getChildByName("game_num");
    local nGameCount = self.nGameCount;
    local wheelNum = math.floor(self.nGameCount/100) + 1;

     local total = 0
    local nOpenColorCount = self.nOpenColorCount;
    for i = 1, 5 do

        total = nOpenColorCount[i] + total
    end

    local nGameCount = total%100+1;
    wheelNum = math.floor(total/100)+1
    circle_num:setString(wheelNum);
    game_num:setString(nGameCount);
    if nGameCount > 1000 then
        game_num:setScale(0.9);
    end
end

-- 卡牌开奖
function GameViewLayer:cardOpenRewardAni(cardValue, isBlink)
    local time1 = 0.35;
    local card = self.m_openRewardCard;

    if card == nil or tolua.isnull(card) then
        return;
    end

    card:setCardValue(cardValue);
    card:stopAllActions();
    card:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.1),
        cc.CallFunc:create(function()
            --翻开牌音效
            ExternalFun.playSoundEffect(cmd.EFFECT_OpenCard1);
            -- card:openCardAnimation();
            card:showCardBack(false);

            if isBlink then
                -- 闪烁动画
                self:PlayOpenCardBlinkAnim();
            end  
        end)
    ));
end

--大奖广播
function GameViewLayer:ShowAwardTip(cmd_table, cbCount)
    if not cmd_table then
        return;
    end
    local labelList = {};

    local tipStr  = "恭喜";
    local tipStr1 = "在本局游戏中赢取";
    local tipStr2 = "金币！";
    local fontSize = 22;
    local fontRes = "fonts/round_body.ttf";

    local name = cmd_table.szNickName;
    local score = cmd_table.lScore;
    local bAI = cmd_table.bAI;

    
    local lb1 =  cc.Label:createWithTTF(tipStr, fontRes, fontSize);
    lb1:setTextColor(cc.c3b(235,183,125));
    lb1:setAnchorPoint(0,0.5);
    table.insert(labelList, lb1);

    local lb2 =  cc.Label:createWithTTF(name, fontRes, fontSize);
    lb2:setTextColor(cc.c3b(255,255,0));
    lb2:setAnchorPoint(0,0.5);
    table.insert(labelList, lb2);

    local lb3 =  cc.Label:createWithTTF(tipStr1, fontRes, fontSize);
    lb3:setTextColor(cc.c3b(235,183,125));
    lb3:setAnchorPoint(0,0.5);
    table.insert(labelList, lb3);

    local lb4 =  cc.Label:createWithTTF(score, fontRes, fontSize);
    lb4:setTextColor(cc.c3b(255,0,0));
    lb4:setAnchorPoint(0,0.5);
    table.insert(labelList, lb4);

    local lb5 =  cc.Label:createWithTTF(tipStr2, fontRes, fontSize);
    lb5:setTextColor(cc.c3b(235,183,125));
    lb5:setAnchorPoint(0,0.5);
    table.insert(labelList, lb5);

    local length = 60;
    for i=1,#labelList do
        local lb = labelList[i];
        lb:setPosition(length - 30 , 20);
        length =  length + lb:getContentSize().width + 5;
    end

    -- local bg = ccui.ImageView:create("game_res/clew_box.png")
    -- bg:setScale9Enabled(true)
    local bg = ccui.Layout:create();
  
    bg:setContentSize(length,40);
    bg:setScale(0.01);
    bg:setVisible(false);

    for i=1,#labelList do
        local lb = labelList[i];
        bg:addChild(lb);
    end

    self:ShowTipsForBg(bg, cbCount);
    labelList = {};
end

--大奖广播
function GameViewLayer:runTipsAction(cbCount)
    local infoCount = #self.m_infoList;

    local delayTime = 2.7;
    if cbCount > 1 or infoCount > 1 then
        delayTime = 0.7;
    end

    if infoCount > 0 then
        local bg = self.m_infoList[1];

        local call = cc.CallFunc:create(function()
            table.remove(self.m_infoList,1);
            bg:removeFromParent();

            self.isHornMoving = false;
            --播放下一条
            local curCount = #self.m_infoList;
            if curCount > 0 then
                self:runTipsAction(curCount);
            else
                --播完隐藏
                self:isShowTipsBottom(false);
            end
        end)
        self.isHornMoving = true;
        bg:setVisible(true);
        bg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.15,1.0),cc.DelayTime:create(delayTime),cc.ScaleTo:create(0.15,0.1,1.0),call));
    end
end

--大奖广播
function GameViewLayer:ShowTipsForBg(bg, cbCount)
    --加到数组
    bg:setPosition(665, 730);
    bg:setAnchorPoint(0.5,0.5);
    self:addChild(bg,30);
    table.insert(self.m_infoList, bg);

    --是否正在广播
    if self.isHornMoving then
    else
        self:runTipsAction(cbCount);
    end
end

--断线重连隐藏广播背景框
function GameViewLayer:hideTipsBottom()
    self:isShowTipsBottom(false);
    self.m_infoList = {};
end

function GameViewLayer:isShowTipsBottom(isShow)
    self.hornBottom:setVisible(isShow);
end

-- 通过牌值获取花色
function GameViewLayer:getCardColorByCard(cardValue)
    if cardValue >= 1 and cardValue <=13 then
        return cmd.TYPE_1; --黑桃
    elseif cardValue >= 17 and cardValue <=29 then
        return cmd.TYPE_2; --红桃
    elseif cardValue >= 33 and cardValue <=45 then
        return cmd.TYPE_3; --梅花
    elseif cardValue >= 49 and cardValue <=61 then
        return cmd.TYPE_4; --方块
    elseif cardValue == 78 then
        return cmd.TYPE_5; --小王
    elseif cardValue == 79 then
        return cmd.TYPE_6; --大王
    end
end

-- 通过牌值获取下注区域index
function GameViewLayer:getAreaIndexByCard(cardValue)
    if cardValue >= 1 and cardValue <=13 then
        return cmd.AREA_HEI;
    elseif cardValue >= 17 and cardValue <=29 then
        return cmd.AREA_HONG;
    elseif cardValue >= 33 and cardValue <=45 then
        return cmd.AREA_MEI;
    elseif cardValue >= 49 and cardValue <=61 then
        return cmd.AREA_FANG;
    elseif cardValue == 78 or cardValue == 79 then
        return cmd.AREA_KING;
    end
end

-- 通过界面viewId获取实际index
function GameViewLayer:getIndexByViewId(viewId)
    if viewId == 1 then 
        return 1;
    elseif viewId == 2 then 
        return 2;
    elseif viewId == 3 then 
        return 3;
    elseif viewId == 4 then 
        return 4;
    elseif viewId == 5 then 
        return 5;
    end
end

-- 通过服务器id获取viewId
function GameViewLayer:getViewIdByIndex(charid)
    if charid == 0 then 
        return 1;
    elseif charid == 1 then 
        return 2;
    elseif charid == 2 then 
        return 3;
    elseif charid == 3 then 
        return 4;
    elseif charid == 4 then 
        return 5;
    end
end

function GameViewLayer:canUseBank()

    if self.m_hasJetton == true then
        showToast(self,"已下注不可进行银行操作",1)
        return false
    end
    if self:isMeChair(self.m_wBankerUser) and self.m_cbGameStatus ~= cmd.GAME_SCENE_FREE then
        showToast(self,"非空闲时间段不可进行银行操作",1)
        return false
    end
    return true 
end

return GameViewLayer
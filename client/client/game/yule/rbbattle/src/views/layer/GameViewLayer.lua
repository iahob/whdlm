local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.rbbattle.src"

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

local RewardPoolLayer = appdf.req(module_pre .. ".views.layer.RewardPoolLayer")
local RoleInfoLayer = appdf.req(module_pre .. ".views.layer.RoleInfoLayer")
local PlayerListLayer = appdf.req(module_pre .. ".views.layer.PlayerListLayer")
local BankerListLayer = appdf.req(module_pre .. ".views.layer.BankerListLayer")

local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

local CardSprite = appdf.req(module_pre..".views.layer.CardSprite")

local BankLayer = appdf.req(module_pre..".views.layer.BankLayer")

-- 元素原点位置
local playerPosList = 
{
	cc.p(103, 550), cc.p(103 , 400), cc.p(103 , 250), cc.p(1232 , 550), cc.p(1232 , 400),cc.p(1232 , 250)
}

local Self = 1;
local OtherPlayer = 2;
local TablePlayer1 = 3;
local TablePlayer2 = 4;
local TablePlayer3 = 5;
local TablePlayer4 = 6;
local TablePlayer5 = 7;
local TablePlayer6 = 8;
--不同玩家下注的筹码起始位置
local PlayerJettonChoumaPos =
{
	[Self] = cc.p(150, 48),
	[OtherPlayer] = cc.p(51, 45),
	[TablePlayer1] = cc.p(103, 550),
	[TablePlayer2] = cc.p(103, 400),
	[TablePlayer3] = cc.p(103, 250),
	[TablePlayer4] = cc.p(1232, 550),
	[TablePlayer5] = cc.p(1232, 400),
	[TablePlayer6] = cc.p(1232, 250)
}

-- 6张牌位置
local cardPosArr = 
{
	cc.p(255, 705), cc.p(322 , 705), cc.p(389 , 705), cc.p(945 , 705), cc.p(1012 , 705),cc.p(1079 , 705)
}

GameViewLayer.TAG_START				= 100
local enumTable = 
{
	"BT_Exit",               -- 退出游戏
	"BT_Menu",               -- 菜单
	"BT_Detail",             -- 详情
	"BT_OtherPlayer",        -- 其他玩家列表
	"BT_ChongFu",            -- 重复
	"BT_Help",               -- 帮助
	"BT_Set",                -- 设置
	"BT_RewardPool",         -- 奖池
	"BT_Myself_Info",        -- 玩家信息
	"BT_JETTONAREA_0",       --下注区域
    "BT_JETTONAREA_1",
    "BT_JETTONAREA_2",
    "BT_JETTONSCORE_0",      --下注按钮
    "BT_JETTONSCORE_1",
    "BT_JETTONSCORE_2",
    "BT_JETTONSCORE_3",
    "BT_JETTONSCORE_4",
    "BT_SEAT_1",             --坐下  
    "BT_SEAT_2",
    "BT_SEAT_3",
    "BT_SEAT_4",
    "BT_SEAT_5",  
    "BT_SEAT_6",
    "BT_APPLY_BANKER",
    "BT_BANK",
	
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

--下注按钮对应的下注数值
local BetNumButtonValue = 
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

--筹码配置
function GameViewLayer:betServerSet(chipArray)
    if chipArray then
        for i = 1, 5 do
            ChoumaType[i] = chipArray[i];
            BetNumButtonValue[i] = chipArray[i];

            local bet_btn = self._csbNode:getChildByName("node_bet_btn"):getChildByName("btn_bet_" .. i);
            local label = self:createChipText(BetNumButtonValue[i], true)
            label:setAnchorPoint(cc.p(0.5,0.5))
            label:setPosition(cc.p(68,40));
            bet_btn:addChild(label)
        end
    end
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    if self.m_bankLayer then
        self.m_bankLayer:refreshBankRate(bankinfo.wRevenueTake, bankinfo.lBankOPButtonArray[1])
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
	if nil ~= GlobalUserItem.tabAccountInfo.dwUserID then
		return self:getDataMgr():getUidUserList()[GlobalUserItem.tabAccountInfo.dwUserID];
	end
	return nil;
end

function GameViewLayer:isMeChair( wchair )
	local useritem = self:getDataMgr():getChairUserList()[wchair + 1];
	if nil == useritem then
		return false
	else 
		return useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID
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
    
    GlobalUserItem.tabAccountInfo.lUserScore= bank_success.lUserScore
    GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

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

	display.loadSpriteFrames(cmd.RES.."game/cardtype.plist",cmd.RES.."game/cardtype.png")
    display.loadSpriteFrames(cmd.RES.."game/chip.plist",cmd.RES.."game/chip.png")
    display.loadSpriteFrames(cmd.RES.."game/game.plist",cmd.RES.."game/game.png")
    display.loadSpriteFrames(cmd.RES.."font/font.plist",cmd.RES.."font/font.png")
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
    self.m_HasJetton = false
	--游戏状态
    self.m_cbGameStatus = cmd.GAME_SCENE_FREE;
    --剩余时间
    self.cbTimeLeave = 0;
    --奖池分
    self.lCurrentPoolScore = 0;
    --上桌限制分数
    self.lLimitScore = 0;

    --显示分数
    self.m_showScore =  0;

    -- 玩家资产
    self.m_PlayerMoney = 0;
    --单局个人下注总数限制
    self.m_PlayerMaxScore = 0;
    --单局红/黑区 下注总数限制
    self.m_RedAndBlackAreaMaxScore = 0;
    --牌型区 下注总数限制
    self.m_PokerTypeAreaMaxScore = 0;
    -- 当前单次下注筹码数
    self.m_CurrentBetNum = 0;

    --倒计时Layout
    self.m_timeLayout = nil;

     --是否重复投注
    self.m_IsReJetton = false;
    --是否已经重复投下注
    self.m_IsReJettonBet = false;
    -- 玩家本轮的总下注数
    self.m_CurrentJettonCount = {};
    self.m_CurrentJettonCount[cmd.RED_INDEX] = 0;
    self.m_CurrentJettonCount[cmd.BLACK_INDEX] = 0;
    self.m_CurrentJettonCount[cmd.POKER_TYPE_INDEX] = 0;
    --本轮全局总下注数
    self.m_CurrentAllJettonCount = {};
    self.m_CurrentAllJettonCount[cmd.RED_INDEX] = 0;
    self.m_CurrentAllJettonCount[cmd.BLACK_INDEX] = 0;
    self.m_CurrentAllJettonCount[cmd.POKER_TYPE_INDEX] = 0;
    --区域下注按钮
    self.AreaJettonButtons = {};
    -- 赌注数量切换按钮
    self.BetNumButtons = {};

    --已上桌玩家
    self.m_TableUserChairId = {};
    --玩家椅子号
    self.m_UserChairID = self._scene:GetMeChairID();

    --各区域下注保存
    self.m_curAreaJettonList = {};
    self.m_curAreaJettonList[cmd.RED_INDEX] = {};
    self.m_curAreaJettonList[cmd.BLACK_INDEX] = {};
    self.m_curAreaJettonList[cmd.POKER_TYPE_INDEX] = {};

    --玩家本轮赢分的筹码
    self.m_PlayerWinChouma = {};
    --桌子上玩家本轮赢分的筹码
    self.m_TablePlayerWinChouma = {};
    for i=1,6 do
        self.m_TablePlayerWinChouma[i] = {};
    end

    --玩家本轮赢分
    self.m_PlayerWinScore = 0;
    --桌子上玩家本轮赢分
    self.m_TablePlayerWinScore = {};
    for i=1,6 do
        self.m_TablePlayerWinScore[i] = 0;
    end
    
    --开奖卡牌
    self.m_openRewardCardArr = {};
    -- 是否游戏结束
    self.isEndGame = false;

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
    self.bIsAllowApplyBanker = false;
    --庄家赢分
    self.m_lBankerWinScore = 0;
    --庄家局数
    self.cbBankerTime = 0;
    --是否已申请下庄
    self.bIsClickCancelBanker = false;
    
end

--重置本局数据
function GameViewLayer:ReSetGameData()
    self.m_HasJetton = false
	--各区域下注保存
    self.m_curAreaJettonList = {};
    self.m_curAreaJettonList[cmd.RED_INDEX] = {};
    self.m_curAreaJettonList[cmd.BLACK_INDEX] = {};
    self.m_curAreaJettonList[cmd.POKER_TYPE_INDEX] = {};
    
    --是否已经重复投下注
    self.m_IsReJettonBet = false;
    
   -- 玩家本轮的总下注数
    self.m_CurrentJettonCount = {};
    self.m_CurrentJettonCount[cmd.RED_INDEX] = 0;
    self.m_CurrentJettonCount[cmd.BLACK_INDEX] = 0;
    self.m_CurrentJettonCount[cmd.POKER_TYPE_INDEX] = 0;
    --本轮全局总下注数
    self.m_CurrentAllJettonCount = {};
    self.m_CurrentAllJettonCount[cmd.RED_INDEX] = 0;
    self.m_CurrentAllJettonCount[cmd.BLACK_INDEX] = 0;
    self.m_CurrentAllJettonCount[cmd.POKER_TYPE_INDEX] = 0;
    
    --玩家本轮赢分的筹码
    self.m_PlayerWinChouma = {};
    --桌子上玩家本轮赢分的筹码
    self.m_TablePlayerWinChouma = {};
    for i=1,6 do
        self.m_TablePlayerWinChouma[i] = {};
    end

    --玩家本轮赢分
    self.m_PlayerWinScore = 0;
    --桌子上玩家本轮赢分
    self.m_TablePlayerWinScore = {};
    for i=1,6 do
        self.m_TablePlayerWinScore[i] = 0;
    end

    --等待界面
    self.waitStationLayer = nil;

    --开奖卡牌
    for i = 1, 6 do
		local card = self.m_openRewardCardArr[i];
		if card then
			card:removeFromParent();
		end
	end

    self.m_openRewardCardArr = {};
    -- 是否游戏结束
    self.isEndGame = false;
    
    self:HideAreaTotalJettonLabel();

    if self._timeLessUpdate then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeLessUpdate);
		self._timeLessUpdate = nil;
	end

	self.node_poker_type:setVisible(false);

	self.node_menu:stopAllActions();
	self.btn_menu:stopAllActions();
	self:stopAllActions();
	self.node_card:removeAllChildren();
	self.node_chip:removeAllChildren();
	self.node_effect:removeAllChildren();
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
	local menu_bg = node_menu:getChildByName("menu_bg");
	

	--桌子操作按钮屏蔽层
	local callbackShield = function(ref)
		local pos = ref:getTouchEndPosition()
        local rectBg = menu_bg:getBoundingBox()
        if not cc.rectContainsPoint(rectBg, pos)then
        	self:onMenuAction(false)
        	self.menuIsUp = false;
        	-- self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Menu);
        end
	end
	self.layoutShield = csbNode:getChildByName("image_touch");
	self.layoutShield:setVisible(true);
	self.layoutShield:setTouchEnabled(false);
	self.layoutShield:addClickEventListener(callbackShield);

	node_menu:setVisible(false);
	-- self:onMenuAction(false);

	local btn_other = csbNode:getChildByName("btn_other");
	btn_other:setTag(TAG_ENUM.BT_OtherPlayer);
	btn_other:addTouchEventListener(btnEvent);

	local btn_detail = csbNode:getChildByName("btn_detail");
	btn_detail:setTag(TAG_ENUM.BT_Detail);
	btn_detail:addTouchEventListener(btnEvent);

	local btn_chongfu = csbNode:getChildByName("btn_chongfu");
	btn_chongfu:setTag(TAG_ENUM.BT_ChongFu);
	btn_chongfu:addTouchEventListener(btnEvent);

	self.icon_select  = btn_chongfu:getChildByName("icon_select");
	self.icon_select:setVisible(false);

	local btn_help = node_menu:getChildByName("btn_help");
	btn_help:setTag(TAG_ENUM.BT_Help);
	btn_help:addTouchEventListener(btnEvent);

	local btn_set = node_menu:getChildByName("btn_set");
	btn_set:setTag(TAG_ENUM.BT_Set);
	btn_set:addTouchEventListener(btnEvent);

    local btn_bank = node_menu:getChildByName("btn_bank");
    btn_bank:setTag(TAG_ENUM.BT_BANK);
    btn_bank:addTouchEventListener(btnEvent);

    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    if not enableBank then
        btn_bank:setVisible(false)
        menu_bg:setContentSize(74,200)
    end
    
	--
	self.node_layer = csbNode:getChildByName("node_layer");

    -- 是否可上庄
    ---------------------------------------------------
    -- self:onIsCanApplyBanker(false);

    local node_title = self._csbNode:getChildByName("node_title");

    local btn_reward_pool = node_title:getChildByName("btn_reward_pool");
    btn_reward_pool:setTag(TAG_ENUM.BT_RewardPool);
    btn_reward_pool:addTouchEventListener(btnEvent);

    self.reward_pool_num = node_title:getChildByName("reward_pool_num");
    self.reward_pool_num:setString("");

    --奖池隐藏
    btn_reward_pool:setVisible(false);
    self.reward_pool_num:setVisible(false);


    self.m_timeLayout = node_title:getChildByName("node_time");
    self.m_timeLayout:setVisible(false);
    local bet_icon = self.m_timeLayout:getChildByName("bet_icon");
    local title_icon = self.m_timeLayout:getChildByName("title_icon");
    self.bet_time = bet_icon:getChildByName("bet_time");

    local node_banker = node_title:getChildByName("node_banker");
    self.node_banker = node_banker;
    local banker_name = node_banker:getChildByName("banker_name");
    local gold_num = node_banker:getChildByName("gold_num");
    local btn_apply_banker = node_banker:getChildByName("btn_apply_banker");
    btn_apply_banker:setTag(TAG_ENUM.BT_APPLY_BANKER);
    btn_apply_banker:addTouchEventListener(btnEvent);
    -----------------------------------------------------------------

	--各区域总下注
	self:HideAreaTotalJettonLabel();

	--各区域我的下注
	self:HideAreaUserJettonLabel();
	
	local node_win_bg = self._csbNode:getChildByName("node_win_bg");
	local balck_win = node_win_bg:getChildByName("balck_win");
	local red_win = node_win_bg:getChildByName("red_win");
	local win_type = node_win_bg:getChildByName("win_type");


	local node_betarea_btn = csbNode:getChildByName("node_betarea_btn");
	local btn_black_bet = node_betarea_btn:getChildByName("btn_black_bet");
	btn_black_bet:setTag(TAG_ENUM.BT_JETTONAREA_1);
	btn_black_bet:addTouchEventListener(btnEvent);
	self.AreaJettonButtons[cmd.BLACK_INDEX] = btn_black_bet;

	local btn_red_bet = node_betarea_btn:getChildByName("btn_red_bet");
	btn_red_bet:setTag(TAG_ENUM.BT_JETTONAREA_0);
	btn_red_bet:addTouchEventListener(btnEvent);
	self.AreaJettonButtons[cmd.RED_INDEX] = btn_red_bet;

	local btn_type_bet = node_betarea_btn:getChildByName("btn_type_bet");
	btn_type_bet:setTag(TAG_ENUM.BT_JETTONAREA_2);
	btn_type_bet:addTouchEventListener(btnEvent);
	self.AreaJettonButtons[cmd.POKER_TYPE_INDEX] = btn_type_bet;

	-- 下注赢分
	local node_role_bet = csbNode:getChildByName("node_role_bet");
	node_role_bet:setVisible(false);
	-- 玩家
	local node_player = csbNode:getChildByName("node_player");

	for i = 1, 6 do
		local player = node_player:getChildByName("player_" .. i);
		local role_icon = player:getChildByName("role_icon");
		local btn_bottom = player:getChildByName("btn_bottom");
		btn_bottom:setTag(TAG_ENUM.BT_SEAT_1 + i - 1);
		btn_bottom:addTouchEventListener(btnEvent);

		local role_rank = player:getChildByName("role_rank");
		local role_name = player:getChildByName("role_name");
	end

	self.node_effect = csbNode:getChildByName("node_effect");

	self.node_chip = csbNode:getChildByName("node_chip");

	self.node_card = csbNode:getChildByName("node_card");
	self.node_poker_type = csbNode:getChildByName("node_poker_type");

	--自己
	local node_myself = csbNode:getChildByName("node_myself");
    local btn_role = node_myself:getChildByName("btn_role");
    btn_role:setTag(TAG_ENUM.BT_Myself_Info);
	btn_role:addTouchEventListener(btnEvent);

    self:initBankerInfo();
	-- self:initSelfInfo();

	local node_bet_btn = csbNode:getChildByName("node_bet_btn");
	for i = 1, 5 do
		local btn_bet = node_bet_btn:getChildByName("btn_bet_" .. i);
		btn_bet:setTag(TAG_ENUM.BT_JETTONSCORE_0 + i - 1);
		btn_bet:addTouchEventListener(btnEvent);

		self.BetNumButtons[i] = btn_bet;
	end

	-- 卡牌创建
	self:openRewardCardCreate();

end
--------------------------------------------------------------------------------------------------
-- 是否可上庄显示
function GameViewLayer:onIsCanApplyBanker(isShow)
    -- --按钮列表
    -- local function btnEvent( sender, eventType )
    --     if eventType == ccui.TouchEventType.ended then
    --         self:onButtonClickedEvent(sender:getTag(), sender);
    --     end
    -- end 

    -- local node_title_1 = self._csbNode:getChildByName("node_title_1");
    -- local node_title_2 = self._csbNode:getChildByName("node_title_2");

    -- if isShow then
    --     node_title_1:setVisible(false);
    --     node_title_2:setVisible(true);
    --     --可上庄
    --     local btn_reward_pool = node_title_2:getChildByName("btn_reward_pool");
    --     btn_reward_pool:setTag(TAG_ENUM.BT_RewardPool);
    --     btn_reward_pool:addTouchEventListener(btnEvent);

    --     self.reward_pool_num = node_title_2:getChildByName("reward_pool_num");

    --     self.m_timeLayout = node_title_2:getChildByName("node_time");
    --     self.m_timeLayout:setVisible(false);
    --     local bet_icon = self.m_timeLayout:getChildByName("bet_icon");
    --     local title_icon = self.m_timeLayout:getChildByName("title_icon");
    --     self.bet_time = bet_icon:getChildByName("bet_time");

    --     local node_banker = node_title_2:getChildByName("node_banker");
    --     local banker_name = node_banker:getChildByName("banker_name");
    --     local gold_num = node_banker:getChildByName("gold_num");
    --     local btn_apply_banker = node_banker:getChildByName("btn_apply_banker");
    --     btn_apply_banker:setTag(TAG_ENUM.BT_APPLY_BANKER);
    --     btn_apply_banker:addTouchEventListener(btnEvent);
    -- else
    --     node_title_1:setVisible(true);
    --     node_title_2:setVisible(false);
    --     --不可上庄
    --     local btn_reward_pool = node_title_1:getChildByName("btn_reward_pool");
    --     btn_reward_pool:setTag(TAG_ENUM.BT_RewardPool);
    --     btn_reward_pool:addTouchEventListener(btnEvent);

    --     self.reward_pool_num = node_title_1:getChildByName("reward_pool_num");

    --     --倒计时
    --     self.m_timeLayout = node_title_1:getChildByName("node_time");
    --     self.m_timeLayout:setVisible(false);
    --     local bet_icon = self.m_timeLayout:getChildByName("bet_icon");
    --     local title_icon = self.m_timeLayout:getChildByName("title_icon");
    --     self.bet_time = bet_icon:getChildByName("bet_time");
    -- end
end
--------------------------------------------------------
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
    self:resetBankerInfo();

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
    local banker_name = self.node_banker:getChildByName("banker_name");
    local gold_num = self.node_banker:getChildByName("gold_num");
    local gold_max_icon = self.node_banker:getChildByName("gold_max_icon");
    gold_max_icon:setVisible(false);

    banker_name:setString("无人坐庄");
    gold_num:setString("0");
end

--刷新庄家信息
function GameViewLayer:resetBankerInfo()
    local banker_name = self.node_banker:getChildByName("banker_name");
    local gold_num = self.node_banker:getChildByName("gold_num");
    local gold_max_icon = self.node_banker:getChildByName("gold_max_icon");

    local userItem = self:getDataMgr():getChairUserList()[self.m_wBankerUser+1];
    if nil ~= userItem then
        local name = string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 26));
        banker_name:setString(name);

        local bankerstr = self:formatScoreText(self.m_lBankerScore);
        gold_num:setString(bankerstr);
        gold_max_icon:setVisible(false);
    else
        banker_name:setString("系统坐庄");
        local bankerstr = self:formatScoreText(self.m_lBankerScore);
        gold_num:setString("");
        gold_max_icon:setVisible(true);
    end
end

--刷新庄家分数
function GameViewLayer:resetBankerScore()
    local gold_num = self.node_banker:getChildByName("gold_num");
    -- local bankerstr = ExternalFun.formatScoreText(self.m_lBankerScore);
    if self.m_lBankerScore > 0 then
        local bankerstr = self:formatScoreText(self.m_lBankerScore);
        gold_num:setString(bankerstr);
    end
end

--刷新申请上庄列表
function GameViewLayer:refreshApplyList(  )
    if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
        local userList = self:getDataMgr():getApplyBankerUserList()     
        self.m_applyListLayer:refreshList(userList)
        -- dump(userList, "-------------刷新申请上庄列表------------", 6)
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
    if gameStatus ~= cmd.GS_PLACE_JETTON then
        self:SetSystemTip(false, gameStatus)
    else
        self:SetSystemTip(true, nil)
    end

    self.m_cbGameStatus = gameStatus;
    if gameStatus == cmd.GAME_SCENE_FREE and self.m_bankLayer and self:isMeChair(self.m_wBankerUser) then
        showToast(self.m_bankLayer, "现在是空闲时间，可以进行银行操作", 1)
    end
end

--定时器调用更新倒计时
function GameViewLayer:OnUpdataClockView(viewId, cbTime)
	self.bet_time:setString(cbTime);

    if cbTime < 6 and cbTime ~= 0 then
        ExternalFun.playSoundEffect(cmd.EFFECT_Countdown);
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
    --区域下注按钮
    for i=1,#self.AreaJettonButtons do
        self.AreaJettonButtons[i]:setEnabled(isEnable);
    end
    --筹码切换按钮
    for i=1,#self.BetNumButtons do
    	
        if isEnable then
        	isEnable = self:CheckPlayerMoneyForBetNumBtn(i) and self:CheckMaxScoreForBetNumBtn(i);
        else
        end

        --当庄时不能下注
        local myChairId = self._scene:GetMeChairID();

        if self.m_wBankerUser == myChairId then
            self.BetNumButtons[i]:setEnabled(false);
        else
            self.BetNumButtons[i]:setEnabled(isEnable);
        end
        
    end
end

--判断玩家资产是否足够启用筹码按钮
function GameViewLayer:CheckPlayerMoneyForBetNumBtn(betNumBtnIndex)
    local isEnable = false
    if self.m_PlayerMoney >= BetNumButtonValue[betNumBtnIndex] then
        isEnable = true
    end
    return isEnable
end

--根据最大下注限制 判断 是否要启用筹码按钮
function GameViewLayer:CheckMaxScoreForBetNumBtn(betNumBtnIndex)
    local isEnable = false
    local playerJettonTotal = self.m_CurrentJettonCount[cmd.RED_INDEX] + self.m_CurrentJettonCount[cmd.BLACK_INDEX] + self.m_CurrentJettonCount[cmd.POKER_TYPE_INDEX]
    local availableBetNum = self.m_PlayerMaxScore - playerJettonTotal;
    if availableBetNum >= BetNumButtonValue[betNumBtnIndex] then
        isEnable = true
    elseif self.m_CurrentBetNum == BetNumButtonValue[betNumBtnIndex] then
        --当前选择的单次下注数 大于 剩余单局可下注 时，清空当前单次下注数
        self:UpdateCurrentBetNum(0);
        --取消按钮Toggle选中
        self:SwitchBetNumToggle(0);
    end

    return isEnable
end

--初始化桌子上的玩家ChairId
function GameViewLayer:InitTableUserChairID(tableUserChairIds)
    self.m_TableUserChairId = tableUserChairIds;
end

--通过ChairId判断是否是桌子上的玩家
function GameViewLayer:GetTableUser(wChairID)
    for k,v in pairs(self.m_TableUserChairId) do
      if v == wChairID then
        --返回座位序号
        return k
      end
    end

    return false
end

--初始化玩家的下注筹码选项
function GameViewLayer:InitUserChouma()
    for i=#self.BetNumButtons, 1, -1 do
        if self:CheckPlayerMoneyForBetNumBtn(i) and self:CheckMaxScoreForBetNumBtn(i) then
            local currentBetNum = BetNumButtonValue[i]
            self:UpdateCurrentBetNum(currentBetNum);
            self:SwitchBetNumToggle(i);
            return
        end
    end
    --当玩家金币少于最低筹码金额
    self:UpdateCurrentBetNum(0);
    self:SwitchBetNumToggle(0);
end

--初始化下注限制
function GameViewLayer:InitUserMaxJetton(userMaxScore, areaLimitScore)
    self.m_PlayerMaxScore = userMaxScore;
    self.m_RedAndBlackAreaMaxScore = areaLimitScore[1];
    self.m_PokerTypeAreaMaxScore = areaLimitScore[2];

    --根据玩家的现有金币，设置默认的下注筹码选项
    self:InitUserChouma();
end

--初始化桌子上的玩家信息
function GameViewLayer:InitTableUser()
    for i,v in ipairs(self.m_TableUserChairId) do
        if v ~= 255 then
            self:freshUserInfo(i, v);
        elseif v == 255 then
            local node_player = self._csbNode:getChildByName("node_player");
            local player = node_player:getChildByName("player_" .. i);
            local role_icon = player:getChildByName("role_icon");
            local role_name = player:getChildByName("role_name");

            role_icon:removeAllChildren();
            role_name:setString("点击坐下");
        end
    end
end

-- 玩家信息
function GameViewLayer:freshUserInfo(index, wChairID)
	local myChairId = self._scene:GetMeChairID();
	
	--
    local node_player = self._csbNode:getChildByName("node_player");
	local player = node_player:getChildByName("player_" .. index);
	local role_icon = player:getChildByName("role_icon");
	local role_name = player:getChildByName("role_name");

	-- local userList = self:getDataMgr():getUserList();
	-- local roleItem = nil;
 --    for i = 1, #userList do
 --        if wChairID == userList[i].wChairID then
 --            roleItem = userList[i];
 --        end
 --    end

    local roleItem = self:getDataMgr():getChairUserList()[wChairID + 1];

    role_icon:removeAllChildren();
    --自己和他人分开处理
    if wChairID == myChairId then
        -- 创建头像
	    local head = HeadSprite:createNormal(GlobalUserItem.tabAccountInfo, role_icon:getContentSize().width)
	    if nil ~= head then
	        head:setPosition(cc.p(role_icon:getContentSize().width/2,role_icon:getContentSize().height/2))
	        role_icon:addChild(head, 1)
	    end

        local roleName = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName,80,string.getConfig("fonts/round_body.ttf" , 18));
		role_name:setString(roleName);
	else
		if not roleItem then
			-- roleItem = self._scene._gameFrame:getTableUserItem(self._scene._gameFrame:GetTableID(),wChairID);
			return;
		end

		local head = HeadSprite:createNormal(roleItem, role_icon:getContentSize().width);
        if nil ~= head then
            head:setPosition(cc.p(role_icon:getContentSize().width/2,role_icon:getContentSize().height/2));
            role_icon:addChild(head, 1);
        end
        local roleName = string.EllipsisByConfig(roleItem.szNickName,80,string.getConfig("fonts/round_body.ttf" , 18));
        role_name:setString(roleName);
    end
end

--初始化玩家已下注的Label
function GameViewLayer:InitUserJettonLabel(userJettonScore_tb)
    for i=1,3 do
        -- 1 红区 、2 黑区、 3 牌型区
        local userAreaTotalJettonScore = userJettonScore_tb[i];
        self:SetAreaUserJettonLabel(i, userAreaTotalJettonScore);
    end
end

--初始化桌面筹码（场景消息中）
function GameViewLayer:InitChouma(choumaNum_tb)
    for i=1,3 do
        -- 1 红区 、2 黑区、 3 牌型区
        local totalJetton = choumaNum_tb[i];
        --更新总下注Label
        self:SetAreaTotalJettonLabel(i, totalJetton);
        --把筹码填充到桌面
        self.m_curAreaJettonList[i] = self:inintChip(i, totalJetton, true);
    end
end

--用户下注点击事件
function GameViewLayer:OnPlayerJettonClick(areaIndex, ref)
    if self.m_cbGameStatus ~= cmd.GS_PLACE_JETTON or self.m_CurrentBetNum == 0 then return end

    --当庄时不能下注
    local myChairId = self._scene:GetMeChairID();
    if self.m_wBankerUser == myChairId then
        showToast(self, "庄家不能下注 ", 2);
        return;
    end

    --每个区域限制不一样，所以客户端不做限制。服务端会限制
    self._scene:sendPlaceJetton(areaIndex, self.m_CurrentBetNum);
end

--用户下注返回
function GameViewLayer:PlayerJetton(jettonData, isSelf, userTableId)
    local startPos = cc.p(0, 0);
    local areaIndex = jettonData.cbJettonArea;
    local jettonScore = jettonData.lJettonScore;

    if isSelf then
        self.m_HasJetton = true -- 已下注
        --自己下注
        startPos = PlayerJettonChoumaPos[Self];
        --玩家下注值累加
        local userTotalJetton = self.m_CurrentJettonCount[areaIndex] + tonumber(jettonScore);
        self:SetAreaUserJettonLabel(areaIndex, userTotalJetton);
        --手动刷新玩家金币
        local currentUserMoney = self.m_PlayerMoney - tonumber(jettonScore);
        self:UpdateUserMoney(currentUserMoney);
    elseif userTableId then
        --桌上玩家下注
        if userTableId == 1 then
            startPos = PlayerJettonChoumaPos[TablePlayer1];
        elseif userTableId == 2 then
            startPos = PlayerJettonChoumaPos[TablePlayer2]
        elseif userTableId == 3 then
            startPos = PlayerJettonChoumaPos[TablePlayer3]
        elseif userTableId == 4 then
            startPos = PlayerJettonChoumaPos[TablePlayer4]
        elseif userTableId == 5 then
            startPos = PlayerJettonChoumaPos[TablePlayer5]
        elseif userTableId == 6 then
            startPos = PlayerJettonChoumaPos[TablePlayer6]
        end

        --桌上玩家下注头像抖动
        self:deskRoleBlinkEffect(userTableId, areaIndex, startPos);
    else
        --其他玩家下注
        startPos = PlayerJettonChoumaPos[OtherPlayer];
    end

    -- 筹码生成移动到红黑区
    self:chipMoveToArea(jettonScore, startPos, areaIndex);

    --全局下注总数累加
    local allTotalJetton = self.m_CurrentAllJettonCount[areaIndex] + tonumber(jettonScore);
    self:SetAreaTotalJettonLabel(areaIndex, allTotalJetton);
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

--桌上玩家抖动
function GameViewLayer:deskRoleBlinkEffect(roleIndex, areaIndex, startPos)
	local btn = self.AreaJettonButtons[areaIndex];
    local endPos = self:getBetRandomPos(btn);

    local disX = endPos.x - startPos.x;
    local disY = endPos.y - startPos.y;

    local scale = 0.05;
    local delayTime = 0.1;
    local disAdd = cc.p(disX*scale, disY*scale);
    local disLess = cc.p(-disX*scale, -disY*scale);

 	local node_player = self._csbNode:getChildByName("node_player");
	local player = node_player:getChildByName("player_" .. roleIndex);

	-- 玩家抖动
	player:runAction(cc.Sequence:create(
	    cc.MoveBy:create(delayTime, disAdd),
	    cc.MoveBy:create(delayTime, disLess),
	    cc.MoveBy:create(delayTime, disAdd),
	    cc.MoveBy:create(delayTime, disLess)
	));

end

--更新系统提示
function GameViewLayer:SetSystemTip(isTime, gameStatus)
	self.m_timeLayout:setVisible(true);
	local bet_icon = self.m_timeLayout:getChildByName("bet_icon");
    local title_icon = self.m_timeLayout:getChildByName("title_icon");

    if isTime then
        self.bet_time:setVisible(true);
        bet_icon:setVisible(true);
    	title_icon:setVisible(false);
    else
        self.bet_time:setVisible(false);
        bet_icon:setVisible(false);
    	title_icon:setVisible(true);
    	----title_open_card   title_result   title_station_time  title_station_wait
    	local frame = nil;
    	if gameStatus == cmd.GAME_SCENE_FREE then
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/title_station_wait.png")
        elseif gameStatus == cmd.GS_GAME_END then
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/title_result.png")
        end
        if nil ~= frame then
	        title_icon:setSpriteFrame(frame);
	    end
    end
end

--刷新自己信息
function GameViewLayer:initSelfInfo()
    --自己
	local node_myself = self._csbNode:getChildByName("node_myself");
	local role_icon = node_myself:getChildByName("role_icon");
	-- 创建头像
	role_icon:removeAllChildren();
    local head = HeadSprite:createNormal(GlobalUserItem.tabAccountInfo, role_icon:getContentSize().width)
    if nil ~= head then
        -- head:setPosition(role_icon:getPosition())
        head:setPosition(cc.p(role_icon:getContentSize().width/2,role_icon:getContentSize().height/2))
        role_icon:addChild(head, 1)
    end
 --    local btn_role = node_myself:getChildByName("btn_role");
 --    btn_role:setTag(TAG_ENUM.BT_Myself_Info);
	-- btn_role:addTouchEventListener(btnEvent);

	local role_name = node_myself:getChildByName("role_name");
    local roleName = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName,110,string.getConfig("fonts/round_body.ttf" , 24));
	role_name:setString(roleName);

	self.role_gold_num = node_myself:getChildByName("role_gold_num");
	local roleGoldNum = self:formatScoreText(GlobalUserItem.tabAccountInfo.lUserScore);
	self.role_gold_num:setString(roleGoldNum);

	--刷新金币
    local my_gold  = self:getMeUserItem().lScore;
    self:UpdateUserMoney(my_gold);

	-- self:UpdateUserMoney(GlobalUserItem.tabAccountInfo.lUserScore);
end
--系统字分数 0、1、2 代表 不显示、万、亿
function GameViewLayer:formatScoreText(score)
	local scorestr = ExternalFun.formatScore(score)
	if score < 10000 then
		return scorestr, 0;
	end

	if score < 100000000 then
		scorestr = string.format("%.1f万", score / 10000)
		return scorestr, 1;
	end
	scorestr = string.format("%.1f亿", score / 100000000)
	return scorestr, 2;
end

--刷新自己信息
function GameViewLayer:UpdateUserMoney(userMoney)
	self.m_PlayerMoney = tonumber(userMoney);
	
	local selfScore = self:formatScoreText(self.m_PlayerMoney);
    -- self.role_gold_num:setString(selfScore);
    if self.role_gold_num then
        self.role_gold_num:setString(selfScore);
    end
    
end

--刷新用户分数
function GameViewLayer:onGetUserScore( useritem )
    --自己
    if useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
        self.m_showScore = useritem.lScore
        self:UpdateUserMoney(useritem.lScore);

    end
end

--重复投点击事件
function GameViewLayer:OnReJettonBtnClick()
    self.m_IsReJetton = not self.m_IsReJetton;
    if self.m_IsReJetton then
    	self.icon_select:setVisible(true);

        local playerJettonTotal = self.m_CurrentJettonCount[cmd.RED_INDEX] + self.m_CurrentJettonCount[cmd.BLACK_INDEX] 
            + self.m_CurrentJettonCount[cmd.POKER_TYPE_INDEX];
        --当勾选重复投按钮时，处于投注状态 且 当前没有下注的情况下，发送重复投注消息
        if self.m_cbGameStatus == cmd.GS_PLACE_JETTON and playerJettonTotal == 0 then
            self._scene:SendReJettonMessage();
        end
    else
    	self.icon_select:setVisible(false);
    end
end

--重复投成功处理、根据上轮投注算本轮筹码投注
function GameViewLayer:UserReJettonHandler(userJettonScoreTb)
    for i,areaScoreTotal in ipairs(userJettonScoreTb) do
    	local chipIndexList = self:CalculateChoumaByTotal(areaScoreTotal);
	    for _,chipIndex in ipairs(chipIndexList) do
	    	local jettonScore = ChoumaType[chipIndex];

	    	self._scene:sendPlaceJetton(i, jettonScore);
	    end
    end
end

--单次下注筹码数量切换按钮事件
function GameViewLayer:OnBetNumButtonClick(tag,ref)
    local currentBetNum = BetNumButtonValue[tag];
    self:UpdateCurrentBetNum(currentBetNum);
    self:SwitchBetNumToggle(tag);
end

--修改单次下注筹码数量
function GameViewLayer:UpdateCurrentBetNum(betNum)
    self.m_CurrentBetNum = tonumber(betNum);
end

--用户点击上桌事件
function GameViewLayer:OnPlayerUpDeskClick(deskIndex,ref)
    
    local deskChairId = self.m_TableUserChairId[deskIndex];
    if deskChairId ~= 255 then
        --若座位不为空，则弹窗显示该玩家的信息
        -- local userList = self:getDataMgr():getUserList();
        -- for j = 1, #userList do
        --     if deskChairId == userList[j].wChairID then
        --     	local roleItem = userList[j];

        --     	self:onDeskRoleInfoLayer(roleItem);
        --     end
        -- end

        local roleItem = self:getDataMgr():getChairUserList()[deskChairId + 1];
        self:onDeskRoleInfoLayer(roleItem);

    else
        if self.m_cbGameStatus ~= cmd.GS_PLACE_JETTON then
            showToast(self, "只能在游戏下注阶段上桌!", 2);
            return
        end
        --若座位为空，则请求上桌
        self._scene:sendApplyDesk(deskChairId, true, deskIndex);
    end
end

--用户上桌返回
function GameViewLayer:OnPlayerUpDeskCallBack(wChairID, deskIndex)
	if self.m_TableUserChairId[deskIndex] ~= 255 then
        if wChairID == self.m_UserChairID then
            showToast(self, "上桌失败,没有足够的座位!", 2);
        end
        return
    end

    --保存数据
    self.m_TableUserChairId[deskIndex] = wChairID;
    -- 更新显示
    self:freshUserInfo(deskIndex, wChairID);
end

--用户下桌
function GameViewLayer:OnPlayerExitDesk(wChairID, deskIndex)
	if self.m_TableUserChairId[deskIndex] == wChairID then
		--初始化桌子位置数据
        self.m_TableUserChairId[deskIndex] = 255;

		local node_player = self._csbNode:getChildByName("node_player");
		local player = node_player:getChildByName("player_" .. deskIndex);
		local role_icon = player:getChildByName("role_icon");
		local role_name = player:getChildByName("role_name");

		-- local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/img_head.png");
	 --    if nil ~= frame then
	 --        role_icon:setSpriteFrame(frame);
	 --    end
	 	role_icon:removeAllChildren();
		role_name:setString("点击坐下");
	end
    
end

--更新彩金池的金额UI
function GameViewLayer:UpdateStorage(storageNumber)
    self.reward_pool_num:setString(storageNumber);
end

--更新玩家区域总下注Label
function GameViewLayer:SetAreaUserJettonLabel(areaIndex, totalNum)
    if totalNum > 0 then
    	self.m_CurrentJettonCount[areaIndex] = tonumber(totalNum);
    	-- 下注显示
    	local showNum, iconType = self:formatScoreText(totalNum);

   		local node_self_num = self._csbNode:getChildByName("node_self_num");
		local bg = node_self_num:getChildByName("bg_" .. areaIndex);
		bg:setVisible(true);
		local selfbet_num = node_self_num:getChildByName("selfbet_num_" .. areaIndex);
		selfbet_num:setVisible(true);
		local icon = node_self_num:getChildByName("icon_" .. areaIndex);

		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_wan.png")
	    if iconType == 0 then
	        icon:setVisible(false);
	    elseif iconType == 1 then
	        icon:setVisible(true);
	        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_wan.png")
	    elseif iconType == 2 then
	        icon:setVisible(true);
	        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_yi.png")
	    end
	    if nil ~= frame then
	        icon:setSpriteFrame(frame);
	    end
	    selfbet_num:setString(showNum);
    end
    
end

--更新区域总下注Label
function GameViewLayer:SetAreaTotalJettonLabel(areaIndex, totalNum)
    if totalNum > 0 then
    	self.m_CurrentAllJettonCount[areaIndex] = tonumber(totalNum);

    	local showNum, iconType = self:formatScoreText(totalNum);

   		local node_all_num = self._csbNode:getChildByName("node_all_num");
		local totalbet_num = node_all_num:getChildByName("totalbet_num_" .. areaIndex);
		totalbet_num:setVisible(true);
		local icon = node_all_num:getChildByName("icon_" .. areaIndex);

		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/total_bet_wan.png")
	    if iconType == 0 then
	        icon:setVisible(false);
	    elseif iconType == 1 then
	        icon:setVisible(true);
	        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/total_bet_wan.png")
	    elseif iconType == 2 then
	        icon:setVisible(true);
	        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/total_bet_yi.png")
	    end
	    if nil ~= frame then
	        icon:setSpriteFrame(frame);
	    end
	    totalbet_num:setString(showNum);
    end
end

--隐藏区域总下注Label
function GameViewLayer:HideAreaTotalJettonLabel()
    local node_all_num = self._csbNode:getChildByName("node_all_num");
	for i = 1, 3 do
		local totalbet_num = node_all_num:getChildByName("totalbet_num_" .. i);
		totalbet_num:setVisible(false);
		local icon = node_all_num:getChildByName("icon_" .. i);
		icon:setVisible(false);
	end
end

--隐藏玩家区域总下注Label
function GameViewLayer:HideAreaUserJettonLabel()
    local node_self_num = self._csbNode:getChildByName("node_self_num");
	for i = 1, 3 do
		local bg = node_self_num:getChildByName("bg_" .. i);
		bg:setVisible(false);
		local selfbet_num = node_self_num:getChildByName("selfbet_num_" .. i);
		selfbet_num:setVisible(false);
		local icon = node_self_num:getChildByName("icon_" .. i);
		icon:setVisible(false);
	end
end

--播放爆池动画
function GameViewLayer:PlayBaoChiAnim(isShow)
	if isShow then
		local animLayer = ExternalFun.loadCSB("aniRes/baochi/baochiLayer.csb", self.node_effect)

	    local animTimeline = cc.CSLoader:createTimeline("aniRes/baochi/baochiLayer.csb")
	    local animinfo = animTimeline:getAnimationInfo("play")
	    -- animTimeline:setTimeSpeed(0.25);
	    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false)
	    animLayer:stopAllActions();
	    animLayer:runAction(animTimeline)

	    animLayer:runAction(cc.Sequence:create(
			cc.DelayTime:create(3.0),
			cc.CallFunc:create( function()
				animLayer:removeAllChildren();
		        animLayer:removeFromParent();
		    end )
		));
	else

	end
end

--播放玩家赢分动画
function GameViewLayer:PlayPlayerWinAnim(selfWinScore, isBigWin)
	--获胜音效
	ExternalFun.playSoundEffect(cmd.EFFECT_Bigwin);

    local animLayer = ExternalFun.loadCSB("aniRes/bigwinerAni/bigwinerLayer.csb", self.node_effect)

    local node = animLayer:getChildByName("node");
    local node_win = node:getChildByName("node_win");
    local role_icon = node_win:getChildByName("role_icon");
    local role_mame = node_win:getChildByName("role_mame");
    local role_win_num = node_win:getChildByName("role_win_num");
    local num_icon = node_win:getChildByName("num_icon");
    
    role_icon:removeAllChildren();
    local head = HeadSprite:createNormal(GlobalUserItem.tabAccountInfo, role_icon:getContentSize().width)
    if nil ~= head then
        -- head:setPosition(role_icon:getPosition())
        head:setPosition(cc.p(role_icon:getContentSize().width/2,role_icon:getContentSize().height/2))
        role_icon:addChild(head, 1)
    end
    local name = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName,120,string.getConfig("fonts/round_body.ttf" , 22));
    role_mame:setString(name or "");
    -- role_win_num:setString(self:formatScoreText(selfWinScore))
    local score, iconType = self:formatScoreText(selfWinScore);
    
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_wan.png")
    if iconType == 0 then
        num_icon:setVisible(false);
    elseif iconType == 1 then
        num_icon:setVisible(true);
        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_wan.png")
    elseif iconType == 2 then
        num_icon:setVisible(true);
        frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_yi.png")
    end
    if nil ~= frame then
        num_icon:setSpriteFrame(frame);
    end
    role_win_num:setString(score);

    local node_title = node:getChildByName("node_title");
    local win_title_1 = node_title:getChildByName("win_title_1");
    local win_title_2 = node_title:getChildByName("win_title_2");
    if isBigWin then
    	win_title_1:setTexture("aniRes/bigwinerAni/bw2.png");
    	win_title_2:setTexture("aniRes/bigwinerAni/bw1.png");
    else
    	win_title_1:setTexture("aniRes/bigwinerAni/bw2_win.png");
    	win_title_2:setTexture("aniRes/bigwinerAni/bw1_win.png");
    end

    local animTimeline = cc.CSLoader:createTimeline("aniRes/bigwinerAni/bigwinerLayer.csb");
    local animinfo = animTimeline:getAnimationInfo("play");
    -- animTimeline:setTimeSpeed(0.25);
    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false);
    animLayer:stopAllActions();
    animLayer:runAction(animTimeline);

    animLayer:runAction(cc.Sequence:create(
		cc.DelayTime:create(3.0),
		cc.CallFunc:create( function()
			animLayer:removeAllChildren();
	        animLayer:removeFromParent();
	    end )
	));
end

--播放开局vs动画
function GameViewLayer:PlayGameStartAnim()

 --    local animLayer = ExternalFun.loadCSB("aniRes/vsAni/vsLayer.csb", self.node_effect);

 --    local animTimeline = cc.CSLoader:createTimeline("aniRes/vsAni/vsLayer.csb");
 --    local animinfo = animTimeline:getAnimationInfo("play");
 --    -- animTimeline:setTimeSpeed(0.25);
 --    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false);
 --    animLayer:stopAllActions();
 --    animLayer:runAction(animTimeline);

 --    ExternalFun.playSoundEffect(cmd.EFFECT_RedVsBlack);

 --    animLayer:runAction(cc.Sequence:create(
	-- 	cc.DelayTime:create(1.0),
	-- 	cc.CallFunc:create( function()
	-- 		animLayer:removeAllChildren();
	--         animLayer:removeFromParent();
	--         --开始下注
	--         self:PlayStartJettonAnim();
	--     end )
	-- ));

	--spin
	ExternalFun.playSoundEffect(cmd.EFFECT_RedVsBlack);
	local vsSpin = sp.SkeletonAnimation:create("aniRes/vsAni/skeleton.json","aniRes/vsAni/skeleton.atlas",1);
    vsSpin:setPosition(cc.p(667, 375));
    vsSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
	-- vsSpin:registerSpineEventHandler(function (event)
	-- 	-- 八卦图替换
	-- 	self.m_rolllist[index]:variableImageChnage()
		
 --  	end, sp.EventType.ANIMATION_END)
    self.node_effect:addChild(vsSpin);

    self:runAction(cc.Sequence:create(
		cc.DelayTime:create(1.0),
		cc.CallFunc:create( function()
			vsSpin:removeAllChildren();
	        vsSpin:removeFromParent();
	        --开始下注
	        self:PlayStartJettonAnim();
	    end )
	));

end

--播放开始下注动画
function GameViewLayer:PlayStartJettonAnim()
	
 --    local animLayer = ExternalFun.loadCSB("aniRes/xiazhukaishiAni/xiazhukaishiLayer.csb", self.node_effect);

 --    local animTimeline = cc.CSLoader:createTimeline("aniRes/xiazhukaishiAni/xiazhukaishiLayer.csb");
 --    local animinfo = animTimeline:getAnimationInfo("play");
 --    -- animTimeline:setTimeSpeed(0.25);
 --    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false);
 --    animLayer:stopAllActions();
 --    animLayer:runAction(animTimeline)

 --    ExternalFun.playSoundEffect(cmd.EFFECT_StartJetton);

 --    animLayer:runAction(cc.Sequence:create(
	-- 	cc.DelayTime:create(1.0),
	-- 	cc.CallFunc:create( function()
	-- 		animLayer:removeAllChildren();
	--         animLayer:removeFromParent();

	--         --发牌动画
 --        	self:PlaySendCardAnim();
	--     end )
	-- ));

	--spin
	ExternalFun.playSoundEffect(cmd.EFFECT_StartJetton);
	local startSpin = sp.SkeletonAnimation:create("aniRes/xiazhukaishiAni/skeleton.json","aniRes/xiazhukaishiAni/skeleton.atlas",1);
    startSpin:setPosition(cc.p(667, 375));
    startSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    self.node_effect:addChild(startSpin);

    self:runAction(cc.Sequence:create(
		cc.DelayTime:create(1.0),
		cc.CallFunc:create( function()
			startSpin:removeAllChildren();
	        startSpin:removeFromParent();
			--发牌动画
       		self:PlaySendCardAnim();
	    end )
	));
end

--播放停止下注动画
function GameViewLayer:PlayStopJettonAnim()
 --    local animLayer = ExternalFun.loadCSB("aniRes/xiazhujieshuAni/xiazhujieshuLayer.csb", self.node_effect);

 --    local animTimeline = cc.CSLoader:createTimeline("aniRes/xiazhujieshuAni/xiazhujieshuLayer.csb");
 --    local animinfo = animTimeline:getAnimationInfo("play");
 --    -- animTimeline:setTimeSpeed(0.25);
 --    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false);
 --    animLayer:stopAllActions();
 --    animLayer:runAction(animTimeline);

 --    ExternalFun.playSoundEffect(cmd.EFFECT_StopJetton);

 --    animLayer:runAction(cc.Sequence:create(
	-- 	cc.DelayTime:create(1.0),
	-- 	cc.CallFunc:create( function()
	-- 		animLayer:removeAllChildren();
	--         animLayer:removeFromParent();
	--     end )
	-- ));

	--spin
	ExternalFun.playSoundEffect(cmd.EFFECT_StopJetton);
	local endSpin = sp.SkeletonAnimation:create("aniRes/xiazhujieshuAni/skeleton.json","aniRes/xiazhujieshuAni/skeleton.atlas",1);
    endSpin:setPosition(cc.p(667, 375));
    endSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    self.node_effect:addChild(endSpin);

    self:runAction(cc.Sequence:create(
		cc.DelayTime:create(1.0),
		cc.CallFunc:create( function()
			endSpin:removeAllChildren();
	        endSpin:removeFromParent();
	    end )
	));
end

--播放发牌动画
function GameViewLayer:PlaySendCardAnim()
	self:openRewardCardCreate(true);
	-- ExternalFun.playSoundEffect(cmd.EFFECT_OpenCard3);
end

--播放开牌动画
function GameViewLayer:PlayOpenCardAnim()
end

--显示等待开始游戏
function GameViewLayer:ShowWaitStartGame(time)
	if not self.waitStationLayer then
		self.waitStationLayer = ExternalFun.loadCSB("game/WaitStationLayer.csb", self.node_effect);
		self.wait_time_lab = self.waitStationLayer:getChildByName("wait_time_lab");
	end
    self.waitStationLayer:setVisible(true);

    self.waitCurTime = time;
    if self.wait_time_lab then
		self.wait_time_lab:setString(self.waitCurTime);
	end

	if self.waitCurTime then
		self:onUpdateTime();
	end

	if not self._timeLessUpdate then
    	-- self.txt_time:setVisible(true)
	    self._timeLessUpdate = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
	    	if self.waitCurTime then
	    		self:onUpdateTime();
	    	end
	    end, 1, false)
	end
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

--显示赢的区域
function GameViewLayer:ShowWinArea(isRedWin, isBlackWin, isPokerTypeWin)
	local node_win_bg = self._csbNode:getChildByName("node_win_bg");
	local balck_win = node_win_bg:getChildByName("balck_win");
	local red_win = node_win_bg:getChildByName("red_win");
	local win_type = node_win_bg:getChildByName("win_type");
	--闪烁时间、次数
	local blinkTime = 2.0;
	local blinkNum = 3;

    if isRedWin then
    	red_win:stopAllActions();
        red_win:runAction(cc.Sequence:create(
        	cc.CallFunc:create( function()
		        red_win:setVisible(true);
		    end ),
			cc.Blink:create(blinkTime,blinkNum),
			cc.CallFunc:create( function()
		        red_win:setVisible(false);
		    end )
		));
    end

    if isBlackWin then
    	balck_win:stopAllActions();
        balck_win:runAction(cc.Sequence:create(
        	cc.CallFunc:create( function()
		        balck_win:setVisible(true);
		    end ),
			cc.Blink:create(blinkTime,blinkNum),
			cc.CallFunc:create( function()
		        balck_win:setVisible(false);
		    end )
		));
    end

    if isPokerTypeWin then
    	win_type:stopAllActions();
        win_type:runAction(cc.Sequence:create(
        	cc.CallFunc:create( function()
		        win_type:setVisible(true);
		    end ),
			cc.Blink:create(blinkTime,blinkNum),
			cc.CallFunc:create( function()
		        win_type:setVisible(false);
		    end )
		));
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
            self._scene.m_cbGameStatus = cmd.GS_PLACE_JETTON;
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
	elseif tag == TAG_ENUM.BT_Detail then         --用户请求记录信息
		self._scene:sendQuraryRecord();
		self:onTrendInfoLayer();
	elseif tag == TAG_ENUM.BT_OtherPlayer then     -- 查询房间中用户的信息
		self._scene:sendQuraryRoomUsersInfo();
		self:onPlayerListLayer();
	elseif tag == TAG_ENUM.BT_ChongFu then       -- 重复投
		self:OnReJettonBtnClick();
	elseif tag == TAG_ENUM.BT_Help then
		self:onButtonClickedEvent(TAG_ENUM.BT_Menu);
        self:onHelpLayer();
	elseif tag == TAG_ENUM.BT_Set then
		self:onButtonClickedEvent(TAG_ENUM.BT_Menu);
        self:onSettingLayer();
	elseif tag == TAG_ENUM.BT_RewardPool then     -- 用户查询开奖信息
		self:onRewardPoolLayer();
		self._scene:sendQuraryLottoryInfo();
    elseif tag == TAG_ENUM.BT_APPLY_BANKER then     -- 上庄列表
        self:onBankerListLayer();
	elseif tag == TAG_ENUM.BT_Myself_Info then
		self:onRoleInfoLayer()
	elseif tag >= TAG_ENUM.BT_JETTONAREA_0 and tag <= TAG_ENUM.BT_JETTONAREA_2 then
		self:OnPlayerJettonClick(ref:getTag()-TAG_ENUM.BT_JETTONAREA_0+1, ref);
	elseif tag >= TAG_ENUM.BT_JETTONSCORE_0 and tag <= TAG_ENUM.BT_JETTONSCORE_4 then
		self:OnBetNumButtonClick(ref:getTag()-TAG_ENUM.BT_JETTONSCORE_0+1, ref);
	elseif tag >= TAG_ENUM.BT_SEAT_1 and tag <= TAG_ENUM.BT_SEAT_6 then
		self:OnPlayerUpDeskClick(ref:getTag()-TAG_ENUM.BT_SEAT_1+1, ref);
    elseif tag == TAG_ENUM.BT_BANK then
        --银行
        if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
            showToast(self, "初次使用，请先开通银行！", 1)
             return 
        end

        -- if self.m_HasJetton == true or (self:isMeChair(self.m_wBankerUser) and self.m_cbGameStatus ~= cmd.GAME_SCENE_FREE)  then 
        --     showToast(self,"游戏过程中不能进行银行操作",1)
        --     return
        -- end
        
        self.m_bankLayer = BankLayer:create(self):addTo(self)
	else
		showToast(self,"功能尚未开放！",1)
	end
end

--移除银行
function GameViewLayer:removeBankerLayer()
    if self.m_bankLayer then
        self.m_bankLayer:removeFromParent()
        self.m_bankLayer = nil
    end
end

-- 菜单上拉下拉
function GameViewLayer:onMenuAction(isDown)
	if isDown then
		self.node_menu:runAction(cc.Sequence:create(
			cc.CallFunc:create( function()
		        self.node_menu:setVisible(true);
		    end),
			cc.MoveTo:create(0.2,cc.p(1270,704))
		));

		self.btn_menu:runAction(
			cc.RotateTo:create(0.2,180)
		);
	else
		self.node_menu:runAction(cc.Sequence:create(
			cc.MoveTo:create(0.2,cc.p(1270,920)),
			cc.CallFunc:create( function()
		        self.node_menu:setVisible(false);
		    end)
		));

		self.btn_menu:runAction(
			cc.RotateTo:create(0.2,0)
		);
	end

	self.layoutShield:setTouchEnabled(isDown)
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

-- 玩家列表
function GameViewLayer:onPlayerListLayer()
	self.playerListLayer = PlayerListLayer:create();
	self.playerListLayer.m_parent = self;
	self.node_layer:addChild(self.playerListLayer,10);
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

-- 玩家列表数据刷新
function GameViewLayer:onPlayerListFresh(roomUsersInfo)
	if self.playerListLayer then
		self.playerListLayer:freshPlayersData(roomUsersInfo);
	end
end

-- 奖池信息
function GameViewLayer:onRewardPoolLayer()
	self.rewardPoolLayer = RewardPoolLayer:create();
	self.rewardPoolLayer.m_parent = self;
	self.node_layer:addChild(self.rewardPoolLayer,10);
end

-- 奖池信息刷新
function GameViewLayer:onRewardPoolFresh(rewardData)
	self.rewardPoolLayer:rewardPoolFresh(rewardData);
end

-- 刷新玩家列表
function GameViewLayer:refreshUserList(  )
	-- 玩家列表
	if self.playerListLayer then
		local userList = self:getDataMgr():getUserList();
        -- dump(userList, "-------------刷新玩家列表------------", 6)		
		self.playerListLayer:refreshList(userList);
	end
end

-- 玩家详情
function GameViewLayer:onRoleInfoLayer()
	local roleInfoLayer = RoleInfoLayer:create();
	roleInfoLayer.m_parent = self;
	self.node_layer:addChild(roleInfoLayer,10);

	local roleItem = self:getMeUserItem();
	roleInfoLayer:freshDeskRoleInfo(roleItem);
end

-- 坐桌玩家详情
function GameViewLayer:onDeskRoleInfoLayer(roleItem)
	local roleInfoLayer = RoleInfoLayer:create();
	roleInfoLayer.m_parent = self;
	self.node_layer:addChild(roleInfoLayer,10);

	roleInfoLayer:freshDeskRoleInfo(roleItem);
end

-- 红黑走势
function GameViewLayer:onTrendInfoLayer()
	self.trendInfoLayer = TrendInfoLayer:create();
	self.trendInfoLayer.m_parent = self;
	self.node_layer:addChild(self.trendInfoLayer,10);
end

-- 红黑走势刷新
function GameViewLayer:onTrendInfoFresh(recordData)
	if self.trendInfoLayer then
		self.trendInfoLayer:trendInfoFresh(recordData);
	end
end

--根据玩家的得分，生成筹码
function GameViewLayer:CreateChoumaObjForScore(messageData)

    --记录实际得分
    local selfWinScore = messageData.lUserScore + messageData.lUserReturnScore;
    self.m_PlayerWinScore = selfWinScore;

    for i,deskScore in ipairs(messageData.lBetDeskScore[1]) do
        --当自己在桌子上时，不重复记录得分
        if self.m_TableUserChairId[i] ~= self.m_UserChairID then
            self.m_TablePlayerWinScore[i] = deskScore;
        end
    end

    --根据不含税的得分，生成筹码
    local selfWinScoreNotWithRevenue = messageData.lUserReturnScore + messageData.lUserScoreT;

    self:chipCreatByWinScore(messageData.bWinRed, selfWinScoreNotWithRevenue, messageData.lDeskScoreT[1]);
end

--开奖筹码移动处理事件
function GameViewLayer:ChoumaMoveingHandler(messageData)
    local isPokerTypeWin = messageData.bWinPoker;
    --隐藏自己下注Label
    self:HideAreaUserJettonLabel();

    --需要移动的筹码的区域index
    local loseAreaIndexs = {};
    local winAreaIndexs = {};

    loseAreaIndexs[1] = messageData.bWinRed and cmd.BLACK_INDEX or cmd.RED_INDEX;

    if messageData.bWinRed then
        loseAreaIndexs[1] = cmd.BLACK_INDEX;
        winAreaIndexs[1] = cmd.RED_INDEX;
    else
        loseAreaIndexs[1] = cmd.RED_INDEX;
        winAreaIndexs[1] = cmd.BLACK_INDEX;
    end
    if isPokerTypeWin then
        winAreaIndexs[2] = cmd.POKER_TYPE_INDEX;
    else
        loseAreaIndexs[2] = cmd.POKER_TYPE_INDEX;
    end
    --筹码移动到赢分区域
    self:chipMoveToWinArea(loseAreaIndexs, winAreaIndexs);

    -- --从赢分区域 移动到 指定玩家区域
    self:chipMoveToPlayerArea(winAreaIndexs);
end

-- 显示玩家得分
function GameViewLayer:ShowPlayerWinScore()
	local node_role_bet = self._csbNode:getChildByName("node_role_bet");
	node_role_bet:setVisible(true);
	--自己的得分
	local role_selfbet_score = node_role_bet:getChildByName("role_selfbet_score");
	role_selfbet_score:setVisible(true);
	local win_num = role_selfbet_score:getChildByName("win_num");
	local win_icon_1 = win_num:getChildByName("win_icon_1");
	local win_icon_2 = role_selfbet_score:getChildByName("win_icon_2");

	local lose_num = role_selfbet_score:getChildByName("lose_num");
	local lose_icon_1 = lose_num:getChildByName("lose_icon_1");
	local lose_icon_2 = role_selfbet_score:getChildByName("lose_icon_2");

	local betScore = math.abs(self.m_PlayerWinScore);
	local showNum, iconType = self:formatScoreText(betScore);
	local frame = nil;
    if iconType == 0 then
        win_icon_2:setVisible(false);
        lose_icon_2:setVisible(false);
    elseif iconType == 1 then
        win_icon_2:setVisible(true);
        lose_icon_2:setVisible(true);
        if self.m_PlayerWinScore > 0 then
        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_wan.png");
        else
        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/lose_wan.png");
        end
    elseif iconType == 2 then
        win_icon_2:setVisible(true);
        lose_icon_2:setVisible(true);
        if self.m_PlayerWinScore > 0 then
        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_yi.png");
        else
        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/lose_yi.png");
        end
    end
    if nil ~= frame then
        win_icon_2:setSpriteFrame(frame);
        lose_icon_2:setSpriteFrame(frame);
    end
    -- local loseScore = math.abs(showNum);
    win_num:setString(showNum);
    lose_num:setString(showNum);

	if self.m_PlayerWinScore > 0 then
		-- win_num:setString(self.m_PlayerWinScore);
		win_num:setVisible(true);
		lose_num:setVisible(false);

		--手动刷新玩家金币
        local currentUserMoney = self.m_PlayerMoney + self.m_PlayerWinScore;
        self:UpdateUserMoney(currentUserMoney);
	elseif self.m_PlayerWinScore < 0 then
		-- local loseScore = math.abs(self.m_PlayerWinScore);
		-- win_num:setString(loseScore);
		win_num:setVisible(false);
		lose_num:setVisible(true);
	else
		role_selfbet_score:setVisible(false);
	end

	--桌子上的玩家得分
	for i = 1, 6 do
		local role_totalbet = node_role_bet:getChildByName("role_totalbet_" .. i);
		role_totalbet:setVisible(true);

		local win_num = role_totalbet:getChildByName("win_num");
		local win_icon_1 = win_num:getChildByName("win_icon_1");
		local win_icon_2 = role_totalbet:getChildByName("win_icon_2");

		local lose_num = role_totalbet:getChildByName("lose_num");
		local lose_icon_1 = lose_num:getChildByName("lose_icon_1");
		local lose_icon_2 = role_totalbet:getChildByName("lose_icon_2");

		local scoreNum = self.m_TablePlayerWinScore[i];
		local betScore = math.abs(scoreNum);
		local showNum, iconType = self:formatScoreText(betScore);

		local frame = nil;
	    if iconType == 0 then
	        win_icon_2:setVisible(false);
	        lose_icon_2:setVisible(false);
	    elseif iconType == 1 then
	        win_icon_2:setVisible(true);
	        lose_icon_2:setVisible(true);
	        if scoreNum > 0 then
	        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_wan.png");
	        else
	        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/lose_wan.png");
	        end
	    elseif iconType == 2 then
	        win_icon_2:setVisible(true);
	        lose_icon_2:setVisible(true);
	        if scoreNum > 0 then
	        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/win_yi.png");
	        else
	        	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/lose_yi.png");
	        end
	    end
	    if nil ~= frame then
	        win_icon_2:setSpriteFrame(frame);
	        lose_icon_2:setSpriteFrame(frame);
	    end
	    -- local loseScore = math.abs(showNum);
	    win_num:setString(showNum);
	    lose_num:setString(showNum);

		
		if scoreNum > 0 then
			-- win_num:setString(scoreNum);
			win_num:setVisible(true);
			lose_num:setVisible(false);
		elseif scoreNum < 0 then
			-- local loseScore = math.abs(scoreNum);
			-- lose_num:setString(loseScore);
			win_num:setVisible(false);
			lose_num:setVisible(true);
		else
			role_totalbet:setVisible(false);
		end
	end

	local time = 2;
	node_role_bet:stopAllActions();
	node_role_bet:runAction(cc.Sequence:create(
		cc.MoveBy:create(2.0,cc.p(0,80)),
		cc.DelayTime:create(0.8),
		cc.CallFunc:create( function()
	        node_role_bet:setVisible(false);
	    end ),
	    cc.MoveBy:create(0.1,cc.p(0,-80))
	));
end

function GameViewLayer:showResultByCardType(wlHistory, cardType)
	local historyStr = "";
	if wlHistory and wlHistory == 1 then  -- 红方胜
        historyStr = "game/game/history_red.png";
    elseif wlHistory and wlHistory == 2 then  -- 黑方胜
       historyStr = "game/game/history_black.png";
    else
    end

	local item = cardType;
    local recordTypeStr = "";
    local typeBg = "";
    if item == 1 then  
        recordTypeStr = "game/cardtype/record_type_1_1.png";
        typeBg = "game/game/box_balck.png";
    elseif item == 2 then --2到8
        recordTypeStr = "game/cardtype/record_type_1_2.png";
        typeBg = "game/game/box_balck.png";
    elseif item == 3 then -- 9到a
        recordTypeStr = "game/cardtype/record_type_2_2.png";
        typeBg = "game/game/box_yellow.png";
    elseif item > 3 then
        recordTypeStr = "game/cardtype/record_type_2_" .. item-1 .. ".png";
        typeBg = "game/game/box_yellow.png";
    else
    end
    return historyStr, recordTypeStr, typeBg;
end

-- 输赢路单移动
function GameViewLayer:showResultMove(cmd_table)
    local historyArr = cmd_table.cbRB_WL_History[1];
    local cardTypeArr = cmd_table.cbPokerType[1];
    
	local result_0 = self._csbNode:getChildByName("result_0");
	result_0:setVisible(true);

	local card_bg_0 = self._csbNode:getChildByName("card_bg_0");
	card_bg_0:setVisible(true);
	local card_type = card_bg_0:getChildByName("card_type");

	local historyStr,recordTypeStr, typeBg = GameViewLayer:showResultByCardType(historyArr[20], cardTypeArr[20]);
	local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame(historyStr);
	local frame2 = cc.SpriteFrameCache:getInstance():getSpriteFrame(typeBg);
	local frame3 = cc.SpriteFrameCache:getInstance():getSpriteFrame(recordTypeStr);
	if frame1 then
		result_0:setSpriteFrame(frame1);
	else
		result_0:setVisible(false);
	end
	if frame2 then
		card_bg_0:setSpriteFrame(frame2);
	else
		card_bg_0:setVisible(false);
	end
	if frame3 then
		card_type:setSpriteFrame(frame3);
	else
	end

	local node_result = self._csbNode:getChildByName("node_result");
	for i = 1, 15 do
		local result = node_result:getChildByName("result_" .. i);
		result:setVisible(true);
		local historyStr,recordTypeStr, typeBg = GameViewLayer:showResultByCardType(historyArr[i+4], cardTypeArr[i+4]);
		local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame(historyStr);
		if frame1 then
			result:setSpriteFrame(frame1);
		else
			result:setVisible(false);
		end
	end
	
	local node_card_type = self._csbNode:getChildByName("node_card_type");
	for i = 1, 7 do
		local card_bg = node_card_type:getChildByName("card_bg_" .. i);
		card_bg:setVisible(true);
		local card_type = card_bg:getChildByName("card_type");
		local historyStr,recordTypeStr, typeBg = GameViewLayer:showResultByCardType(historyArr[i+12], cardTypeArr[i+12]);
		local frame2 = cc.SpriteFrameCache:getInstance():getSpriteFrame(typeBg);
		local frame3 = cc.SpriteFrameCache:getInstance():getSpriteFrame(recordTypeStr);
		if frame2 then
			card_bg:setSpriteFrame(frame2);
		else
			card_bg:setVisible(false);
		end
		if frame3 then
			card_type:setSpriteFrame(frame3);
		else
		end
	end

	local time1 = 1.0;
	local time2 = 0.001;
	--平移
	result_0:stopAllActions();
	card_bg_0:stopAllActions();
	node_result:stopAllActions();
	node_card_type:stopAllActions();

	result_0:runAction(cc.Sequence:create(
		cc.MoveBy:create(time1,cc.p(-46,-82)),
		cc.CallFunc:create( function()
	        result_0:setVisible(false);
	    end ),
	    cc.MoveBy:create(time2,cc.p(46,82))
	));
	card_bg_0:runAction(cc.Sequence:create(
		cc.MoveBy:create(time1,cc.p(-74,-82)),
		cc.CallFunc:create( function()
	        card_bg_0:setVisible(false);
	    end ),
	    cc.MoveBy:create(time2,cc.p(74,82))
	));

	node_result:runAction(cc.Sequence:create(
		cc.MoveBy:create(time1,cc.p(-56,0)),
		cc.CallFunc:create( function()
	        node_result:setPosition(cc.p(0,0))
	    end )
	));

	node_card_type:runAction(cc.Sequence:create(
		cc.MoveBy:create(time1,cc.p(-122,0)),
		cc.CallFunc:create( function()
	        node_card_type:setPosition(cc.p(0,0))

	        self:showRecordResult(cmd_table);
	    end )
	));
end

-- 输赢路单结果
function GameViewLayer:showRecordResult(cmd_table)
    local historyArr = cmd_table.cbRB_WL_History[1];
    local cardTypeArr = cmd_table.cbPokerType[1];
    
	local result_0 = self._csbNode:getChildByName("result_0");
	result_0:setVisible(false);
	local card_bg_0 = self._csbNode:getChildByName("card_bg_0");
	card_bg_0:setVisible(false);
	local card_type = card_bg_0:getChildByName("card_type");

	local node_result = self._csbNode:getChildByName("node_result");
	for i = 1, 15 do
		local result = node_result:getChildByName("result_" .. i);
		result:setVisible(true);
		local historyStr,recordTypeStr, typeBg = GameViewLayer:showResultByCardType(historyArr[i+5], cardTypeArr[i+5]);
		local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame(historyStr);
		if frame1 then
			result:setSpriteFrame(frame1);
		else
			result:setVisible(false);
		end
	end
	
	local node_card_type = self._csbNode:getChildByName("node_card_type");
	for i = 1, 7 do
		local card_bg = node_card_type:getChildByName("card_bg_" .. i);
		card_bg:setVisible(true);
		local card_type = card_bg:getChildByName("card_type");
		local historyStr,recordTypeStr, typeBg = GameViewLayer:showResultByCardType(historyArr[i+13], cardTypeArr[i+13]);
		local frame2 = cc.SpriteFrameCache:getInstance():getSpriteFrame(typeBg);
		local frame3 = cc.SpriteFrameCache:getInstance():getSpriteFrame(recordTypeStr);
		if frame2 then
			card_bg:setSpriteFrame(frame2);
		else
			card_bg:setVisible(false);
		end
		if frame3 then
			card_type:setSpriteFrame(frame3);
		else
		end
	end
end
-- 第三张亮牌
function GameViewLayer:cardClippingShow(cardIcon)
	local sp = cc.Node:create();
	sp:setPosition(cc.p(667,430 + 75));
	self.node_card:addChild(sp);

	local card = cc.Sprite:create(cardIcon);
	if not card then
		return;
	end
	card:setScale(1.5);
	card:setPosition(cc.p(0,-200));

	--创建裁剪
	local strClip = "game/cardmask.png";
	local clipSp = nil;
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strClip);
	if nil ~= frame then
		clipSp = cc.Sprite:createWithSpriteFrame(frame);
	else
		clipSp = cc.Sprite:create(strClip);
	end
	clipSp:setScale(1.5);
	if nil ~= clipSp then
		--裁剪
		local clip = cc.ClippingNode:create();
		clip:setStencil(clipSp);
		clip:setAlphaThreshold(0.05);
		clip:addChild(card);
		-- local selfSize = sp:getContentSize();
		clip:setPosition(cc.p(0, 0));
		sp:addChild(clip);

		--背面
		local m_spBack = cc.Sprite:create("card/Card001.png");
		m_spBack:setScale(1.5);
		m_spBack:setPosition(cc.p(0,-75));
	    clip:addChild(m_spBack,-1);

		card:stopAllActions();
		card:runAction(cc.Sequence:create(
			cc.MoveBy:create(0.5,cc.p(0,125)),
			cc.MoveBy:create(0.3,cc.p(0,75)),
	        cc.DelayTime:create(0.2),
			cc.CallFunc:create(function()
				m_spBack:removeFromParent();
	            clipSp:removeFromParent();
	            clip:removeFromParent();
	            sp:removeAllChildren();
	            sp:removeFromParent();
	        end)
		));

		clip:stopAllActions();
		clip:runAction(cc.Sequence:create(
			cc.DelayTime:create(0.5),
			cc.CallFunc:create(function()
	            m_spBack:setVisible(false);
	        end),
			cc.MoveBy:create(0.3,cc.p(0,-75))
			
		));
	end
end

-- -- 第三张亮牌、只上移
-- function GameViewLayer:cardClippingShow(cardIcon)
-- 	local sp = cc.Node:create();
-- 	sp:setPosition(cc.p(667,430));
-- 	self.node_effect:addChild(sp);
-- 	local selfSize = sp:getContentSize();

-- 	local card = cc.Sprite:create(cardIcon);
-- 	if not card then
-- 		return;
-- 	end
-- 	card:setScale(1.4);
-- 	card:setPosition(cc.p(selfSize.width * 0.5,-200));

-- 	--创建裁剪
-- 	local strClip = "game/cardmask.png";
-- 	local clipSp = nil;
-- 	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strClip);
-- 	if nil ~= frame then
-- 		clipSp = cc.Sprite:createWithSpriteFrame(frame);
-- 	else
-- 		clipSp = cc.Sprite:create(strClip);
-- 	end
-- 	clipSp:setScale(1.4);
-- 	if nil ~= clipSp then
-- 		--裁剪
-- 		local clip = cc.ClippingNode:create();
-- 		clip:setStencil(clipSp);
-- 		clip:setAlphaThreshold(0.05);
-- 		clip:addChild(card);
-- 		clip:setPosition(cc.p(selfSize.width * 0.5, selfSize.height * 0.5));
-- 		sp:addChild(clip);

-- 		card:stopAllActions();
-- 		card:runAction(cc.Sequence:create(
-- 			cc.MoveBy:create(0.6,cc.p(0,200)),
-- 	        cc.DelayTime:create(0.4),
-- 			cc.CallFunc:create(function()
-- 	            clipSp:removeFromParent();
-- 	            clip:removeFromParent();
-- 	            sp:removeAllChildren();
-- 	            sp:removeFromParent();
-- 	        end)
-- 		));
-- 	end
-- end
-- 筹码相关
-----------------------------------------------------------------
-- 筹码创建
function GameViewLayer:chipCreatByType(chipType)
	-- local chip = nil;
    print("chipType=========>",chipType)
	local file = cmd.RES.."game/chip_" .. chipType ..".png"
    local chip = display.newSprite(file);
    -- chip:setScale(0.6)
	-- if nil ~= file then
 --        print("===================>kong")
 --        return
	-- end

    --创建筹码字
    local lab = cc.LabelAtlas:create("","game/atlas.png",13, 17, string.byte("."))
    chip:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(chip:getContentSize().width/2,chip:getContentSize().height/2 + 2)
    local num = ChoumaType[chipType]
    lab:setString(num)
    -- lab:setScale(0.5)
    if num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("game/wan.png")
        chip:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(chip:getContentSize().width/2-8)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , chip:getContentSize().height/2 + 2)
    end
    -- local label = self:createChipText(ChoumaType[chipType], false, chipType)
    -- label:setAnchorPoint(cc.p(0.5,0.5))
    -- chip:addChild(label)
    -- if chipType == 5 then
    --     label:setPosition(cc.p(60,36));
    -- else
    --     label:setPosition(cc.p(32,36));
    -- end

	return chip;
end

-- 筹码移动到红黑区
function GameViewLayer:chipMoveToArea(jettonScore, startPos, areaIndex)
	if not jettonScore or jettonScore < ChoumaType[1] then
		return;
	end

	--创建对应下注数值的筹码
    local chipIndex = self:CalculateChoumaByTotal(jettonScore)[1];
    local chip = self:chipCreatByType(chipIndex);
    chip:setPosition(startPos);
    self.node_chip:addChild(chip);

    table.insert(self.m_curAreaJettonList[areaIndex], chip);

    --筹码移动
    local btn = self.AreaJettonButtons[areaIndex];
    local endPos = self:getBetRandomPos(btn);

    if chip and endPos then
    	ExternalFun.playSoundEffect(cmd.EFFECT_AreaGoldMoveing);
    	
    	self:chipMoveAnimation(chip, endPos);
    end
	
end

-- 筹码移动动画、由快至慢
function GameViewLayer:chipMoveAnimation(chip, pos, isHide)
	if chip == nil or tolua.isnull(chip) then
		return;
	end

	local moveTime = 0.25;
	
	local moveTo = cc.MoveTo:create(moveTime, cc.p(pos.x, pos.y));
	local seq = cc.Sequence:create(
				cc.DelayTime:create(0.01),
				cc.EaseOut:create(moveTo, 2),
				cc.DelayTime:create(0.02),
				cc.CallFunc:create(function()
					if isHide then
						chip:removeAllChildren();
						chip:removeFromParent();
					end
				end)
			);
	if chip then
		chip:stopAllActions();
		chip:runAction(seq);
	end
end
-- 区域随机位置
function GameViewLayer:getBetRandomPos(areaBtn)
	if nil == areaBtn then
		return {x = 0, y = 0};
	end

	local tag = areaBtn:getTag();
	local disY = 20;
	if tag == TAG_ENUM.BT_JETTONAREA_2 then
		disY = 10;
	end

	local nodeSize = cc.size(areaBtn:getContentSize().width - 120, areaBtn:getContentSize().height - 120);
	local xOffset = math.random();
	local yOffset = math.random();

	local posX = areaBtn:getPositionX() - areaBtn:getAnchorPoint().x * nodeSize.width;
	local posY = areaBtn:getPositionY() - areaBtn:getAnchorPoint().y * nodeSize.height - disY;
	return cc.p(xOffset * nodeSize.width + posX, yOffset * nodeSize.height + posY);
end
-- 由快至慢移动
function GameViewLayer:getBetAnimation(pos, call_back)
	local moveTo = cc.MoveTo:create(0.25, cc.p(pos.x, pos.y))
	if nil ~= call_back then
		return cc.Sequence:create(cc.EaseOut:create(moveTo, 2), call_back);
	else
		return cc.EaseOut:create(moveTo, 2);
	end
end

-- 开奖后 筹码移动到指定区域
function GameViewLayer:chipMoveToWinArea(loseAreaIndexs, winAreaIndexs)
	-- table.insert(self.m_curAreaJettonList[areaIndex], chip);

	local needMoveChip = {};
	--拿到所有需要移动的筹码
	for _,areaIndex in ipairs(loseAreaIndexs) do
		local chipCount = #self.m_curAreaJettonList[areaIndex];
		if chipCount > 0 then
			for i=1,chipCount do
				local chip = self.m_curAreaJettonList[areaIndex][i];
				table.insert(needMoveChip, chip)
			end
		end
	end

	--没有筹码需要移动区域，则不执行
	if #needMoveChip > 0 then
		--移动到指定点
		local endPos = cc.p(667, 710);
		
		self:runAction(cc.Sequence:create(
	        cc.CallFunc:create( function()
	        	ExternalFun.playSoundEffect(cmd.EFFECT_AreaGoldMoveing);

	            for _,chip in ipairs(needMoveChip) do
	            	self:chipMoveAnimation(chip, endPos);
				end
	        end ),
	        cc.DelayTime:create(0.5),
	        cc.CallFunc:create( function()
	        	ExternalFun.playSoundEffect(cmd.EFFECT_AreaGoldMoveing);

	            --从指定点 移动到 赢的区域
				local winAreaNum = #winAreaIndexs;
				if winAreaNum == 1 then
					--只有一个赢区域时，全部筹码移动到赢的区域
					local winAreaIndex = winAreaIndexs[1];
					for _,chip in ipairs(needMoveChip) do
						local btn = self.AreaJettonButtons[winAreaIndex];
	    				local endPos = self:getBetRandomPos(btn);
						self:chipMoveAnimation(chip, endPos);
					end
				elseif winAreaNum == 2 then
					--当有两个赢的区域时，每个区域各放一半的筹码
					local chipHalfNum = math.floor(#needMoveChip / 2);

					local winAreaIndex1 = winAreaIndexs[1];
					for i=1,chipHalfNum do
						local chip = needMoveChip[i];

						local btn = self.AreaJettonButtons[winAreaIndex1];
	    				local endPos = self:getBetRandomPos(btn);
						self:chipMoveAnimation(chip, endPos);
					end

					local winAreaIndex2 = winAreaIndexs[2];
					for i=chipHalfNum+1,#needMoveChip do
						local chip = needMoveChip[i];

						local btn = self.AreaJettonButtons[winAreaIndex2];
	    				local endPos = self:getBetRandomPos(btn);
						self:chipMoveAnimation(chip, endPos);
					end

				end
	        end ),
	        cc.DelayTime:create(0.25),
	        cc.CallFunc:create( function()
	            --显示之前隐藏的 玩家赢得筹码
		    	self:ShowAreaAllChouma();
	        end )
	    ));
	else
		self:ShowAreaAllChouma();
	end
end

-- 根据开奖结果，生成玩家赢的筹码(自己,桌上玩家)
function GameViewLayer:chipCreatByWinScore(isWinRed, selfWinScore, betDeskScores)
	local winAreaIndex = isWinRed and cmd.RED_INDEX or cmd.BLACK_INDEX;

	--根据玩家的得分，生成对应的筹码
	if selfWinScore > 0 then
		--生成并隐藏玩家赢得筹码
		self.m_PlayerWinChouma = self:inintChip(winAreaIndex, selfWinScore, false);
	end
	
	--根据桌子上的玩家赢分，生成对应的筹码
	local myChairId = self._scene:GetMeChairID();

	for i,deskScore in ipairs(betDeskScores) do
		local deskUserChairId = self.m_TableUserChairId[i];

		local tablePlayerWinChoumaObjs = {}
		--当桌子上的玩家赢分 且 不是自己 的情况下，生成筹码
		if deskScore > 0 and deskUserChairId ~= myChairId then
			--生成并隐藏桌子上玩家赢得筹码
			self.m_TablePlayerWinChouma[i] = self:inintChip(winAreaIndex, deskScore, false);
		end
	end
end

--初始化桌面筹码
function GameViewLayer:inintChip(areaIndex, totalJetton, isHide)
	local chipList = {};

    local chipIndexList = self:CalculateChoumaByTotal(totalJetton);

    local btn = self.AreaJettonButtons[areaIndex];
    for _,chipIndex in ipairs(chipIndexList) do
    	local startPos = self:getBetRandomPos(btn);

        local chip = self:chipCreatByType(chipIndex);
	    chip:setPosition(startPos);
	    chip:setVisible(isHide);
	    self.node_chip:addChild(chip);

	    table.insert(chipList, chip);
    end

    return chipList;
end

--显示指定区域所有筹码、生成的
function GameViewLayer:ShowAreaAllChouma()
	for i=1,#self.m_PlayerWinChouma do
        if self.m_PlayerWinChouma[i] then
            self.m_PlayerWinChouma[i]:setVisible(true);
        end
	end

	for i=1,6 do
		for j=1,#self.m_TablePlayerWinChouma[i] do
			self.m_TablePlayerWinChouma[i][j]:setVisible(true);
		end
	end
end

-- 筹码移动到指定玩家区
function GameViewLayer:chipMoveToPlayerArea(winAreaIndexs)
	
	self:runAction(cc.Sequence:create(
		cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
        	ExternalFun.playSoundEffect(cmd.EFFECT_AreaGoldMoveing);

            --其他筹码飞向其他玩家位置
            for i = 1, 3 do
            	for j=1,#self.m_curAreaJettonList[i] do
					local chip = self.m_curAreaJettonList[i][j];
					local endPos = PlayerJettonChoumaPos[OtherPlayer];

					local xOffset = math.random()*100-100;
					local yOffset = math.random()*100-100;
					endPos = cc.p(endPos.x + xOffset,endPos.y + yOffset);
					-- local btn = self.AreaJettonButtons[i];
    	-- 			local endPos = self:getBetRandomPos(btn);
					self:chipMoveAnimation(chip, endPos, true);
				end
            end
            --玩家赢的筹码飞向玩家头像
            for _,chip in ipairs(self.m_PlayerWinChouma) do
				local endPos = PlayerJettonChoumaPos[Self];
				local xOffset = math.random()*100-50;
				local yOffset = math.random()*100-80;
				endPos = cc.p(endPos.x + xOffset,endPos.y + yOffset);
				self:chipMoveAnimation(chip, endPos, true);
			end

			--桌子上玩家赢的筹码飞向相应头像
			for i,tablePlayerInfo in ipairs(self.m_TablePlayerWinChouma) do
				if tablePlayerInfo then
					--筹码移动
				    local endPos = nil
				    if i == 1 then
				    	endPos = PlayerJettonChoumaPos[TablePlayer1];
				    elseif i == 2 then
				    	endPos = PlayerJettonChoumaPos[TablePlayer2];
				    elseif i == 3 then
				    	endPos = PlayerJettonChoumaPos[TablePlayer3];
			    	elseif i == 4 then
			    		endPos = PlayerJettonChoumaPos[TablePlayer4];
		    		elseif i == 5 then
		    			endPos = PlayerJettonChoumaPos[TablePlayer5];
	    			elseif i == 6 then
	    				endPos = PlayerJettonChoumaPos[TablePlayer6];
	    			end
				    
					for _,chip in ipairs(tablePlayerInfo) do
						local xOffset = math.random()*40-20;
						local yOffset = math.random()*80-40;
						endPos = cc.p(endPos.x + xOffset,endPos.y + yOffset);
						self:chipMoveAnimation(chip, endPos, true);
					end
				end
			end
			
			
        end ),
        cc.DelayTime:create(0.5),
        cc.CallFunc:create( function()
            --筹码隐藏并回收
            -- self.node_chip:removeAllChildren();
        end )
    ));
end
--根据筹码总数计算筹码的个数
function GameViewLayer:CalculateChoumaByTotal(totalChoumaNum)
    local choumaTable = {}
    --计算500W筹码个数
    while(totalChoumaNum >= ChoumaType[5])
    do
        totalChoumaNum = totalChoumaNum - ChoumaType[5];
        table.insert(choumaTable, 5);
    end
    --计算100W筹码个数
    while(totalChoumaNum >= ChoumaType[4])
    do
        totalChoumaNum = totalChoumaNum - ChoumaType[4];
        table.insert(choumaTable, 4);
    end
    --计算10W筹码个数
    while(totalChoumaNum >= ChoumaType[3])
    do
        totalChoumaNum = totalChoumaNum - ChoumaType[3];
        table.insert(choumaTable, 3);
    end
    --计算1W筹码个数
    while(totalChoumaNum >= ChoumaType[2])
    do
        totalChoumaNum = totalChoumaNum - ChoumaType[2];
        table.insert(choumaTable, 2);
    end
    --计算100筹码个数
    while(totalChoumaNum >= ChoumaType[1])
    do
        totalChoumaNum = totalChoumaNum - ChoumaType[1];
        table.insert(choumaTable, 1);
    end

    return choumaTable;
end

----------------------------------------------------------------------------------------------
--牌显示
-- 卡牌创建
function GameViewLayer:openRewardCardCreate(isFaPai)
	self.node_card:removeAllChildren();
	self.m_openRewardCardArr = {};

	self.node_poker_type:setVisible(false);
	for i=1,6 do
        local card = CardSprite:createCard();
        card:setScaleY(0.6);
        card:setScaleX(0.7);
        -- card:setVisible(false);
        if isFaPai then
        	card:setPosition(cc.p(667,705));
        else
        	card:setPosition(cardPosArr[i]);
        end
        
        self.node_card:addChild(card);

        self.m_openRewardCardArr[i] = card;
    end

    if isFaPai then
    	self:cardFaPaiAnimation();
    else
    end 
end

-- 发牌
function GameViewLayer:cardFaPaiAnimation()
	local delaytime = 0.2;
	for i=1,2 do
        for j=1,3 do
            local cardIndex = (i-1)*3 + j;
            local card = self.m_openRewardCardArr[cardIndex];
            if card == nil or tolua.isnull(card) then
				return;
			end
            local index = (i-1)*3 + j - 1;
            -- card:setPosition(652, 514)
            card:stopAllActions();
            card:runAction(cc.Sequence:create(
            	cc.DelayTime:create(delaytime*index), 
            	cc.CallFunc:create(function()
                    card:runAction(cc.FadeTo:create(0.04, 255));
                    card:runAction(cc.Sequence:create(cc.MoveTo:create(0.25, cardPosArr[cardIndex])));
                    	-- cc.MoveBy:create(0.04*(j-1), cc.p(32*(j-1),0))
            	end
            )));
        end
    end
end

-- 卡牌牌型显示
function GameViewLayer:cardTypeShow(cbPokerTypeArr, isRed, isBlack)
	self.node_poker_type:setVisible(true);
	local node_poker_type = self.node_poker_type;

	local black_bg = node_poker_type:getChildByName("black_bg");
	local red_bg = node_poker_type:getChildByName("red_bg");

	local black_card_type = node_poker_type:getChildByName("black_card_type");
	local red_card_type = node_poker_type:getChildByName("red_card_type");
	
	
	local blackStr = self:getCardTypeStr(cbPokerTypeArr[2]);
	local redStr = self:getCardTypeStr(cbPokerTypeArr[1]);

    local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame(redStr);
    if frame1 then
        red_card_type:setSpriteFrame(frame1);
    end

    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(blackStr);
    if frame then
        black_card_type:setSpriteFrame(frame);
    end

    red_card_type:setVisible(false);
    black_card_type:setVisible(false);
    red_bg:setVisible(false);
    black_bg:setVisible(false);

    if isRed then
    	red_bg:setVisible(true);
    	red_card_type:setVisible(true);
    end
    if isBlack then
    	black_bg:setVisible(true);
    	black_card_type:setVisible(true);
    end
end

-- 卡牌牌型
function GameViewLayer:getCardTypeStr(openType)
	local recordTypeStr = "";
    if openType == 1 then  
        recordTypeStr = "game/cardtype/type_1_1.png";
    elseif openType == 2 then --2到8
        recordTypeStr = "game/cardtype/type_1_2.png";
    elseif openType == 3 then -- 9到a
        recordTypeStr = "game/cardtype/type_2_2.png";
    elseif openType > 3 then
        recordTypeStr = "game/cardtype/type_2_" .. openType-1 .. ".png";
    else
    end
    return recordTypeStr;
end

-- 卡牌开奖
function GameViewLayer:cardOpenRewardAni(cardValueArr, cbPokerTypeArr)
	local time1 = 0.35;
	for i = 1, 6 do
		local card = self.m_openRewardCardArr[i];

		if card == nil or tolua.isnull(card) then
			return;
		end

		local cardValue = 0;
		if i > 3 then
			cardValue = cardValueArr[1][i-3];
		else
			cardValue = cardValueArr[2][i];
		end
		
		card:setCardValue(cardValue);
		if i == 1 or i == 2 then
			
			--翻牌
			if i == 2 then
				card:stopAllActions();
				card:runAction(cc.Sequence:create(
					cc.DelayTime:create(0.15 + time1),
					cc.CallFunc:create(function()
						card:showCardBack(false);
				       card:openCardAnimation();
				    end)
				));
				
			else
				card:showCardBack(false);
				card:openCardAnimation();
			end
			
		end
		
		local cardIcon = card:getCardIcon();
		local pointPos = cc.p(667, 430);
		local startPos1 = cardPosArr[3];
		local startPos2 = cardPosArr[6];
		if i == 3 then
			-- 黑区移动
			card:stopAllActions();
			card:runAction(cc.Sequence:create(
				cc.DelayTime:create(0.3 + time1),
				cc.CallFunc:create( function()
			        self:cardMoveAnimation(card, pointPos);
			    end ),
			    cc.DelayTime:create(0.5),
			    cc.CallFunc:create(function()
			    	card:setVisible(false);
			    	card:showCardBack(false);
			        self:cardClippingShow(cardIcon);
			    end),
			    cc.DelayTime:create(1.0),
				cc.CallFunc:create( function()
					card:setVisible(true);
					-- card:setScaleY(0.6);
     --    			card:setScaleX(0.7);
			        card:showCardBack(false);
			        -- self:cardTypeShow(cbPokerTypeArr, false, true);
			    end ),
			    cc.MoveTo:create(0.5, startPos1),
			    cc.CallFunc:create( function()
			    	card:setScaleY(0.6);
        			card:setScaleX(0.7);
			        self:cardTypeShow(cbPokerTypeArr, false, true);
			        card:setPosition(startPos1);
			    end )
				
			));

			card:runAction(cc.Sequence:create(
				cc.DelayTime:create(0.3 + time1),
				cc.ScaleTo:create(0.5, 1.5),
				cc.DelayTime:create(1.0),
				cc.ScaleTo:create(0.45, 0.65)
			));
		end
		
		if i == 4 or i == 5 then
			card:stopAllActions();
			card:runAction(cc.Sequence:create(
				cc.DelayTime:create(2.5 + time1),
				cc.CallFunc:create(function()
			        

			        --翻牌
			        if i == 4 then
			        	card:showCardBack(false);
						card:openCardAnimation();
					end
			    end),
			    cc.DelayTime:create(0.15 + time1),
				cc.CallFunc:create(function()
			        --翻牌
			        if i == 5 then
			        	card:showCardBack(false);
						card:openCardAnimation();
					end
			    end)
			));
		end

		if i == 6 then
			-- 黑区移动
			card:stopAllActions();
			card:runAction(cc.Sequence:create(
				cc.DelayTime:create(2.8 + time1*2),
				cc.CallFunc:create( function()
			        self:cardMoveAnimation(card, pointPos);
			    end ),
			    cc.DelayTime:create(0.5),
			    cc.CallFunc:create(function()
			    	card:showCardBack(false);
			    	card:setVisible(false);
			        self:cardClippingShow(cardIcon);
			    end),
			    cc.DelayTime:create(1.0),
				cc.CallFunc:create( function()
					card:setVisible(true);
					-- card:setScaleY(0.6);
     --    			card:setScaleX(0.7);
			        card:showCardBack(false);
			        -- self:cardTypeShow(cbPokerTypeArr, true, true);
			    end ),
			    cc.MoveTo:create(0.5, startPos2),
			    cc.CallFunc:create( function()
			    	card:setScaleY(0.6);
        			card:setScaleX(0.7);
			        self:cardTypeShow(cbPokerTypeArr, true, true);
			        card:setPosition(startPos2);
			    end )
			));

			card:runAction(cc.Sequence:create(
				cc.DelayTime:create(2.8 + time1*2),
				cc.ScaleTo:create(0.5, 1.5),
				cc.DelayTime:create(1.0),
				cc.ScaleTo:create(0.45, 0.65)
			));
		end

	end
end

-- 卡牌移动动画
function GameViewLayer:cardMoveAnimation(card, pos)
	if card == nil or tolua.isnull(card) then
		return;
	end

	local action = self:getCardAnimation(pos, cc.CallFunc:create(function()
		--播放下注声音
		-- ExternalFun.playSoundEffect("ADD_SCORE.wav")
	end));
	card:runAction(action);
end

-- 由快至慢移动
function GameViewLayer:getCardAnimation(pos, call_back)
	local moveTo = cc.MoveTo:create(0.5, cc.p(pos.x, pos.y))
	if nil ~= call_back then
		return cc.Sequence:create(cc.EaseOut:create(moveTo, 2), call_back);
	else
		return cc.EaseOut:create(moveTo, 2);
	end
end

--创建筹码上的文字
function GameViewLayer:createChipText(score, isBetBtn, chipType)
    local function formatScoreText(score)
        if score < 1000 then
            return score
        elseif score < 10000 then
            if isBetBtn then
                return score
            else
                return score/1000
            end
        elseif score < 100000000 then
            return score/10000
        else
            return score/100000000
        end
    end

    local panelwidth = 0
    local labelWidth = 0
    local imgWidth = 0
    --数字与汉字之间的距离
    local gap = 0       

    local ly = ccui.Layout:create()
    local text = formatScoreText(score)

    local label = nil;
    local img = nil

    if isBetBtn then
        label = cc.LabelAtlas:create(text,"font/bet_btn_font.png",19,26,string.byte("0"))
        label:setAnchorPoint(cc.p(0.5,0.5))
        labelWidth = label:getContentSize().width
        
        if score < 1000 then
        elseif score < 10000 then
        elseif score < 100000000 then
            img = cc.Sprite:create("font/bet_btn_wan.png")
        else
            img = cc.Sprite:create("font/bet_btn_yi.png")
        end
    else
        label = cc.LabelAtlas:create(text,"font/chip_font.png",13,17,string.byte("0"))
        label:setAnchorPoint(cc.p(0.5,0.5))
        labelWidth = label:getContentSize().width
        if chipType ==5 then
            label:setScale(1.0)
            labelWidth = labelWidth*1.0
        elseif chipType ==4 then
            label:setScale(0.7)
            labelWidth = labelWidth*0.7
        else
            label:setScale(0.8)
            labelWidth = labelWidth*0.8
        end
        
        if score < 1000 then
            
        elseif score < 10000 then
            img = cc.Sprite:create("font/chip_qian.png")
        elseif score < 100000000 then
            img = cc.Sprite:create("font/chip_wan.png")
        else
            img = cc.Sprite:create("font/chip_yi.png")
        end
    end
    

    if img ~= nil then
        img:setAnchorPoint(cc.p(0.5, 0.5))
        imgWidth = img:getContentSize().width
    end

    --标签设置
    if imgWidth == 0 then
        panelwidth = labelWidth
    else
         panelwidth = labelWidth + imgWidth + gap
    end
    ly:setContentSize(cc.size(panelwidth, 30))  

    --添加数字和汉字
    if label ~= nil then
        label:setPosition(cc.p(labelWidth /2, 15))
        ly:addChild(label) 
    end
    if img ~= nil then
        img:setPosition(cc.p(labelWidth+gap + imgWidth /2 , 15))
        ly:addChild(img)
    end

    return ly
end

return GameViewLayer
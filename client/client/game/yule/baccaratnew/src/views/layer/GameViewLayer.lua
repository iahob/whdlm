local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.baccaratnew.src"

--external
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
--

local cmd = module_pre .. ".models.CMD_Game"
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local QueryDialog   = require("app.views.layer.other.QueryDialog")

--utils
--
local UserListLayer = module_pre .. ".views.layer.userlist.UserListLayer"
local ApplyListLayer = module_pre .. ".views.layer.userlist.ApplyListLayer"
local SettingLayer = module_pre .. ".views.layer.SettingLayer"
local WallBillLayer = module_pre .. ".views.layer.WallBillLayer"
local SitRoleNode = module_pre .. ".views.layer.SitRoleNode"
local GameCardLayer = module_pre .. ".views.layer.GameCardLayer"
local GameResultLayer = module_pre .. ".views.layer.GameResultLayer"

local BankLayer = appdf.req(module_pre..".views.layer.BankLayer")
local HelpLayer = appdf.req(module_pre .. ".views.layer.HelpLayer")
local RoleInfoLayer = appdf.req(module_pre .. ".views.layer.RoleInfoLayer")
--

GameViewLayer.TAG_START				= 100
local enumTable = 
{
	"BT_EXIT",
	"BT_START",
	"BT_LUDAN",
	"BT_BANK",
	"BT_SET",
	"BT_ROBBANKER",
	"BT_APPLYBANKER",
	"BT_REJECT",
	"BT_USERLIST",
	"BT_APPLY",
	"BT_APPLYLIST",
	"BANK_LAYER",
	"BT_CLOSEBANK",
	"BT_TAKESCORE",
	"BT_HELP",
	"BT_CHAT"
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(GameViewLayer.TAG_START, enumTable);
local zorders = 
{
	"CLOCK_ZORDER",
	"SITDOWN_ZORDER",
	"DROPDOWN_ZORDER",
	"DROPDOWN_CHECK_ZORDER",
	"GAMECARD_ZORDER",
	"SETTING_ZORDER",
	"ROLEINFO_ZORDER",
	"BANK_ZORDER",
	"USERLIST_ZORDER",
	"WALLBILL_ZORDER",
	"GAMERS_ZORDER",	
	"ENDCLOCK_ZORDER"
}
local TAG_ZORDER = ExternalFun.declarEnumWithTable(1, zorders);

local enumApply =
{
	"kCancelState",
	"kApplyState",
	"kApplyedState",
	"kSupperApplyed"
}

-- 下注区域
local betPosList = 
{
	cc.p(286, 322), cc.p(667 , 401), cc.p(1047 , 324),cc.p(434 , 241),cc.p(894 , 241), cc.p(664 , 225),cc.p(357 , 507), cc.p(973 , 509)
}

-- 坐下玩家位置
local sitPlayerPosList = 
{
	cc.p(60, 245), cc.p(60 , 402), cc.p(60 , 558),cc.p(1274 , 558),cc.p(1274 , 402), cc.p(1274 , 245)
}

--飞筹码位置
local BankerPos = cc.p(780, 650);
local OtherPos = cc.p(1275, 90);
local MyPos = cc.p(100, 30);

--筹码对应的数值
local ChoumaType =
{
	[1] = 1,
	[2] = 10,
	[3] = 100,
	[4]	= 1000,
	[5] = 10000,
	[6] = 100000
}

GameViewLayer._apply_state = ExternalFun.declarEnumWithTable(0, enumApply)
local APPLY_STATE = GameViewLayer._apply_state

--默认选中的筹码
local DEFAULT_BET = 1
--筹码运行时间
local BET_ANITIME = 0.2

function GameViewLayer:ctor(scene)
	--注册node事件
	ExternalFun.registerNodeEvent(self)
	
	self._scene = scene
	self:gameDataInit();

	--动画层
	self.animateUI = nil

	--初始化csb界面
	self:initCsbRes();
	--初始化通用动作
	self:initAction();
end

function GameViewLayer:loadRes(  )
	--加载卡牌纹理
	cc.Director:getInstance():getTextureCache():addImage("game/card.png");
end

---------------------------------------------------------------------------------------
--界面初始化
function GameViewLayer:initCsbRes(  )
	local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameLayer.csb", self);
	self.m_rootLayer = rootLayer
	self.csbNode = csbNode
	-- self.animateUI = cc.Node:create():addTo(self, TAG_ZORDER.DROPDOWN_CHECK_ZORDER)
	self.animateUI = cc.Node:create();
	self:addToRootLayer(self.animateUI, TAG_ZORDER.CLOCK_ZORDER)
	--底部按钮
	local bottom_sp = csbNode:getChildByName("node_bottom");
	self.m_spBottom = bottom_sp;

	-- 玩家分数
	local node_score = csbNode:getChildByName("node_score");
	self.node_score = node_score;
	-- self.node_score:setVisible(false);
	self.node_score:setOpacity(0)

	--分数
	self.m_atlasScore = node_score:getChildByName("altas_score")
	self.m_atlasScore:setString("")
	--分数背景
	self.m_scoreBg = node_score:getChildByName("score_bg")
	--庄家飘分
	self.altas_score_banker = node_score:getChildByName("altas_score_banker")
	self.altas_score_banker:setString("")
	self.score_bg_banker = node_score:getChildByName("score_bg_banker")

		
	--初始化按钮
	self:initBtn(csbNode);

	--初始化庄家信息
	self:initBankerInfo();

	--初始化玩家信息
	self:initUserInfo();

	--初始化桌面下注
	self:initJetton(csbNode);

	--初始化座位列表
	self:initSitDownList(csbNode)

	--倒计时
	self:createClockNode()	
end

function GameViewLayer:reSet(  )

end

function GameViewLayer:reSetForNewGame(  )
	--重置下注区域
	self:cleanJettonArea()

	--移除动画
	if self.animateUI then
		self.animateUI:removeAllChildren()
	end

	--闪烁停止
	self:jettonAreaBlinkClean()

	self:showGameResult(false)

	-- if nil ~= self.m_cardLayer then
	-- 	self.m_cardLayer:showLayer(false)
	-- end
end

--初始化按钮
function GameViewLayer:initBtn( csbNode )
	local this = self
	------
	--切换checkbox
	local function checkEvent( sender,eventType )
		self:onCheckBoxClickEvent(sender, eventType);
	end
	local btnlist_check = csbNode:getChildByName("btnlist_check");
	btnlist_check:addEventListener(checkEvent);
	btnlist_check:setSelected(false);
	btnlist_check:setLocalZOrder(TAG_ZORDER.DROPDOWN_CHECK_ZORDER)
	------

	------
	--按钮列表
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			this:onButtonClickedEvent(sender:getTag(), sender);
		end
	end	
	local btn_list = csbNode:getChildByName("sp_btn_list");
	self.m_btnList = btn_list;
	btn_list:setScaleY(0.0000001)
	btn_list:setLocalZOrder(TAG_ZORDER.DROPDOWN_ZORDER)

	ExternalFun.registerTouchEvent(btn_list)
 	btn_list.onTouchBegan=function() return true end
 	btn_list.onTouchEnded=function() 
	 	if btn_list:isVisible() then 
	 		btnlist_check:setSelected(false);
	 		self.m_btnList:stopAllActions();
			self.m_btnList:runAction(self.m_actDropOut);
	 	end
 	end

 	--离开
	btn_back = csbNode:getChildByName("btn_back");
	btn_back:setTag(TAG_ENUM.BT_EXIT);
	btn_back:addTouchEventListener(btnEvent);

	--帮助
	local help_btn = btn_list:getChildByName("help_btn");
	help_btn:setTag(TAG_ENUM.BT_HELP);
	help_btn:addTouchEventListener(btnEvent);

	--设置
	btn = btn_list:getChildByName("set_btn");
	btn:setTag(TAG_ENUM.BT_SET);
	btn:addTouchEventListener(btnEvent);

    -- 是否开启银行服务
    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
	--银行
	bank_btn = btn_list:getChildByName("bank_btn");
	bank_btn:setTag(TAG_ENUM.BT_BANK);
	bank_btn:addTouchEventListener(btnEvent);
	bank_btn:setVisible(enableBank)

	--银行
	btn = csbNode:getChildByName("bank_btn");
	btn:setTag(TAG_ENUM.BT_BANK);
	btn:addTouchEventListener(btnEvent);
	btn:setVisible(enableBank)

	--路单
	local node_ludan = csbNode:getChildByName("node_ludan");
	self.node_ludan = node_ludan;
	local btn_record = node_ludan:getChildByName("btn_record");
	btn_record:setTag(TAG_ENUM.BT_LUDAN);
	btn_record:addTouchEventListener(btnEvent);

	local node_record  = node_ludan:getChildByName("node_record");
	local btn_record  = node_record:getChildByName("btn_record");
	btn_record:setTag(TAG_ENUM.BT_LUDAN);
	btn_record:addTouchEventListener(btnEvent);
	self.btn_record = btn_record;

	local node_roadBig  = node_ludan:getChildByName("node_roadBig");
	local btn_record_roadBig  = node_roadBig:getChildByName("btn_record_roadBig");
	btn_record_roadBig:setTag(TAG_ENUM.BT_LUDAN);
	btn_record_roadBig:addTouchEventListener(btnEvent);
	self.btn_record_roadBig = btn_record_roadBig;

	-- for i = 1, 20 do
	-- 	local record = self.node_ludan:getChildByName("record_" .. i);
	-- 	record:setVisible(false);
	-- end

	------
	local node_banker = csbNode:getChildByName("node_banker");
	--上庄、抢庄
	local banker_bg = node_banker:getChildByName("banker_bg");
	self.node_banker = node_banker;
	--抢庄
	btn = node_banker:getChildByName("rob_btn");
	btn:setTag(TAG_ENUM.BT_ROBBANKER);
	btn:addTouchEventListener(btnEvent);
	self.m_btnRob = btn;
	self.m_btnRob:setEnabled(false);

	--上庄
	btn = node_banker:getChildByName("apply_btn");
	btn:setTag(TAG_ENUM.BT_APPLY);
	btn:addTouchEventListener(btnEvent);
	self.btnApply = btn

	--重复下注
	btn = self.m_spBottom:getChildByName("rejetton_btn");
	btn:setTag(TAG_ENUM.BT_REJECT);
	btn:addTouchEventListener(btnEvent);
	-- btn:setVisible(false)

	--上庄列表
	btn = node_banker:getChildByName("banklist_btn");
	btn:setTag(TAG_ENUM.BT_APPLYLIST);
	btn:addTouchEventListener(btnEvent);	

	self.banker_apply_num = node_banker:getChildByName("player_num");
	self.banker_apply_num:setString(0);
	------

	--玩家列表
	btn = self.csbNode:getChildByName("userlist_btn");
	btn:setTag(TAG_ENUM.BT_USERLIST);
	btn:addTouchEventListener(btnEvent);

	self.player_num = self.m_spBottom:getChildByName("player_num");
	self.player_num:setString(0);

	-- 帮助按钮 gameviewlayer -> gamelayer -> clientscene
    --self:getParentNode():getParentNode():createHelpBtn2(cc.p(1287, 620), 0, 122, 0, csbNode)
end

--初始化庄家信息
function GameViewLayer:initBankerInfo( ... )
	local node_banker = self.node_banker;
	--庄家姓名
	local tmp = node_banker:getChildByName("name_text");
	self.m_clipBankerNick = g_var(ClipText):createClipText(tmp:getContentSize(), "");
	self.m_clipBankerNick:setAnchorPoint(tmp:getAnchorPoint());
	self.m_clipBankerNick:setPosition(tmp:getPosition());
	node_banker:addChild(self.m_clipBankerNick);

	--庄家游戏币
	self.m_textBankerCoin = node_banker:getChildByName("bankercoin_text");

	self:reSetBankerInfo();
end

function GameViewLayer:reSetBankerInfo(  )
	self.m_clipBankerNick:setString("");
	self.m_textBankerCoin:setString("");
end

function GameViewLayer:cleanTable()
	if self.m_gameResultLayer then
		self.m_gameResultLayer:removeFromParent()
		self.m_gameResultLayer = nil
	end
end

--初始化玩家信息
function GameViewLayer:initUserInfo(  )	
	-- --玩家头像
	-- local tmp = self.m_spBottom:getChildByName("player_head")
	-- local head = g_var(PopupInfoHead):createClipHead(self:getMeUserItem(), tmp:getContentSize().width -20, "game/head_mask.png")
	-- head:setPosition(tmp:getPosition())
	-- tmp:addChild(head)

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onRoleInfoLayer()
		end
	end	
	local head_bottom = self.m_spBottom:getChildByName("head_bottom")
	head_bottom:addTouchEventListener(btnEvent)

	--玩家游戏币
	self.m_textUserCoint = self.m_spBottom:getChildByName("coin_text")

	self:reSetUserInfo()
end

function GameViewLayer:formatScoreText(score)
    local scorestr = ExternalFun.formatScore(score)
    if score < 10000 then
        return scorestr,0
    end

    if score < 100000000 then
        scorestr = string.format("%.2f", score / 10000)
        return scorestr,1
    end
    scorestr = string.format("%.2f", score / 100000000)
    return scorestr,2
end

--分数显示
function GameViewLayer:formatScoreShow(score, scoreType, scoreLabel, scoreIcon, wanStr, yiStr)
	scoreLabel:setString(score);
	local labelW = scoreLabel:getContentSize().width/2;
	local labelPosX = scoreLabel:getPositionX();
	if scoreType == 0 then
		scoreIcon:setVisible(false);
	elseif scoreType == 1 then
		scoreIcon:setVisible(true);
		scoreIcon:setTexture(wanStr);
		scoreIcon:setPositionX(labelPosX + labelW);
	elseif scoreType == 2 then
		scoreIcon:setVisible(true);
		scoreIcon:setTexture(yiStr);
		scoreIcon:setPositionX(labelPosX + labelW);
	end
end

function GameViewLayer:reSetUserInfo(  )
	-- local node = self.csbNode:getChildByName("Node_noNotice")
	self.m_scoreUser = 0
	local myUser = self:getMeUserItem()
	if nil ~= myUser then
		self.m_scoreUser = myUser.lScore;
	end	

	local name = self.m_spBottom:getChildByName("name")
	name:setString(myUser.szNickName);
	-- local str = self.m_scoreUser
	-- if self.m_scoreUser >= 100000000 then
	-- 	node:getChildByName("yi_1"):setVisible(true)
 --        node:getChildByName("nostr_1"):setVisible(false)
	-- elseif self.m_scoreUser >= 10000 then
	-- 	node:getChildByName("yi_1"):setVisible(false)
 --        node:getChildByName("nostr_1"):setVisible(true)
	-- else
	-- 	node:getChildByName("yi_1"):setVisible(false)
 --        node:getChildByName("nostr_1"):setVisible(false)
	-- end

	-- print("自己游戏币:" .. ExternalFun.formatScore(self.m_scoreUser))
	-- self.m_textUserCoint:setString(self:formatScoreText(self.m_scoreUser));

	self:freshUserScore()
	--我选择的区域
	self.m_nextSelect = 0
end

--庄家分数显示
function GameViewLayer:freshBankerInfo(lScore)
	local node = self.node_banker;
	local icon_yi = node:getChildByName("icon_yi");
	local icon_wan = node:getChildByName("icon_wan");
	if lScore >= 100000000 then
		icon_yi:setVisible(true)
        icon_wan:setVisible(false)
	elseif lScore >= 10000 then
		icon_yi:setVisible(false)
        icon_wan:setVisible(true)
	else
		icon_yi:setVisible(false)
        icon_wan:setVisible(false)
	end

	print("庄家游戏币:" .. ExternalFun.formatScore(lScore))
	local scoreStr, scoreType = self:formatScoreText(lScore);
	self.m_textBankerCoin:setString(scoreStr);

	--万、亿位置
	local scoreW = self.m_textBankerCoin:getContentSize().width;
	local scorePosX = self.m_textBankerCoin:getPositionX();
	icon_yi:setPositionX(scorePosX + scoreW + 8);
	icon_wan:setPositionX(scorePosX + scoreW +8);
end

-- 玩家金币动画
function GameViewLayer:playerEndScoreEffect(changescore, lBankerWinScore)
	-- if 0 == changescore then
	-- 	return
	-- end

	--玩家赢分
	local str = "/" .. ExternalFun.numberThousands(changescore)
	self.m_atlasScore:setVisible(true);
	self.m_scoreBg:setVisible(true);
	if changescore > 0 then
		self.m_atlasScore:setProperty(str, "fonts/atlas_add.png", 27, 43, ".")
		self.m_scoreBg:loadTexture("game/score_bg_add.png");
	elseif changescore < 0 then
		self.m_atlasScore:setProperty(str, "fonts/atlas_sub.png", 27, 43, ".")
		self.m_scoreBg:loadTexture("game/score_bg_sub.png");
	else
		self.m_atlasScore:setVisible(false);
		self.m_scoreBg:setVisible(false);
	end
	self.m_scoreBg:setContentSize(cc.size(self.m_atlasScore:getContentSize().width +120, 45))

	--庄家赢分
	local bankerStr = "/" .. ExternalFun.numberThousands(lBankerWinScore)
	self.score_bg_banker:setVisible(true);
	self.altas_score_banker:setVisible(true);
	if lBankerWinScore > 0 then
		self.altas_score_banker:setProperty(bankerStr, "fonts/atlas_add.png", 27, 43, ".")
		self.score_bg_banker:loadTexture("game/score_bg_add.png");
	elseif lBankerWinScore < 0 then
		self.altas_score_banker:setProperty(bankerStr, "fonts/atlas_sub.png", 27, 43, ".")
		self.score_bg_banker:loadTexture("game/score_bg_sub.png");
	else
		self.altas_score_banker:setVisible(false);
		self.score_bg_banker:setVisible(false);
	end
	self.score_bg_banker:setContentSize(cc.size(self.altas_score_banker:getContentSize().width +120, 45))

	self.node_score:setOpacity(255)
	--飞行动画
	local moveBy = cc.MoveBy:create(2.0, cc.p(0, 50))
	local fadeout = cc.FadeOut:create(0.5)
	local call = cc.CallFunc:create(function( )
		self.node_score:setPositionY(0)
	end)
	self.node_score:runAction(cc.Sequence:create(moveBy, fadeout, call))
end

-- 玩家金币刷新
function GameViewLayer:freshUserScore()
	--自己
	-- local str = ExternalFun.formatScore(self.m_scoreUser)
	-- if string.len(str) > 11 then
	-- 	str = string.sub(str,1,11) .. "...";
	-- end
	local node = self.csbNode:getChildByName("Node_noNotice");
	local icon_yi = node:getChildByName("yi_1");
	local icon_wan = node:getChildByName("nostr_1");
	
	if self.m_scoreUser >= 100000000 then
		icon_yi:setVisible(true);
        icon_wan:setVisible(false);
	elseif self.m_scoreUser >= 10000 then
		icon_yi:setVisible(false);
        icon_wan:setVisible(true);
	else
		icon_yi:setVisible(false);
        icon_wan:setVisible(false);
	end
	local scoreStr, scoreType = self:formatScoreText(self.m_scoreUser);
	self.m_textUserCoint:setString(scoreStr);
	--万、亿位置
	local scoreW = self.m_textUserCoint:getContentSize().width;
	local scorePosX = self.m_textUserCoint:getPositionX();
	icon_yi:setPositionX(scorePosX + scoreW/2);
	icon_wan:setPositionX(scorePosX + scoreW/2);
end

-- 玩家金币刷新
function GameViewLayer:freshSitUserScore(wChairID, changeScore)
	--桌上玩家
	local index = 0;
	for i = 1,g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
		if (nil ~= self.m_tabSitDownUser[i]) and (wChairID == self.m_tabSitDownUser[i]:getChair()) then
			index = i
			break
		end
	end

	if index <= 0 then
		return;
	end

	self.m_tabSitDownUser[index]:freshSiteRoleScore(changeScore);
end

--初始化桌面下注
function GameViewLayer:initJetton( csbNode )
	local bottom_sp = self.m_spBottom;
	------
	--下注按钮	
	local clip_layout = bottom_sp:getChildByName("clip_layout");
	self.m_layoutClip = clip_layout;
	
	local node_bet = self.csbNode:getChildByName("node_bet");
	for i = 1,8 do
		local bet_node = node_bet:getChildByName("bet_" .. i);
		self.m_tableJettonNode[i] = bet_node;
		self:reSetJettonNode(self.m_tableJettonNode[i])
	end
	self.bet_total = node_bet:getChildByName("bet_total");
	--重置总下注
	self:reSetTotalJetton();
	------
	--下注区域
	self:initJettonArea(csbNode);
	------

	-----
	--下注胜利提示
	-----
	self:initJettonSp(csbNode);
end

function GameViewLayer:enableJetton( var )
	--下注按钮
	self:reSetJettonBtnInfo(var);

	--下注区域
	self:reSetJettonArea(var);
end

--筹码
function GameViewLayer:addLabelAtlasWithParent(parent, str)
	-- local lab = cc.LabelAtlas:create("0","fonts/font_bet_num.png",19, 29, string.byte("."))
	local lab = cc.LabelAtlas:create("","fonts/font_bet_num_1.png",8, 12, string.byte("."))
	parent:addChild(lab)
	lab:setAnchorPoint(0.5,0.5)
	lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2)
	local num = str
	lab:setString(num)
	if num >= 10000 then
		num = num / 10000
		lab:setString(num)
		local sp = cc.Sprite:create("game/wan.png")
		sp:setScale(0.5)
		parent:addChild(sp)
		sp:setAnchorPoint(0, 0.5)
		lab:setPositionX(parent:getContentSize().width/2 - 5)
		sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2)
	end
end

--筹码按钮
function GameViewLayer:addBetBtnAtlasWithParent(parent, str)
	local lab = cc.LabelAtlas:create("0","fonts/font_bet_num.png",19, 29, string.byte("."))
	parent:addChild(lab)
	lab:setAnchorPoint(0.5,0.5)
	lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2+5)
	local num = str
	lab:setString(num)
	if num >= 10000 then
		num = num / 10000
		lab:setString(num)
		local sp = cc.Sprite:create("game/wan.png")
		sp:setScale(1.2)
		parent:addChild(sp)
		sp:setAnchorPoint(0, 0.5)
		lab:setPositionX(parent:getContentSize().width/2 - 10)
		sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2+5)
	end
end

--下注按钮
function GameViewLayer:initJettonBtnInfo(  )
	local clip_layout = self.m_layoutClip;
	local this = self

	local function clipEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			this:onJettonButtonClicked(sender:getTag(), sender);
		end
	end

	self.m_tabJettonAnimate = {}
	for i=1,#self.m_pJettonNumber do
		local tag = i - 1
		local str = string.format("chip%d_btn", tag)
		local btn = clip_layout:getChildByName(str)
		btn:setTag(i)
		btn:addTouchEventListener(clipEvent)
		self.m_tableJettonBtn[i] = btn
		
		local num = self.m_pJettonNumber[i].k
		self:addBetBtnAtlasWithParent(btn, num)
		
		str = string.format("chip%d", tag)
		self.m_tabJettonAnimate[i] = clip_layout:getChildByName(str)
	end

	self:reSetJettonBtnInfo(false);
end

function GameViewLayer:reSetJettonBtnInfo( var )
	for i=1,#self.m_tableJettonBtn do
		self.m_tableJettonBtn[i]:setTag(i)
		self.m_tableJettonBtn[i]:setEnabled(var)

		self.m_tabJettonAnimate[i]:stopAllActions()
		self.m_tabJettonAnimate[i]:setVisible(false)
	end
end

function GameViewLayer:adjustJettonBtn(  )
	--可以下注的数额
	local lCanJetton = self.m_llMaxJetton - self.m_lHaveJetton;
	local lCondition = math.min(self.m_scoreUser, lCanJetton);
	print("=================================>",self.m_llMaxJetton, self.m_lHaveJetton,lCanJetton,self.m_scoreUser)

	for i=1,#self.m_tableJettonBtn do
		local enable = false
		if self.m_bOnGameRes then
			enable = false
		else
			enable = self.m_bOnGameRes or (lCondition >= self.m_pJettonNumber[i].k)
		end
		self.m_tableJettonBtn[i]:setEnabled(enable);
	end

	if self.m_nJettonSelect > self.m_scoreUser then
		self.m_nJettonSelect = -1;
	end

	--筹码动画
	local enable = lCondition >= self.m_pJettonNumber[self.m_nSelectBet].k;
	if false == enable then
		self.m_tabJettonAnimate[self.m_nSelectBet]:stopAllActions()
		self.m_tabJettonAnimate[self.m_nSelectBet]:setVisible(false)
	end
end

function GameViewLayer:refreshJetton(  )
	-- local str = ExternalFun.numberThousands(self.m_lHaveJetton)
	-- self.m_clipJetton:setString(str)
	-- self.m_userJettonLayout:setVisible(self.m_lHaveJetton > 0)
end

function GameViewLayer:switchJettonBtnState( idx )
	for i=1,#self.m_tabJettonAnimate do
		self.m_tabJettonAnimate[i]:stopAllActions()
		self.m_tabJettonAnimate[i]:setVisible(false)
	end

	--可以下注的数额
	local lCanJetton = self.m_llMaxJetton - self.m_lHaveJetton;
	local lCondition = math.min(self.m_scoreUser, lCanJetton);
	if nil ~= idx and nil ~= self.m_tabJettonAnimate[idx] then
		local enable = lCondition >= self.m_pJettonNumber[idx].k;
		if enable then
			-- local blink = cc.Blink:create(1.0,1)
			-- self.m_tabJettonAnimate[idx]:runAction(cc.RepeatForever:create(blink))
			self.m_tabJettonAnimate[idx]:setVisible(true)
		end		
	end
end

-- --下注筹码结算动画
-- function GameViewLayer:betAnimation( )
-- 	local cmd_gameend = self:getDataMgr().m_tabGameEndCmd
-- 	if nil == cmd_gameend then
-- 		return
-- 	end

-- 	local tmp = self.m_betAreaLayout:getChildren()
-- 	dump(tmp,"------------下注筹码结算动画------------",6)
-- 	--数量控制
-- 	local maxCount = 300
-- 	local count = 0
-- 	local children = {}
-- 	for k,v in pairs(tmp) do
-- 		table.insert(children, v)
-- 		count = count + 1
-- 		if count > maxCount then
-- 			break
-- 		end
-- 	end
-- 	local left = {}
-- 	print("bankerscore:" .. ExternalFun.formatScore(cmd_gameend.lBankerScore))
-- 	print("selfscore:" .. ExternalFun.formatScore(cmd_gameend.lPlayAllScore))

-- 	--庄家的
-- 	local call = cc.CallFunc:create(function()
-- 		left = self:userBetAnimation(children, "banker", cmd_gameend.lBankerScore)
-- 		if cmd_gameend.lBankerScore < 0 then
-- 			local tmp = self.m_betAreaLayout:getChildren()
-- 			--数量控制
-- 			local maxCount = 300
-- 			local count = 0
-- 			local children = {}
-- 			for k,v in pairs(tmp) do
-- 				table.insert(children, v)
-- 				count = count + 1
-- 				if count > maxCount then
-- 					break
-- 				end
-- 			end
-- 			left = children
-- 		end
-- 	end)
-- 	local delay = cc.DelayTime:create(0.5)

-- 	--自己的
-- 	local meChair =  self:getMeUserItem().wChairID
-- 	local call2 = cc.CallFunc:create(function()		
-- 		left = self:userBetAnimation(left, meChair, cmd_gameend.lPlayAllScore)

-- 		--刷新玩家分数
-- 		if cmd_gameend.lPlayAllScore > 0 then
-- 			local myUser = self:getMeUserItem()
-- 			if nil ~= myUser then
-- 				self.m_scoreUser = myUser.lScore;
-- 			end	
-- 			self.m_scoreUser = self.m_scoreUser + cmd_gameend.lPlayAllScore;
-- 			self:freshUserScore();
-- 		end

-- 		--玩家游戏分数输赢动画
-- 		self:playerEndScoreEffect(cmd_gameend.lPlayAllScore, cmd_gameend.lBankerWinScore);
		
-- 	end)	
-- 	local delay2 = cc.DelayTime:create(0.5)

-- 	--坐下的
-- 	local call3 = cc.CallFunc:create(function()
-- 		for i = 1, g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
-- 			if nil ~= self.m_tabSitDownUser[i] then
-- 				--非自己
-- 				local chair = self.m_tabSitDownUser[i]:getChair()
-- 				local score = cmd_gameend.lOccupySeatUserWinScore[1][i]
-- 				-- if meChair ~= chair then
-- 					left = self:userBetAnimation(left, chair, cmd_gameend.lOccupySeatUserWinScore[1][i])
-- 				-- end

-- 				local useritem = self:getDataMgr():getChairUserList()[chair + 1]
-- 				--游戏币动画
-- 				self.m_tabSitDownUser[i]:gameEndScoreChange(useritem, score)
-- 			end
-- 		end
-- 	end)
-- 	local delay3 = cc.DelayTime:create(0.5)	

-- 	--其余玩家的
-- 	local call4 = cc.CallFunc:create(function()
-- 		self:userBetAnimation(left, "other", 1)
-- 	end)

-- 	--剩余没有移走的
-- 	local call5 = cc.CallFunc:create(function()
-- 		--下注筹码数量显示移除
-- 		self:cleanJettonArea()
-- 	end)

-- 	local seq = cc.Sequence:create(call, delay, call2, delay2, call3, delay3, call4, cc.DelayTime:create(2), call5)
-- 	self:stopAllActions()
-- 	self:runAction(seq)	
-- end


-- function GameViewLayer:flyFromBanker(data)
-- 	local pos = cc.p(self.m_textBankerCoin:getPositionX(), self.m_textBankerCoin:getPositionY())
-- 	-- pos = self.m_textBankerCoin:convertToWorldSpace(pos)
-- 	-- pos = self.m_betAreaLayout:convertToNodeSpace(pos)

-- 	local area = data.areaIndex
-- 	local nIdx = self:getJettonIdx(data.lBetScore)-1
-- 	local str = string.format("game/chip_%d.png", nIdx);
-- 	local sp = cc.Sprite:create(str)
	
-- 	local num = self.m_pJettonNumber[nIdx].k
	
-- 	self:addLabelAtlasWithParent(sp, num)
	
-- 	local btn = self.m_tableJettonArea[area];
-- 	if nil == sp then
-- 		print("sp nil");
-- 	end

-- 	if nil == btn then
-- 		print("btn nil");
-- 	end
-- 	if nil ~= sp and nil ~= btn then
-- 		--下注
-- 		-- sp:setTag(wUser);
-- 		local name = string.format("%d", area) 
-- 		sp:setName(name)
		
-- 		--筹码飞行起点位置
-- 		sp:setPosition(pos)
-- 		--筹码飞行动画
-- 		local act = self:getBetAnimation(self:getBetRandomPos(area), cc.CallFunc:create(function()
-- 			--播放飞金币声音
-- 			ExternalFun.playSoundEffect("CoinFly.wav")
-- 		end))
-- 		sp:stopAllActions()
-- 		sp:runAction(act)
-- 		self.m_betAreaLayout:addChild(sp)
-- 	end
-- end

-- --玩家分数
-- function GameViewLayer:userBetAnimation( children, wchair, score, cmd_gameend )
-- 	print(children,wchair,score,"--------玩家分数---------")
-- 	--庄家输了吐金币
-- 	if score < 0 and "banker" == wchair then
-- 		local blinkArea = self:getDataMgr().m_tabBetArea
-- 		local score = math.abs(score)

-- 		local winCount = {}
-- 		for i=1, #blinkArea do
-- 			if blinkArea[i] > 0 then
-- 				table.insert(winCount, i)
-- 			end
-- 		end

-- 		local leftScore = score
-- 		for i=#self.m_pJettonNumber, 1, -1 do
-- 			local num = math.floor(leftScore/self.m_pJettonNumber[i].k)
-- 			leftScore = score%self.m_pJettonNumber[i].k

-- 			for j=1, num do
-- 				local index = math.random(#winCount)
-- 				local tmp = {}
-- 				tmp.areaIndex = winCount[index]
-- 				-- tmp.wChairID = 
-- 				tmp.lBetScore = self.m_pJettonNumber[i].k
-- 				self:flyFromBanker(tmp)
-- 			end
-- 		end
-- 	end
-- 	------------------------------------------------------------------------
-- 	if nil == score or score <= 0 then
-- 		return children
-- 	end

-- 	local left = {}
-- 	local getScore = score
-- 	local tmpScore = 0
-- 	local totalIdx = #self.m_pJettonNumber
-- 	local winSize = self.m_betAreaLayout:getContentSize()
-- 	local remove = true
-- 	local count = 0
-- 	for k,v in pairs(children) do
-- 		local idx = nil

-- 		if remove then
-- 			if nil ~= v and not tolua.isnull(v) and v:getTag() and  v:getTag() == wchair then
-- 				idx = tonumber(v:getName())
				
-- 				local pos = self.m_betAreaLayout:convertToNodeSpace(self:getBetFromPos(wchair))
-- 				self:generateBetAnimtion(v, {x = pos.x, y = pos.y}, count)

-- 				if nil ~= idx and nil ~= self.m_pJettonNumber[idx] then
-- 					tmpScore = tmpScore + self.m_pJettonNumber[idx].k
-- 				end

-- 				if tmpScore >= score then
-- 					remove = false
-- 				end
-- 			elseif yl.INVALID_CHAIR == wchair then
-- 				--随机抽下注筹码
-- 				idx = self:randomGetBetIdx(getScore, totalIdx)

-- 				local pos = self.m_betAreaLayout:convertToNodeSpace(self:getBetFromPos(wchair))

-- 				if nil ~= idx and nil ~= self.m_pJettonNumber[idx] then
-- 					tmpScore = tmpScore + self.m_pJettonNumber[idx].k
-- 					getScore = getScore - tmpScore
-- 				end

-- 				if tmpScore >= score then
-- 					remove = false
-- 				end
-- 			elseif "banker" == wchair then
-- 				--随机抽下注筹码
-- 				idx = self:randomGetBetIdx(getScore, totalIdx)

-- 				local pos = cc.p(self.m_textBankerCoin:getPositionX(), self.m_textBankerCoin:getPositionY())
-- 				-- pos = self.m_textBankerCoin:convertToWorldSpace(pos)
-- 				-- pos = self.m_betAreaLayout:convertToNodeSpace(pos)
				
-- 				self:generateBetAnimtion(v, {x = pos.x, y = pos.y}, count)

-- 				if nil ~= idx and nil ~= self.m_pJettonNumber[idx] then
-- 					tmpScore = tmpScore + self.m_pJettonNumber[idx].k
-- 					getScore = getScore - tmpScore
-- 				end
-- 				print("banker win bet",count,idx,tmpScore,score)
-- 				if tmpScore >= score then
-- 					remove = false
-- 				end
-- 			elseif "other" == wchair then
-- 				local posx = self.csbNode:getChildByName("userlist_btn"):getPositionX()
-- 				local posy = self.csbNode:getChildByName("userlist_btn"):getPositionY()
-- 				-- local worldPos = self.m_spBottom:convertToWorldSpace(cc.p(posx,posy))
-- 				-- local _pos = self.csbNode:convertToNodeSpace(worldPos)
-- 				local pos = cc.p(posx,posy)
-- 				self:generateBetAnimtion(v, {x = pos.x, y = pos.y}, count)
-- 			else
-- 				table.insert(left, v)
-- 			end
-- 		else
-- 			table.insert(left, v)
-- 		end	
-- 		count = count + 1	
-- 	end
-- 	return left
-- end

-- function GameViewLayer:generateBetAnimtion( bet, pos, count)
-- 	if bet == nil or tolua.isnull(bet) then
-- 		return;
-- 	end
-- 	--筹码动画	
-- 	local moveTo = cc.MoveTo:create(BET_ANITIME, cc.p(pos.x, pos.y))
-- 	local call = cc.CallFunc:create(function ( )
-- 		bet:removeAllChildren()
-- 		bet:removeFromParent()
-- 		bet = nil
-- 	end)
-- 	bet:stopAllActions()
-- 	-- bet:runAction(cc.Sequence:create(cc.DelayTime:create(0.05 * count),moveTo, call))
-- 	bet:runAction(cc.Sequence:create(cc.DelayTime:create(0.05),moveTo, call))

-- 	--播放飞金币声音
-- 	ExternalFun.playSoundEffect("CoinFly.wav")
-- end

-- function GameViewLayer:randomGetBetIdx( score, totalIdx )
-- 	if score > self.m_pJettonNumber[1].k and score < self.m_pJettonNumber[2].k then
-- 		return math.random(1,2)
-- 	elseif score > self.m_pJettonNumber[2].k and score < self.m_pJettonNumber[3].k then
-- 		return math.random(1,3)
-- 	elseif score > self.m_pJettonNumber[3].k and score < self.m_pJettonNumber[4].k then
-- 		return math.random(1,4)
-- 	else
-- 		return math.random(totalIdx)
-- 	end	
-- end

--新筹码动画
-- 筹码创建
function GameViewLayer:chipCreatByType(chipType, m_llScore)
	local file = "game/chip_" .. chipType ..".png"
    local chip = display.newSprite(file);
    --创建筹码字
    self:addLabelAtlasWithParent(chip, m_llScore);

	return chip;
end

--生成筹码、飞筹码
function GameViewLayer:chipCreatByScore(score, startPos)
	local chipList = {};
	if score <= 0 then
		return chipList;
	end
	local numbers = self.m_pJettonNumber;
	local betNums = {numbers[1].k, numbers[2].k, numbers[3].k, numbers[4].k, numbers[5].k, numbers[6].k};
	local vec = self:getDataMgr().calcuteJetton(score, false, betNums);
	
	-- local winAreList = self:getWinAreaIndexList(tabArea);
	--赢家区域
	local winAreList = self:getWinAreaIndexList();
	local index = 0;
	for k,v in pairs(vec) do
		local info = v;
		for i=1,info.m_cbCount do
			local chip = self:chipCreatByType(info.m_cbIdx, info.m_llScore);
			chip:setPosition(startPos);
			self.m_betAreaLayout:addChild(chip);

			table.insert(chipList, chip);

			--飞到赢家区域
			local index = math.random(#winAreList);
			local areaIndex = winAreList[index];
			local endPos = self:getBetRandomPos(areaIndex);
			self:betFlyAnimtion(chip, endPos, 0.01*index, 0.4, false);
			index = index +1;
		end
	end
	return chipList;
end

--飞筹码到赢家
function GameViewLayer:chipFlyToWinPlayer(winBetArr, endPos)
	local index = 0;
	for i = 1,#winBetArr do
		local chip = winBetArr[i];
		if chip then
			self:betFlyAnimtion(chip, endPos, 0.01*index, 0.4, true);
			index = index +1;
		end
	end
end

--飞到庄家、自己、坐下玩家、其他玩家
function GameViewLayer:endBetAnimation()
	local cmd_gameend = self:getDataMgr().m_tabGameEndCmd;
	if nil == cmd_gameend then
		return;
	end

	--总下注分
	local allAreaTotalScore = 0;
	for i = 1,g_var(cmd).AREA_MAX do
		allAreaTotalScore = allAreaTotalScore + self.m_tableJettonNode[i].m_llAreaTotal;
	end

	--飞到庄家
	local callFun1 = cc.CallFunc:create(function()
		local children = self.m_betAreaLayout:getChildren();
		local index = 0;
		for k,v in pairs(children) do
			if nil ~= v and not tolua.isnull(v) then
				--飞筹码
				self:betFlyAnimtion(v, BankerPos, 0.01*index, 0.4, true);
				index = index +1;
			end
		end
	end)
	local delayTime1 = cc.DelayTime:create(1.0);

	--重新生成筹码、自己、坐下玩家、其他玩家
	local meChair =  self:getMeUserItem().wChairID;
	local otherScore = 0;
	local myWinBetArr = {};
	local sitPlayerWinBetArr = {{}, {}, {}, {}, {}, {}, {}, {}};
	local otherWinBetArr ={};
	local totalBetArr ={};
	local startPos = BankerPos;

	local callFun2 = cc.CallFunc:create(function()
		self.m_betAreaLayout:removeAllChildren();

		myWinBetArr = self:chipCreatByScore(cmd_gameend.lPlayAllScore, startPos);
		otherScore = allAreaTotalScore - cmd_gameend.lPlayAllScore;

		--刷新玩家分数
		if cmd_gameend.lPlayAllScore > 0 then
			local myUser = self:getMeUserItem();
			if nil ~= myUser then
				self.m_scoreUser = myUser.lScore;
			end	
			self.m_scoreUser = self.m_scoreUser + cmd_gameend.lPlayAllScore;
			self:freshUserScore();
		end

		--玩家游戏分数输赢动画
		self:playerEndScoreEffect(cmd_gameend.lPlayAllScore, cmd_gameend.lBankerWinScore);
	end)
	
	local callFun3 = cc.CallFunc:create(function()
		for i = 1,g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
			if nil ~= self.m_tabSitDownUser[i] then
				local sitPlayerScore = cmd_gameend.lOccupySeatUserWinScore[1][i];
				local chair = self.m_tabSitDownUser[i]:getChair()
				if sitPlayerScore > 0 then
					sitPlayerWinBetArr[i] = self:chipCreatByScore(sitPlayerScore, startPos);
					if meChair ~= chair then
						otherScore = otherScore - sitPlayerScore;
					end
				end

				--非自己
				-- local chair = self.m_tabSitDownUser[i]:getChair()
				-- local score = cmd_gameend.lOccupySeatUserWinScore[1][i]
				local useritem = self:getDataMgr():getChairUserList()[chair + 1];
				--游戏币动画
				self.m_tabSitDownUser[i]:gameEndScoreChange(useritem, sitPlayerScore);
			end
		end
	end)
	local callFun4 = cc.CallFunc:create(function()
		otherWinBetArr = self:chipCreatByScore(otherScore, startPos);
	end)
	local delayTime4 = cc.DelayTime:create(1.5);

	--赢分区域飞到赢家
	local callFun5 = cc.CallFunc:create(function()
		--自己赢
		self:chipFlyToWinPlayer(myWinBetArr, MyPos);
		--坐下玩家赢
		for i = 1,#sitPlayerWinBetArr do
			self:chipFlyToWinPlayer(sitPlayerWinBetArr[i], sitPlayerPosList[i]);
		end
		--其他玩家赢
		local userList = self:getDataMgr():getUserList();
		if #userList > 1 then
			self:chipFlyToWinPlayer(otherWinBetArr, OtherPos);
		end
	end)
	local delayTime5 = cc.DelayTime:create(1.0);
	
	--剩余没有移走的
	local callFun6 = cc.CallFunc:create(function()
		--下注筹码数量显示移除
		self:cleanJettonArea();
	end)

	local seq = cc.Sequence:create(callFun1, delayTime1, callFun2, callFun3, callFun4, delayTime4, callFun5, delayTime5, callFun6);
	self:stopAllActions();
	self:runAction(seq);
end

function GameViewLayer:betFlyAnimtion(betNode, endPos, delayTime, moveTime, isRemoveSelf)
	if betNode == nil or tolua.isnull(betNode) then
		return;
	end
	--筹码动画
	local delay = cc.DelayTime:create(delayTime);
	local moveTo = cc.MoveTo:create(moveTime, endPos);
	local call = cc.CallFunc:create(function()
		if isRemoveSelf then
			betNode:removeAllChildren();
			betNode:removeFromParent();
			betNode = nil;
		end
	end)
	betNode:stopAllActions();
	betNode:runAction(cc.Sequence:create(delay,moveTo, call));

	--播放飞金币声音
	ExternalFun.playSoundEffect("CoinFly.wav");
end

--胜利区域
function GameViewLayer:getWinAreaIndexList()
	local tabArea = self:getDataMgr().m_tabBetArea;
	local winAreList = {};
	for i = 1, g_var(cmd).AREA_MAX do
		local score = tabArea[i]
		if score > 0 then
			table.insert(winAreList, i);
		end
	end
	return winAreList;
end

--下注区域
function GameViewLayer:initJettonArea( csbNode )
	local this = self
	local tag_control = csbNode:getChildByName("tag_control");
	self.m_tagControl = tag_control

	--筹码区域
	self.m_betAreaLayout = tag_control:getChildByName("bet_area")

	--按钮列表
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			this:onJettonAreaClicked(sender:getTag(), sender);
		end
	end	

	for i=1,8 do
		local tag = i - 1;
		local str = string.format("tag%d_btn", tag);
		local tag_btn = tag_control:getChildByName(str);
		tag_btn:setTag(i);
		tag_btn:addTouchEventListener(btnEvent);
		self.m_tableJettonArea[i] = tag_btn; 
	end

	--下注信息
	-- local m_userJettonLayout = csbNode:getChildByName("jetton_control");
	-- local infoSize = m_userJettonLayout:getContentSize()
	-- local text = ccui.Text:create("本次下注为:", "fonts/round_body.ttf", 20)
	-- text:setAnchorPoint(cc.p(1.0,0.5))
	-- text:setPosition(cc.p(infoSize.width * 0.495, infoSize.height * 0.19))
	-- m_userJettonLayout:addChild(text)
	-- m_userJettonLayout:setVisible(false)

	-- local m_clipJetton = g_var(ClipText):createClipText(cc.size(120, 23), "")
	-- m_clipJetton:setPosition(cc.p(infoSize.width * 0.5, infoSize.height * 0.19))
	-- m_clipJetton:setAnchorPoint(cc.p(0,0.5));
	-- m_clipJetton:setTextColor(cc.c4b(255,165,0,255))
	-- m_userJettonLayout:addChild(m_clipJetton)

	-- self.m_userJettonLayout = m_userJettonLayout;
	-- self.m_clipJetton = m_clipJetton;

	self:reSetJettonArea(false);
end

function GameViewLayer:reSetJettonArea( var )
	for i=1,#self.m_tableJettonArea do
		self.m_tableJettonArea[i]:setEnabled(var);
	end
end

function GameViewLayer:cleanJettonArea(  )
	--移除界面已下注
	self.m_betAreaLayout:removeAllChildren()

	for i=1,#self.m_tableJettonArea do
		if nil ~= self.m_tableJettonNode[i] then
			--self.m_tableJettonNode[i]:reSet()
			self:reSetJettonNode(self.m_tableJettonNode[i])
		end
	end
	-- self.m_userJettonLayout:setVisible(false)
	-- self.m_clipJetton:setString("")

	--重置总下注
	self:reSetTotalJetton();
end

--下注胜利提示
function GameViewLayer:initJettonSp( csbNode )
	self.m_tagSpControls = {};
	local sp_control = csbNode:getChildByName("tag_sp_control");
	for i=1,8 do
		local tag = i - 1;
		local str = string.format("tagsp_%d", tag);
		local tagsp = sp_control:getChildByName(str);
		self.m_tagSpControls[i] = tagsp;
	end

	self:reSetJettonSp();
end

function GameViewLayer:reSetJettonSp(  )
	for i=1,#self.m_tagSpControls do
		self.m_tagSpControls[i]:setVisible(false);
	end
end

--胜利区域闪烁
function GameViewLayer:jettonAreaBlink( tabArea )
	for i = 1, #tabArea do
		local score = tabArea[i]
		if score > 0 then
			local rep = cc.RepeatForever:create(cc.Blink:create(1.0,1))
			self.m_tagSpControls[i]:runAction(rep)
		end
	end
end

function GameViewLayer:jettonAreaBlinkClean(  )
	for i = 1, g_var(cmd).AREA_MAX do
		self.m_tagSpControls[i]:stopAllActions()
		self.m_tagSpControls[i]:setVisible(false)
	end
end

--座位列表
function GameViewLayer:initSitDownList( csbNode )
	local m_roleSitDownLayer = csbNode:getChildByName("role_control")
	self.m_roleSitDownLayer = m_roleSitDownLayer

	--按钮列表
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onSitDownClick(sender:getTag(), sender);
		end
	end

	local str = ""
	for i=1,g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
		str = string.format("sit_btn_%d", i)
		self.m_tabSitDownList[i] = m_roleSitDownLayer:getChildByName(str)
		self.m_tabSitDownList[i]:setTag(i)
		self.m_tabSitDownList[i]:addTouchEventListener(btnEvent);
	end
end

function GameViewLayer:initAction(  )
	local dropIn = cc.ScaleTo:create(0.2, 1.0);
	dropIn:retain();
	self.m_actDropIn = dropIn;

	local dropOut = cc.ScaleTo:create(0.2, 1.0, 0.0000001);
	dropOut:retain();
	self.m_actDropOut = dropOut;
end
---------------------------------------------------------------------------------------

function GameViewLayer:onButtonClickedEvent(tag,ref)
	ExternalFun.playClickEffect()
	if tag == TAG_ENUM.BT_EXIT then
		print("----------BT_EXIT------------")
		if self.m_lHaveJetton > 0 then
			self._scene.m_cbGameStatus = g_var(cmd).GAME_PLAY
		else
			self._scene.m_cbGameStatus = g_var(cmd).GAME_SCENE_FREE
		end

		self:getParentNode():onQueryExitGame()
	elseif tag == TAG_ENUM.BT_START then
		self:getParentNode():onStartGame()
	elseif tag == TAG_ENUM.BT_USERLIST then
		if nil == self.m_userListLayer then
			self.m_userListLayer = g_var(UserListLayer):create(self)
			self:addToRootLayer(self.m_userListLayer, TAG_ZORDER.USERLIST_ZORDER)
		end
		local userList = self:getDataMgr():getUserList()		
		self.m_userListLayer:refreshList(userList)
	elseif tag == TAG_ENUM.BT_APPLYLIST then
		if nil == self.m_applyListLayer then
			self.m_applyListLayer = g_var(ApplyListLayer):create(self)
			self:addToRootLayer(self.m_applyListLayer, TAG_ZORDER.USERLIST_ZORDER)
		end
		local userList = self:getDataMgr():getApplyBankerUserList()		
		self.m_applyListLayer:refreshList(userList)
	elseif tag == TAG_ENUM.BT_APPLY then
		self:applyBanker(self.m_enApplyState)
	elseif tag == TAG_ENUM.BT_REJECT then
		self:reJettonScore()
	elseif tag == TAG_ENUM.BT_BANK then
		--银行未开通
		if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
			showToast(self,"初次使用，请先开通银行！",1)
			return
		end

		--房间规则
		local rule = self:getParentNode()._roomRule
		if rule == yl.GAME_GENRE_SCORE 
		or rule == yl.GAME_GENRE_EDUCATE then 
			print("练习 or 积分房")
		end
		-- 当前游戏币场
		local rom = GlobalUserItem.GetRoomInfo()
		if nil ~= rom then
			if rom.wServerType ~= yl.GAME_GENRE_GOLD then
				showToast(self, "当前房间禁止操作银行!", 2)
				return
			end
		end

		if false == self:getParentNode():getFrame():OnGameAllowBankTake() then
			--showToast(self,"不允许银行取款操作操作",1)
			--return
		end

		self.m_bankLayer = BankLayer:create(self):addTo(self, TAG_ZORDER.BANK_ZORDER)
		self._scene:sendRequestBankInfo()
	elseif tag == TAG_ENUM.BT_SET then
		local setting = g_var(SettingLayer):create(self)
		self:addToRootLayer(setting, TAG_ZORDER.SETTING_ZORDER)
	elseif tag == TAG_ENUM.BT_LUDAN then
		self:onOpenBigLudanLayer();
	elseif tag == TAG_ENUM.BT_ROBBANKER then
		--超级抢庄
		if g_var(cmd).SUPERBANKER_CONSUMETYPE == self.m_tabSupperRobConfig.superbankerType then
			local str = "超级抢庄将花费 " .. self.m_tabSupperRobConfig.lSuperBankerConsume .. ",确定抢庄?"
			local query = QueryDialog:create(str, function(ok)
		        if ok == true then
		            self:getParentNode():sendRobBanker()
		        end
		    end):setCanTouchOutside(false)
		        :addTo(self) 
		else
			self:getParentNode():sendRobBanker()
		end
	elseif tag == TAG_ENUM.BT_CLOSEBANK then
		if nil ~= self.m_bankLayer then
			self.m_bankLayer:setVisible(false)
		end
	elseif tag == TAG_ENUM.BT_TAKESCORE then
		self:onTakeScore()
	elseif tag == TAG_ENUM.BT_CHAT then

	elseif tag == TAG_ENUM.BT_HELP then
		local help = HelpLayer:create();
		help.m_parent = self;
		self:addChild(help,10);
	else
		showToast(self,"功能尚未开放！",1)
	end
end

function GameViewLayer:onOpenBigLudanLayer()
	if nil == self.m_wallBill then
		self.m_wallBill = g_var(WallBillLayer):create(self)
		self:addToRootLayer(self.m_wallBill, TAG_ZORDER.WALLBILL_ZORDER)
	end
	self.m_wallBill:refreshWallBillList()
end

--更新重复下注按钮状态
function GameViewLayer:updateRejecttonState()
	--自己下注总分数
	local score = 0

	--如果我是庄家
	if self:isMeChair(self.m_wBankerUser) then
		self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(false)
		return 
	end

	if self.isReject then
		self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(false)
		return 
	end

	for k, v in pairs(self.lastRoundJettonScore) do
		score = score + v.m_llMyTotal
	end

	--如果金币不足
	if score > self.m_scoreUser then
		self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(false)
		return 
	end

	--上一局未下注
	if score == 0 then
		self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(false)
		return 
	end

	--非下注状态
	if self.m_cbGameStatus ~= g_var(cmd).GAME_JETTON then
		self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(false)
		return
	end

	self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(true)
end

--重复下注
function GameViewLayer:reJettonScore()
	self:getParentNode():sendRejectton()
end

--重复下注成功
function GameViewLayer:onReJetton(cmd_table)
	self.m_spBottom:getChildByName("rejetton_btn"):setEnabled(false)
	self.isReject = true

	local tmp = {}
	tmp.wChairID = cmd_table.wChairID

	for i=1, 8 do
		tmp.cbBetArea = i-1
		tmp.lBetScore = cmd_table.lBetScore[1][i]
		if tmp.lBetScore ~= 0 then
			self:onGetUserBet(tmp)
		end
	end
end

--重复下注失败
function GameViewLayer:onReJettonFail(cmd_table)

end

function GameViewLayer:onJettonButtonClicked( tag, ref )
	if tag >= 1 and tag <= 6 then
		self.m_nJettonSelect = self.m_pJettonNumber[tag].k;
	else
		self.m_nJettonSelect = -1;
	end

	self.m_nSelectBet = tag
	self:switchJettonBtnState(tag)
	print("click jetton:" .. self.m_nJettonSelect);
end

function GameViewLayer:onJettonAreaClicked( tag, ref )
	local m_nJettonSelect = self.m_nJettonSelect;

	if m_nJettonSelect < 0 then
		return;
	end

	local area = tag - 1;	
	if self.m_lHaveJetton > self.m_llMaxJetton then
		showToast(self,"已超过最大下注限额",1)
		self.m_lHaveJetton = self.m_lHaveJetton - m_nJettonSelect;
		return;
	end

	--记录我上次下注
	self.m_nextSelect = area
	--下注
	self:getParentNode():sendUserBet(area, m_nJettonSelect);	
end

function GameViewLayer:showGameResult( bShow, cbCardCount, cbTableCardArray, mHaveAnim, isBreakLine)
	if true == bShow then
		if nil == self.m_gameResultLayer then
			self.m_gameResultLayer = g_var(GameResultLayer):create(self)
			self:addToRootLayer(self.m_gameResultLayer, TAG_ZORDER.GAMECARD_ZORDER)
		end

		-- if true == bShow and true == self:getDataMgr().m_bJoin then
		-- 	self.m_gameResultLayer:showGameResult(self:getDataMgr().m_tabGameResult)
		-- end
		if true == bShow then
			self.m_gameResultLayer:showGameResult(bShow, cbCardCount, cbTableCardArray, mHaveAnim, isBreakLine);
		end
	else
		if nil ~= self.m_gameResultLayer then
			self.m_gameResultLayer:hideGameResult()
		end
	end
end

function GameViewLayer:onCheckBoxClickEvent( sender,eventType )
	ExternalFun.playClickEffect()
	if eventType == ccui.CheckBoxEventType.selected then
		self.m_btnList:stopAllActions();
		self.m_btnList:runAction(self.m_actDropIn);
	elseif eventType == ccui.CheckBoxEventType.unselected then
		self.m_btnList:stopAllActions();
		self.m_btnList:runAction(self.m_actDropOut);
	end
end

function GameViewLayer:onSitDownClick( tag, sender )
	print("sit ==> " .. tag)
	local useritem = self:getMeUserItem()
	if nil == useritem then
		return
	end

	--重复判断
	if nil ~= self.m_nSelfSitIdx and tag == self.m_nSelfSitIdx then
		return
	end

	if nil ~= self.m_nSelfSitIdx then --and tag ~= self.m_nSelfSitIdx  then
		showToast(self, "当前已占 " .. self.m_nSelfSitIdx .. " 号位置,不能重复占位!", 2)
		return
	end	

	--坐下条件限制
	if useritem.lScore < self.m_tabSitDownConfig.lForceStandUpCondition then
		local str = "坐下需要携带 " .. self.m_tabSitDownConfig.lForceStandUpCondition .. " 游戏币,游戏币不足!"
		showToast(self, str, 2)
		return
	end
	if self.m_tabSitDownConfig.occupyseatType == g_var(cmd).OCCUPYSEAT_CONSUMETYPE then --游戏币占座
		if useritem.lScore < self.m_tabSitDownConfig.lOccupySeatConsume then
			local str = "坐下需要消耗 " .. self.m_tabSitDownConfig.lOccupySeatConsume .. " 游戏币,游戏币不足!"
			showToast(self, str, 2)
			return
		end
		local str = "坐下将花费 " .. self.m_tabSitDownConfig.lOccupySeatConsume .. ",确定坐下?"
			local query = QueryDialog:create(str, function(ok)
		        if ok == true then
		            self:getParentNode():sendSitDown(tag - 1, useritem.wChairID)
		        end
		    end):setCanTouchOutside(false)
		        :addTo(self)
	elseif self.m_tabSitDownConfig.occupyseatType == g_var(cmd).OCCUPYSEAT_VIPTYPE then --会员占座
		if useritem.cbMemberOrder < self.m_tabSitDownConfig.enVipIndex then
			local str = "坐下需要会员等级为 " .. self.m_tabSitDownConfig.enVipIndex .. " 会员等级不足!"
			showToast(self, str, 2)
			return
		end
		self:getParentNode():sendSitDown(tag - 1, self:getMeUserItem().wChairID)
	elseif self.m_tabSitDownConfig.occupyseatType == g_var(cmd).OCCUPYSEAT_FREETYPE then --免费占座
		if useritem.lScore < self.m_tabSitDownConfig.lOccupySeatFree then
			local str = "免费坐下需要携带游戏币大于 " .. self.m_tabSitDownConfig.lOccupySeatFree .. " ,当前携带游戏币不足!"
			showToast(self, str, 2)
			return
		end
		self:getParentNode():sendSitDown(tag - 1, self:getMeUserItem().wChairID)
	end
end

function GameViewLayer:onResetView()
	self:stopAllActions()
	self:gameDataReset()
end

function GameViewLayer:onExit()
	self:onResetView()
end

--上庄状态
function GameViewLayer:applyBanker( state )
	-- print("-----上庄状态-----", state, APPLY_STATE.kCancelState, APPLY_STATE.kApplyState, APPLY_STATE.kApplyedState)
	if state == APPLY_STATE.kCancelState then
        self._scene:sendCancelApply(); 
    elseif state == APPLY_STATE.kApplyState then
        self._scene:sendApplyBanker();
    elseif state == APPLY_STATE.kApplyedState then
        self._scene:sendCancelApply();    
    end
end

---------------------------------------------------------------------------------------
--网络消息

------
--网络接收
function GameViewLayer:onGetUserScore( item )
	--自己
	if item.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
       self:reSetUserInfo()
    end

    --坐下用户
    for i = 1, g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
    	if nil ~= self.m_tabSitDownUser[i] then
    		if item.wChairID == self.m_tabSitDownUser[i]:getChair() then
    			self.m_tabSitDownUser[i]:updateScore(item)
    		end
    	end
    end

    --庄家
    if self.m_wBankerUser == item.wChairID then
    	--庄家游戏币
		-- local str = string.formatNumberThousands(item.lScore);
		-- if string.len(str) > 11 then
		-- 	str = string.sub(str, 1, 9) .. "...";
		-- end
		-- self.m_textBankerCoin:setString("游戏币:" .. str);
		self:freshBankerInfo(item.lScore);
    end
end

function GameViewLayer:refreshCondition(  )
	local applyable = self:getApplyable()
	if applyable then
		------
		--超级抢庄

		--如果当前有超级抢庄用户且庄家不是自己
		if (yl.INVALID_CHAIR ~= self.m_wCurrentRobApply) or (true == self:isMeChair(self.m_wBankerUser)) then
			ExternalFun.enableBtn(self.m_btnRob, false)
		else
			local useritem = self:getMeUserItem()
			--判断抢庄类型
			if g_var(cmd).SUPERBANKER_VIPTYPE == self.m_tabSupperRobConfig.superbankerType then
				--vip类型				
				ExternalFun.enableBtn(self.m_btnRob, useritem.cbMemberOrder >= self.m_tabSupperRobConfig.enVipIndex)
			elseif g_var(cmd).SUPERBANKER_CONSUMETYPE == self.m_tabSupperRobConfig.superbankerType then
				--游戏币消耗类型(抢庄条件+抢庄消耗)
				local condition = self.m_tabSupperRobConfig.lSuperBankerConsume + self.m_llCondition
				ExternalFun.enableBtn(self.m_btnRob, useritem.lScore >= condition)
			end
		end		
	else
		ExternalFun.enableBtn(self.m_btnRob, false)
	end
end

--游戏free
function GameViewLayer:onGameFree( )
	self.m_cbGameStatus = g_var(cmd).GAME_SCENE_FREE
	yl.m_bDynamicJoin = false
	self.m_lHaveJetton = 0;

	self:reSetForNewGame()

	--上庄条件刷新
	self:refreshCondition()

	--申请按钮状态更新
	self:refreshApplyBtnState()
	self:updateRejecttonState()
	self.isReject = false
	if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() and true == self:isMeChair(self.m_wBankerUser) then
		showToast(self.m_bankLayer,"现在是空闲时间，可以进行银行操作",1)
	end

	--自动打开路单
	if self.isAutoOpenLudan then
		self:onOpenBigLudanLayer();
	end

	--刷新路单
	local records = self:getDataMgr():getRecords();
	local recordNum = #records;
	if recordNum > g_var(cmd).RECORD_NUM then
		self:getDataMgr():subFirstGameRecord();
		--更新路单列表
		self:updateWallBill();
	end
end

--游戏开始
function GameViewLayer:onGameStart( )
	self.m_cbGameStatus = g_var(cmd).GAME_JETTON
	self.m_nJettonSelect = self.m_pJettonNumber[DEFAULT_BET].k;
	self.m_lHaveJetton = 0;

	--获取玩家携带游戏币	
	self:reSetUserInfo();

	self.m_bOnGameRes = false

	--不是自己庄家,且有庄家
	if false == self:isMeChair(self.m_wBankerUser) and false == self.m_bNoBanker then
		--下注
		self:enableJetton(true);
		--调整下注按钮
		self:adjustJettonBtn();

		--默认选中的筹码
		self:switchJettonBtnState(DEFAULT_BET)
	end	

	math.randomseed(tostring(os.time()):reverse():sub(1, 6))

	--申请按钮状态更新
	self:refreshApplyBtnState()
	self:updateRejecttonState()	
end

--游戏进行
function GameViewLayer:reEnterStart( lUserJetton )
	self.m_cbGameStatus = g_var(cmd).GAME_JETTON
	self.m_nJettonSelect = self.m_pJettonNumber[DEFAULT_BET].k;
	self.m_lHaveJetton = lUserJetton;

	--获取玩家携带游戏币
	self.m_scoreUser = 0
	self:reSetUserInfo();

	self.m_bOnGameRes = false

	--不是自己庄家
	if false == self:isMeChair(self.m_wBankerUser) then
		--下注
		self:enableJetton(true);
		--调整下注按钮
		self:adjustJettonBtn();

		--默认选中的筹码
		self:switchJettonBtnState(DEFAULT_BET)
	end
end

--下注条件
function GameViewLayer:onGetApplyBankerCondition( llCon , rob_config)
	self.m_llCondition = llCon
	--超级抢庄配置
	self.m_tabSupperRobConfig = rob_config

	self:refreshCondition();
end

--刷新庄家信息
function GameViewLayer:onChangeBanker( wBankerUser, lBankerScore, bEnableSysBanker )
	print("更新庄家数据:" .. wBankerUser .. "; coin =>" .. lBankerScore)

	--上一个庄家是自己，且当前庄家不是自己，标记自己的状态
	if self.m_wBankerUser ~= wBankerUser and self:isMeChair(self.m_wBankerUser) then
		self.m_enApplyState = APPLY_STATE.kCancelState
	end
	self.m_wBankerUser = wBankerUser
	--获取庄家数据
	self.m_bNoBanker = false

	local nickstr = "";
	--庄家姓名
	if true == bEnableSysBanker then --允许系统坐庄
		if yl.INVALID_CHAIR == wBankerUser then
			nickstr = "系统坐庄"
		else
			local userItem = self:getDataMgr():getChairUserList()[wBankerUser + 1];
			if nil ~= userItem then
				nickstr = userItem.szNickName 

				if self:isMeChair(wBankerUser) then
					self.m_enApplyState = APPLY_STATE.kApplyedState
				end
			else
				print("获取用户数据失败")
			end
		end	
	else
		if yl.INVALID_CHAIR == wBankerUser then
			nickstr = "无人坐庄"
			self.m_bNoBanker = true
		else
			local userItem = self:getDataMgr():getChairUserList()[wBankerUser + 1];
			if nil ~= userItem then
				nickstr = userItem.szNickName 

				if self:isMeChair(wBankerUser) then
					self.m_enApplyState = APPLY_STATE.kApplyedState
				end
			else
				print("获取用户数据失败")
			end
		end
	end
	self.m_clipBankerNick:setString(nickstr);

	--庄家游戏币
	-- local str = string.formatNumberThousands(lBankerScore);
	-- if string.len(str) > 11 then
	-- 	str = string.sub(str, 1, 7) .. "...";
	-- end
	-- self.m_textBankerCoin:setString("游戏币:" .. str);
	self:freshBankerInfo(lBankerScore);

	--如果是超级抢庄用户上庄
	if wBankerUser == self.m_wCurrentRobApply then
		self.m_wCurrentRobApply = yl.INVALID_CHAIR
		self:refreshCondition()
	end

	--坐下用户庄家
	local chair = -1
	for i = 1, g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
		if nil ~= self.m_tabSitDownUser[i] then
			chair = self.m_tabSitDownUser[i]:getChair()
			self.m_tabSitDownUser[i]:updateBanker(chair == wBankerUser)
		end
	end
end

--超级抢庄申请
function GameViewLayer:onGetSupperRobApply(  )
	if yl.INVALID_CHAIR ~= self.m_wCurrentRobApply then
		self.m_bSupperRobApplyed = true
		ExternalFun.enableBtn(self.m_btnRob, false)
	end
	--如果是自己f
	if true == self:isMeChair(self.m_wCurrentRobApply) then
		--普通上庄申请不可用
		self.m_enApplyState = APPLY_STATE.kSupperApplyed
	end
end

--超级抢庄用户离开
function GameViewLayer:onGetSupperRobLeave( wLeave )
	if yl.INVALID_CHAIR == self.m_wCurrentRobApply then
		--普通上庄申请不可用
		self.m_bSupperRobApplyed = false

		ExternalFun.enableBtn(self.m_btnRob, true)
	end

	--如果是自己
end

--更新用户下注
function GameViewLayer:onGetUserBet( cmd_placebet)
	local data = cmd_placebet
	if nil == data then
		return
	end
	local area = data.cbBetArea + 1;
	local wUser = data.wChairID;
	local llScore = data.lBetScore

	local nIdx = self:getJettonIdx(data.lBetScore) - 1
	print("nIdx",nIdx)
	local str = string.format("game/chip_%d.png", nIdx);
	local sp = cc.Sprite:create(str)
	
	local num = self.m_pJettonNumber[nIdx].k
	
	self:addLabelAtlasWithParent(sp, num)
	
	local btn = self.m_tableJettonArea[area];
	if nil == sp then
		print("sp nil");
	end

	if nil == btn then
		print("btn nil");
	end
	if nil ~= sp and nil ~= btn then
		--下注
		sp:setTag(wUser);
		local name = string.format("%d", area) --ExternalFun.formatScore(data.lBetScore);
		sp:setName(name)
		
		--筹码飞行起点位置
		local pos = self.m_betAreaLayout:convertToNodeSpace(self:getBetFromPos(wUser))
		--pos = self.m_betAreaLayout:convertToNodeSpace(self:getBetFromPos(wUser))
		sp:setPosition(pos)
		--筹码飞行动画
		local act = self:getBetAnimation(self:getBetRandomPos(area), cc.CallFunc:create(function()
			--播放下注声音
			ExternalFun.playSoundEffect("BetFly.mp3")
		end))
		sp:stopAllActions()
		sp:runAction(act)
		self.m_betAreaLayout:addChild(sp)

		--下注信息显示
		self:refreshJettonNode(self.m_tableJettonNode[area], llScore, llScore, self:isMeChair(wUser))
	end

	if self:isMeChair(wUser) then
		self.m_scoreUser = self.m_scoreUser - self.m_nJettonSelect;
		self.m_lHaveJetton = self.m_lHaveJetton + llScore;
		
		--调整下注按钮
		self:adjustJettonBtn();

		--显示下注信息
		self:refreshJetton();

		--刷新玩家分数
		self:freshUserScore();
	end
	-- self:updateRejecttonState()
	--刷新玩家分数
	self:freshSitUserScore(wUser, -llScore);
end

--更新用户下注失败
function GameViewLayer:onGetUserBetFail(  )
	local data = self:getParentNode().cmd_jettonfail;
	if nil == data then
		return;
	end

	--下注玩家
	local wUser = data.wPlaceUser;
	--下注区域
	local cbArea = data.cbBetArea + 1;
	--下注数额
	local llScore = data.lPlaceScore;

	if self:isMeChair(wUser) then
		--提示下注失败
		local str = string.format("下注 %s 失败", ExternalFun.formatScore(llScore))
		showToast(self,str,1)

		--自己下注失败
		self.m_scoreUser = self.m_scoreUser + llScore;
		self.m_lHaveJetton = self.m_lHaveJetton - llScore;
		self:adjustJettonBtn();
		self:refreshJetton()

		--
		if 0 ~= self.m_lHaveJetton then
			if nil ~= self.m_tableJettonNode[cbArea] then
				self:refreshJettonNode(self.m_tableJettonNode[cbArea],-llScore, -llScore, true)
			end

			--移除界面下注元素
			local name = string.format("%d", cbArea) --ExternalFun.formatScore(llScore);
			self.m_betAreaLayout:removeChildByName(name)
		end
	end
end

--断线重连更新界面已下注
function GameViewLayer:reEnterGameBet( cbArea, llScore )
	local btn = self.m_tableJettonArea[cbArea];
	if nil == btn or 0 == llSocre then
		return;
	end

	local numbers = self.m_pJettonNumber;
	local betNums = {numbers[1].k, numbers[2].k, numbers[3].k, numbers[4].k, numbers[5].k, numbers[6].k};
	local vec = self:getDataMgr().calcuteJetton(llScore, false, betNums);
	for k,v in pairs(vec) do
		local info = v;
		for i=1,info.m_cbCount do
			-- local str = string.format("chip_%d.png", info.m_cbIdx);
			-- local sp = cc.Sprite:createWithSpriteFrameName(str);
			local str = string.format("game/chip_%d.png", info.m_cbIdx);
			local sp = cc.Sprite:create(str)
			self:addLabelAtlasWithParent(sp, info.m_llScore);

			if nil ~= sp then
				sp:setTag(yl.INVALID_CHAIR);
				local name = string.format("%d", cbArea) --ExternalFun.formatScore(info.m_llScore);
				sp:setName(name);

				self:randomSetJettonPos(cbArea, sp);
				self.m_betAreaLayout:addChild(sp);
			end
		end
	end

	--下注信息显示
	self:refreshJettonNode(self.m_tableJettonNode[cbArea], llScore, llScore, false)
end

--断线重连更新玩家已下注
function GameViewLayer:reEnterUserBet( cbArea, llScore )
	local btn = self.m_tableJettonArea[cbArea];
	if nil == btn or 0 == llSocre then
		return;
	end

	--下注信息显示
	self:refreshJettonNode(self.m_tableJettonNode[cbArea], llScore, 0, true)
end

--游戏结束
function GameViewLayer:onGetGameEnd(  )
	self.m_cbGameStatus = g_var(cmd).GAME_END
	--记录上一局下注信息
	for k, v in pairs(self.m_tableJettonNode) do
		self.lastRoundJettonScore[k].m_llMyTotal = self.m_tableJettonNode[k].m_llMyTotal or  0
	end

	self:updateRejecttonState()

	self.m_bOnGameRes = true

	--不可下注
	self:enableJetton(false)

	--界面资源清理
	self:reSet()
end

--申请庄家
function GameViewLayer:onGetApplyBanker( )
	if self:isMeChair(self:getParentNode().cmd_applybanker.wApplyUser) then
		self.m_enApplyState = APPLY_STATE.kApplyState
	end

	self:refreshApplyList()
end

--取消申请庄家
function GameViewLayer:onGetCancelBanker(  )
	if self:isMeChair(self:getParentNode().cmd_cancelbanker.wCancelUser) then
		self.m_enApplyState = APPLY_STATE.kCancelState
	end
	
	self:refreshApplyList()
end

--刷新列表
function GameViewLayer:refreshApplyList(  )
	local userList = self:getDataMgr():getApplyBankerUserList()
	if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
        self.m_applyListLayer:refreshList(userList)
        -- dump(userList, "-------------刷新申请上庄列表------------", 6)
    end

    --刷新人数
	self.banker_apply_num:setString(#userList);
end 

function GameViewLayer:refreshUserList(  )
	local userList = self:getDataMgr():getUserList()
	if nil ~= self.m_userListLayer and self.m_userListLayer:isVisible() then
		self.m_userListLayer:refreshList(userList)
	end

	--刷新人数
	self.player_num:setString(#userList);
end

--刷新申请列表按钮状态
function GameViewLayer:refreshApplyBtnState(  )
	if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
		self.m_applyListLayer:refreshBtnState()
	end
end

--刷新路单
function GameViewLayer:updateWallBill()
	if nil ~= self.m_wallBill and self.m_wallBill:isVisible() then
		self.m_wallBill:refreshWallBillList()
	end

	--主界面路单
	self:showGameResultRecord()
end

--更新扑克牌
function GameViewLayer:onGetGameCard( tabRes, bAni, cbTime )
	self.m_cbGameStatus = g_var(cmd).GAME_END
	-- if nil == self.m_cardLayer then
	-- 	self.m_cardLayer = g_var(GameCardLayer):create(self)
	-- 	self:addToRootLayer(self.m_cardLayer, TAG_ZORDER.GAMECARD_ZORDER)
	-- end
	-- self.m_cardLayer:showLayer(true)
	-- self.m_cardLayer:refresh(tabRes, bAni, cbTime)
end

--桌面坐下玩家显示
function GameViewLayer:onGetSitDownShowFresh( index, isLeave)
	local sitBtn = self.m_tabSitDownList[index];
	local role_name = sitBtn:getChildByName("role_name");
	local role_score_bg = sitBtn:getChildByName("role_score_bg");
	local role_score = sitBtn:getChildByName("role_score");
	if isLeave then
		role_name:setVisible(true);
		role_score_bg:setVisible(true);
		role_score:setVisible(true);
	else
		role_name:setVisible(false);
		role_score_bg:setVisible(false);
		role_score:setVisible(false);
	end
end

--座位坐下信息
function GameViewLayer:onGetSitDownInfo( config, info )
	self.m_tabSitDownConfig = config
	
	local pos = cc.p(0,0)
	--获取已占位信息
	for i = 1, g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
		print("sit chair " .. info[i])
		self:onGetSitDown(i - 1, info[i], false)
	end
end

--座位坐下
function GameViewLayer:onGetSitDown( index, wchair, bAni )
	if wchair ~= nil 
		and nil ~= index
		and index ~= g_var(cmd).SEAT_INVALID_INDEX 
		and wchair ~= yl.INVALID_CHAIR then
		local useritem = self:getDataMgr():getChairUserList()[wchair + 1]

		if nil ~= useritem then
			--下标加1
			index = index + 1
			if nil == self.m_tabSitDownUser[index] then
				self.m_tabSitDownUser[index] = g_var(SitRoleNode):create(self, index)
				self.m_tabSitDownUser[index]:setPosition(self.m_tabSitDownList[index]:getPosition())
				self.m_roleSitDownLayer:addChild(self.m_tabSitDownUser[index],10)

				self:onGetSitDownShowFresh( index, false);
			end
			self.m_tabSitDownUser[index]:onSitDown(useritem, bAni, wchair == self.m_wBankerUser)

			if useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
				self.m_nSelfSitIdx = index
			end
		end
	end
end

--座位失败/离开
function GameViewLayer:onGetSitDownLeave( index )
	if index ~= g_var(cmd).SEAT_INVALID_INDEX 
		and nil ~= index then
		index = index + 1
		if nil ~= self.m_tabSitDownUser[index] then
			self.m_tabSitDownUser[index]:removeFromParent()
			self.m_tabSitDownUser[index] = nil

			self:onGetSitDownShowFresh( index, true);
		end

		if self.m_nSelfSitIdx == index then
			self.m_nSelfSitIdx = nil
		end
	end
end

--银行操作成功
function GameViewLayer:onBankSuccess( )
	local bank_success = self:getParentNode().bank_success
	if nil == bank_success then
		return
	end
	GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore
	GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

	if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
		-- self:refreshScore()
		self.m_bankLayer:refreshBankScore();

		showToast(self.m_bankLayer, bank_success.szDescribrString, 2)
	end

	
end

--银行操作失败
function GameViewLayer:onBankFailure( )
	local bank_fail = self:getParentNode().bank_fail
	if nil == bank_fail then
		return
	end

	if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
		showToast(self.m_bankLayer, bank_fail.szDescribeString, 2)
	end
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
	bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
	if nil ~= self.m_bankLayer then
		self.m_bankLayer:refreshBankRate(bankinfo.wRevenueTake, bankinfo.lBankOPButtonArray[1])
	end
end
------
---------------------------------------------------------------------------------------
function GameViewLayer:getParentNode( )
	return self._scene;
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

function GameViewLayer:addToRootLayer( node , zorder)
	if nil == node then
		return
	end

	self.m_rootLayer:addChild(node)
	node:setLocalZOrder(zorder)
end

function GameViewLayer:getChildFromRootLayer( tag )
	if nil == tag then
		return nil
	end
	return self.m_rootLayer:getChildByTag(tag)
end

function GameViewLayer:getApplyState(  )
	return self.m_enApplyState
end

function GameViewLayer:getApplyCondition(  )
	return self.m_llCondition
end

--获取能否上庄
function GameViewLayer:getApplyable(  )
	--自己超级抢庄已申请，则不可进行普通申请
	if APPLY_STATE.kSupperApplyed == self.m_enApplyState then
		return false
	end

	local userItem = self:getMeUserItem();
	if nil ~= userItem then
		return self.m_scoreUser > self.m_llCondition
	else
		return false
	end
end

--获取能否取消上庄
function GameViewLayer:getCancelable(  )
	return self.m_cbGameStatus == g_var(cmd).GAME_SCENE_FREE
end

--下注区域闪烁
function GameViewLayer:showBetAreaBlink(  )
	local blinkArea = self:getDataMgr().m_tabBetArea
	self:jettonAreaBlink(blinkArea)
end

function GameViewLayer:getDataMgr( )
	return self:getParentNode():getDataMgr()
end

function GameViewLayer:logData(msg)
	local p = self:getParentNode()
	if nil ~= p.logData then
		p:logData(msg)
	end	
end

function GameViewLayer:showPopWait( )
	self:getParentNode():showPopWait()
end

function GameViewLayer:dismissPopWait( )
	self:getParentNode():dismissPopWait()
end

function GameViewLayer:gameDataInit( )

    --播放背景音乐
    ExternalFun.playBackgroudAudio("bg.mp3")

    --用户列表
	self:getDataMgr():initUserList(self:getParentNode():getUserList())

    --加载资源
	self:loadRes()

	--变量声明
	self.m_nJettonSelect = -1
	self.m_lHaveJetton = 0;
	self.m_llMaxJetton = 0;
	self.m_llCondition = 0;
	yl.m_bDynamicJoin = false;
	self.m_scoreUser = self:getMeUserItem().lScore or 0

	--下注信息
	self.m_tableJettonBtn = {};
	self.m_tableJettonArea = {};

	--下注提示
	self.m_tableJettonNode = {};

	self.m_applyListLayer = nil
	self.m_userListLayer = nil
	self.m_wallBill = nil
	-- self.m_cardLayer = nil
	self.m_gameResultLayer = nil
	self.m_pClock = nil
	self.m_bankLayer = nil

	--申请状态
	self.m_enApplyState = APPLY_STATE.kCancelState
	--超级抢庄申请
	self.m_bSupperRobApplyed = false
	--超级抢庄配置
	self.m_tabSupperRobConfig = {}
	--游戏币抢庄提示
	self.m_bRobAlert = false

	--用户坐下配置
	self.m_tabSitDownConfig = {}
	self.m_tabSitDownUser = {}
	--自己坐下
	self.m_nSelfSitIdx = nil

	--座位列表
	self.m_tabSitDownList = {}

	--当前抢庄用户
	self.m_wCurrentRobApply = yl.INVALID_CHAIR

	--当前庄家用户
	self.m_wBankerUser = yl.INVALID_CHAIR

	--选中的筹码
	self.m_nSelectBet = DEFAULT_BET

	--是否结算状态
	self.m_bOnGameRes = false

	--是否无人坐庄
	self.m_bNoBanker = false

	--上一局下注信息记录
	self.lastRoundJettonScore = {}
	for i=1, 8 do
		self.lastRoundJettonScore[i] = {}
		self.lastRoundJettonScore[i].m_llMyTotal = 0
	end

	--重复下注是否点击过
	self.isReject = false

	--是否自动打开路单
	self.isAutoOpenLudan = false;
end

function GameViewLayer:gameDataReset(  )
	--资源释放
	cc.Director:getInstance():getTextureCache():removeTextureForKey("game/card.png")
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/game.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("game/game.png")
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/pk_card.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("game/pk_card.png")
	-- cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("bank/bank.plist")
	-- cc.Director:getInstance():getTextureCache():removeTextureForKey("bank/bank.png")

	--特殊处理public_res blank.png 冲突
	local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/public.plist")
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			if k ~= "blank.png" then
				cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(k)
			end
		end
	end
	-- cc.Director:getInstance():getTextureCache():removeTextureForKey("public_res/public_res.png")

	-- cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("setting/setting.plist")
	-- cc.Director:getInstance():getTextureCache():removeTextureForKey("setting/setting.png")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
	cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()


	--播放大厅背景音乐
	ExternalFun.playPlazzBackgroudAudio()

	--变量释放
	self.m_actDropIn:release();
	self.m_actDropOut:release();
	-- if nil ~= self.m_cardLayer then
	-- 	self.m_cardLayer:clean()
	-- end

	yl.m_bDynamicJoin = false;
	self:getDataMgr():removeAllUser()
	self:getDataMgr():clearRecord()
end

function GameViewLayer:getJettonIdx( llScore )
	local idx = 2;
	for i=1,#self.m_pJettonNumber do
		if llScore == self.m_pJettonNumber[i].k then
			idx = self.m_pJettonNumber[i].i;
			break;
		end
	end
	return idx;
end

function GameViewLayer:randomSetJettonPos( nodeArea, jettonSp )
	if nil == jettonSp then
		return;
	end

	local pos = self:getBetRandomPos(nodeArea)
	jettonSp:setPosition(cc.p(pos.x, pos.y));
end

--筹码移动位置
function GameViewLayer:getBetFromPos( wchair )
	if nil == wchair then
		return {x = 0, y = 0}
	end
	local winSize = cc.Director:getInstance():getWinSize()

	local useritem = self:getDataMgr():getChairUserList()[wchair + 1]
	if nil == useritem then
		return {x = winSize.width, y = 0}
	end

	--是否是坐下列表
	local idx = nil
	for i = 1,g_var(cmd).MAX_OCCUPY_SEAT_COUNT do
		if (nil ~= self.m_tabSitDownUser[i]) and (wchair == self.m_tabSitDownUser[i]:getChair()) then
			idx = i
			break
		end
	end
	if nil ~= idx then
		local pos = cc.p(self.m_tabSitDownUser[idx]:getPositionX(), self.m_tabSitDownUser[idx]:getPositionY())
		-- pos = self.m_roleSitDownLayer:convertToWorldSpace(pos)
		return {x = pos.x, y = pos.y}
	end

	--是否是自己
	if self:isMeChair(wchair) then
		local tmp = self.m_spBottom:getChildByName("player_head")
		if nil ~= tmp then
			local pos = cc.p(tmp:getPositionX(), tmp:getPositionY())
			-- pos = self.m_spBottom:convertToWorldSpace(pos)
			return {x = pos.x, y = pos.y}
		else
			return {x = winSize.width, y = 0}
		end
	end

	--默认位置
	local pos = cc.p(self.csbNode:getChildByName("userlist_btn"):getPositionX(), self.csbNode:getChildByName("userlist_btn"):getPositionY())
	return {x = pos.x, y = pos.y};
end

function GameViewLayer:getBetAnimation( pos, call_back )
	local moveTo = cc.MoveTo:create(BET_ANITIME, cc.p(pos.x, pos.y))
	if nil ~= call_back then
		return cc.Sequence:create(cc.EaseIn:create(moveTo, 2), call_back)
	else
		return cc.EaseIn:create(moveTo, 2)
	end
end

function GameViewLayer:getBetRandomPos(areaIndex)
	local node_chip = self.csbNode:getChildByName("node_chip");
	local chip_area = node_chip:getChildByName("chip_area_" .. areaIndex);
	
	if nil == chip_area then
		return {x = 0, y = 0};
	end

	local nodeSize = cc.size(100, 80);
	if areaIndex == 1 or areaIndex == 3 then
		nodeSize = cc.size(200, 160);
	elseif areaIndex == 2 then
		nodeSize = cc.size(300, 120);
	end

	local xOffset = math.random()-0.5;
	local yOffset = math.random()-0.5;

	local posX = chip_area:getPositionX();
	local posY = chip_area:getPositionY();
	return cc.p(xOffset * nodeSize.width/2 + posX, yOffset * nodeSize.height/2 + posY);
end

------
--倒计时节点
function GameViewLayer:createClockNode()
	self.m_pClock = cc.Node:create()
	self.m_pClock:setPosition(667,514)
	self:addToRootLayer(self.m_pClock, TAG_ZORDER.CLOCK_ZORDER)

	--加载csb资源
	-- local csbNode = ExternalFun.loadCSB("game/GameClockNode.csb", self.m_pClock)
	local csbNode =  self.csbNode:getChildByName("node_clock");

	--倒计时
	self.m_pClock.m_atlasTimer = csbNode:getChildByName("timer_atlas")
	self.m_pClock.m_atlasTimer:setString("")

	--提示
	self.m_pClock.m_spTip = csbNode:getChildByName("sp_tip")

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("blank.png")
	if nil ~= frame then
		self.m_pClock.m_spTip:setSpriteFrame(frame)
	end
end

function GameViewLayer:updateClock(tag, left)
	-- self.m_pClock:setVisible(left > 0)

	local str = string.format("%02d", left)
	self.m_pClock.m_atlasTimer:setString(str)

	if left <= 3 and left >= 0 then
    	--倒计时
		ExternalFun.playSoundEffect("TimeWarn.wav")
	end

	--下注时间
	if g_var(cmd).kGAMEFREE_COUNTDOWN == tag then
		if left == 1 then
			if nil ~= self.m_wallBill and self.m_wallBill:isVisible() then
				self.m_wallBill:showLayer(false)
			end

			--播放开始下注声音
			ExternalFun.playSoundEffect("startJetton.mp3")

			local aniNode = cc.CSLoader:createNode("startjetton/tongsha.csb"):addTo(self.animateUI)
            aniNode:setPosition(display.cx,display.cy)

            local ani = cc.CSLoader:createTimeline("startjetton/tongsha.csb")
            ani:gotoFrameAndPlay(0,false)
            aniNode:runAction(ani)
        end
	end

	if g_var(cmd).kGAMEPLAY_COUNTDOWN == tag then
		if left == 1 then
			--播放结束下注声音
			ExternalFun.playSoundEffect("stopJetton.mp3")

			local aniNode = cc.CSLoader:createNode("startjetton/tongpei.csb"):addTo(self.animateUI)
            aniNode:setPosition(display.cx,display.cy)

            local ani = cc.CSLoader:createTimeline("startjetton/tongpei.csb")
            ani:gotoFrameAndPlay(0,false)
            aniNode:runAction(ani)
		end
	end
	
	--结束时间
	if g_var(cmd).kGAMEOVER_COUNTDOWN == tag then
		if 12 == left then
			-- if true == self:getDataMgr().m_bJoin then
			-- 	self:showGameResult(true)
			-- end	
			-- self:showGameResult(true)
		elseif 8 == left then
			if self:getDataMgr().m_bJoin then
				-- if nil ~= self.m_cardLayer then
				-- 	self.m_cardLayer:showLayer(false)
				-- end
			end					
			--筹码动画
			-- self:betAnimation()	

			self:endBetAnimation();	
		elseif 4 == left then
			
			--更新路单列表
			self:updateWallBill()		
		elseif 3 == left then
			-- if nil ~= self.m_cardLayer then
			-- 	self.m_cardLayer:showLayer(false)
			-- end

			self:showGameResult(false);
		elseif 0 == left then
			--闪烁停止
			self:jettonAreaBlinkClean()
		end
	end
end

function GameViewLayer:showTimerTip(tag)
	tag = tag or -1
	local scale = cc.ScaleTo:create(0.2, 0.0001, 1.0)
	local call = cc.CallFunc:create(function (  )
		local str = string.format("sp_tip_%d.png", tag)
		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)

		self.m_pClock.m_spTip:setVisible(false)
		if nil ~= frame then
			self.m_pClock.m_spTip:setVisible(true)
			self.m_pClock.m_spTip:setSpriteFrame(frame)
		end
	end)
	local scaleBack = cc.ScaleTo:create(0.2,1.0)
	local seq = cc.Sequence:create(scale, call, scaleBack)

	self.m_pClock.m_spTip:stopAllActions()
	self.m_pClock.m_spTip:runAction(seq)
end

--下注显示
function GameViewLayer:refreshJettonNode( node, my, total, bMyJetton )	
	local bet_all = node:getChildByName("bet_all");
	local icon_1 = node:getChildByName("icon_1");
	local bet_bg = node:getChildByName("bet_bg");
	local bet_self = node:getChildByName("bet_self");

	if true == bMyJetton then
		node.m_llMyTotal = node.m_llMyTotal + my
	end

	node.m_llAreaTotal = node.m_llAreaTotal + total
	node:setVisible( node.m_llAreaTotal > 0)

	--自己下注数额
	local str = ExternalFun.numberThousands(node.m_llMyTotal);
	bet_self:setString(str);
	
	local scoreStr, scoreType = self:formatScoreText(node.m_llAreaTotal);
	self:formatScoreShow(scoreStr, scoreType, bet_all, icon_1, "fonts/icon_all_bet_wan_1.png", "fonts/icon_all_bet_yi_1.png")

	local allAreaTotalScore = 0;
	for i = 1,8 do
		allAreaTotalScore = allAreaTotalScore + self.m_tableJettonNode[i].m_llAreaTotal;
		local scoreStr, scoreType = self:formatScoreText(allAreaTotalScore);
		self.bet_total:setVisible(true);
		self:formatScoreShow(scoreStr, scoreType, self.bet_total:getChildByName("bet_all"), self.bet_total:getChildByName("icon_1"), "fonts/icon_all_bet_wan.png", "fonts/icon_all_bet_yi.png")
	end
end

function GameViewLayer:reSetJettonNode(node)
	node:setVisible(false);
	local bet_all = node:getChildByName("bet_all");
	local icon_1 = node:getChildByName("icon_1");
	local bet_bg = node:getChildByName("bet_bg");
	local bet_self = node:getChildByName("bet_self");

	bet_self:setString("")
	bet_all:setString("")
	-- node.m_imageBg:setContentSize(cc.size(34, 32))

	node.m_llMyTotal = 0
	node.m_llAreaTotal = 0
end

function GameViewLayer:reSetTotalJetton()
	self.bet_total:setVisible(false);
	local bet_all = self.bet_total:getChildByName("bet_all");
	local icon_1 = self.bet_total:getChildByName("icon_1");
	local bet_bg = self.bet_total:getChildByName("bet_bg");

	bet_all:setString("");
end

--路单显示
function GameViewLayer:showGameResultRecord()
	local records = self:getDataMgr():getRecords();
	local recordNum = #records;
	local startPos = cc.p(14, 126);
	local disX = 29;
	local disY = 23;
	local maxNum = 64;
	-- print(recordNum,"-----------路单显示------------")
	local node_record  = self.node_ludan:getChildByName("node_record");
	local content  = node_record:getChildByName("content");
	
	local numX = math.floor(recordNum/6) + 1;
	local size = cc.size(29 * numX, 138);
	if recordNum > 42 then
		node_record:setInnerContainerSize(size);
		content:setContentSize(size);
		self.btn_record:setContentSize(size);
	end
	content:removeAllChildren();
	for i = 1, recordNum do
		local rec = records[i];
		-- if recordNum >= maxNum then
		-- 	rec = records[recordNum -maxNum + i];
		-- elseif recordNum < maxNum and i <= recordNum then
		-- 	rec = records[i];
		-- else
		-- end
		if rec then
			local str = "";
			if g_var(cmd).AREA_XIAN == rec.m_cbGameResult then
				str = "waybill_xian.png";
			elseif g_var(cmd).AREA_ZHUANG == rec.m_cbGameResult then
				str = "waybill_zhuang.png";
			elseif g_var(cmd).AREA_PING == rec.m_cbGameResult then
				str = "waybill_ping.png";
			end
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str);
			local record = cc.Sprite:createWithSpriteFrame(frame);
			content:addChild(record);
			record:setPosition(startPos.x + math.floor((i-1) / 6)*disX, startPos.y - ((i-1) % 6) * disY);
		end
	end
	--大路单
	self:roadBig(records);

	--刷新局数
	self:refreshWallBillList(records);
end

-- 玩家详情
function GameViewLayer:onRoleInfoLayer()
	local roleInfoLayer = RoleInfoLayer:create();
	roleInfoLayer.m_parent = self;
	self:addChild(roleInfoLayer,10);

	local roleItem = self:getMeUserItem();
	roleInfoLayer:freshRoleInfo(roleItem, cc.p(300, 210));
end

-- 坐桌玩家详情
function GameViewLayer:onDeskRoleInfoLayer(roleItem, pos)
	local roleInfoLayer = RoleInfoLayer:create();
	roleInfoLayer.m_parent = self;
	self:addChild(roleInfoLayer,10);

	roleInfoLayer:freshRoleInfo(roleItem, pos);
end

function GameViewLayer:canUseBank()

	if self.m_lHaveJetton > 0 and nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then 
		showToast(self.m_bankLayer,"游戏过程中不能进行银行操作",1)
		return
	end
	if (self:isMeChair(self.m_wBankerUser) and self.m_cbGameStatus ~= g_var(cmd).GAME_SCENE_FREE ) and nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
		showToast(self.m_bankLayer,"非空闲时间段不可进行银行操作",1)
		return
	end
	return true
end


--大路
function GameViewLayer:roadBig(records)
	local node_roadBig  = self.node_ludan:getChildByName("node_roadBig");
	local content  = node_roadBig:getChildByName("content");

    self:setBigRoadData(records);

    local recordNum = self.bigXLen;
    local numX = recordNum + 1;
	local size = cc.size(29 * numX, 138);
	if recordNum > 7 then
		node_roadBig:setInnerContainerSize(size);
		content:setContentSize(size);
		self.btn_record_roadBig:setContentSize(size);
	end
	content:removeAllChildren();
    -- local startPos = cc.p(513, 756);
    local startPos = cc.p(14, 126);
    self:showWaybillItem(self.dataBig, self.bigXLen, content, "ludan/cellMid.png", "ludan/circle_red_big.png", "ludan/circle_blue_big.png", 30, 29, false, startPos);
end

--设置大路数据
function GameViewLayer:setBigRoadData(records)
	self.dataBig, self.bigXLen, self.dataBigReality, self.tempBiglist = self:getDataMgr():setBigRoadData(records);
end

function GameViewLayer:showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
	--显示路单
	local startIndex = 1;
	for i = 1 , #showData do
		for j = startIndex , bigXLen do
			local val = showData[i][j];
			if val then
				local item = cc.Sprite:create(1 == val and spRed or spBlack);
				if small then
					item:setPosition(startPos.x + (j-startIndex) * 29,startPos.y - ((i-1) / 2) * 23);
				else
					item:setPosition(startPos.x + (j-startIndex) * 29,startPos.y - (i-1) * 23);
				end
				node:addChild(item);

				--闪烁
				-- if i == #showData and j == #showData[1] then
				-- 	item:runAction(cc.Sequence:create(
				-- 		cc.Blink:create(5, 5), 
				-- 		cc.CallFunc:create( function()
				--             item:setVisible(true);
				--             item:setOpacity(255);
				--         end)
				-- 	));
				-- end
			end
		end
	end
end

--刷新局数
function GameViewLayer:refreshWallBillList(vec)
	local num_1  = self.node_ludan:getChildByName("num_1");
	local num_2  = self.node_ludan:getChildByName("num_2");
	local num_3  = self.node_ludan:getChildByName("num_3");
	local num_4  = self.node_ludan:getChildByName("num_4");
	local num_5  = self.node_ludan:getChildByName("num_5");

	local nTotal = #vec
	local nXian = 0
	local nZhuang = 0
	local nPing = 0
	local nXianDouble = 0
	local nZhuangDouble = 0
	local nXianTian = 0
	local nZhuangTian = 0
	for i = 1, nTotal do
		local rec = vec[i]
		if g_var(cmd).AREA_XIAN == rec.m_cbGameResult then
			nXian = nXian + 1
		elseif g_var(cmd).AREA_PING == rec.m_cbGameResult then
			nPing = nPing + 1
		elseif g_var(cmd).AREA_ZHUANG == rec.m_cbGameResult then
			nZhuang = nZhuang + 1
		end
		if rec.m_pServerRecord.bBankerTwoPair then
			nZhuangDouble = nZhuangDouble + 1
		end
		if rec.m_pServerRecord.bPlayerTwoPair then
			nXianDouble = nXianDouble + 1
		end
		if rec.m_pServerRecord.cbBankerCount < rec.m_pServerRecord.cbPlayerCount and rec.m_pServerRecord.cbPlayerCount >= 8 then
			nXianTian = nXianTian + 1
		end
		if rec.m_pServerRecord.cbPlayerCount < rec.m_pServerRecord.cbBankerCount and rec.m_pServerRecord.cbBankerCount >= 8 then
			nZhuangTian = nZhuangTian + 1
		end
	end

	local per = 0
	if 0 ~= nTotal then
		per = (nXian / nTotal) * 100
	end
	local str = string.format("%d", nXian)
	num_2:setString(str)

	per = 0
	if 0 ~= nTotal then
		per = (nPing / nTotal) * 100
	end
	str = string.format("%d", nPing)
	num_3:setString(str)

	per = 0
	if 0 ~= nTotal then
		per = (nZhuang / nTotal) * 100
	end
	str = string.format("%d", nZhuang)
	num_1:setString(str)

	str = string.format("%d", nXianDouble)
	num_5:setString(str)

	str = string.format("%d", nZhuangDouble)
	num_4:setString(str)
end

------
return GameViewLayer
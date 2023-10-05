local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.4tianjiu.src"

local cmd = appdf.req(module_pre .. ".models.CMD_Game")

local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local HelpLayer = appdf.req(module_pre .. ".views.layer.HelpLayer")
local CardLayer = appdf.req(module_pre .. ".views.layer.CardLayer")
local CardNode = appdf.req(module_pre .. ".views.layer.CardNode")
local CardPileLayer = appdf.req(module_pre .. ".views.layer.CardPileLayer")
local OperateBetNode = appdf.req(module_pre .. ".views.layer.OperateBetNode")
local TopUINode = appdf.req(module_pre .. ".views.layer.TopUINode")
local uiLocation = require(module_pre .. ".views.layer.uiLocation")
local RecordLayer = appdf.req(module_pre .. ".views.layer.RecordLayer")

local GameChatLayer = appdf.req(module_pre .. ".views.layer.GameChatLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local SpineObj = appdf.req("client.src.external.SpineObj")




local anchorPointHead = {cc.p(1, 1), cc.p(0, 0.5), cc.p(0, 0), cc.p(1, 0.5)}
local posReady = {cc.p(-333, 0), cc.p(135, 0), cc.p(516, -80), cc.p(-134, 0)}
local posPlate = {cc.p(667, 589), cc.p(237, 464), cc.p(667, 174), cc.p(1093, 455)}
local posChat = {cc.p(873, 660), cc.p(229, 558), cc.p(270, 285), cc.p(1095, 528)}

-- local posHead = {cc.p(492, 150), cc.p(1260, 304), cc.p(1260, 520), cc.p(492, 669),cc.p(72,520),cc.p(72,304)}
						  --100
local posHead = {cc.p(495, 106), cc.p(1247, 281), cc.p(1247, 510), cc.p(495, 675),cc.p(87,510),cc.p(87,281)}
local posRobSp= {}
local posCards={{},{},{},{},{},{}} --六个viewID的四张牌的位置
local posCardPoint={{},{},{},{},{},{}}
local posResultSp={{},{}}
local posScore={}
local posChip={}
local posReadySp={}
local posRect={{},{},{},{},{},{}}

-- local GameViewLayer = class("GameViewLayer",function(scene)
-- 	return cc.CSLoader:createNode("game/GameView.csb")
-- end)

local Zorder_PriGameLayer=2

local Zorder_RectLayer=0
local Zorder_CardLayer=1
local Zorder_EndLayer=2
local Zorder_SetNode=99
local Zorder_Clock=2
local Zorder_Chat=90

local Zorder_SettingLayer=100
local Zorder_HelpLayer=110
local Zorder_ZhanjiLayer=120

local Zorder_Head= 10
local Zorder_Flycoin= 11

local enumTable = 
{
	"BT_Exit",               -- 退出游戏
	"BT_Menu",               -- 菜单
	"BT_Help",               -- 帮助
	"BT_Set",                -- 设置
	"BT_Location",           -- 定位
	"BT_Sit",                -- 坐下
	"BT_Start",              -- 开始
	"BT_Exchange_menu",      -- 换桌
	"BT_Exchange",           -- 换桌
	"BT_Ready",              -- 准备
	"BT_Rob_banker",         -- 抢庄
	"BT_Buqiang",            -- 不抢
	"BT_Tip",                -- 提示
	"BT_Cancel",             -- 取消提示
	"BT_Zupaied",            -- 组牌
	"BT_Card_pile",          -- 牌堆
	"BT_Buqie",              -- 不抢
	"BT_Qieguo",             --切锅
    "BT_Voice",              --语音
    "BT_Chat",               --聊天
	
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(GameViewLayer.TAG_START, enumTable);

local tag_flycoin =13017


GameViewLayer.TagEnum=TagEnum

local Tag_Dice=1111

--手牌位置
local handCardPos = {   
						{cc.p(537, 650), cc.p(617, 650), cc.p(697, 650), cc.p(777, 650)},
						{cc.p(184, 420), cc.p(264, 420), cc.p(344, 420), cc.p(424, 420)},
						{cc.p(517, 160), cc.p(612, 160), cc.p(707, 160), cc.p(802, 160)},
					    {cc.p(920, 420), cc.p(1000, 420), cc.p(1080, 420), cc.p(1160, 420)}
					}
local playerPosArr = {cc.p(430, 643), cc.p(82, 453), cc.p(367, 181), cc.p(1258, 453)};
local playerPopList = {cc.p(430, 643-300), cc.p(82, 453-100), cc.p(367, 181), cc.p(1258-510, 453-100)};

 function GameViewLayer:ctor(scene)
 	ExternalFun.registerNodeEvent(self)

 	self._scene=scene

 	self:loadRes();
	self:gameDataInit();

	--初始化csb界面
	self:initCsbRes();

	--卡牌层
	self.CardLayer = CardLayer:create(self);
	self.cardLayerNode:addChild(self.CardLayer);

	self.TopUINode = TopUINode:create(self, self.topUI);
	self.topUI:addChild(self.TopUINode);

	self.OperateBetNode = OperateBetNode:create(self, self.node_operate_bet);
	self.node_operate_bet:addChild(self.OperateBetNode);

	--倒计时动画
	self:createHeadAnim();

	--聊天窗口层
    local tabconfig = {} 
    tabconfig.csbfile = cmd.RES.."chat/GameChatLayer.csb"
    tabconfig.szItemSelectFrameName = cmd.RES.."chat/public_chat_txtSelect.png"
    tabconfig.szItemFrameName = cmd.RES.."chat/public_chatLine.png"
    self.m_GameChat = GameChatLayer:create(scene._gameFrame ,tabconfig)
        :setLocalZOrder(50)
        :addTo(self)
	--创建手牌
	-- local playerInfo = PlayerInfo:create(userItem, viewId)
 --    self.m_tabUserHead[viewId] = playerInfo
 --    self.nodePlayer[viewId]:addChild(playerInfo)
end

--初始化数据
function GameViewLayer:gameDataInit()
	--游戏状态
    self._scene.m_cbGameStatus = cmd.GAME_SCENE_FREE;

    self.bTrustee = false;                           --托管
    self.cbTrusteeMode = 0;                          --托管模式
    self.cbSelectedCardData = {nil, nil, nil, nil};  --选中牌
    self.cardNumLimit = 4;                           --牌数
    self.myCardData = {};                            --手牌
    self.bRobBanker = {false,false,false,false};     --是否抢庄
    self.dBankerMoney = 0;                             --锅底分
    self.nUsedCardNum = 0;                           --用过的牌
    self.cbBankerHandCard = {};                      --庄家亮牌
    self.cardHeapArr = {};                           --剩余牌
    self.bGameFree = false;                          --是否空闲状态
    self.selectTrusteeMode = 1;                      --选择托管模式
    self.cbAutoHandCard = {};                        --最优手牌

    self._scene.cbPlayStatus = {0,0,0,0};            --玩家游戏状态
    self._scene.wBankerUser = yl.INVALID_WORD;       --庄家
    self.dotCnt = 1;                                 --点数

    self.bZupaiFinished = false;                     --组牌完成
    --通用
    self.cbPlayMode = -1;                             --游戏模式
    self.cbTimeRobBanker = 10;                        --抢庄时间
    self.cbTimeChip = 10;                             --下注时间
    self.cbTimeStartGame = 30;                        --开始时间
    self.cbTimeOpenCard = 20;                         --组牌时间
    self.cbTimeChangeBanker = 20;                     --切庄时间
    self.dBetScore = {};                              --下注
    self.bJiaGuaPaiJiu = false;                       --加锅牌九
    self.bDaPaiJiu = false;                           --大小牌九
    self.cbBankerMode = 0;                            --庄家模式（0：轮庄 1：抢庄 2：霸王庄）
    self.nChangeBankerCount = 0;                      --换庄局数
    self.cbBankerOpenCardCount = 0;                   --庄家亮牌数
    self.cbBetDaoShu = 0;                             --下注道数
    self.bAutoOpenCard = false;                       --自动开牌
    self.bSelectScore = false;                        --每局选分
    self.dGuoDiScore = 0;                             --锅底分数
    self.dMinBetScore = 0;                            --最小下注
    self.bHeadAllBet = false;                         --头局下满
    self.bZhaDan = false;                             --炸弹
    self.bDiJiuNiangNiang = false;                    --地九娘娘
    self.bGuiZi = false;                              --鬼子
    self.bTianJiuWang = false;                        --天九王

    self.cbTimeRemain = 0;                            --剩余时间
    self.bGameEnd = false;                            --游戏结束

    self.node_player = {};                            --玩家信息

    self.m_TimeProgress = {};                         --倒计时动画
    self.node_hand_anim = {};                         --倒计时动画节点
    self.m_userChat = {};                             --聊天
    self.timeSchedule = nil;                          --系统时间
    self.card_type = {};                              --牌型
    self.isEnabledChange = true;                      --是否可换桌
    self.zhanjiLayer = nil;                           --战绩流水

end

--重置本局数据
function GameViewLayer:ReSetGameData()
	self.cbSelectedCardData = {nil, nil, nil, nil};  --选中牌
	self.myCardData = {};                            --手牌
	self.nUsedCardNum = 0;                           --用过的牌
	self.cbBankerHandCard = {};                      --庄家亮牌
	self.cardHeapArr = {};                           --剩余牌
	self.cbAutoHandCard = {};                        --最优手牌

	self.bGameEnd = false;                           --游戏结束
	
	self.isEnabledChange = true;                     --是否可换桌

	self:resertCardHeapArr();
	self:OnResetView();
end

--重置游戏界面
function GameViewLayer:OnResetView()
	--重置界面UI
	self:stopAllActions();
	if PriRoom and GlobalUserItem.dwCurRoomGroupID ~= 0 then
	else
		self.btn_ready:setVisible(false);
	end

	self.robLayer:setVisible(false);
	self.robSpine:stopAllActions();
    self.robSpine:removeAllChildren();
    self.node_card_send:removeAllChildren();

    self.CardLayer:onResetData();

	--界面初始化
    for i = 1, cmd.GAME_PLAYER do
        local player = self.node_player[i];
        --玩家
        local player_item = player:getChildByName("player_item");
        local icon_banker = player_item:getChildByName("icon_banker");
        icon_banker:setVisible(false);
        --下注
        local node_bet = player:getChildByName("node_bet");
        node_bet:setVisible(false);
        --提示
        local node_tip = player:getChildByName("node_tip");
        node_tip:setVisible(false);
        --飘分
        local end_score = player:getChildByName("end_score");
        end_score:setVisible(false);
        --操作提示
        local icon_operate_tip = player:getChildByName("icon_operate_tip");
        icon_operate_tip:setVisible(false);
        --组牌完成
        local icon_zupaied = player:getChildByName("icon_zupaied");
        icon_zupaied:setVisible(false);
        --组牌结果
        local icon_end = player:getChildByName("icon_end");
        icon_end:setVisible(false);

        --牌型
        self.card_type[i]:setVisible(false);

        --聊天
        local chat_view = self.node_chat:getChildByName("chat_view_" .. i);
        chat_view:setVisible(false);
        self.m_userChat[i] = chat_view;
    end

    if self._timeLessUpdate then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeLessUpdate);
		self._timeLessUpdate = nil;
	end
end

function GameViewLayer:loadRes()
	--加载卡牌纹理
	-- cc.Director:getInstance():getTextureCache():addImage("game/card.png");

	display.loadSpriteFrames(cmd.RES.."game/bet.plist",cmd.RES.."game/bet.png")
    display.loadSpriteFrames(cmd.RES.."card_type/card_type.plist",cmd.RES.."card_type/card_type.png")
    display.loadSpriteFrames(cmd.RES.."game/game.plist",cmd.RES.."game/game.png")
    display.loadSpriteFrames(cmd.RES.."card/card.plist",cmd.RES.."card/card.png")
end

--界面初始化
function GameViewLayer:initCsbRes()
	local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameView.csb", self);
	self.m_rootLayer = rootLayer

	self._csbNode = csbNode;

	-- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)
    -- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()

	-- 初始化ui
	self:initUi(csbNode);
end

 function GameViewLayer:onExit()
 	
	print("GameViewLayer onExit")
	cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timeSchedule)
	self.timeSchedule = nil;
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    self._scene.m_tabUserItem = {}
    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)

    if self._beginTimeId then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
		self._beginTimeId = nil
	end

	if self._timeLessUpdate then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeLessUpdate);
		self._timeLessUpdate = nil;
	end
end

--ui
function GameViewLayer:initUi(csbNode)
	self.bg = csbNode:getChildByName("bg");

	--按钮列表
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender);
		end
	end	

	-- topUI
	local topUI = csbNode:getChildByName("topUI");
	self.topUI = topUI;

	--定位
	self.btn_location = csbNode:getChildByName("btn_location");
	self.btn_location:setTag(TAG_ENUM.BT_Location);
	self.btn_location:addTouchEventListener(btnEvent);

	--menu
	local btn_menu = topUI:getChildByName("btn_menu");
	btn_menu:setTag(TAG_ENUM.BT_Menu);
	btn_menu:addTouchEventListener(btnEvent);
	self.btn_menu = btn_menu;

	local node_menu = topUI:getChildByName("node_menu");
	self.node_menu = node_menu;
	
	local menu_bg = node_menu:getChildByName("menu_bg");

	local btn_set = node_menu:getChildByName("btn_set");
	btn_set:setTag(TAG_ENUM.BT_Set);
	btn_set:addTouchEventListener(btnEvent);

	local btn_exchange_menu = node_menu:getChildByName("btn_exchange");
	btn_exchange_menu:setTag(TAG_ENUM.BT_Exchange_menu);
	btn_exchange_menu:addTouchEventListener(btnEvent);
	self.btn_exchange_menu = btn_exchange_menu;

	local btn_help = node_menu:getChildByName("btn_help");
	btn_help:setTag(TAG_ENUM.BT_Help);
	btn_help:addTouchEventListener(btnEvent);
	self.btn_help = btn_help;

	local btn_exit = node_menu:getChildByName("btn_exit");
	btn_exit:setTag(TAG_ENUM.BT_Exit);
	btn_exit:addTouchEventListener(btnEvent);

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

	--btn
	self.btn_ready = csbNode:getChildByName("btn_ready");
	self.btn_ready:setTag(TAG_ENUM.BT_Ready);
	self.btn_ready:addTouchEventListener(btnEvent);

	self.btn_exchange = csbNode:getChildByName("btn_exchange");
	self.btn_exchange:setTag(TAG_ENUM.BT_Exchange);
	self.btn_exchange:addTouchEventListener(btnEvent);

	--坐下、开始
	self.btn_sit = csbNode:getChildByName("btn_sit");
	self.btn_sit:setTag(TAG_ENUM.BT_Sit);
	self.btn_sit:addTouchEventListener(btnEvent);

	self.btn_start = csbNode:getChildByName("btn_start");
	self.btn_start:setTag(TAG_ENUM.BT_Start);
	self.btn_start:addTouchEventListener(btnEvent);

	self.btn_rob_banker = csbNode:getChildByName("btn_rob_banker");
	self.btn_rob_banker:setTag(TAG_ENUM.BT_Rob_banker);
	self.btn_rob_banker:addTouchEventListener(btnEvent);

	self.btn_buqiang = csbNode:getChildByName("btn_buqiang");
	self.btn_buqiang:setTag(TAG_ENUM.BT_Buqiang);
	self.btn_buqiang:addTouchEventListener(btnEvent);

	self.btn_tip = csbNode:getChildByName("btn_tip");
	self.btn_tip:setTag(TAG_ENUM.BT_Tip);
	self.btn_tip:addTouchEventListener(btnEvent);

	self.btn_cancel = csbNode:getChildByName("btn_cancel");
	self.btn_cancel:setTag(TAG_ENUM.BT_Cancel);
	self.btn_cancel:addTouchEventListener(btnEvent);

	self.btn_zupaied = csbNode:getChildByName("btn_zupaied");
	self.btn_zupaied:setTag(TAG_ENUM.BT_Zupaied);
	self.btn_zupaied:addTouchEventListener(btnEvent);

	self.btn_card_pile = csbNode:getChildByName("btn_card_pile");
	self.btn_card_pile:setTag(TAG_ENUM.BT_Card_pile);
	self.btn_card_pile:addTouchEventListener(btnEvent);

	self.btn_buqie = csbNode:getChildByName("btn_buqie");
	self.btn_buqie:setTag(TAG_ENUM.BT_Buqie);
	self.btn_buqie:addTouchEventListener(btnEvent);

	self.btn_qieguo = csbNode:getChildByName("btn_qieguo");
	self.btn_qieguo:setTag(TAG_ENUM.BT_Qieguo);
	self.btn_qieguo:addTouchEventListener(btnEvent);

	--语音按钮
	local btnVoice = csbNode:getChildByName("btn_voice")
 	btnVoice:addTouchEventListener(function(ref, eventType)
 		if eventType == ccui.TouchEventType.began then
 			if self._scene.m_bLookOn then
				showToast(self, "观战中不可使用该功能!", 1)
			else
 				self._scene:getParentNode():startVoiceRecord()
 			end
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
    		if not self._scene.m_bLookOn then
        		self._scene:getParentNode():stopVoiceRecord()
        	end
        end
	end)

	-- AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

	self.btn_chat = csbNode:getChildByName("btn_chat");
	self.btn_chat:setTag(TAG_ENUM.BT_Chat);
	self.btn_chat:addTouchEventListener(btnEvent);
	
	self.node_guo = csbNode:getChildByName("node_guo");
	self.node_operate_bet = csbNode:getChildByName("node_operate_bet");

	self.node_chat = csbNode:getChildByName("node_chat");
	self.node_expression = csbNode:getChildByName("node_expression");
	self.node_card_send = csbNode:getChildByName("node_card_send");
	self.cardLayerNode = csbNode:getChildByName("cardLayer");
	self.waitLayer = csbNode:getChildByName("waitLayer");
	self.robLayer = csbNode:getChildByName("robLayer");
	self.robRect = self.robLayer:getChildByName("robRect");
	self.robSpine = self.robLayer:getChildByName("robSpine");
	self.diceLayer = csbNode:getChildByName("diceLayer");
	self.diceSpine = self.diceLayer:getChildByName("diceSpine");
	self.node_layer = csbNode:getChildByName("node_layer");
	self.node_layer:setLocalZOrder(10);
	
	local node_card_type = csbNode:getChildByName("node_card_type");

	local node_player = csbNode:getChildByName("node_player");
	for i = 1, cmd.GAME_PLAYER do
		--玩家
        local player = node_player:getChildByName("player_" .. i);
        self.node_player[i] = player;

        --玩家
        local player_item = player:getChildByName("player_item");
        player_item:setVisible(false);
        self.node_hand_anim[i] = player_item:getChildByName("node_hand_anim");

        local icon_banker = player_item:getChildByName("icon_banker");
        icon_banker:setVisible(false);

        --房主
        local icon_house_owner = player_item:getChildByName("icon_house_owner");
        icon_house_owner:setVisible(false);
        --下注
        local node_bet = player:getChildByName("node_bet");
        node_bet:setVisible(false);

        --提示
        local node_tip = player:getChildByName("node_tip");
        node_tip:setVisible(false);

        --飘分
        local end_score = player:getChildByName("end_score");
        end_score:setVisible(false);

        --操作提示
        local icon_operate_tip = player:getChildByName("icon_operate_tip");
        icon_operate_tip:setVisible(false);

        --组牌完成
        local icon_zupaied = player:getChildByName("icon_zupaied");
        icon_zupaied:setVisible(false);

        --组牌结果
        local icon_end = player:getChildByName("icon_end");
        icon_end:setVisible(false);

        --准备
        local icon_ready = player:getChildByName("icon_ready");
        icon_ready:setVisible(false);

        --牌型
        local card_type = node_card_type:getChildByName("card_type_" .. i);
        card_type:setVisible(false);
        self.card_type[i] = card_type;

        --聊天
        local chat_view = self.node_chat:getChildByName("chat_view_" .. i);
        chat_view:setVisible(false);
        self.m_userChat[i] = chat_view;
	end


end

--
function GameViewLayer:setPlayStatus(viewid,status)
	self.tabPlayStatus[viewid]=status
end

--玩家信息
function GameViewLayer:showUserInfo(userItem)
	if userItem == nil then
		return;
	end

	local viewId = self._scene:SwitchViewChairID(userItem.wChairID);
    --玩家弹窗
    local anr = {cc.p(0.0,1.0),cc.p(0.0,0.0),cc.p(0.0,0.0),cc.p(1.0,0.0)};
    --玩家
    local player_item = self.node_player[viewId]:getChildByName("player_item");
    player_item:setVisible(true)
    local role_icon = player_item:getChildByName("icon_head");
    role_icon:setVisible(true);
    -- local btn_bottom = player_item:getChildByName("btn_bottom")
    -- btn_bottom:setVisible(false)
    local head = PopupInfoHead:createClipHead(userItem, 92, "game/head_mask.png")
    head:setScale(1.4)
    role_icon:removeChildByTag(1)
    head:setTag(1)
    head:setAnchorPoint(cc.p(0.5,0.5))
    head:setPosition(cc.p(role_icon:getContentSize().width/2,role_icon:getContentSize().height/2))
    head:enableInfoPop(not self._scene.m_bLookOn,playerPopList[viewId] , anr[viewId])
    role_icon:addChild(head)
    
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "popInfo/role_info_bg.png" --弹出框背景
    head:setPropInfo(param)
    head:setPopCsbFile("popInfo/GamePopInfo.csb") -- 自定义CSB

    local bottom_head = player_item:getChildByName("bottom_head");
    -- bottom_head:setVisible(false);
    local icon_banker = player_item:getChildByName("icon_banker");
    -- local icon_house_owner = player_item:getChildByName("icon_house_owner");
    -- icon_house_owner:setVisible(false);
    local player_name = player_item:getChildByName("player_name");

    local roleName = string.EllipsisByConfig(userItem.szNickName,80,string.getConfig("fonts/round_body.ttf" , 18));
    player_name:setString(roleName);
    self:updateScore(userItem.lScore, userItem.wChairID);

    --房主标识
    if PriRoom and GlobalUserItem.bPrivateRoom then
        local _scoreUser = 0;
        if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == userItem.dwUserID and not GlobalUserItem.bPrivateGoldRoom then
            self:showFangzhuIcon(viewId);
        end
        -- --积分场分数显示
        -- if PriRoom:getInstance().m_tabPriData.lIniScore ~= nil and 
        --     PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
        --     dump(PriRoom:getInstance().m_tabPriData, "约战房信息", 10);
        --     _scoreUser = _scoreUser - PriRoom:getInstance().m_tabPriData.lIniScore;
        --     role_score_num:setString(string.format("%d", _scoreUser));
        -- end
    end

    --是否准备
    if userItem.cbUserStatus == yl.US_READY then
        self:showReady(userItem.wChairID, true)
    else
        self:showReady(userItem.wChairID, false)
    end

    if yl.US_READY == userItem.cbUserStatus and viewId == cmd.MY_VIEWID then
		self._scene:KillGameClock()
	end
end

--删除玩家
function GameViewLayer:deleteUserInfo(userItem, newstatus, oldstatus)
	-- print(userItem.dwUserID,"-----------删除玩家-------------")
    if userItem == self._scene:GetMeUserItem() then
        return;
    end
    local userinfo = clone(self._scene:getUserInfoByUserID(userItem.dwUserID));
    
    if not userinfo then
        return;
    end

    local wChairID = userinfo.wChairID;
    if userinfo.wChairID == yl.INVALID_CHAIR then
    	wChairID = oldstatus.wChairID;
    end

    self:resetUser(wChairID);
end

--重置玩家
function GameViewLayer:resetUser(chairId)
	self._scene.m_tabUserItem[chairId+1] = nil;

    local pos = self._scene:SwitchViewChairID(chairId);
    --清除玩家
    local player = self.node_player[pos];
    local player_item = player:getChildByName("player_item");
    player_item:setVisible(false);
    --下注
    local node_bet = player:getChildByName("node_bet");
    node_bet:setVisible(false);

    --提示
    local node_tip = player:getChildByName("node_tip");
    node_tip:setVisible(false);
    --飘分
    local end_score = player:getChildByName("end_score");
    end_score:setVisible(false);
    --操作提示
    local icon_operate_tip = player:getChildByName("icon_operate_tip");
    icon_operate_tip:setVisible(false);
    --组牌完成
    local icon_zupaied = player:getChildByName("icon_zupaied");
    icon_zupaied:setVisible(false);
    --组牌结果
    local icon_end = player:getChildByName("icon_end");
    icon_end:setVisible(false);

    self.card_type[pos]:setVisible(false);

    -- self._scene.m_tabUserItem[chairId+1] = nil;
    --重置界面
    self:showReady(chairId,false);
    --聊天、牌
    local chat_view = self.node_chat:getChildByName("chat_view_" .. pos);
    chat_view:setVisible(false);

    self.CardLayer:onRemoveHandCard(pos);

  --   local node_hand = self.CardLayer:getHandCard();
  --   if node_hand and #node_hand > 0 and not tolua.isnull(node_hand[pos]) and not tolua.isnull(node_hand) then
  --   	node_hand[pos]:removeAllChildren();
		-- node_hand[pos] = {};
  --   end
	

    -- local role_score = self.node_role_score:getChildByName("role_score_" .. pos);
    -- role_score:setVisible(false);
    -- local player_bet = self.node_bet_num:getChildByName("player_bet_" .. pos);
    -- player_bet:setVisible(false);
    -- --抢庄
    -- local role_rob_num = self.node_rob_icon:getChildByName("role_rob_num_" .. pos);
    -- role_rob_num:setVisible(false);

    -- local cardArr = self.m_CardArr[pos];
    -- if cardArr and #cardArr > 0 and not tolua.isnull(self.m_CardArr[pos]) and not tolua.isnull(self.m_CardArr) then
    --     self.m_CardArr[pos]:removeAllChildren();
    --     self.m_CardArr[pos] = {};
    -- end
end

--刷新分数
function GameViewLayer:updateScore(lScore, wChairID)
    local viewId = self._scene:SwitchViewChairID(wChairID);
    -- self.m_PlayerMoney[viewId] = tonumber(lScore);

    local player_item = self.node_player[viewId]:getChildByName("player_item");

    local score_num = player_item:getChildByName("score_num");
    local str = ExternalFun.formatScoreText(lScore);
    score_num:setString(str);
    --房主标识
    if PriRoom and GlobalUserItem.bPrivateRoom then
        local _scoreUser = 0;
        if nil ~= lScore then
	      _scoreUser = lScore
	    end 
        --积分场分数显示
        if PriRoom:getInstance().m_tabPriData.lIniScore ~= nil and 
            PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
            -- dump(PriRoom:getInstance().m_tabPriData, "约战房信息", 10);
            _scoreUser = _scoreUser - PriRoom:getInstance().m_tabPriData.lIniScore;
            score_num:setString(string.format("%d", _scoreUser));
        end
    end
end

function GameViewLayer:getTableCardNum()
	local leftCard=self._scene.allCardCount
	for i=1,cmd.GAME_PLAYER do
		if self.tabPlayStatus[i]==cmd.PS_SITTING then
			leftCard=leftCard - self._scene.cardNumLimit
		end
	end
	return leftCard
end


function GameViewLayer:priGameLayerZorder()
	return Zorder_PriGameLayer
end

function GameViewLayer:setGameInfo(bBigPaiJiu)
	self.gameinfoText=self:getChildByName("gameinfo")
	if self.gameinfoText then
		self.gameinfoText:setString(bBigPaiJiu==true and "大牌九" or "小牌九")
	end
end

--按钮事件
function GameViewLayer:onButtonClickedEvent(tag,btn)
	if btn==nil then
		btn=self:getChildByTag(tag)
	end

	if tag==TAG_ENUM.BT_Menu then         --菜单
		self:onShowMenuPanel();
	elseif tag==TAG_ENUM.BT_Set then      --设置
		self:onSetLayer();
	elseif tag==TAG_ENUM.BT_Help then     --帮助
		self:onHelpLayer();
	
	elseif tag==TAG_ENUM.BT_Exit then     --退出游戏
		self:onExitLayer();

	elseif tag == TAG_ENUM.BT_Exchange_menu then       --换桌
		self:onMenuChangeDesk(btn);
	elseif tag == TAG_ENUM.BT_Exchange then       --换桌
		self:onChangeDesk(btn);
	elseif tag==TAG_ENUM.BT_Ready then         --准备
		self:onReadyEvent();

	elseif tag==TAG_ENUM.BT_Rob_banker then    --抢庄
		self:onRobBankerEvent();
	elseif tag==TAG_ENUM.BT_Buqiang then       --不抢
		self:onBuqiangEvent();
	elseif tag==TAG_ENUM.BT_Card_pile then     --牌堆
		self:onCardPileLayer();
	elseif tag==TAG_ENUM.BT_Buqie then         --不切
		self:onBuqieEvent();
	elseif tag==TAG_ENUM.BT_Qieguo then        --切锅
		self:onQieguoEvent();
	elseif tag==TAG_ENUM.BT_Voice then         --语音
		-- self:onReadyEvent();
	elseif tag==TAG_ENUM.BT_Chat then          --聊天
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1);
		else
			self.m_GameChat:showGameChat(true);
		end
		
	elseif tag==TAG_ENUM.BT_Tip then     --提示
		self:ontipEvent();
	elseif tag==TAG_ENUM.BT_Cancel then      --取消提示
		self:onCancelEvent();
	elseif tag==TAG_ENUM.BT_Zupaied then         --组牌
		self:onZupaiedEvent();
	elseif tag==TAG_ENUM.BTN_CHAT then    --聊天
		self._chatLayer:showGameChat(true)
	elseif  tag == TAG_ENUM.BT_AUTO then    --自动托管
		--自动托管
		self:onButtonClickedEvent(TAG_ENUM.BTN_TRUST)

		if self._scene.bTrustee then
			self._scene:sendTrustee(false)
		else
			self._scene:sendTrustee(true)
		end
	elseif tag==TAG_ENUM.BTN_TRUST then    --托管
		self.setNode:setVisible(false)
		-- self._scene:setTrustee()
	elseif tag==TAG_ENUM.BT_Location then      --定位
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1);
		else
			self:showAdressLayer();
		end
		
	elseif tag==TAG_ENUM.BT_Sit then      --坐下
		self.btn_sit:setEnabled(false);
		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ()
            self.btn_sit:setEnabled(true);
        end)))

		self._scene._gameFrame.bChangeDesk = true;
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR);
	elseif tag==TAG_ENUM.BT_Start then      --开始
		self.btn_start:setVisible(false)
		self._scene._gameFrame:SendUserBegin()

	
	end
end

--显示坐下按钮
function GameViewLayer:showSitBtn(visible,time)
	if not self._scene.m_bLookOn then
		if visible then
			if not self._beginTimeId and time then
		        self.m_beginTimeNum = time
		        self.btn_start:getChildByName("Text_1"):setString(""..self.m_beginTimeNum)
			    self._beginTimeId = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
			    	self:updateBeginTime()
			    end, 1, false)		
			end
		else
			if self._beginTimeId then
				cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
				self._beginTimeId = nil
			end			
		end	
	end
	self.btn_sit:setVisible(visible and self._scene.m_bLookOn)
	self.btn_sit:setEnabled(true);
	self.btn_start:setVisible(visible and not self._scene.m_bLookOn)
	if visible and not self._scene.m_bLookOn then
		self.btn_ready:setVisible(false);
	end
end


function GameViewLayer:updateBeginTime()
	if self.m_beginTimeNum and self.m_beginTimeNum >0 then 
		self.m_beginTimeNum = self.m_beginTimeNum -1
		self.btn_start:getChildByName("Text_1"):setString(""..self.m_beginTimeNum)	
	else
		if self._beginTimeId then
			cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
			self._beginTimeId = nil
		end
	end

end

--加载点动画
function GameViewLayer:showDot()
	for i = 1, 3 do
		local dot = self.waitLayer:getChildByName("dot_" .. i);
		dot:setVisible(i-1<self.dotCnt)
	end
	self.dotCnt = self.dotCnt + 1;
	if self.dotCnt > 4 then
		self.dotCnt = 1;
	end
end

--显示等待界面
function GameViewLayer:showWaitLayer(isShow)
	local myChairId = self._scene:GetMeChairID();
	if self._scene.cbPlayStatus and self._scene.cbPlayStatus[myChairId+1] == cmd.PS_SITTING then
		isShow = false;
	end

	self.waitLayer:setVisible(isShow);
	if isShow then
		if not self._timeLessUpdate then
			self._timeLessUpdate = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		    	self:showDot();
		    end, 1, false)
		end
	else
		if self._timeLessUpdate then
			cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeLessUpdate);
			self._timeLessUpdate = nil;
		end
	end
end

--显示功能按钮底板
function GameViewLayer:onShowMenuPanel()
	self.menuIsUp = not self.menuIsUp;
	self.node_menu:setVisible(self.menuIsUp);

	if self._scene.m_bLookOn or GlobalUserItem.bPrivateRoom then
		self.btn_exchange_menu:setEnabled(false);
		self.btn_exchange_menu:setVisible(false);
		self.btn_help:setPositionY(463);
	end
end

--设置
function GameViewLayer:onSetLayer()
	local set = SettingLayer:create();
	set.m_parent = self;
	self:addChild(set,10);

	self:onShowMenuPanel();
end

--退出
function GameViewLayer:onExitLayer()
	self:onShowMenuPanel();
	local myChairId = self._scene:GetMeChairID();
	--判断自己是否在游戏中
	if self._scene.cbPlayStatus and  self._scene.cbPlayStatus[myChairId+1] ~= cmd.PS_SITTING then
		self._scene.m_cbGameStatus=cmd.GAME_STATUS_FREE;
	end

	self._scene:onQueryExitGame();
end

--帮助
function GameViewLayer:onHelpLayer()
	local help = HelpLayer:create();
	help.m_parent = self;
	self:addChild(help,10);

	self:onShowMenuPanel();
end

--换桌
function GameViewLayer:onChangeDesk(ref)
	if GlobalUserItem.bPrivateRoom == true then
		return
	end
	--判断状态
	if self._scene:GetMeUserItem().cbUserStatus >= yl.US_PLAYING then
        local query = QueryDialog:create("您正在游戏状态无法换桌!", function(ok)
            if ok then
                return;
            end
        end, 30, QueryDialog.QUERY_SURE)
        :setCanTouchOutside(false)
        :addTo(self, GameViewLayer.ZORDER_RESULT)
	else
		if self.isEnabledChange then
            ref:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ()
                self.isEnabledChange = true;
                ref:setEnabled(self.isEnabledChange);
            end)))
            self.isEnabledChange = false;
            ref:setEnabled(self.isEnabledChange);
            
            
            --清空玩家
            for i = 1, cmd.GAME_PLAYER do
                self:resetUser(i-1);
            end
            self.node_expression:stopAllActions();
            self.node_expression:removeAllChildren();
            
            self.btn_exchange:setVisible(false);
            self.btn_ready:setVisible(false);
            self._scene:onChangeDesk();
        else
            showToast(self, "换桌过于频繁，请稍后再试！", 1)
        end
		-- self._scene._gameFrame:QueryChangeDesk();

		-- --1秒只能点击一次换桌
		-- self.btn_exchange:runAction(cc.Sequence:create(cc.CallFunc:create(function()
		-- 	self.btn_exchange:setEnabled(false);
		-- 	-- self.setNode:setVisible(false)
		-- end), cc.DelayTime:create(1), 
		-- cc.CallFunc:create(function()
		-- 	self.btn_exchange:setEnabled(true);
		-- end)))
	end
end

--牌堆
function GameViewLayer:onCardPileLayer()
	local cardPileLayer = CardPileLayer:create(self);
	self:addChild(cardPileLayer,10);
end

function GameViewLayer:onMenuChangeDesk(ref)
	-- if self._scene.m_cbGameStatus == cmd.GAME_STATUS_FREE then
	-- 	self.btnChangeMenu:setVisible(not GlobalUserItem.bPrivateRoom)
	-- else
	-- 	self.btnChangeMenu:setVisible(false)
	-- end
	self:onChangeDesk(ref);
	self:onShowMenuPanel();
end

--开始
function GameViewLayer:onReadyEvent()
	-- self._scene:SendUserReady();
	self._scene:onStartGame(true);
end

--不切
function GameViewLayer:onBuqieEvent()
	self._scene:sendChangeBanker(false);
	self:onShowQieguoBtn(false);
end

--切锅
function GameViewLayer:onQieguoEvent()
	self._scene:sendChangeBanker(true);
	self:onShowQieguoBtn(false);
end

--抢庄
function GameViewLayer:onRobBankerEvent()
	self._scene:sendRobBanker(true);
	self:onShowRobBtn(false);
end

--不抢
function GameViewLayer:onBuqiangEvent()
	self._scene:sendRobBanker(false);
	self:onShowRobBtn(false);
end

--提示
function GameViewLayer:ontipEvent()
	local myChairId = self._scene:GetMeChairID();

	local myCardData = {0,0,0,0};

	for i=1,cmd.handCardNum do
		self:popMyCard(i, true);

		local cardValue = self.myCardData[i];
		myCardData[i] = cardValue;
	end

	--最优手牌
	local tabIndex = {};
	for j=1,cmd.handCardNum do
		local myAutoValue = self.cbAutoHandCard[j];
		for k=1,cmd.handCardNum do
			local myCardValue = myCardData[k];
			if myAutoValue == myCardValue then
				tabIndex[j] = k;
				myCardData[k] = 0;
				break;
			end
		end
	end

	self.cbSelectedCardData = {};
	self:popMyCard(tabIndex[1], false);
	self:popMyCard(tabIndex[2], false);

	self:showPoints(cmd.MY_VIEWID, self.myCardData,tabIndex, myChairId);

	if not self.bTrustee then
		self:showZupaiedBtn(true);
	end
end

--显示组牌按钮
function GameViewLayer:showZupaiedBtn(isShow)
	self.btn_zupaied:setVisible(isShow);

	if self._scene.m_bLookOn then
		self.btn_zupaied:setVisible(false);
	end
end

--取消
function GameViewLayer:onCancelEvent()
	local myChairId = self._scene:GetMeChairID();

	for i=1,cmd.handCardNum do
		self:popMyCard(i, true);
	end
	self:showPoints(cmd.MY_VIEWID, nil,nil, myChairId);
	self.btn_zupaied:setVisible(false);
	self.btn_cancel:setVisible(false);
	self.btn_tip:setVisible(true);
	if self._scene.m_bLookOn then
		self.btn_tip:setVisible(false);
	end
end

--组牌完成
function GameViewLayer:onZupaiedEvent()
	local cardData = self.myCardData;
	--小牌九
	if self.cardNumLimit <= 2 then
		
		self._scene:sendOpen({cardData[1],cardData[2]}, {0,0});
		self:showOperateButton(false);
		return;
	end

	--其他牌九
	local popCardNum = 0;
	local cardData1 = {};
	local cardData2 = {};
	for i=1,self.cardNumLimit do
		if self.cbSelectedCardData[i] and self.cbSelectedCardData[i]:getSelected() then
			popCardNum = popCardNum + 1;
			table.insert(cardData1, cardData[i]);
		else
			table.insert(cardData2, cardData[i]);
		end
	end
	if popCardNum ~= 2 then
		showToast(self,"请选择两张牌组合",2);
		return;
	end

	self.myCardData[1] = cardData1[1];
	self.myCardData[2] = cardData1[2];
	self.myCardData[3] = cardData2[1];
	self.myCardData[4] = cardData2[2];

	self._scene:sendOpen(cardData1, cardData2);

	self:showOperateButton(false);
end

--换庄按钮刷新
function GameViewLayer:onSetChangeEnable(isShow)
	
end

--开始按钮显示
function GameViewLayer:onStartBtnShow(isShow)
	self.btn_buqie:setVisible(false);
	self.btn_qieguo:setVisible(false);

	if isShow then
		if not self._scene.m_bLookOn then
			self.btn_ready:setVisible(isShow);
			self.btn_exchange:setVisible(isShow);
		end
	else
		self.btn_ready:setVisible(isShow);
		self.btn_exchange:setVisible(isShow);
	end
	
	if isShow then
		self:showWaitLayer(false);
		self:showWithAction(self.btn_ready);
		self:showWithAction(self.btn_exchange);
	end

	if GlobalUserItem.bPrivateRoom then
		self.btn_exchange:setVisible(false);
		self.btn_ready:setPositionX(667);
		self.btn_exchange_menu:setEnabled(false);

		self.btn_exchange_menu:setVisible(false);
		self.btn_help:setPositionY(463);
	end
end

--切锅按钮显示
function GameViewLayer:onShowQieguoBtn(isShow)
	local myChairId = self._scene:GetMeChairID();
	if self._scene.cbPlayStatus and  self._scene.cbPlayStatus[myChairId+1] ~= cmd.PS_SITTING then
		return;
	end

	if self.bTrustee and isShow then
		isShow = false;
	end

	if isShow then
		if not self._scene.m_bLookOn then
			self.btn_buqie:setVisible(isShow);
			self.btn_qieguo:setVisible(isShow);
		end
	else
		self.btn_buqie:setVisible(isShow);
		self.btn_qieguo:setVisible(isShow);
	end
end

--抢庄按钮显示
function GameViewLayer:onShowRobBtn(isShow)
	local myChairId = self._scene:GetMeChairID();
	if self._scene.cbPlayStatus and  self._scene.cbPlayStatus[myChairId+1] ~= cmd.PS_SITTING then
		return;
	end

	if self.bTrustee and isShow then
		isShow = false;
	end

	self.btn_rob_banker:setVisible(isShow);
	self.btn_buqiang:setVisible(isShow);
	if self._scene.m_bLookOn then
		self.btn_rob_banker:setVisible(false);
		self.btn_buqiang:setVisible(false);
	end
end

--设置庄家
function GameViewLayer:setBankerUser(bankerChairId,bRobBanker)
	self:onShowRobBtn(false);
	if bRobBanker then
		self:robAnimationBegin(bankerChairId);
	else
		self:showBanker(bankerChairId);
	end
end

--房主显示
function GameViewLayer:setRoomHolder(viewId)
	if not viewId or viewId == yl.INVALID_CHAIR then
		return
	end
	-- for i=1 , cmd.GAME_PLAYER do
	-- 	local flag = self.nodePlayer[i]:getChildByName("Room_owner")
	-- 	if viewid==i and GlobalUserItem.dwCurRoomGroupID == 0 then
	-- 		flag:setVisible(true)
	-- 	else
	-- 		flag:setVisible(false)
	-- 	end
	-- end
end

--抢庄动画
function GameViewLayer:robAnimationBegin(bankerChairId)
	local bankerViewId = self._scene:SwitchViewChairID(bankerChairId);
	self.robLayer:setVisible(true);
	self.robRect:setOpacity(0);

	local totalTime = 3;
	local tabDelay = {1,2,3,4,5,6,7};       --轮数越多越快
	local s = 0;
	for i=1,#tabDelay do
		s = s+tabDelay[i];
	end
	local tabPos={};
	local tabAct={};
	local tabViewId={};

	for i=1,cmd.GAME_PLAYER do
		local viewId = self._scene:SwitchViewChairID(i-1);
        --玩家
        local player_item = self.node_player[viewId]:getChildByName("player_item");
        if player_item:isVisible() and self.bRobBanker[i] then
        	local pos = playerPosArr[viewId];
        	table.insert(tabPos, pos);
        	table.insert(tabViewId, viewId);
        end
	end

	--都不抢庄
	if #tabViewId == 0 then
		for i=1,cmd.GAME_PLAYER do
			local viewId = self._scene:SwitchViewChairID(i-1);
	        if self._scene.cbPlayStatus[i] == cmd.PS_SITTING then
	        	local pos = playerPosArr[viewId];
	        	table.insert(tabPos, pos);
	        	table.insert(tabViewId, viewId);
	        end
		end
	end

	local unitDelay=totalTime/(s*#tabPos);

	for i=1,#tabDelay do
		for j=1,#tabPos do --人数
			local act1=cc.CallFunc:create(function()
				self.robRect:setOpacity(255);
				self.robRect:setPosition(tabPos[j]);
			end)
			local act2=cc.DelayTime:create(tabDelay[i]*unitDelay);
			table.insert(tabAct,act1);
			table.insert(tabAct,act2);
			if i==#tabDelay and tabViewId[j] == bankerViewId then
				table.insert(tabAct,cc.DelayTime:create(0.1));
				break;
			end
		end
	end
	
	local act3 = cc.CallFunc:create(function() 
		self:robAnimationEnd(bankerChairId, true);
	end);

	table.insert(tabAct,act3);
    if #tabViewId>1 then
        self.robRect:runAction(cc.Sequence:create(tabAct));
    else
        self.robRect:setOpacity(0);
        self.robRect:runAction(cc.Sequence:create(cc.FadeTo:create(0.01,1),cc.CallFunc:create(function() 
        	self:robAnimationEnd(bankerChairId, false);
        end)));
    end
end

function GameViewLayer:robAnimationEnd(bankerChairId, isPlayRobAnim)
	
	local myChairId = self._scene:GetMeChairID();
	local myViewId = self._scene:SwitchViewChairID(myChairId);
	local bankerViewId = self._scene:SwitchViewChairID(bankerChairId);
    local movePos = playerPosArr[bankerViewId];
 	
    --庄家动画
    self:PlayBankerAnim();
    self.robSpine:setPosition(cc.p(667,375));
    self.robSpine:setScale(1.0);
    self.robSpine:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.8),
        cc.Spawn:create(
            cc.ScaleTo:create(0.2,0.5),
            cc.MoveTo:create(0.2, movePos)
        ),
        cc.CallFunc:create(function()
        	self.robLayer:setVisible(false);
            self:showBanker(bankerChairId);

            self:onStopAllHeadAnim();
        end),
        cc.DelayTime:create(0.5),
        cc.CallFunc:create(function()
        	--下注
			for i = 1, cmd.GAME_PLAYER do
				if self._scene.cbPlayStatus[i] == cmd.PS_SITTING then
					self:showPlayerTip(i-1, cmd.actChip);

					--倒计时
					if isPlayRobAnim then
						self:onStartHeadAnim(i-1, self.cbTimeChip, self.cbTimeChip);
						self._scene:SetGameClock(self._scene:GetMeChairID(),cmd.IDI_SETCHIP,self.cbTimeChip);
					else
						self:onStartHeadAnim(i-1, self.cbTimeChip+5, self.cbTimeChip+5);
						self._scene:SetGameClock(self._scene:GetMeChairID(),cmd.IDI_SETCHIP,self.cbTimeChip+5);
					end
					
				end
			end
			if self._scene.wBankerUser ~= myChairId and myViewId == cmd.MY_VIEWID then
				self:showChipBtns(true);
			end
			-- self.robLayer:setVisible(false);
        end)
    ));
end

--换庄
function GameViewLayer:exchangeBankerAnimation(bankerChairId)
	local bankerViewId = self._scene:SwitchViewChairID(bankerChairId);
	local movePos = playerPosArr[bankerViewId];

	self.robLayer:setVisible(true);
	self.robRect:setOpacity(255);
	--庄家动画
    self:PlayBankerAnim();
    self.robSpine:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.5),
        cc.Spawn:create(
            cc.ScaleTo:create(0.2, 0.5),
            cc.MoveTo:create(0.2, movePos)
        ),
        cc.CallFunc:create(function()
            self.robLayer:setVisible(false);
            self:showBanker(bankerChairId);

            self:onStopAllHeadAnim();
        end)
    ));
end

--色子动画
function GameViewLayer:diceAnimation(dice)
	dump(dice,"-----色子动画-----",6)
	self.diceLayer:setVisible(true);

	local icon_dice_1 = self.diceLayer:getChildByName("dice_1");
	local icon_dice_2 = self.diceLayer:getChildByName("dice_2");
	icon_dice_1:setVisible(false);
	icon_dice_2:setVisible(false);

	local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("sb_s_" .. dice[1] .. ".png");
	local frame2 = cc.SpriteFrameCache:getInstance():getSpriteFrame("sb_s_" .. dice[2] .. ".png");
	icon_dice_1:setSpriteFrame(frame1);
	icon_dice_2:setSpriteFrame(frame2);

	self:PlayDiceAnim();
	
    self.diceSpine:stopAllActions();
    self.diceSpine:runAction(cc.Sequence:create(
        cc.DelayTime:create(2.266),
        cc.CallFunc:create(function()
            icon_dice_2:setVisible(true);
        end),
        cc.DelayTime:create(2.334-2.266),
        cc.CallFunc:create(function()
            icon_dice_1:setVisible(true);
        end),
        cc.DelayTime:create(0.5),
        cc.CallFunc:create(function()
        	self.diceLayer:setVisible(false);
        end)
    ));

    ExternalFun.playSoundEffect(cmd.DRAW_SICE);
end

--显示房主
function GameViewLayer:showFangzhuIcon(viewId)
    for i = 1, cmd.GAME_PLAYER do
        local player = self.node_player[i];
        --玩家
        local player_item = player:getChildByName("player_item");
        local icon_house_owner = player_item:getChildByName("icon_house_owner");
        icon_house_owner:setVisible(false);
        if i == viewId and PriRoom and GlobalUserItem.bPrivateRoom then
        	icon_house_owner:setVisible(true);
        end
    end
end

--显示庄家
function GameViewLayer:showBanker(wChairID)
	local  viewId = self._scene:SwitchViewChairID(wChairID); 
	local player_item = self.node_player[viewId]:getChildByName("player_item");

    local icon_banker = player_item:getChildByName("icon_banker");
    icon_banker:setVisible(true);
end

--庄家隐藏
function GameViewLayer:hideAllBanker()
	for i = 1, cmd.GAME_PLAYER do 
		local player_item = self.node_player[i]:getChildByName("player_item");

	    local icon_banker = player_item:getChildByName("icon_banker");
	    icon_banker:setVisible(false);
	end
end

--玩家提示
function GameViewLayer:showPlayerTip(wChairID, kind)
	local  viewId = self._scene:SwitchViewChairID(wChairID);
	local  bankerPos = self._scene:SwitchViewChairID(self._scene.wBankerUser);
	local node_tip = self.node_player[viewId]:getChildByName("node_tip");
	node_tip:setVisible(true);

    local tip = node_tip:getChildByName("tip");
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("tip_in_rob_banker.png");
    if kind == cmd.actRob then
    	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("tip_in_rob_banker.png");
    elseif kind == cmd.actChip then
    	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("tip_in_bet.png");
    	if bankerPos == viewId then
    		node_tip:setVisible(false);
    	end
    elseif kind == cmd.actCombine then
    	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("tip_in_zupai.png");
    end
    tip:setSpriteFrame(frame);
end
--隐藏玩家提示
function GameViewLayer:hidePlayerTip(wChairID)
	local  viewId = self._scene:SwitchViewChairID(wChairID);

	local node_tip = self.node_player[viewId]:getChildByName("node_tip");
	node_tip:setVisible(false);
end

--显示准备
function GameViewLayer:showReady(wChairID, isShow)
    local  pos = self._scene:SwitchViewChairID(wChairID);

    local icon_ready = self.node_player[pos]:getChildByName("icon_ready");
	icon_ready:setVisible(isShow);

	--隐藏倒计时
	if isShow then
		self:onStopHeadAnim(wChairID);
	end

    if isShow and pos == cmd.MY_VIEWID then
        self:onStartBtnShow(false);
    end
end

--隐藏准备
function GameViewLayer:hideAllReady()
    for i = 1, cmd.GAME_PLAYER do
        local icon_ready = self.node_player[i]:getChildByName("icon_ready");
        icon_ready:setVisible(false);
    end
end

--抢庄操作提示
function GameViewLayer:showOperateTip(wChairID, bRob)
	local  viewId = self._scene:SwitchViewChairID(wChairID);

	local icon_operate_tip = self.node_player[viewId]:getChildByName("icon_operate_tip");
	
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_rob.png");
	if bRob then
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_rob.png");
	else
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_give_up.png");
	end
	icon_operate_tip:setSpriteFrame(frame);

	self:showWithAction(icon_operate_tip);
end

--隐藏抢庄操作提示
function GameViewLayer:hideAllOperateTip()
	for i = 1, cmd.GAME_PLAYER do
		local icon_operate_tip = self.node_player[i]:getChildByName("icon_operate_tip");
		icon_operate_tip:setVisible(false);
	end
end

--下注显示
function GameViewLayer:showUserChip(wChairID, chipData)
	local  viewId = self._scene:SwitchViewChairID(wChairID);

	if chipData[1] == 0 and chipData[2] == 0 and chipData[3] == 0 then
		return;
	end
	local node_bet = self.node_player[viewId]:getChildByName("node_bet");
	node_bet:setVisible(true);
	
	local bet_num_1 = node_bet:getChildByName("bet_num_1");
	local bet_num_2 = node_bet:getChildByName("bet_num_2");
	local bet_num_3 = node_bet:getChildByName("bet_num_3");
	local bottom_bet = node_bet:getChildByName("bottom_bet");
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("bottom_bet_1.png");
	if self.cbBetDaoShu == 2 then
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("bottom_bet_1.png");
		bet_num_1:setVisible(true);
		bet_num_2:setVisible(true);
		bet_num_3:setVisible(false);
	else
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("bottom_bet.png");
		bet_num_1:setVisible(true);
		bet_num_2:setVisible(true);
		bet_num_3:setVisible(true);
	end
	bottom_bet:setSpriteFrame(frame);

	bet_num_1:setString(chipData[1]);
	bet_num_2:setString(chipData[2]);
	bet_num_3:setString(chipData[3]);
end

--组牌完成
function GameViewLayer:showZupaiFinished(wChairID, bShow)
	local  viewId = self._scene:SwitchViewChairID(wChairID);
	if viewId == cmd.MY_VIEWID then
		return;
	end

	local icon_zupaied = self.node_player[viewId]:getChildByName("icon_zupaied");
	
	if bShow then
		icon_zupaied:setVisible(true);
		self:showWithAction(icon_zupaied);
	else
		icon_zupaied:setVisible(false);
	end
end

--输赢结果
function GameViewLayer:showEndResult(wChairID, score)
	local  viewId = self._scene:SwitchViewChairID(wChairID);
	local icon_end = self.node_player[viewId]:getChildByName("icon_end");
	local icon_end_result = icon_end:getChildByName("icon_end_result");
	icon_end:setVisible(true);

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_lose.png");
	local soundPath = cmd.GAME_END;
	if score < 0 then
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_lose.png");
		soundPath = cmd.GAME_LOST;
	elseif score == 0 then
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_ping.png");
		soundPath = cmd.GAME_END;
	elseif score > 0 then
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("effect_win.png");
		soundPath = cmd.GAME_WIN;
	end
	icon_end_result:setSpriteFrame(frame);

	if viewId == cmd.MY_VIEWID then
		ExternalFun.playSoundEffect(soundPath);
	end
	self:showWithAction(icon_end_result);
end

--创建计时器
function GameViewLayer:createHeadAnim()
	for i=1, cmd.GAME_PLAYER do
		self.m_TimeProgress[i] = cc.ProgressTimer:create(display.newSprite("game/bottom_cut_time.png"));
        self.m_TimeProgress[i]:setReverseDirection(true);
        -- self.m_TimeProgress[i]:setPosition(cc.p(playerPosArr[i].x, playerPosArr[i].y-2));
        self.m_TimeProgress[i]:setPosition(cc.p(0, 0));
        self.m_TimeProgress[i]:setVisible(false);
        self.m_TimeProgress[i]:setPercentage(100);
        self.node_hand_anim[i]:addChild(self.m_TimeProgress[i]);
	end
end

--开始计时器
function GameViewLayer:onStartHeadAnim(wChairID, timeRemain, timeTotal)
	local  viewId = self._scene:SwitchViewChairID(wChairID);
	--庄家
	if wChairID == self._scene.wBankerUser then
		return;
	end
	local percentage = 100 * (timeRemain/timeTotal);
	local progress = self.m_TimeProgress[viewId];
	progress:setPercentage(percentage);
    progress:setVisible(true);
    progress:stopAllActions();
    progress:runAction(cc.Sequence:create(cc.ProgressTo:create(timeRemain, 0), cc.CallFunc:create(function()
        progress:setVisible(false);
    end)));
end

-- 显示玩家飘分
function GameViewLayer:showPlayerWinScore(wChairID, score)
    local  viewId = self._scene:SwitchViewChairID(wChairID);

    local end_score = self.node_player[viewId]:getChildByName("end_score");
	local bg_sub = end_score:getChildByName("bg_sub");
	local score_sub = end_score:getChildByName("score_sub");
	local bg_add = end_score:getChildByName("bg_add");
	local score_add = end_score:getChildByName("score_add");

	if score >= 0 then
		bg_sub:setVisible(false);
		score_sub:setVisible(false);
		bg_add:setVisible(true);
		score_add:setVisible(true);
		score_add:setString("/" .. score);
	else
		bg_sub:setVisible(true);
		score_sub:setVisible(true);
		bg_add:setVisible(false);
		score_add:setVisible(false);
		score_sub:setString("/" .. score);
	end

	--飘分
	local moveToPos = playerPosArr[viewId];
	end_score:setVisible(true);
	end_score:setOpacity(255);
	end_score:stopAllActions();
	end_score:setPosition(cc.p(moveToPos));
	
	end_score:runAction(cc.Sequence:create(
		cc.MoveTo:create(1.0,cc.p(moveToPos.x, moveToPos.y + 80)),
		cc.DelayTime:create(1.0),
		cc.FadeTo:create(0.5, 0),
		cc.CallFunc:create( function()
	        end_score:setVisible(false);
	    end)
	));
end

--停止计时器
function GameViewLayer:onStopHeadAnim(wChairID)
	local  viewId = self._scene:SwitchViewChairID(wChairID);

 	local progress = self.m_TimeProgress[viewId];
	progress:setPercentage(100);
    progress:setVisible(false);
    progress:stopAllActions();
end

--停止计时器
function GameViewLayer:onStopAllHeadAnim()
	for i=1, cmd.GAME_PLAYER do
		local  viewId = self._scene:SwitchViewChairID(i-1);

	 	local progress = self.m_TimeProgress[viewId];
		progress:setPercentage(100);
	    progress:setVisible(false);
	    progress:stopAllActions();
	end
end

--按钮效果
function GameViewLayer:showWithAction(sprite)
	sprite:setVisible(true);
	sprite:setScale(0.0001);
	sprite:runAction(cc.ScaleTo:create(0.1, 1.0));
end

--牌型动画
function GameViewLayer:showCardTypeAction(sprite)
	sprite:setVisible(true);
	sprite:setScale(0.0001);
	sprite:setOpacity(0);

	sprite:runAction(cc.Sequence:create(cc.Spawn:create(cc.ScaleTo:create(0.2, 1.5),cc.FadeTo:create(0.2,255)), cc.ScaleTo:create(0.1, 1.0)));
end

--发牌
function GameViewLayer:sendCards(startChairId)
	--发牌动画
	local startViewId = self._scene:SwitchViewChairID(startChairId);
    local curViewId = startViewId;
    if curViewId >= cmd.GAME_PLAYER then
        curViewId = 1;
    end
    local cbPlayStatusViewId = {0,0,0,0};
    for i=1,cmd.GAME_PLAYER do
    	local viewId = self._scene:SwitchViewChairID(i-1);
    	if self._scene.cbPlayStatus[i] == cmd.PS_SITTING then
    		cbPlayStatusViewId[viewId] = cmd.PS_SITTING;
    	end
    end
    
	local delaytime = 0.2;
    local playerNum = 0;
    local maxDelayTime = 0;
    local cardScale = 0;
    for i=1,cmd.GAME_PLAYER do
    	-- local viewId = self._scene:SwitchViewChairID(i-1);
        if cbPlayStatusViewId[curViewId] == cmd.PS_SITTING then
        	if curViewId == cmd.MY_VIEWID then
	            cardScale = 1.0;
	        else
	            cardScale = 0.85;
	        end
			playerNum = playerNum + 1;
			-- local index = (playerNum-1);
            for j=1, self.cardNumLimit do
            	local CardNode = CardNode:create();
            	CardNode:setPosition(cc.p(667, 375));
            	CardNode:showCardBack(true);
				self.node_card_send:addChild(CardNode);
				
                local index = (playerNum-1)*self.cardNumLimit + j - 1;
                maxDelayTime = delaytime*index;
                
                local movePos = cc.p(handCardPos[curViewId][j].x, handCardPos[curViewId][j].y);
                CardNode:stopAllActions();
                CardNode:runAction(cc.Sequence:create(
                    cc.DelayTime:create(delaytime*index), 
                    cc.Spawn:create(cc.ScaleTo:create(0.1, cardScale), cc.MoveTo:create(0.1, movePos)),
                    cc.CallFunc:create(function()
                        --发牌音效
                        ExternalFun.playSoundEffect(cmd.SEND_CARD);
                    end
                )));
            end
        end
        
        if curViewId >= cmd.GAME_PLAYER then
            curViewId = 1;
        else
            curViewId = curViewId + 1;
        end
    end

    --创建手牌
    self:runAction(cc.Sequence:create(
        cc.DelayTime:create(maxDelayTime + 0.6), 
        cc.CallFunc:create(function()
            self.node_card_send:removeAllChildren();
            for i=1,cmd.GAME_PLAYER do
            	local viewId = self._scene:SwitchViewChairID(i-1);
            	if self._scene.cbPlayStatus[i] == cmd.PS_SITTING then
            		-- self.CardLayer:createHandCard(viewId);
            		
            		self:createHandCard(viewId, self.cardNumLimit);
            		self:showPlayerTip(i-1, cmd.actCombine);
	            	--倒计时
					self:onStartHeadAnim(i-1, self.cbTimeOpenCard, self.cbTimeOpenCard);
					self._scene:SetGameClock(self._scene:GetMeChairID(),cmd.IDI_SETCHIP,self.cbTimeOpenCard);
					--组牌按钮
					if viewId == cmd.MY_VIEWID then
						self:showOperateButton(true);
					end
            	end
            end
        end
    )));
end

--创建手牌
function GameViewLayer:createHandCard(viewId, cardNumLimit)
	self.CardLayer:createHandCard(viewId, cardNumLimit);
end

--组牌完成
function GameViewLayer:showZupaiResult(viewId)
	self.CardLayer:showZupaiResult(viewId);
	ExternalFun.playSoundEffect(cmd.KOU_CARD);
end

--显示手牌、翻牌
function GameViewLayer:showHandCard(viewId, cbHandCard)
	self.CardLayer:showHandCard(viewId, cbHandCard);
end

--翻牌
function GameViewLayer:cardFanPaiAnimation(cbOpenCard)
	local bankerViewId = self._scene:SwitchViewChairID(self._scene.wBankerUser);
    local curViewId = bankerViewId; --庄家
    if bankerViewId >= cmd.GAME_PLAYER then
        curViewId = 1;
    end
    local delaytime = 0.8;
    local playerNum = 0;
	local maxDelayTime = 0;
    local cbPlayStatusViewId = {0,0,0,0};
    local cardOpenData = {{},{},{},{}};
    for i=1,cmd.GAME_PLAYER do
    	local viewId = self._scene:SwitchViewChairID(i-1);
    	if self._scene.cbPlayStatus[i] == cmd.PS_SITTING then
    		cbPlayStatusViewId[viewId] = cmd.PS_SITTING;
    		cardOpenData[viewId] = cbOpenCard[i];
    	end
    end
    -- 
    for i=1,cmd.GAME_PLAYER do
    	local chairId = self._scene:getChairIDByPosition(curViewId);
        if cbPlayStatusViewId[curViewId] == cmd.PS_SITTING then
			playerNum = playerNum + 1;
			local index = (playerNum-1);
            maxDelayTime = delaytime*index;
            local cardData = cardOpenData[curViewId];
            local viewId = curViewId;
            if cardData[1] > 0 and cardData[2] > 0 then
            	self:runAction(cc.Sequence:create(
	                cc.DelayTime:create(delaytime*index), 
	                cc.CallFunc:create(function()
	                	self:showHandCard(viewId, cardData);
	                	self:showPoints(viewId, cardData, {1,2,3,4}, chairId);
	                end
	            )));
            end
            
        end
        if curViewId >= cmd.GAME_PLAYER then
            curViewId = 1;
        else
            curViewId = curViewId + 1;
        end
    end
end

--卡牌弹出
function GameViewLayer:popMyCard(index, bDown)
	if self.bTrustee then
		if self.bZupaiFinished then
			return;
		end
	end

	local node_hand = self.CardLayer:getHandCard();
	local CardNode = node_hand[cmd.MY_VIEWID][index];
	if CardNode == nil then
		return;
	end

	if bDown then
		CardNode:setSelected(false, true);
		self.cbSelectedCardData[index] = nil;
	else
		CardNode:setSelected(true, true);
		self.cbSelectedCardData[index] = CardNode;
	end

	--是否可组牌
	self:onIsCanZuPai();
end

--是否可组牌
function GameViewLayer:onIsCanZuPai()
	local node_hand = self.CardLayer:getHandCard();
	local myHand = node_hand[cmd.MY_VIEWID];
	if not myHand or #myHand <= 0 then
		return;
	end

	local myChairId = self._scene:GetMeChairID();
	local tabIndex = {};
	if self.cardNumLimit == 4 and self.cbSelectedCardData then
		local pn = 0;
		local pn2 = 0;
		for i = 1, self.cardNumLimit do
			if self.cbSelectedCardData[i] and self.cbSelectedCardData[i]:getSelected() then
				pn = pn + 1;
				tabIndex[pn] = i;
			else
				pn2 = pn2 + 1;
				tabIndex[pn2+2] = i;
			end
		end
		self:showZupaiedBtn(pn == 2);
		if pn == 2 then
			self:showPoints(cmd.MY_VIEWID, self.myCardData, tabIndex, myChairId)
		else
			self:showPoints(cmd.MY_VIEWID, nil, nil, myChairId)
		end
	end

	ExternalFun.playSoundEffect(cmd.XUAN_CARD);
end

--点数
function GameViewLayer:showPoints(index, myCardData, tabIndex, wChairID)
	local card_type = self.card_type[index];
	card_type:setVisible(false);
	if myCardData == nil or myCardData[1] == 0 then
		return;
	end

	card_type:setVisible(true);
	--选中牌型显示第一位
	local cardTwo1 = {myCardData[tabIndex[1]], myCardData[tabIndex[2]]};
	local cardTwo2 = {myCardData[tabIndex[3]], myCardData[tabIndex[4]]};

	-- if tabIndex[1] > 1 then
	-- 	self:cardTypeFun(cardTwo1, 2, index, wChairID);
	-- 	if self.cardNumLimit == 4 then
	-- 		self:cardTypeFun(cardTwo2, 1, index, wChairID);
	-- 	end
	-- else
		self:cardTypeFun(cardTwo1, 1, index, wChairID);
		if self.cardNumLimit == 4 then
			self:cardTypeFun(cardTwo2, 2, index, wChairID);
		end
	-- end
end

function GameViewLayer:cardTypeFun(cardTwo, typeIndex, index, wChairID)
	local bCardType = GameLogic.GetCardType(cardTwo, 2, self);
	self:showCardType(bCardType, index, typeIndex, cardTwo, wChairID);
end

--牌型显示
function GameViewLayer:showCardType(bCardType, index, typeIndex, cardTwo, wChairID)
	local card_type = self.card_type[index];
	card_type:setVisible(true);

	local card_type_bg_0 = card_type:getChildByName("card_type_bg_0");
	local card_type_0_0 = card_type_bg_0:getChildByName("card_type_0");
	local card_type_0_1 = card_type_bg_0:getChildByName("card_type_1");
	local card_type_bg_1 = card_type:getChildByName("card_type_bg_1");
	local card_type_1_0 = card_type_bg_1:getChildByName("card_type_0");
	local card_type_1_1 = card_type_bg_1:getChildByName("card_type_1");
	card_type_bg_0:setScale(1.0);
	card_type_bg_1:setScale(1.0);

	card_type_bg_1:setVisible(true);
	if self.cardNumLimit == 2 then
		card_type_bg_1:setVisible(false);
	end

	local bCardPoint = -1;
	local cardTypeStr = "";
	local spStr1 = "";
	local spStr2 = "";
	local sanpaiType = -1;

	local userItem = self._scene._gameFrame:getTableUserItem(self._scene:GetMeTableID(), wChairID);
	local sex = 1;
	if userItem then
		sex = userItem.cbGender;
	end

	local delayTime = 0.4*(typeIndex-1);
	if bCardType == GameLogic.CT_SANPAI then
		bCardPoint = GameLogic.GetCardPoint(cardTwo, 2);
		sanpaiType = GameLogic.GetSanpaiType(cardTwo, 2, bCardPoint);
		spStr1 = "sanpai_" .. sanpaiType;
		spStr2 = "point_" .. bCardPoint;
		if self.bGameEnd then
			self:runAction(cc.Sequence:create(
		        cc.DelayTime:create(delayTime),
		        cc.CallFunc:create( function()
		        	local soundStr = "sanpai_type_" .. sanpaiType .. "_" .. bCardPoint .. ".mp3";
		            if sex == 1 then
		            	ExternalFun.playSoundEffect(cmd.MAN .. soundStr);
		            else
		            	ExternalFun.playSoundEffect(cmd.WOMAN .. soundStr);
		            end
		        end)
		    ));
		end
	else
		cardTypeStr = "card_type_" .. bCardType;
		if self.bGameEnd then
			self:runAction(cc.Sequence:create(
		        cc.DelayTime:create(delayTime),
		        cc.CallFunc:create( function()
		            if sex == 1 then
		            	ExternalFun.playSoundEffect(cmd.MAN .. cardTypeStr .. ".mp3");
		            else
		            	ExternalFun.playSoundEffect(cmd.WOMAN .. cardTypeStr .. ".mp3");
		            end
		        end)
		    ));
		end
	end

	local card_type_bg = card_type_bg_0;
	local card_type_0 = card_type_0_0;
	local card_type_1 = card_type_0_1;
	if typeIndex == 1 then
		card_type_bg = card_type_bg_0;
		card_type_0 = card_type_0_0;
		card_type_1 = card_type_0_1;
	elseif typeIndex == 2 then
		card_type_bg = card_type_bg_1;
		card_type_0 = card_type_1_0;
		card_type_1 = card_type_1_1;
	end

	local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame(spStr1 .. ".png");
	local frame2 = cc.SpriteFrameCache:getInstance():getSpriteFrame(spStr2 .. ".png");
	local frame3 = cc.SpriteFrameCache:getInstance():getSpriteFrame(cardTypeStr .. ".png");

	if bCardType == GameLogic.CT_SANPAI then
		card_type_0:setVisible(true);
		card_type_1:setVisible(true);
		if frame1 then
			card_type_0:setSpriteFrame(frame1);
		end
		if frame2 then
			card_type_1:setSpriteFrame(frame2);
		end
		card_type_0:setPosition(cc.p(48, 24));
		card_type_1:setPosition(cc.p(88, 24));
	else
		card_type_0:setVisible(true);
		card_type_1:setVisible(false);
		if frame3 then
			card_type_0:setSpriteFrame(frame3);
			card_type_1:setSpriteFrame(frame3);
		end
		card_type_0:setPosition(cc.p(68, 24));
		card_type_1:setPosition(cc.p(68, 24));
	end

	if index == cmd.MY_VIEWID then
		return;
	end

	self:showCardTypeAction(card_type_bg);
	-- self:showCardTypeAction(card_type_0);
	-- self:showCardTypeAction(card_type_1);
end

--操作按钮显示
function GameViewLayer:showOperateButton(isShow)
	self:showZupaiedBtn(isShow);
	if isShow and self.cardNumLimit > 2 then
		self.btn_zupaied:setVisible(false);
	end

	self.btn_tip:setVisible(isShow and self.cardNumLimit > 2);
	if self._scene.m_bLookOn then
		self.btn_tip:setVisible(false);
	end
	self.btn_cancel:setVisible(false);
end

--0没动作 1放弃 2抢
function GameViewLayer:showRobResult(wChairID,bRobBanker)
	if wChairID == nil then
		self:hideAllOperateTip();
		return;
	end
	self:showOperateTip(wChairID, bRobBanker);
end

--下注
function GameViewLayer:showChipBtns(isShow)
	local myChairId = self._scene:GetMeChairID();
	local myViewId = self._scene:SwitchViewChairID(myChairId);

	if self._scene.cbPlayStatus[myChairId + 1] ~= cmd.PS_SITTING then
		return;
	end

	self.node_operate_bet:setVisible(isShow);
	if self._scene.m_bLookOn then
		self.node_operate_bet:setVisible(false);
	end
	self.OperateBetNode:resertData();

	--刷新下注
	if self.node_operate_bet:isVisible() then
		self.OperateBetNode:freshUi();
	end
end

--隐藏按钮
function GameViewLayer:hideAllButtons(isShow)
	self.btn_ready:setVisible(false);
	self.btn_rob_banker:setVisible(false);
	self.btn_buqiang:setVisible(false);
	self.btn_zupaied:setVisible(false);
	self.btn_tip:setVisible(false);
	self.btn_cancel:setVisible(false);
	self.btn_buqie:setVisible(false);
	self.btn_qieguo:setVisible(false);
	self.btn_exchange:setVisible(false);

	self.node_operate_bet:setVisible(false);
	self.OperateBetNode:resertData();
end

--飞金币
function GameViewLayer:playCoinFlyAnimation(dGameScore)
	for i = 1, cmd.GAME_PLAYER do
		local viewId = self._scene:SwitchViewChairID(i-1);
		if self._scene.cbPlayStatus[i] == cmd.PS_SITTING then
			if self._scene.wBankerUser ~= yl.INVALID_CHAIR and i-1 ~= self._scene.wBankerUser and dGameScore[i] ~= 0 then
				local viewIdBanker = self._scene:SwitchViewChairID(self._scene.wBankerUser);
				local posA = playerPosArr[viewId];
				local posB = playerPosArr[viewIdBanker];
				if dGameScore[i] > 0 then
					posA = playerPosArr[viewIdBanker];
					posB = playerPosArr[viewId];
				end
				self:coinFlyAnimation(posA,posB);
			end
		end
	end
end

--飞金币
function GameViewLayer:coinFlyAnimation(posA,posB)
	local speed = 2239.516;
	local n = 50;
	local a = 10;
	local b = 15;
	for i=1,n do
		local adjPosA=cc.p(posA.x+math.random(-a,a),posA.y+math.random(-a,a));
		local adjPosB=cc.p(posB.x+math.random(-b,b),posB.y+math.random(-b,b));
		local dis=cc.pGetDistance(adjPosA,adjPosB);
		local tabAct={};
		local t=math.floor(i/3);
		table.insert(tabAct,cc.DelayTime:create(4*(t/n)));
		table.insert(tabAct,cc.FadeTo:create(0.1,255));
		local mov=cc.MoveTo:create(dis/speed,adjPosB)
		table.insert(tabAct, mov);
		table.insert(tabAct,cc.DelayTime:create(2*(1-t/n)));
		table.insert(tabAct,cc.RemoveSelf:create());

		local sp=cc.Sprite:create("game/flycoin.png"):addTo(self,Zorder_Flycoin):setPosition(adjPosA):setOpacity(0);
		sp:runAction(cc.Sequence:create(tabAct));
		sp:setTag(tag_flycoin);
	end
end

--获得剩余牌
function GameViewLayer:getCardHeapArr(nUsedCardNum)
	local num = 0;
	for i=1,32 do
		-- local isUse = (nUsedCardNum&(0x01<<i)) ~= 0;
		local numLeft = bit:_lshift(0x01,i-1);
		if numLeft == 0 then
			numLeft = 1;
		end
		local isUse =  bit:_and(numLeft,nUsedCardNum) ~= 0;
		if isUse then
			num = num + 1;
			local cardData = GameLogic.m_bCardListData[i];
			self.cardHeapArr[i] = cardData;
		else
			self.cardHeapArr[i] = 0;
		end
	end
end

--重置牌堆
function GameViewLayer:resertCardHeapArr()
	local cardListData = {};
	for i=1,32 do
		cardListData[i] = GameLogic.m_bCardListData[i];
	end
	self.cardHeapArr = cardListData;
end

--锅底分
function GameViewLayer:showdGuoDiScore(score)
	local bet_num = self.node_guo:getChildByName("bet_num");
	bet_num:setString(score);

	self.node_guo:setVisible(self.bJiaGuaPaiJiu);
end

function GameViewLayer:removeExitedUsers() --游戏开始后离开的玩家
	for viewId=1,cmd.GAME_PLAYER do
		if self.tabUserItem[viewId] then
			if viewId~=cmd.MY_VIEWID and self.tabUserItem[viewId].bExit==true  then
				self.tabHeadSp[viewId]:removeSelf()
				self:removeActionSps(viewId)
				self.tabHeadSp[viewId] = nil  
				self.tabUserItem[viewId] = nil
			end
		end
	end
end

function GameViewLayer:updateClockView(time,viewid)
	-- self.clock:setVisible(false)
	-- self.clockTime:setVisible(false)
	-- if true then return end

	-- --------------------------------
	-- if viewid==yl.INVALID_CHAIR then
	-- 	self.clock:setVisible(false)
	-- else
	-- 	self.clock:setVisible(true)
	-- 	if time and time<10 then
	-- 		time="0"..time
	-- 	end
	-- 	print("time:",time)
	-- 	self.clockTime:setString(time or "00")
	-- end
end

--庄家动画
function GameViewLayer:PlayBankerAnim()
    local spin = sp.SkeletonAnimation:create("spine/banker/skeleton.json","spine/banker/skeleton.atlas",1);
    spin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    spin:setPosition(cc.p(0, 0));
    self.robSpine:addChild(spin);
end

--色子动画
function GameViewLayer:PlayDiceAnim()
	self.diceSpine:removeAllChildren();

    local spin = sp.SkeletonAnimation:create("spine/dice/skeleton.json","spine/dice/skeleton.atlas",1);
    spin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    spin:setPosition(cc.p(0, 0));
    self.diceSpine:addChild(spin);
end

--显示聊天
function GameViewLayer:onUserChat(viewid ,message)
    if message and #message > 0 then
        if viewid==cmd.MY_VIEWID then
            self.m_GameChat:showGameChat(false)
        end 
       
        self.m_userChat[viewid]:stopAllActions()
        self.m_userChat[viewid]:removeAllChildren()
        self.m_userChat[viewid]:setVisible(true)

        local chatTxt = cc.LabelTTF:create(message, "fonts/round_body.ttf", 20, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER)
        chatTxt:addTo(self.m_userChat[viewid])
        chatTxt:setColor(cc.c3b(24,24,24))

        local labSize = chatTxt:getContentSize()
        self.m_userChat[viewid]:setContentSize(labSize.width+30, labSize.height + 30)

        chatTxt:setPosition(cc.p(self.m_userChat[viewid]:getContentSize().width/2+0 , self.m_userChat[viewid]:getContentSize().height/2 +6))
        -- if viewid>=4 then
        --     chatTxt:setPositionX(self.m_userChat[viewid]:getContentSize().width/2-3)
        -- end

        --动作
        self.m_userChat[viewid]:runAction(cc.Sequence:create(
            cc.DelayTime:create(3),
            cc.CallFunc:create(function()
                self.m_userChat[viewid]:removeAllChildren()
                self.m_userChat[viewid]:setVisible(false)
            end)
        ))
    end
end

--显示表情
function GameViewLayer:onUserExpression(viewid,index)
    if viewid==cmd.MY_VIEWID then
        self.m_GameChat:showGameChat(false)
    end
    --取消上次
    self.m_userChat[viewid]:stopAllActions()
    self.m_userChat[viewid]:removeAllChildren()
    self.m_userChat[viewid]:setVisible(false)

    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", index))
    --缓存表情
    --加载动画
    local frames = {}
    local actionTime = 0.2
    for i=1,32 do
        local strName = string.format("emoj%d_%d.png",index, i)
        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
        if spriteFrame then
            table.insert(frames, spriteFrame)
        else
            break
        end
    end

    local  animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", index))
    local expression = cc.Sprite:createWithSpriteFrame(spriteFrame)
    self.node_expression:addChild(expression)
    expression:setPosition(playerPosArr[viewid])
    expression:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.node_expression:runAction(cc.Sequence:create(cc.DelayTime:create(3.0) ,cc.CallFunc:create(function()
       expression:removeFromParent();
    end)))
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
    --取消文字，表情
    self.m_userChat[viewid]:stopAllActions()
    self.m_userChat[viewid]:removeAllChildren()
    self.m_userChat[viewid]:setVisible(true)
    self.m_userChat[viewid]:setContentSize(199, 73)
    if isPlay == false then
        self.m_userChat[viewid]:setVisible(false)
    else
        --创建帧动画
        -- 聊天表情
        local sp = display.newSprite("game/icon_voice.png")
        sp:setAnchorPoint(cc.p(0.5, 0.5))
        sp:runAction(self.m_actVoiceAni)
        sp:setScaleX(-1)
        sp:addTo(self.m_userChat[viewid])
        sp:setPosition(cc.p(100, 36))

        
        -- if viewid>3 then
        --     sp:setScaleX(1)
        -- else
        --     sp:setScaleX(-1)
        -- end
    end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    local beginPoint = nil
    local endPoint = nil
    local zOrder = 1
    print("赠送道具信息",dwUserID,dwRecvUserID,dwPropID)
    --获取头像坐标
    dump(self._scene.m_tabUserItem)
    --获取头像坐标
    for k,v in pairs(self._scene.m_tabUserItem) do
        if v then
            local wChairID = v.wChairID;
            local viewId = self._scene:SwitchViewChairID(wChairID);
            -- local head = self.nodePlayer[k]:getChildByTag(GameViewLayer.SP_HEAD)
            if v.dwUserID ==  dwUserID then
                beginPoint = playerPosArr[viewId];
            elseif v.dwUserID ==  dwRecvUserID then
                endPoint = playerPosArr[viewId];
                zOrder = self.node_player[viewId]:getZOrder() + 1
            end
        end
    end
    print("beginPoint ," , beginPoint.x , beginPoint.y)
    print("endPoint ," , endPoint.x , endPoint.y)
  if beginPoint and endPoint then
          local actSprite = cc.Sprite:create(string.format("public/prop_%d.png",dwPropID)):move(beginPoint):addTo(self,zOrder)
          if actSprite then
              actSprite:runAction(cc.Sequence:create(cc.DelayTime:create(0.2),cc.MoveTo:create(0.5,endPoint),cc.CallFunc:create(function ( ... )
                  actSprite:removeFromParent()
                  local animateName = {"chun","hua","fanqie","ji","jidan","zan"}
                  local spine = SpineObj:create("public/interact.json", "public/interact.atlas", false)
                  spine:setAnimation(0, animateName[dwPropID], false)
                  spine:registerSpineEventHandler(function (event)
                    spine:destroy()
                  end, 2)
                  spine:setPosition(endPoint)
                  self:addChild(spine,zOrder)
              end)))
          end     
  end
end

--定位
function GameViewLayer:showAdressLayer()
    local playerList = {}
    local validPlayerNum = 0
    for i=1,cmd.GAME_PLAYER do
        local viewid = self._scene:SwitchViewChairID(i-1)
        local useritem = self._scene.m_tabUserItem[viewid]
        local player = self.node_player[viewid];
        --玩家
        local player_item = player:getChildByName("player_item");

        local isVisible = player_item:isVisible()
        if useritem and isVisible then
            useritem = clone(useritem)
            local playerData = {}
            playerData.uid = useritem.dwUserID
            playerData.viewid = viewid
            table.insert(playerList, playerData)
            validPlayerNum = validPlayerNum + 1
        end
    end
    
    local locationObj = uiLocation:ctor(self, playerList, validPlayerNum)
end

--战绩流水
function GameViewLayer:createZhanjiView()
	if self._scene.m_bLookOn then
      	showToast(self, "观战中不可使用该功能", 1)
      	return
  	end

	if self.zhanjiLayer and not tolua.isnull(self.zhanjiLayer) then
		self.zhanjiLayer:removeAllChildren();
		self.zhanjiLayer = nil;
	end
	self.zhanjiLayer  = RecordLayer:create(self);
	self.zhanjiLayer.m_parent = self;
	self:addChild(self.zhanjiLayer ,10);

	self:refreshZhanjiView();
end

function GameViewLayer:refreshZhanjiView()
	if self.zhanjiLayer and not tolua.isnull(self.zhanjiLayer) then
		if self._scene.recordData then
			local m_tabList = self._scene.recordData.lDetailScore;
			local nCount = self._scene.recordData.nCount;
			self.zhanjiLayer:refreshList(m_tabList, nCount)
		end
		
	end
end

return GameViewLayer
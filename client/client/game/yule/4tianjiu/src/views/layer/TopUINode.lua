
--顶部节点
local TopUINode = class("TopUINode", cc.Node)
local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

TopUINode.Btn_Auto = 10;
TopUINode.Btn_Arrow_Down = 11;
TopUINode.Btn_Arrow_Up = 12;
TopUINode.Btn_Bet_Max = 13;
TopUINode.Btn_Bet_Min = 14;

local betArr = {100,50,30,20,10,5};

function TopUINode:ctor(viewParent, topUINode)
	--注册触摸事件
    ExternalFun.registerTouchEvent(self, true);

	self.m_parent = viewParent;

	local csbNode = topUINode;
	self.m_csbNode = topUINode;

    self:inintCsb(csbNode);
    self:inintDateInfo(csbNode);
end

function TopUINode:inintCsb(csbNode)
	--回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender);
        end
    end

    local node_auto = csbNode:getChildByName("node_auto");
    self.btn_auto = node_auto:getChildByName("btn_auto");
    self.btn_auto:setTag(TopUINode.Btn_Auto);
    self.btn_auto:addTouchEventListener(cbtlistener);

    self.btn_arrow_down = node_auto:getChildByName("btn_arrow_down");
    self.btn_arrow_down:setTag(TopUINode.Btn_Arrow_Down);
    self.btn_arrow_down:addTouchEventListener(cbtlistener);

    self.btn_arrow_up = node_auto:getChildByName("btn_arrow_up");
    self.btn_arrow_up:setTag(TopUINode.Btn_Arrow_Up);
    self.btn_arrow_up:addTouchEventListener(cbtlistener);

    self.btn_bet_max = node_auto:getChildByName("btn_bet_max");
    self.btn_bet_max:setTag(TopUINode.Btn_Bet_Max);
    self.btn_bet_max:addTouchEventListener(cbtlistener);

    self.btn_bet_min = node_auto:getChildByName("btn_bet_min");
    self.btn_bet_min:setTag(TopUINode.Btn_Bet_Min);
    self.btn_bet_min:addTouchEventListener(cbtlistener);

    self:resetAutoSet();

    if GlobalUserItem.bPrivateRoom then --and GlobalUserItem.dwCurRoomGroupID == 0 then
		self.btn_auto:setVisible(false);
		self.btn_arrow_down:setVisible(false);
		self.btn_arrow_up:setVisible(false);
		self.btn_bet_max:setVisible(false);
		self.btn_bet_min:setVisible(false);
	end

	-- if GlobalUserItem.dwCurRoomGroupID ~= 0 then
	-- 	node_auto:setPositionY(-200);
	-- end
end

--系统
function TopUINode:inintDateInfo(csbNode)
	-- 电量,时间，信号
	local sys_Bg = csbNode:getChildByName("sys_Bg");
	local icon_net = csbNode:getChildByName("icon_net");
	local battery_bg = csbNode:getChildByName("battery_bg");
	local battery_loading = csbNode:getChildByName("battery_loading");
	local label_time = csbNode:getChildByName("label_time");

	-- 电量,时间，信号
	-- local sp = cc.Sprite:create(cmd.RES.."game/img_battery_bar.png")
	-- self.batteryBar = cc.ProgressTimer:create(sp)
	-- self.batteryBar:setType(cc.PROGRESS_TIMER_TYPE_BAR)
 --    self.batteryBar:setMidpoint(cc.p(0,0.5))
 --    self.batteryBar:setBarChangeRate(cc.p(1, 0))
 --    self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
 --    self.batteryBar:setPosition(battery_loading:getPosition())
 --    battery_bg:addChild(self.batteryBar)
    

	label_time:setString(os.date("%H:%M"))

	-- if not (PriRoom and GlobalUserItem.bPrivateRoom) then
	-- 	label_time:setPositionY(45)
	-- 	self.batteryBar:setPositionY(45)
	-- 	self.btRoomInfo:getChildByName("Net_status"):setPositionY(45)
	-- 	self.btRoomInfo:getChildByName("Battery_bg"):setPositionY(45)
	-- end

    self.m_parent.timeSchedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		-- self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
		battery_loading:setPercent(MultiPlatform:getInstance():getBatteryLevel());
		label_time:setString(os.date("%H:%M"))

		local netType = MultiPlatform:getInstance():getNetworkType()
		local Strength = MultiPlatform:getInstance():getNetworkStrength()
		local TypePic = {"img_4G_","wifi_"}
		if netType and Strength and netType>=1 and netType<=2 and Strength>=1 and Strength<=3 then
			local sp = cc.Sprite:create(cmd.RES.."game/"..TypePic[netType]..Strength..".png")
			icon_net:setSpriteFrame(sp:getSpriteFrame())
		end
	end, 1, false)
end

--按钮回调方法
function TopUINode:OnButtonClickedEvent(tag, sender)
    if tag == TopUINode.Btn_Auto then
        self:onAuto();
    elseif tag == TopUINode.Btn_Arrow_Down then
        self:onArrowDown();
    elseif TopUINode.Btn_Arrow_Up == tag then
        self:onArrowUp();
    elseif TopUINode.Btn_Bet_Max == tag then
        self:onBetMax();
    elseif TopUINode.Btn_Bet_Min == tag then
        self:onBetMin();
    end
end

--自动
function TopUINode:onAuto()
	if self.m_parent.cbTrusteeMode ~= 0 then
		self.m_parent._scene:sendTrustee(0);
	else
		self.m_parent._scene:sendTrustee(self.m_parent.selectTrusteeMode);
	end 
end

--向下
function TopUINode:onArrowDown()
	self.btn_arrow_up:setVisible(true);
	self.btn_arrow_down:setVisible(false);
	self.btn_bet_min:setVisible(true);
	self.btn_bet_max:setVisible(true);
end

--向上
function TopUINode:onArrowUp()
	self.btn_arrow_up:setVisible(false);
	self.btn_arrow_down:setVisible(true);
	self.btn_bet_min:setVisible(false);
	self.btn_bet_max:setVisible(false);
end

--最大下注
function TopUINode:onBetMax()
	self.m_parent.selectTrusteeMode = 2;
	self:selectMaxOrMinBet();
	self:onArrowUp();
	if self.m_parent.cbTrusteeMode ~= 0 then
		self.m_parent._scene:sendTrustee(self.m_parent.selectTrusteeMode);
	end
end

--最小下注
function TopUINode:onBetMin()
	self.m_parent.selectTrusteeMode = 1;
	self:selectMaxOrMinBet();
	self:onArrowUp();
	if self.m_parent.cbTrusteeMode ~= 0 then
		self.m_parent._scene:sendTrustee(self.m_parent.selectTrusteeMode);
	end
end

--选择托管模式
function TopUINode:selectMaxOrMinBet()
	-- local btn_bet_max_1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_max_1.png");
	-- local btn_bet_max_2 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_max_2.png");
	-- local btn_bet_min_1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_min_1.png");
	-- local btn_bet_min_2 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_min_2.png");
	local btn_bet_max_1 = "game/btn_bet_max_1.png";
	local btn_bet_max_2 = "game/btn_bet_max_2.png";
	local btn_bet_min_1 = "game/btn_bet_min_1.png";
	local btn_bet_min_2 = "game/btn_bet_min_2.png";
	
	if self.m_parent.cbTrusteeMode == 1 then  --最小下注
		self.btn_bet_min:loadTextureNormal(btn_bet_min_2);
		self.btn_bet_min:loadTexturePressed(btn_bet_min_2);

		self.btn_bet_max:loadTextureNormal(btn_bet_max_1);
		self.btn_bet_max:loadTexturePressed(btn_bet_max_1);
	elseif self.m_parent.cbTrusteeMode == 1 then  --最大下注
		self.btn_bet_min:loadTextureNormal(btn_bet_min_1);
		self.btn_bet_min:loadTexturePressed(btn_bet_min_1);

		self.btn_bet_max:loadTextureNormal(btn_bet_max_2);
		self.btn_bet_max:loadTexturePressed(btn_bet_max_2);
	end
end

--托管模式
function TopUINode:changeMaxOrMinBet()
	-- local btn_bet_max_1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_max_1.png");
	-- local btn_bet_max_2 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_max_2.png");
	-- local btn_bet_min_1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_min_1.png");
	-- local btn_bet_min_2 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_bet_min_2.png");

	-- local btn_auto_1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_auto_1.png");
	-- local btn_auto_2 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_auto_2.png");

	local btn_bet_max_1 = "game/btn_bet_max_1.png";
	local btn_bet_max_2 = "game/btn_bet_max_2.png";
	local btn_bet_min_1 = "game/btn_bet_min_1.png";
	local btn_bet_min_2 = "game/btn_bet_min_2.png";

	local btn_auto_1 = "game/btn_auto_1.png";
	local btn_auto_2 = "game/btn_auto_2.png";


	if self.m_parent.cbTrusteeMode == 1 then  --最小下注
		self.btn_bet_min:loadTextureNormal(btn_bet_min_2);
		self.btn_bet_min:loadTexturePressed(btn_bet_min_2);

		self.btn_bet_max:loadTextureNormal(btn_bet_max_1);
		self.btn_bet_max:loadTexturePressed(btn_bet_max_1);
	elseif self.m_parent.cbTrusteeMode == 2 then  --最大下注
		self.btn_bet_min:loadTextureNormal(btn_bet_min_1);
		self.btn_bet_min:loadTexturePressed(btn_bet_min_1);

		self.btn_bet_max:loadTextureNormal(btn_bet_max_2);
		self.btn_bet_max:loadTexturePressed(btn_bet_max_2);
	else
		self.btn_bet_min:loadTextureNormal(btn_bet_min_2);
		self.btn_bet_min:loadTexturePressed(btn_bet_min_2);

		self.btn_bet_max:loadTextureNormal(btn_bet_max_1);
		self.btn_bet_max:loadTexturePressed(btn_bet_max_1);
	end

	if self.m_parent.cbTrusteeMode > 0 then  
		self.btn_auto:loadTextureNormal(btn_auto_2);
		self.btn_auto:loadTexturePressed(btn_auto_2);
	else
		self.btn_auto:loadTextureNormal(btn_auto_1);
		self.btn_auto:loadTexturePressed(btn_auto_1);

		self.m_parent.selectTrusteeMode = 1;
	end
end

--是否托管
function TopUINode:changeAutoImg()
	-- local btn_auto = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_auto_" .. self.m_parent.bTrustee == true and 2 or 1 .. ".png");
	local btn_auto = "game/btn_auto_" .. self.m_parent.bTrustee == true and 2 or 1 .. ".png";
	self.btn_auto:loadTextureNormal(btn_auto);
	self.btn_auto:loadTexturePressed(btn_auto);
end

--重置托管
function TopUINode:resetAutoSet()
	self.m_parent.bTrustee = false;   --是否托管
	self.m_parent.cbTrusteeMode = 0;   --托管模式
	self:changeMaxOrMinBet();
	self:onArrowUp();

	-- local btn_auto_1 = cc.SpriteFrameCache:getInstance():getSpriteFrame("btn_auto_1.png");
	local btn_auto_1 = "game/btn_auto_1.png";
	self.btn_auto:loadTextureNormal(btn_auto_1);
	self.btn_auto:loadTexturePressed(btn_auto_1);
end

return TopUINode

--下注操作节点
local OperateBetNode = class("OperateBetNode", cc.Node)

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")


-- 玩家弹窗信息
local palyerInfoPos = 
{
	cc.p(310, 290), cc.p(305 , 440), cc.p(335 , 580),cc.p(1000 , 570), cc.p(1030 , 440),cc.p(1030 , 300)
}

OperateBetNode.Btn_Guo_1 = 10;
OperateBetNode.Btn_Guo_2 = 11;
OperateBetNode.Btn_Guo_3 = 12;

OperateBetNode.Btn_Bet_1 = 20;
OperateBetNode.Btn_Bet_2 = 21;
OperateBetNode.Btn_Bet_3 = 22;
OperateBetNode.Btn_Bet_4 = 23;
OperateBetNode.Btn_Bet_5 = 24;
OperateBetNode.Btn_Bet_6 = 25;

OperateBetNode.Btn_Clean = 30;
OperateBetNode.Btn_Sure = 31;

local betArr = {100,50,30,20,10,5};

function OperateBetNode:ctor(viewParent, betNode)
	-- local function nodeEvent( event )
	-- 	if event == "exit" then
	-- 		ExternalFun.SAFE_RELEASE(self.m_actShowScore)
	-- 	end
	-- end
	-- self:registerScriptHandler(nodeEvent)

	--注册触摸事件
    ExternalFun.registerTouchEvent(self, true);

	self.m_parent = viewParent;

	local csbNode = betNode;
	self.m_csbNode = csbNode;

	self.betNumArr = {0, 0, 0};
	self.betNum = 0;
	self.betArr = betArr;
	self.isSelectGuo = false;

	self:freshUi();

end

function OperateBetNode:resertData()
	self.betNumArr = {0, 0, 0};
	self.isSelectGuo = false;
	self:freshUi();
end

function OperateBetNode:freshUi()
	self.betArr = self.m_parent.dBetScore;
	
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender);
        end
    end

	self.bet_num_1 = self.m_csbNode:getChildByName("bet_num_1");
	self.bet_num_2 = self.m_csbNode:getChildByName("bet_num_2");
	self.bet_num_3 = self.m_csbNode:getChildByName("bet_num_3");
	self.bet_num_1:setString(0);
	self.bet_num_2:setString(0);
	self.bet_num_3:setString(0);

	--下注分
	local bet_bottom = self.m_csbNode:getChildByName("bet_bottom");
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("guo_bg_2.png");
	if self.m_parent.cbBetDaoShu == 2 then
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("guo_bg_2.png");
		if nil ~= frame then
			bet_bottom:setSpriteFrame(frame);
		end
		-- bet_bottom:setContentSize(cc.size(281,63));
		self.bet_num_1:setPosition(cc.p(627, 320));
		self.bet_num_2:setPosition(cc.p(761, 320));
		self.bet_num_3:setVisible(false);
	else
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("guo_bg_1.png");
		if nil ~= frame then
			bet_bottom:setSpriteFrame(frame);
		end
		-- bet_bottom:setContentSize(cc.size(447,63));
		self.bet_num_1:setPosition(cc.p(551, 320));
		self.bet_num_2:setPosition(cc.p(692, 320));
		self.bet_num_3:setVisible(true);
	end

	--锅头
	for i = 1, 3 do
		local btn_guo = self.m_csbNode:getChildByName("btn_guo_" .. i);
		btn_guo:setTag(OperateBetNode.Btn_Guo_1 + i - 1);
    	btn_guo:addTouchEventListener(cbtlistener);
    	btn_guo:setVisible(true);

    	if self.m_parent.cbBetDaoShu == 2 then
    		if i == 2 then
    			btn_guo:setPosition(cc.p(863, 247));
    		elseif i == 3 then
    			btn_guo:setVisible(false);
    		end
    	end

    	if self.m_parent.bJiaGuaPaiJiu == false then
    		btn_guo:setVisible(false);
    	end
	end

	--下注
	for i = 1, 6 do
		local btn_bet = self.m_csbNode:getChildByName("btn_bet_" .. i);
		btn_bet:setTag(OperateBetNode.Btn_Bet_1 + i - 1);
    	btn_bet:addTouchEventListener(cbtlistener);
    	btn_bet:setVisible(true);

    	local num = btn_bet:getChildByName("num");
    	num:setString(self.betArr[6-i+1]);
	end

	local btn_clean = self.m_csbNode:getChildByName("btn_clean");
	btn_clean:setTag(OperateBetNode.Btn_Clean);
	btn_clean:addTouchEventListener(cbtlistener);

	local btn_sure = self.m_csbNode:getChildByName("btn_sure");
	btn_sure:setTag(OperateBetNode.Btn_Sure);
	btn_sure:addTouchEventListener(cbtlistener);
end

--按钮回调方法
function OperateBetNode:OnButtonClickedEvent( tag, sender )
    if tag >= OperateBetNode.Btn_Guo_1 and tag <= OperateBetNode.Btn_Guo_1 + 3 then
        self:onGuoBet(tag - OperateBetNode.Btn_Guo_1 + 1);
    elseif tag >= OperateBetNode.Btn_Bet_1 and tag <= OperateBetNode.Btn_Bet_1 + 6 then
        self:onBet(tag - OperateBetNode.Btn_Bet_1 + 1);
    elseif OperateBetNode.Btn_Clean == tag then
        self:onClean();
    elseif OperateBetNode.Btn_Sure == tag then
        self:onSure();
    end
end

--锅
function OperateBetNode:onGuoBet(index)
	self.isSelectGuo = true;
	local betTotal = self.m_parent.dBankerMoney;
	local userItem = self.m_parent._scene:GetMeUserItem();
	if betTotal > userItem.lScore then
		betTotal = userItem.lScore;
	end

	local betNum = 0;
	if index == 1 then
		betNum = betTotal;
		if self.m_parent.bJiaGuaPaiJiu and betNum < self.m_parent.dMinBetScore then
				showToast(self,"每道下注不能小于最小下注值",2);
			return;
		end

		self.betNumArr[1] = betTotal;
		self.betNumArr[2] = 0;
		self.betNumArr[3] = 0;
	elseif index == 2 then
		betNum = math.floor(betTotal/2);
		if self.m_parent.bJiaGuaPaiJiu and betNum < self.m_parent.dMinBetScore then
				showToast(self,"每道下注不能小于最小下注值",2);
			return;
		end

		self.betNumArr[1] = math.floor(betTotal/2) + (betTotal%2);
		self.betNumArr[2] = math.floor(betTotal/2);
		self.betNumArr[3] = 0;
	elseif index == 3 then
		betNum = math.floor(betTotal/3);
		if self.m_parent.bJiaGuaPaiJiu and betNum < self.m_parent.dMinBetScore then
				showToast(self,"每道下注不能小于最小下注值",2);
			return;
		end

		self.betNumArr[1] = math.floor(betTotal/3) + (betTotal%3);
		self.betNumArr[2] = math.floor(betTotal/3);
		self.betNumArr[3] = math.floor(betTotal/3);
	end

	
	self.bet_num_1:setString(self.betNumArr[1]);
	self.bet_num_2:setString(self.betNumArr[2]);
	self.bet_num_3:setString(self.betNumArr[3]);
end

--下注
function OperateBetNode:onBet(index)
	if self.isSelectGuo then
		self.betNumArr = {0,0,0};
		self.isSelectGuo = false;
	end

	local indexNum1 = 1;
	local indexNum2 = 1;

	for i = 1, 3 do
		if self.betNumArr[i] > 0 and (self.m_parent.bJiaGuaPaiJiu and self.betNumArr[i] >= self.m_parent.dMinBetScore) then
			indexNum1 = indexNum1 + 1;
		elseif self.betNumArr[i] > 0 then
			indexNum2 = indexNum2 + 1;
		end
	end

	if self.m_parent.cbBetDaoShu == 2 and indexNum1 > 2 then
		return;
	end
	if indexNum1 > 3 or indexNum2 > 3 then
		return;
	end
	
	local allBetTotal = self.betNumArr[1] + self.betNumArr[2] + self.betNumArr[3] + self.betArr[6-index+1];
	local userItem = self.m_parent._scene:GetMeUserItem();
	if self.m_parent.bJiaGuaPaiJiu then
		if allBetTotal > self.m_parent.dBankerMoney then
			showToast(self,"总下注不能超过锅底分",2);
			return;
		end

		if allBetTotal > userItem.lScore then
			--积分场
			if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
			else
				showToast(self,"总下注不能超过玩家分",2);
				return;
			end
		end

		self.betNumArr[indexNum1] = self.betNumArr[indexNum1] + self.betArr[6-index+1];
	else
		if allBetTotal > self.m_parent.dBankerMoney then
			showToast(self,"总下注不能超过庄家分",2);
			return;
		end

		if allBetTotal > userItem.lScore then
			--积分场
			if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
			else
				showToast(self,"总下注不能超过玩家分",2);
				return;
			end
		end

		self.betNumArr[indexNum2] = self.betArr[6-index+1];
	end

	self.bet_num_1:setString(self.betNumArr[1]);
	self.bet_num_2:setString(self.betNumArr[2]);
	self.bet_num_3:setString(self.betNumArr[3]);
end

--清除
function OperateBetNode:onClean()
	self.betNumArr = {0,0,0};
	self.isSelectGuo = false;

	self.bet_num_1:setString(self.betNumArr[1]);
	self.bet_num_2:setString(self.betNumArr[2]);
	self.bet_num_3:setString(self.betNumArr[3]);
end

--确定
function OperateBetNode:onSure()
	local isCanSure = true;
	for i = 1, 3 do
		if self.betNumArr[i] > 0 and (self.m_parent.bJiaGuaPaiJiu and self.betNumArr[i] < self.m_parent.dMinBetScore) then
			isCanSure = false;
		end
	end

	if self.betNumArr[1] == 0 then
		showToast(self,"下注不能为空",2);
		return;
	end

	if not isCanSure then
		showToast(self,"每道下注不能小于最小下注值",2);
		return;
	end

	local allBetTotal = self.betNumArr[1] + self.betNumArr[2] + self.betNumArr[3];
	local userItem = self.m_parent._scene:GetMeUserItem();
	if self.m_parent.bJiaGuaPaiJiu then
		if allBetTotal > self.m_parent.dBankerMoney then
			showToast(self,"总下注不能超过锅底分",2);
			return;
		end

		if allBetTotal > userItem.lScore then
			--积分场
			if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
			else
				showToast(self,"总下注不能超过玩家分",2);
				return;
			end
		end
	else
		if allBetTotal > self.m_parent.dBankerMoney then
			showToast(self,"总下注不能超过庄家分",2);
			return;
		end

		if allBetTotal > userItem.lScore then
			--积分场
			if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
			else
				showToast(self,"总下注不能超过玩家分",2);
				return;
			end
		end
	end

	--发送下注
	self.m_parent._scene:sendChip(self.betNumArr);
end

return OperateBetNode
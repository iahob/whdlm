
--游戏结果层
local GameResultLayer = class("GameResultLayer", cc.Layer)
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local module_pre = "game.yule.baccaratnew.src"
local cmd = module_pre .. ".models.CMD_Game"

local GameLogic = module_pre .. ".models.GameLogic"
local bjlDefine = module_pre .. ".models.bjlGameDefine"

-- 牌位置
-- local cardPosList = 
-- {
-- 	cc.p(276, 398), cc.p(926 , 398), cc.p(405 , 398),cc.p(1056 , 398),cc.p(380 , 251), cc.p(948 , 251)
-- }

local cardPosList = 
{
	cc.p(267, 398), cc.p(937 , 398), cc.p(397 , 398),cc.p(1067 , 398),cc.p(374 , 250), cc.p(960 , 250)
}

local kDraw = 1 --平局
local kIdleWin = 2 --闲赢
local kMasterWin = 3 --庄赢
local kLEFT_ROLE = 1;
local kRIGHT_ROLE = 2;
function GameResultLayer:ctor(parent)
	self.m_parent = parent;
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("result/GameResultLayer.csb",self)
	local bg = csbNode:getChildByName("bg")
	self.csbNode = csbNode;
	--
	local pokerAni = csbNode:getChildByName("pokerAni");
	self.pokerAni = pokerAni;

	local card_1 = csbNode:getChildByName("card_1");
	local icon_xian = csbNode:getChildByName("icon_xian");
	self.icon_xian = icon_xian;
	local icon_zhuang = csbNode:getChildByName("icon_zhuang");
	self.icon_zhuang = icon_zhuang;
	local point_xian = csbNode:getChildByName("point_xian");
	self.point_xian = point_xian;
	local point_zhuang = csbNode:getChildByName("point_zhuang");
	self.point_zhuang = point_zhuang;
	local icon_result = csbNode:getChildByName("icon_result");
	self.icon_result = icon_result;

	self:resetResult();
end

function GameResultLayer:hideGameResult()
	self:setVisible(false);
end

function GameResultLayer:showGameResult(bShow, cbCardCount, cbTableCardArray, mHaveAnim, isBreakLine)
	self:resetResult();
	self:setVisible(bShow);

	self.cbCardCount = cbCardCount;
	self.cbTableCardArray = cbTableCardArray;
	self.isBreakLine = isBreakLine;

	self:calResult(false);
	self:showCardAnimate(mHaveAnim);
end

function GameResultLayer:resetResult()
	self.cbCardCount = {};
	self.cbTableCardArray = {{}, {}};
	self.curCardCount = {0, 0};
	self.m_enPointResult = 1;
	self.isBreakLine = false;

	self.icon_result:setVisible(false);
	self.icon_result:setScale(0.0001);
	self.point_xian:setString(0);
	self.point_zhuang:setString(0);

	for i = 1, 6 do
		local card = self.csbNode:getChildByName("card_" .. i);
		card:setVisible(false);
		local card_back = self.csbNode:getChildByName("card_back_" .. i);
		card_back:setVisible(false);

		if i > 2 then
			card:setRotation(90);
		else
			card:setRotation(0);
		end
	end
end

function GameResultLayer:resetPokeAni()
	self.pokerAni:setRotation(26);
	self.pokerAni:setScale(0.15);
	self.pokerAni:setPosition(763, 547);
	self.pokerAni:stopAllActions();
	self.pokerAni:setVisible(false);
end

--牌型动画
function GameResultLayer:showCardAnimate(mHaveAnim)
	local maxIndex = self.cbCardCount[1] + self.cbCardCount[2];
	--翻牌
	for i = 1, 6 do
		local isLeft = (i % 2) == 1;
		local index = 1;
		if i == 1 or i == 2 then
			index = 1;
		elseif i == 3 or i == 4 then
			index = 2;
		else
			index = 3;
		end
		
		if isLeft == true then
			if index <= self.cbCardCount[1] then
				self:showCardItemAnimate(mHaveAnim, index, isLeft, maxIndex, i);
			end
		else
			if index <= self.cbCardCount[2] then
				self:showCardItemAnimate(mHaveAnim, index, isLeft, maxIndex, i);
			end
		end
	end
end

--牌型动画
function GameResultLayer:showCardItemAnimate(mHaveAnim, index, isLeft, maxIndex, i)
	local card = self.csbNode:getChildByName("card_" .. i);
	card:setVisible(false);
	local card_back = self.csbNode:getChildByName("card_back_" .. i);
	card_back:setVisible(false);
	local endPos = cardPosList[i];

	local isEnd = false;
	if i >= maxIndex then
		isEnd = true;
	end

	if mHaveAnim then
		card:runAction(cc.Sequence:create(
	        cc.DelayTime:create(1.5 * (i-1)),
	        cc.CallFunc:create( function()
	            self:flyCardAnim(card, card_back, endPos, index, isLeft, isEnd);
	        end)
	    ));
	else
		self:showNoAnimResult(card, card_back, isLeft, index, isEnd);
	end
end

--发牌
function GameResultLayer:flyCardAnim(card, card_back, endPos, index, isLeft, isEnd)
	self:resetPokeAni();
	local rotation = 0;
	if index >= 2 then
		rotation = 90;
	end
	local move = cc.MoveTo:create(0.5, endPos);
	local scale = cc.ScaleTo:create(0.5, 0.57);
	local rotate = cc.RotateBy:create(0.5, 694 + rotation);
	local delay = cc.DelayTime:create(0.2);
	local call = cc.CallFunc:create(function()
		--翻牌
		if index > 1 then
			self.pokerAni:setVisible(false);
			rotation = 0;
			if index == 2 then
				rotation = -90;
			end
			self:openCardAnimate(card, card_back, endPos, rotation, isLeft, index, isEnd);
		else
			self:resetPokeAni();
			card:setVisible(true);
            card_back:setVisible(true);

            self:refreshResult(isLeft, index, card);
		end
	end);

	local soundStr = "send_card_xian.mp3";
	if isLeft == true then
		soundStr = "send_card_xian.mp3";
	else
		soundStr = "send_card_banker.mp3";
	end
	-- ExternalFun.playSoundEffect(soundStr);
	local call2 = cc.CallFunc:create(function()
		--立即翻牌
		if index == 1 then
			card:setVisible(true);
	        card_back:setVisible(true);
		end
		ExternalFun.playSoundEffect(soundStr);
	end);

	local action = cc.Sequence:create(cc.Spawn:create(move, scale, rotate),call2,delay, call);
	self.pokerAni:setVisible(true);
	self.pokerAni:runAction(action);
end

--发牌
function GameResultLayer:showNoAnimResult(card, card_back, isLeft, index, isEnd)
	self:resetPokeAni();
	card:setVisible(true);
	if index > 2 then
		card:setRotation(90);
	else
		card:setRotation(0);
	end
    card_back:setVisible(true);
    self:refreshResult(isLeft, index, card, isEnd);
end

--翻牌动画
function GameResultLayer:openCardAnimate(card, card_back, endPos, rotation, isLeft, index, isEnd)
	local vsSpin = sp.SkeletonAnimation:create("spine/fanpai/fp.json","spine/fanpai/fp.atlas",1);
    vsSpin:setPosition(endPos);
    vsSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
 	vsSpin:setScale(0.85);
    self:addChild(vsSpin);

    card:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.5),
        cc.CallFunc:create( function()
            vsSpin:removeAllChildren();
            vsSpin:removeFromParent();
            --重置牌
            self:resetPokeAni();
            card:setVisible(true);
            self:refreshResult(isLeft, index, card, isEnd);
            
        end),
        cc.DelayTime:create(0.2),
        cc.RotateBy:create(0.2, rotation),
        cc.CallFunc:create( function()
            card_back:setVisible(true);
        end)
    ));
end

--结果
function GameResultLayer:refreshResult(isLeft, index, card, isEnd)
	if isLeft == true then
    	self.curCardCount[kLEFT_ROLE] = self.curCardCount[kLEFT_ROLE] + 1;
    	self:setCardValue(kLEFT_ROLE, index, card);
    	self:refreshPoint(kLEFT_ROLE);
	else
		self.curCardCount[kRIGHT_ROLE] = self.curCardCount[kRIGHT_ROLE] + 1;
		self:setCardValue(kRIGHT_ROLE, index, card);
		self:refreshPoint(kRIGHT_ROLE);
    end

    if isEnd then
		local pointLeftNum = self:getCardPoint(kLEFT_ROLE);
		local pointRightNum = self:getCardPoint(kRIGHT_ROLE);
		local soundLeftStr = "point_xian_" .. pointLeftNum .. ".mp3";
		local soundRightStr = "point_banker_" .. pointRightNum .. ".mp3";
    	self:runAction(cc.Sequence:create(
	        cc.CallFunc:create( function()
	        	ExternalFun.playSoundEffect(soundLeftStr);
	        end),
	        cc.DelayTime:create(1.0),
	        cc.CallFunc:create( function()
	        	ExternalFun.playSoundEffect(soundRightStr);
	        end),
	        cc.DelayTime:create(1.0),
	        cc.CallFunc:create( function()
	        	self:calResult(isEnd);
	        end)
	    ));
    end
end

--结果
function GameResultLayer:refreshResultShow(enResult)
	local resultStr = "result/icon_ping.png";
	local soundStr = "result_ping.mp3";
	if kDraw == enResult then
		resultStr = "result/icon_ping.png";
	elseif kIdleWin == enResult then
		resultStr = "result/icon_xian_win.png";
		soundStr = "result_xian_win.mp3";
	elseif kMasterWin == enResult then
		resultStr = "result/icon_banker_win.png";
		soundStr = "result_banker_win.mp3";
	end

	ExternalFun.playSoundEffect(soundStr);

	self.icon_result:setVisible(true);
	self.icon_result:setTexture(resultStr);
	self.icon_result:runAction(cc.Sequence:create(
		cc.ScaleTo:create(0.2,1.0),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
        	if not self.isBreakLine then
        		self:hideGameResult();
        	end
        end)
    ));
end

--点数显示
function GameResultLayer:showCardPoint(index)
	if index == 1 or index == 3 or index == 5 then
		self.point_xian:setVisible(true);
	else
		self.point_zhuang:setVisible(true);
	end
end

--牌型结果
function GameResultLayer:setCardValue(areaIndex, index, card)
	local cardValue = self.cbTableCardArray[areaIndex][index];
	local cardStr = "card/" .. "card_" .. cardValue .. ".png";
	card:setTexture(cardStr);
end

--刷新点数
function GameResultLayer:refreshPoint(areaIndex, isEnd)
	local soundStr = "point_banker_0.mp3";
	local pointNum = self:getCardPoint(areaIndex);
	if areaIndex == kLEFT_ROLE then
		self.point_xian:setString(pointNum);
		soundStr = "point_xian_" .. pointNum .. ".mp3";
	elseif areaIndex == kRIGHT_ROLE then
		self.point_zhuang:setString(pointNum);
		soundStr = "point_banker_" .. pointNum .. ".mp3";
	end

	-- ExternalFun.playSoundEffect(soundStr);
end

--获得点数
function GameResultLayer:getCardPoint(areaIndex)
	local point = 0;
	local cardList = {};
	for i = 1 , #self.cbTableCardArray[areaIndex] do
		if i <= self.curCardCount[areaIndex] then
			table.insert(cardList, self.cbTableCardArray[areaIndex][i]);
		end
	end
	point = g_var(GameLogic).GetCardListPip(cardList);
	return point, cardList;
end

--获得点数
function GameResultLayer:getCardResultPoint(areaIndex)
	local point = 0;
	local cardList = {};
	for i = 1 , #self.cbTableCardArray[areaIndex] do
		if i <= self.cbCardCount[areaIndex] then
			table.insert(cardList, self.cbTableCardArray[areaIndex][i]);
		end
	end
	point = g_var(GameLogic).GetCardListPip(cardList);
	return point, cardList;
end

--计算点数结果
function GameResultLayer:calResult(isEnd)
	local idlePoint, idleCards = self:getCardResultPoint(kLEFT_ROLE);
	local masterPoint, masterCards = self:getCardResultPoint(kRIGHT_ROLE);

	--点数记录
	self.m_parent:getDataMgr().m_tabGameResult.m_cbIdlePoint = idlePoint
	self.m_parent:getDataMgr().m_tabGameResult.m_cbMasterPoint = masterPoint

	local nowCBWinner = g_var(cmd).AREA_MAX
	local nowCBKingWinner = g_var(cmd).AREA_MAX

	local cbBetAreaBlink = {0,0,0,0,0,0,0,0}
	if idlePoint > masterPoint then		
		self.m_enPointResult = kIdleWin

		--闲
		nowCBWinner = g_var(cmd).AREA_XIAN
		cbBetAreaBlink[g_var(cmd).AREA_XIAN + 1] = 1
		--闲天王
		if 8 == idlePoint or 9 == idlePoint then
			nowCBKingWinner = g_var(cmd).AREA_XIAN_TIAN
			cbBetAreaBlink[g_var(cmd).AREA_XIAN_TIAN + 1] = 1
		end
	elseif idlePoint < masterPoint then
		self.m_enPointResult = kMasterWin

		--庄
		nowCBWinner = g_var(cmd).AREA_ZHUANG
		cbBetAreaBlink[g_var(cmd).AREA_ZHUANG + 1] = 1
		if 8 == masterPoint or 9 == masterPoint then
			nowCBKingWinner = g_var(cmd).AREA_ZHUANG_TIAN
			cbBetAreaBlink[g_var(cmd).AREA_ZHUANG_TIAN + 1] = 1
		end
	elseif idlePoint == masterPoint then
		self.m_enPointResult = kDraw

		--平
		nowCBWinner = g_var(cmd).AREA_PING
		cbBetAreaBlink[g_var(cmd).AREA_PING + 1] = 1
		--判断是否为同点平
		local bAllPointSame = false
		if #idleCards == #masterCards then
			local cbCardIdx = 1
			for i = cbCardIdx, #idleCards do
				local cbBankerValue = g_var(GameLogic).GetCardValue(masterCards[cbCardIdx])
				local cbIdleValue = g_var(GameLogic).GetCardValue(idleCards[cbCardIdx])

				if cbBankerValue ~= cbIdleValue then
					break
				end

				if cbCardIdx == #masterCards then
					bAllPointSame = true
				end

                cbCardIdx = cbCardIdx + 1 ;

			end
		end

		--同点平
		if true == bAllPointSame then
			nowCBKingWinner = g_var(cmd).AREA_TONG_DUI
			cbBetAreaBlink[g_var(cmd).AREA_TONG_DUI + 1] = 1
		end
	end

	--对子判断
	local nowBIdleTwoPair = false
	local nowBMasterTwoPair = false
	--闲对子
	if g_var(GameLogic).GetCardValue(idleCards[1]) == g_var(GameLogic).GetCardValue(idleCards[2]) then
		nowBIdleTwoPair = true
		cbBetAreaBlink[g_var(cmd).AREA_XIAN_DUI + 1] = 1
	end
	--庄对子
	if g_var(GameLogic).GetCardValue(masterCards[1]) == g_var(GameLogic).GetCardValue(masterCards[2]) then
		nowBMasterTwoPair = true
		cbBetAreaBlink[g_var(cmd).AREA_ZHUANG_DUI + 1] = 1
	end
	self.m_parent:getDataMgr().m_tabBetArea = cbBetAreaBlink

	if not isEnd then
		return;
	end
	if nil ~= self.m_parent then
		self.m_parent:showBetAreaBlink()
	end

	local bJoin = self.m_parent:getDataMgr().m_bJoin
	local res = self.m_parent:getDataMgr().m_tabGameResult
	if nil ~= self.m_parent and false == yl.m_bDynamicJoin then
		--添加路单记录
		local rec = g_var(bjlDefine).getEmptyRecord()

        local serverrecord = g_var(bjlDefine).getEmptyServerRecord()
        serverrecord.cbKingWinner = nowCBKingWinner
        serverrecord.bPlayerTwoPair = nowBIdleTwoPair
        serverrecord.bBankerTwoPair = nowBMasterTwoPair
        serverrecord.cbPlayerCount = idlePoint
        serverrecord.cbBankerCount = masterPoint
        rec.m_pServerRecord = serverrecord
        rec.m_cbGameResult = nowCBWinner
        
        rec.m_tagUserRecord.m_bJoin = bJoin
        if bJoin then        	
        	rec.m_tagUserRecord.m_bWin = res.m_llTotal > 0
        end

        self.m_parent:getDataMgr():addGameRecord(rec)
	end

	--刷新结果
	self:refreshResultShow(self.m_enPointResult);

	-- --播放音效
	-- if true == bJoin then
	-- 	--
	-- 	if res.m_llTotal > 0 then
	-- 		ExternalFun.playSoundEffect("END_WIN.wav")
	-- 	elseif res.m_llTotal < 0 then
	-- 		ExternalFun.playSoundEffect("END_LOST.wav")
	-- 	else
	-- 		ExternalFun.playSoundEffect("END_DRAW.wav")
	-- 	end
	-- else
	-- 	ExternalFun.playSoundEffect("END_DRAW.wav")
	-- end
end

return GameResultLayer
--
-- Author: zhong
-- Date: 2016-07-14 17:42:14
--
--坐下玩家
local SitRoleNode = class("SitRoleNode", cc.Node)

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = require(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = require(appdf.EXTERNAL_SRC .. "PopupInfoHead")

-- 玩家弹窗信息
local palyerInfoPos = 
{
	cc.p(310, 290), cc.p(305 , 440), cc.p(335 , 580),cc.p(1000 , 570), cc.p(1030 , 440),cc.p(1030 , 300)
}

function SitRoleNode:ctor( viewParent, index )
	local function nodeEvent( event )
		if event == "exit" then
			ExternalFun.SAFE_RELEASE(self.m_actShowScore)
		end
	end
	self:registerScriptHandler(nodeEvent)

	self.m_parent = viewParent
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("game/SitRoleNode.csb", self)
	self.m_csbNode = csbNode

	--背景特效
	self.m_spEffect = csbNode:getChildByName("sitdown_effect")
	self.m_spEffect:setVisible(false)

	--信息背景
	local infoBg = csbNode:getChildByName("player_info_1")
	infoBg:setLocalZOrder(1)

	-- local function btnEvent( sender, eventType )
	-- 	if eventType == ccui.TouchEventType.ended then
	-- 		-- this:onJettonAreaClicked(sender:getTag(), sender);
	-- 		print("-------------初始化玩家信息---3333--------")
	-- 		self.m_parent:onDeskRoleInfoLayer(roleItem)
	-- 	end
	-- end	
	-- infoBg:addTouchEventListener(btnEvent)

	--头像
	local tmp = csbNode:getChildByName("head_bg")
	self.m_headSize = tmp:getContentSize().width
	tmp:removeFromParent()

	--游戏币
	self.m_textScore = csbNode:getChildByName("score_text")
	self.m_textScore:setLocalZOrder(1)

	--名字
	tmp = csbNode:getChildByName("name_text")
	self.m_clipNick = ClipText:createClipText(tmp:getContentSize(),"")
	self.m_clipNick:setPosition(tmp:getPosition())
	csbNode:addChild(self.m_clipNick)
	self.m_clipNick:setLocalZOrder(1)
	self.m_clipNick:setAnchorPoint(cc.p(0.5,0.5))
	tmp:removeFromParent()

	--庄家标示
	self.m_spBanker = csbNode:getChildByName("sp_banker")
	self.m_spBanker:setVisible(false)
	self.m_spBanker:setLocalZOrder(3)

	local node_score = csbNode:getChildByName("node_score");
	self.node_score = node_score;
	node_score:setLocalZOrder(4)

	--分数
	self.m_atlasScore = node_score:getChildByName("altas_score")
	self.m_atlasScore:setString("")
	-- self.m_atlasScore:setLocalZOrder(2)

	--分数背景
	self.m_scoreBg = node_score:getChildByName("score_bg")
	self.m_scoreBg:setVisible(false)
	-- self.m_scoreBg:setLocalZOrder(2)

	--隐藏其他信息
	-- local icon_score_bottom = csbNode:getChildByName("icon_score_bottom")
	-- self.m_clipNick:setVisible(false);
	-- icon_score_bottom:setVisible(false);
	-- self.m_textScore:setVisible(false);

	self.m_nIndex = index
	self.m_spHead = nil

	--飞行动画
	local moveBy = cc.MoveBy:create(2.0, cc.p(0, 50))
	local fadeout = cc.FadeOut:create(0.5)
	local call = cc.CallFunc:create(function( )
		self.node_score:setPositionY(0)
	end)
	self.m_actShowScore = cc.Sequence:create(moveBy, fadeout, call)
	ExternalFun.SAFE_RETAIN(self.m_actShowScore)
end

function SitRoleNode:onSitDown( useritem, bAni, isBanker )
	if nil == useritem then
		return
	end
	isBanker = isBanker or false
	
	self:setVisible(true)
	self.m_wChair = useritem.wChairID

	--坐下特效
	if bAni then
		local act = cc.Repeat:create(cc.Blink:create(1.0,1),5)
		self.m_spEffect:stopAllActions()
		self.m_spEffect:runAction(act)
	end	

	--头像
	if nil ~= self.m_spHead and nil ~= self.m_spHead:getParent() then
		self.m_spHead:removeFromParent()
	end
	-- local head = PopupInfoHead:createClipHead(useritem, self.m_headSize, "game/head_mask.png")
	local head = PopupInfoHead:createNormal(useritem, self.m_headSize-10)
	if nil ~= head then
		self.m_csbNode:addChild(head)
		local size = cc.Director:getInstance():getWinSize()
		local pos = cc.p(size.width * 0.11, size.height * 0.33)
		local anchor = cc.p(self.m_nIndex / 8, 1.0)
		if self.m_nIndex > 4 then			
			pos = cc.p(size.width * 0.595, size.height * 0.33)
		end

		if (self.m_nIndex == 1) or (self.m_nIndex == 8) then
			anchor = cc.p(self.m_nIndex / 8, 0)	
		end
		head:setPopCsbFile("popInfo/GamePopInfo.csb") -- 自定义CSB
		head:enableInfoPop(true, pos, anchor)

	    -- local param = {}
	    -- param.isProp = false     --显示道具互动，百人游戏不显示
	    -- param.imgPath = "popInfo/info_bg.png" --弹出框背景
	    -- head:setPropInfo(param)
        -- head:setPopCsbFile("popInfo/GamePopInfo.csb") -- 自定义CSB
	end	

	self.m_spHead = head

	--昵称
	self.m_clipNick:setString(useritem.szNickName)

	--游戏币
	local str = ExternalFun.formatScoreText(useritem.lScore)
	self.m_textScore:setString(str)

	-- if useritem.lScore > 0 then
	-- 	-- if self.goldSprite then
	-- 	-- else
	-- 	-- 	-- self.goldSprite = cc.Sprite:create("game/gold.png"):addTo(self.m_textScore)
	-- 	-- 	-- self.goldSprite:setPosition(cc.p(-30, 10))
	-- 	-- end
	-- 	self.m_textScore:setString(str)
	-- end

	--庄家
	-- self.m_spBanker:setVisible(isBanker)

	--玩家分数
	self.lScore = useritem.lScore;

	--信息背景
	local infoBg = self.m_csbNode:getChildByName("player_info_1")
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self.m_parent:onDeskRoleInfoLayer(useritem, palyerInfoPos[self.m_nIndex])
		end
	end	
	infoBg:addTouchEventListener(btnEvent)
end

function SitRoleNode:getChair(  )
	return self.m_wChair
end

--是否庄家
function SitRoleNode:updateBanker( isBanker )
	--庄家
	-- self.m_spBanker:setVisible(isBanker)
end

--游戏币动画、更新自己游戏币
function SitRoleNode:updateScore( useritem )
	if nil == useritem or nil == useritem.lScore then
		return
	end
	--游戏币
	self.lScore = useritem.lScore;
	local str = ExternalFun.formatScoreText(useritem.lScore)
	
	self.m_textScore:setString(str)
end

--游戏币动画、更新自己游戏币
function SitRoleNode:freshSiteRoleScore(changeScore , isEnd, useritem)
	if isEnd and useritem then
		self.lScore = useritem.lScore + changeScore;
	else
		self.lScore = self.lScore + changeScore;
	end
	
	--游戏币
	local str = ExternalFun.formatScoreText(self.lScore)
	self.m_textScore:setString(str)
end

function SitRoleNode:gameEndScoreChange( useritem, changescore )
	-- self:updateScore(total)
	self:freshSiteRoleScore(changescore, true, useritem)

	if 0 == changescore then
		return
	end

	self.m_atlasScore:setOpacity(255)

	local str = "/" .. ExternalFun.numberThousands(changescore)
	if string.len(str) > 10 then
		str = string.sub(str,1,10) .. "///"
	end
	if changescore > 0 then
		self.m_atlasScore:setProperty(str, "fonts/atlas_add.png", 27, 43, ".")
		self.m_scoreBg:loadTexture("game/score_bg_add.png");

		--赢家星星动画
		self:roleWinShowStarSpin();
	elseif changescore < 0 then
		self.m_atlasScore:setProperty(str, "fonts/atlas_sub.png", 27, 43, ".")
		self.m_scoreBg:loadTexture("game/score_bg_sub.png");
	end
	
	-- self.m_atlasScore:stopAllActions()
	-- self.m_atlasScore:runAction(self.m_actShowScore)

	if self.m_nIndex > 3 then
		self.node_score:setPositionX(-87);
	else
		self.node_score:setPositionX(87);
	end
	self.node_score:setOpacity(255)
	self.node_score:stopAllActions()
	self.m_scoreBg:setVisible(true)
	self.m_scoreBg:setContentSize(cc.size(self.m_atlasScore:getContentSize().width +120, 45))
	self.node_score:runAction(self.m_actShowScore)
end

function SitRoleNode:reSet(  )
	self.m_textScore:setString("")
	self.m_wChair = nil

	-- if self.goldSprite then
	-- 	self.goldSprite:removeFromParent()
	-- 	self.goldSprite = nil
	-- end
end

-- 星星动画
function SitRoleNode:roleWinShowStarSpin()
	-- local starSpin = sp.SkeletonAnimation:create("startjetton/skeleton2.json","spine/skeleton2.atlas",1);
 --    -- starSpin:setPosition(cc.p(667, 375));
 --    starSpin:setAnimation(0, "animation", false);    --true是指循环播放walk动作
 --    self:addChild(starSpin);

 --    self:runAction(cc.Sequence:create(
	-- 	cc.DelayTime:create(1.0),
	-- 	cc.CallFunc:create( function()
	-- 		starSpin:removeAllChildren();
	--         starSpin:removeFromParent();
	--     end )
	-- ));
end
return SitRoleNode
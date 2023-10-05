--
-- Author: zhong
-- Date: 2016-11-11 09:59:23
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local module_pre = "game.qipai.land.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local Define = appdf.req(module_pre .. ".models.Define")

local GameResultLayer = class("GameResultLayer", cc.Layer)
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local TAG_ENUM = Define.TAG_ENUM

local BT_CONTINUE = 101
local BT_QUIT = 102
local BT_CLOSE = 103

function GameResultLayer.getTagSettle()
	return
	{
	-- 用户名
	m_userName = "",
	-- 文本颜色
	nameColor = cc.c4b(255,255,255,255),
	nameColor1 = cc.c4b(255,196,100, 255),
	-- 计算游戏币
	m_settleCoin = "",
	-- 文本颜色
	coinColor = cc.c4b(255,255,255,255),
	-- 特殊标志
	m_cbFlag = cmd.kFlagDefault,
	}
end

function GameResultLayer.getTagGameResult()
	return
	{
	-- 结果
	enResult = cmd.kDefault,
	-- 结算
	settles =
	{
	GameResultLayer.getTagSettle(),
	GameResultLayer.getTagSettle(),
	GameResultLayer.getTagSettle(),
	},
	-- 春天/反春天
	cbFlag = cmd.kFlagDefault
	}
end

function GameResultLayer:ctor( parent )
	local this = self
	self.m_parent = parent
	--注册node事件
	ExternalFun.registerTouchEvent(this, true)
	
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("game/GameResultLayer.csb", self)
	local bg = csbNode:getChildByName("bg")
	self.m_spBg = bg
	
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			this:onBtnClick(sender:getTag(), sender)
		end
	end
	-- 继续按钮
	local continuebtn = bg:getChildByName("btnContinue")
	continuebtn:setTag(BT_CONTINUE)
	continuebtn:addTouchEventListener(btnEvent)
	
	if MatchRoom and GlobalUserItem.bMatch then
		continuebtn:setVisible(false)
	end
	
	-- 退出按钮
	btn = bg:getChildByName("btnQuit")
	btn:setTag(BT_QUIT)
	btn:addTouchEventListener(btnEvent)
	
	if MatchRoom and GlobalUserItem.bMatch then
		btn:setVisible(false)
	end
	
	local btnChange = bg:getChildByName("btnChange")
	btnChange:addClickEventListener(function (ref)
		print("换桌")
		self:hideGameResult()
		self.m_parent:onButtonClickedEvent(207)
		--self.m_parent._scene:onChangeDesk()
	end)
	if GlobalUserItem.bPrivateRoom then
		btnChange:setVisible(false)
		btn:setVisible(false)
		continuebtn:setPosition(btnChange:getPosition())
	end

	if MatchRoom and GlobalUserItem.bMatch then 
		btnChange:setVisible(false)
		btn:setVisible(false)
	end
	
	--回放隐藏操作按钮
	if GlobalUserItem.bVideo then
		btn:setVisible(false);
		btnChange:setVisible(false);
		continuebtn:setVisible(false);
	end

	-- 关闭按钮
	btn = bg:getChildByName("btnClose")
	btn:setTag(BT_CLOSE)
	btn:addTouchEventListener(btnEvent)
	
	local str = ""
	self.m_tabClipNickName = {}
	self.m_tabTextCoin = {}
	self.m_textCoin = {}
	self.m_tabFlag = {}
	
	-- 用户信息
	local csbGroup = bg:getChildByName("u_group")
	for i = 1, 3 do
		local idx = i - 1
		str = "user" .. idx .. "Text"
		local txt = csbGroup:getChildByName(str)
		self.m_tabClipNickName[i] = ClipText:createClipText(txt:getContentSize(), "", nil, 30)
		self.m_tabClipNickName[i]:setPosition(txt:getPosition())
		self.m_tabClipNickName[i]:setAnchorPoint(txt:getAnchorPoint())
		csbGroup:addChild(self.m_tabClipNickName[i])
		txt:removeFromParent()
		
		str = "user" .. idx .. "Coin"
		self.m_tabTextCoin[i] = csbGroup:getChildByName(str)
		
		str = "flag" .. idx
		self.m_tabFlag[i] = csbGroup:getChildByName(str)
	end
	
	self:hideGameResult()
end

function GameResultLayer:onTouchBegan(touch, event)
	return self:isVisible()
end

function GameResultLayer:onTouchEnded(touch, event)
	local pos = touch:getLocation()
	local m_spBg = self.m_spBg
	pos = m_spBg:convertToNodeSpace(pos)
	local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
	if false == cc.rectContainsPoint(rec, pos) then
		self:hideGameResult()
	end
end

function GameResultLayer:onBtnClick(tag, sender)
	if BT_CONTINUE == tag then
		self:hideGameResult()
		self.m_parent._scene.bIsHaveShowEnd = true  --已经显示并且关掉了，设置为true

		if self.m_parent._scene.m_bLookOn  then
			
			return 
		end
		if self.m_parent._scene.bPriGameEnd  and GlobalUserItem.bPrivateRoom  then

			if nil ~= self.m_parent._priView and nil ~= self.m_parent._priView.showEndLayer then
		            		self.m_parent._priView:showEndLayer()
		          	end
			return 
		end

		if self.m_parent:getParentNode().m_bPriScoreLow then
			local msg = self.m_parent:getParentNode().m_szScoreMsg or ""
			self.m_querydialog = QueryDialog:create(msg, function ()
				self.m_parent:getParentNode():onExitTable()
				end,nil,1)
				self.m_querydialog:setCanTouchOutside(false)
				self.m_querydialog:addTo(self.m_parent)
		else
			self.m_parent:onClickReady()
		end
	elseif BT_QUIT == tag then
		self.m_parent:getParentNode():onQueryExitGame()
	elseif BT_CLOSE == tag then
		self:hideGameResult()
		self.m_parent._scene.bIsHaveShowEnd = true  --已经显示并且关掉了，设置为true
		if self.m_parent._scene.bPriGameEnd  and GlobalUserItem.bPrivateRoom  then

			if nil ~= self.m_parent._priView and nil ~= self.m_parent._priView.showEndLayer then
		            	self.m_parent._priView:showEndLayer()
		          end
			return 
		end
	end
end
	
function GameResultLayer:hideGameResult()
	self:reSet()
	self:setVisible(false)
end

function GameResultLayer:showGameResult( rs )
	self:reSet()
	self:setVisible(true)
	
	local strBg 
	local strBg1 
	local strBack

	if rs.enResult == cmd.kLanderWin then
		self:winAnimate("dizhu")
		strBg = "result/win/dikuang.png"
		strBg1 = "result/win/tiao.png"
		strBack = "public/back_red.png"
	elseif rs.enResult == cmd.kLanderLose then
		self:loseAnimate("dizhu")
		strBg = "result/lose/dikuang.png"
		strBg1 = "result/lose/tiao2.png"
		strBack = "public/back_blue.png"
	elseif rs.enResult == cmd.kFarmerWin then
		self:winAnimate("nongmin")
		strBg = "result/win/dikuang.png"
		strBg1 = "result/win/tiao.png"
		strBack = "public/back_red.png"
	elseif rs.enResult == cmd.kFarmerLose then
		self:loseAnimate("nongmin")
		strBg = "result/lose/dikuang.png"
		strBg1 = "result/lose/tiao2.png"
		strBack = "public/back_blue.png"
	end

	if strBg and strBg1 and strBack then
		self.m_spBg:setTexture(strBg)
		self.m_spBg:getChildByName("bg_content"):loadTexture(strBg1)
		self.m_spBg:getChildByName("btnClose"):loadTextureNormal(strBack)
	end
	
	-- 更新文本
	local tmpbg = self.m_spBg:getChildByName("u_group")
	for i = 1, 3 do
		local settle = rs.settles[i]
		local txtscore
		local x, y = self.m_tabTextCoin[i]:getPosition()
		local offY = -15

		if settle.m_settleCoin >= 0 then
			-- 昵称
			self.m_tabClipNickName[i]:setString(settle.m_userName)
			self.m_tabClipNickName[i]:setTextColor(settle.nameColor1)
			
			-- 游戏币
			txtscore = cc.LabelAtlas:create("/"..math.abs(settle.m_settleCoin),"privateroom/no/grade1.png",20,26,string.byte("/"))
			txtscore:setPosition(cc.p(x, y+offY))
			txtscore:addTo(tmpbg)
			-- self.m_tabTextCoin[i]:setString(settle.m_settleCoin)
			-- self.m_tabTextCoin[i]:setColor(settle.coinColor)
			table.insert(self.m_textCoin, txtscore)

		else
			-- 昵称
			self.m_tabClipNickName[i]:setString(settle.m_userName)
			self.m_tabClipNickName[i]:setTextColor(settle.nameColor)
			
			-- 游戏币
            txtscore = cc.LabelAtlas:create("/"..math.abs(settle.m_settleCoin),"privateroom/no/grade.png",20,26,string.byte("/"))
			txtscore:setPosition(cc.p(x, y+offY))
			txtscore:addTo(tmpbg)
			table.insert(self.m_textCoin, txtscore)
		end
		
		-- 标志
		if cmd.kFlagChunTian == settle.m_cbFlag then
			self.m_tabFlag[i]:setString("春天*2")
		elseif cmd.kFlagFanChunTian == settle.m_cbFlag then
			self.m_tabFlag[i]:setString("反春天*2")
		else
			self.m_tabFlag[i]:setString("")
		end

		if rs.cbMaxTimes == 1 then
			tmpbg:getChildByName("fd"..(i-1)):setVisible(true)
			if settle.m_settleCoin >= 0 then
				tmpbg:getChildByName("fd"..(i-1)):setTexture("game/fdWin.png")
			else
				tmpbg:getChildByName("fd"..(i-1)):setTexture("game/fdLose.png")
			end
		else
			tmpbg:getChildByName("fd"..(i-1)):setVisible(false)
		end
	end
end

function GameResultLayer:reSet()
	if self.startJettonAnimate then
		self.startJettonAnimate:removeFromParent()
		self.startJettonAnimate = nil
	end

	for i = 1, 3 do
		-- 昵称
		self.m_tabClipNickName[i]:setString("")
		self.m_tabClipNickName[i]:setTextColor(cc.c4b(255,255,255,255))
		
		-- 游戏币
		self.m_tabTextCoin[i]:setString("")
		self.m_tabTextCoin[i]:setColor(cc.c4b(255,255,255,255))

		if self.m_textCoin[i] then
			self.m_textCoin[i]:removeFromParent()
			self.m_textCoin[i] = nil
		end
		
		self.m_tabFlag[i]:setString("")
	end
end

function GameResultLayer:loseAnimate(person)
    self.startJettonAnimate  = sp.SkeletonAnimation:create("animate/lose/skeleton.json", "animate/lose/skeleton.atlas", 1)
    self.startJettonAnimate:setPosition(display.cx,display.cy-15)
    self.startJettonAnimate:setAnimation(0, "animation", false)
    self.startJettonAnimate:setSkin(person)
    self.startJettonAnimate:addTo(self)
end

function GameResultLayer:winAnimate(person)
    self.startJettonAnimate  = sp.SkeletonAnimation:create("animate/win/skeleton.json", "animate/win/skeleton.atlas", 1)
    self.startJettonAnimate:setPosition(display.cx,display.cy-15)
    self.startJettonAnimate:setAnimation(0, "animation", false)
    self.startJettonAnimate:setSkin(person)
    self.startJettonAnimate:addTo(self)
end

return GameResultLayer
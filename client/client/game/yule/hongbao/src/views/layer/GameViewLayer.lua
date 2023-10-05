--
-- Author: Tang
-- Date: 2016-10-11 17:22:24
--

--[[

	游戏交互层
]]

local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.hongbao.src"

--external
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
local  BankerList = module_pre..".views.layer.BankerList"
local  HelpLayer = module_pre..".views.layer.HelpLayer"
local  SettingLayer = module_pre..".views.layer.SettingLayer"
local  BankLayer = module_pre..".views.layer.BankLayer"
local GameChatLayer = appdf.req(module_pre..".views.layer.GameChatLayer")
local cmd = module_pre .. ".models.CMD_Game"
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local SpineObj = appdf.req(appdf.EXTERNAL_SRC.."SpineObj")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

GameViewLayer.LAYER_CHAT = 100
GameViewLayer.LAYER_POPVIEWS = 99

GameViewLayer.ZOrder_ATLAS = 10
GameViewLayer.ZOrder_USERVIEW = 11

local ClipTextTag = 123

local TAG_ATLAS = 101

local ANIM_TAG = {
	TouchMe = 1,
	OpenBag = 2,
	Thunder = 3
}

local posChat = {cc.p(500,57), cc.p(900,57), cc.p(950,57)}


--中文长度计算(同步pc,中文长度为2)
local function stringLen(szText, Len)
	if nil == Len or len == 0 then
		return szText
	end

	local i = 1
    while true do
        local cur = string.sub(szText,i,i)
        local byte = string.byte(cur)
        if byte == nil then
            break
        end
        if byte > 128 then
            i = i + 3
            Len = Len - 3
        elseif  byte ~= 32 then
            i = i + 1
            Len = Len - 2
        else
            Len = Len - 2
            i = i + 1
        end
        if Len <= 0 then
        	break
        end
    end

	local cur = string.sub(szText, 1,i-1)
	if cur == szText then
		return szText
	else
		return cur .. "..."
	end
end

function GameViewLayer:ctor(scene)

	self._scene = scene

 	self._bankerView = nil
 	self._UserView = nil         --玩家列表

 	self.m_cbGameStatus = 0

	self.chatDetails = {}

 	self.m_eApplyStatus = GameViewLayer.unApply

	self:gameDataInit()

	self.m_getBagUserNum = 0

	self._openBagKey = nil

	--初始化csb界面
	self:initCsbRes()
	
	self:playBackGroundMusic()
	-- 注册事件
 	ExternalFun.registerTouchEvent(self,true)
	
end

function GameViewLayer:restData()
	self.m_getBagUserNum = 0
	self._openBagKey = nil
end

function GameViewLayer:gameDataInit(  )
	--搜索路径
    local gameList = self:getParentNode():getParentNode():getApp()._gameList;
    local gameInfo = {};
    for k,v in pairs(gameList) do 
        if tonumber(v._KindID) == tonumber(g_var(cmd).KIND_ID) then
            gameInfo = v;
            break;
        end
    end
    
    if nil ~= gameInfo._Module then
    	self._searchPath = device.writablePath.."game/" .. gameInfo._Module .. "/res/";

        cc.FileUtils:getInstance():addSearchPath(self._searchPath);
    end

    --加载资源
	self:loadRes()
end

function GameViewLayer:getParentNode( )
	return self._scene;
end

function GameViewLayer:getDataMgr( )
	return self:getParentNode():getDataMgr()
end

function GameViewLayer:showPopWait( )
	self:getParentNode():showPopWait()
end

function GameViewLayer:loadRes()

end

function GameViewLayer:bankerViewReload()
	if tolua.cast(self._bankerView, "cc.Node") then
		self._bankerView:reloadData()
	end
end

function GameViewLayer:showSendBagSuccess()
	if tolua.cast(self._bankerView, "cc.Node") then
		self._bankerView:onSendBagSuccess()
	end
end

---------------------------------------------------------------------------------------
--界面初始化
function GameViewLayer:initCsbRes()
	local rootLayer, csbNode = ExternalFun.loadRootCSB("image/gameLayer.csb",self)
	self._rootNode = csbNode

 	
	self._rootNode:getChildByName("timeClock"):setPositionX(667):setVisible(false)

 	self._chatLayer = GameChatLayer:create(self._scene._gameFrame):addTo(self._rootNode, GameViewLayer.LAYER_CHAT)	--聊天框

 	self._UserView = self._rootNode:getChildByName("Panel_userList")
 	self._UserView:setVisible(false)
 	self._UserView:setZOrder(GameViewLayer.ZOrder_USERVIEW)

	--聊天泡泡
	self.chatBubble = {}
	local bubble = self._rootNode:getChildByName("chatPop")
	for i = 1, 3 do
		local temp = bubble:clone()
		temp:addTo(self._rootNode,99)
		temp:setPosition(posChat[i])
		table.insert(self.chatBubble, temp)
	end

	self:initButtons()
	for i=1, 10 do
		local bg = self._rootNode:getChildByName("bg")
		bg:getChildByName("player_bg_"..i):setVisible(false)
	end
	
end

function GameViewLayer:updateScore(lScore)
	local score = math.floor((lScore / self._scene.nRatio)/0.01)/100
	if score > 10000 then
		score = math.floor((score/10000)/0.01)/100 .. "万" -- string.format( "%0.2f万",score/10000)
	end
	self._rootNode:getChildByName("Text_25"):setString(score)
end

function GameViewLayer:initSelfHead(meInfo)
	if meInfo then
		--玩家头像
		self._rootNode:getChildByName("Text_24"):setString(stringLen(meInfo.szNickName, 15))
		local headBg = self._rootNode:getChildByName("img_face_24")
		headBg:removeChildByTag(5)
		local head = g_var(PopupInfoHead):createNormal(clone(meInfo), 66)
    	head:enableInfoPop(true, cc.p(500,100), cc.p(0,0))
		headBg:addChild(head,5,5)
		head:setPosition(43,43)
	end
end

function GameViewLayer:initButtons()  --初始化按钮
	
	local function callfunc(ref,eventType)
        if eventType == ccui.TouchEventType.ended then
        	ExternalFun.playClickEffect()
        	-- print("++++==============self._openBagKey and self._scene.m_lBagKey",self._openBagKey ,self._scene.m_lBagKey)
	        if not self._openBagKey and self._scene.m_lBagKey and self._openBagKey ~= self._scene.m_lBagKey and self._scene.m_lBagKey ~= 0 then
	        	self._scene:sendRobBanker()
	        	self._openBagKey = self._scene.m_lBagKey
	        elseif self._scene.m_lBagKey == nil then
	        	showToast(self,"暂无红包可抢", 2)
	        end

        	if tolua.cast(self.obj_touchMe, "cc.Node") then
        		self.obj_touchMe:removeFromParent()
        	end
        end
    end

   --红包按钮
	local btn =  self._rootNode:getChildByName("Button_1")
	btn:addTouchEventListener(callfunc)
	self.btn_bag = btn

	--发红包
	local banker = self._rootNode:getChildByName("Button_2")
	banker:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
       			self._bankerView = g_var(BankerList):create(self._scene):addTo(self._rootNode, GameViewLayer.LAYER_POPVIEWS)
            end
        end)

   --聊天
	local btn =  self._rootNode:getChildByName("Button_Chat"):setVisible(false)
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
    			self._chatLayer:showGameChat(true)
            end
        end)

   --玩家列表
	local btn =  self._rootNode:getChildByName("Button_UserList")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
            	self:showUserListPanel()
            end
        end)

    -- 下拉菜单
	local btn =  self._rootNode:getChildByName("Button_menu")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
           		local bg = self._rootNode:getChildByName("menuBG")
           		bg:setVisible(not bg:isVisible())
           		if bg:isVisible() then
           			btn:getChildByName("Image_1"):loadTexture("image/btn_menuUp.png")
           		else
           			btn:getChildByName("Image_1"):loadTexture("image/btn_menuDown.png")
           		end
            end
        end)

	self:InitMenu()
end


--初始化菜单按钮
function GameViewLayer:InitMenu()
	local menuBG = self._rootNode:getChildByName("menuBG"):setVisible(false)

	local btn = menuBG:getChildByName("Button_back")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
       			self._scene:onExitRoom()
            end
        end)

	local btn = menuBG:getChildByName("Button_bank")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
	            if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
		            showToast(self, "初次使用，请先开通银行！", 1)
		            return
		        end
            	-- self.m_bankLayer = g_var(BankLayer):create(self._scene):addTo(self._rootNode, GameViewLayer.LAYER_POPVIEWS)
            	self._scene:sendRequestBankInfo()
            end
        end)
    -- 是否开启银行服务
    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    btn:setVisible(enableBank)

	local btn = menuBG:getChildByName("Button_help")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
            	g_var(HelpLayer):create():addTo(self._rootNode, GameViewLayer.LAYER_POPVIEWS)
            end
        end)
    if not enableBank then
    	btn:setPositionY(220)
    end

	local btn = menuBG:getChildByName("Button_set")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
            	g_var(SettingLayer):create():addTo(self._rootNode, GameViewLayer.LAYER_POPVIEWS)
            end
        end)
	
end


function GameViewLayer:onExit()
	self:gameDataReset()
	if tolua.cast(self.obj_OpenBag, "cc.Node") then
		self.obj_OpenBag:removeFromParent()
	end
end


function GameViewLayer:gameDataReset(  )
	--资源释放

	--播放大厅背景音乐
	AudioEngine.stopMusic()
	ExternalFun.playPlazzBackgroudAudio()

	--重置搜索路径
	local oldPaths = cc.FileUtils:getInstance():getSearchPaths();
	local newPaths = {};
	for k,v in pairs(oldPaths) do
		if tostring(v) ~= tostring(self._searchPath) then
			table.insert(newPaths, v);
		end
	end
	cc.FileUtils:getInstance():setSearchPaths(newPaths);

end
----------------------------------------------------------------------------------------
function GameViewLayer:setRoomInfo( count, times, nRatio)
	local txtCount = self._rootNode:getChildByName("bagCount")
	local txtTimes = self._rootNode:getChildByName("pei")
	local txtRatio = self._rootNode:getChildByName("ratio")
	if count and times then
		txtCount:setString(count)
		txtTimes:setString(string.format("%0.1f",times))
		txtRatio:setString(nRatio)
	end
end

function GameViewLayer:startGame(buff)
	self:restData()
	self:updateBankerInfo(buff)
	self:playAnim(ANIM_TAG.TouchMe)
	self.btn_bag:setVisible(true)
end

--更新庄家信息
function GameViewLayer:updateBankerInfo(info)
	if (type(info) ~= "table") then
		return
	end
	print("**************************updateBankerInfo(**********************************")
	self._rootNode:getChildByName("Text_3"):setString(info.lScore or "")
	self._rootNode:getChildByName("Text_redCount"):setString(info.cbLeftCount or self._scene._playPlayerCount)
	self._rootNode:getChildByName("Text_BoomNm"):setString(info.cbThunderPoint or "")


	--玩家头像
	local bankerItem = self._scene:getUserByChair(info.wBankerUser)
	local bankerNode = self._rootNode:getChildByName("Node_1")
	local bankerName = bankerNode:getChildByName("Text_23"):setString("")
	if bankerItem then
		bankerName:setString(stringLen(bankerItem.szNickName, 15))
		local headBG = bankerNode:getChildByName("img_face_23")
		headBG:removeChildByTag(5)

		local head = g_var(PopupInfoHead):createNormal(bankerItem, 66)
		headBG:addChild(head)
		head:setPosition(43,43)
		head:setTag(5)
	end
end

function GameViewLayer:showGetBagUser(useritem,cbLeftCount,fBagScore)
	self.m_getBagUserNum = self.m_getBagUserNum + 1
	local bg = self._rootNode:getChildByName("bg")
	if self.m_getBagUserNum > 10 then
		print("=============self.m_getBagUserNum > 10  cbLeftCount,fBagScore",cbLeftCount,fBagScore)
		assert(false)
		return
	end

	local countLab = self._rootNode:getChildByName("Text_redCount")
	countLab:setString(cbLeftCount )

	if useritem.wChairID == self._scene:GetMeChairID() then
    	self:showBagOpen(fBagScore, true)
    end

	local playerNode = bg:getChildByName("player_bg_"..self.m_getBagUserNum)
	playerNode:setTexture("image/player_bg_1.png")
	playerNode:setVisible(true)
	playerNode:getChildByName("Text_1"):setString("")

	local name = playerNode:getChildByName("name")
	name:setString(stringLen(useritem.szNickName,9))

	local headBG = playerNode:getChildByName("img_face_2")
	headBG:removeChildByTag(33)
	local head = g_var(PopupInfoHead):createNormal(useritem, 66)
	headBG:addChild(head, 33)
	head:setPosition(43,43)
end

function GameViewLayer:showEndInfo(BankerScore,fBagScore, bThunderPoint, fUserScore)

	self.isReset = false
	
	local bg = self._rootNode:getChildByName("bg")
	self._rootNode:getChildByName("timeClock"):setVisible(false)

	if BankerScore > 0 then
		local lable = cc.LabelAtlas:create("12","font/hbslWinFont_0.png",27, 36, string.byte("+"))
		lable:setTag(TAG_ATLAS)
		self._rootNode:addChild(lable)
		lable:setPosition(432,626)
		lable:setString("+"..string.format("%.2f",BankerScore))
	end

	local atlasPos = {cc.p(300,510),cc.p(1033,510),cc.p(300,402),cc.p(1033,402),cc.p(300,294),
				      cc.p(1033,294),cc.p(300,186),cc.p(1033,186),cc.p(300,78),cc.p(1033,78)}
	for i = 1 , 10 do
		if self._scene.m_getUser[i] ~= nil then

			if self._scene.m_getUser[i].wChairID == self._scene:GetMeChairID() then

		    	self:showBagOpen(fBagScore[i], false)

				-- local openBg = self._rootNode:getChildByName("hbao_open")
				-- openBg:getChildByName("AtlasLabel"):setString(string.format("%.2f",fBagScore[i]))
			end
			local playerBg = bg:getChildByName("player_bg_"..i)
			
			playerBg:getChildByName("Text_1"):setString(string.format("%.2f", fBagScore[i]))
			local anchor = cc.p(0.5, 0.5)
			if i%2 == 0 then
				anchor = cc.p(1,0.5)
			else
				anchor = cc.p(0,0.5)
			end
			if bThunderPoint[i] == true then
				playerBg:setTexture("image/player_bg_2.png")

				local lable = cc.LabelAtlas:create("12","font/hbslLoseFont_0.png",27, 36, string.byte("+"))
				lable:setTag(TAG_ATLAS + i)
				lable:setAnchorPoint(anchor)
				self._rootNode:addChild(lable, GameViewLayer.ZOrder_ATLAS)
				lable:setPosition(atlasPos[i])
				lable:setString(string.format("%.2f",fUserScore[i]))
				if self._scene:GetMeChairID() == self._scene.m_getUser[i].wChairID then
					self:playAnim(ANIM_TAG.Thunder)
				end
			else
				local lable = cc.LabelAtlas:create("12","font/hbslWinFont_0.png",27, 36, string.byte("+"))
				lable:setTag(TAG_ATLAS + i)
				lable:setAnchorPoint(anchor)
				self._rootNode:addChild(lable,GameViewLayer.ZOrder_ATLAS)
				lable:setPosition(atlasPos[i])
				lable:setString("+"..string.format("%.2f",fUserScore[i]))
			end
		end
	end
	local action = cc.Sequence:create(cc.DelayTime:create(3),
					cc.CallFunc:create(
						function()
							print("==================================>#self._scene.m_DispatchBagList",#self._scene.m_DispatchBagList)
							if #self._scene.m_DispatchBagList < 1 and self.isReset == false then
								self:resetGameView()
							end
						end))
	action:setTag(39)
	self:stopActionByTag(39)
	self:runAction(action)
end

function GameViewLayer:showBagOpen( fBagScore, touched )
	-- 隐藏红包按钮
	self.btn_bag:setVisible(false)

	local openBg = self._rootNode:getChildByName("hbao_open")
	openBg:getChildByName("AtlasLabel"):setString(fBagScore == nil and "" or string.format("%.2f",fBagScore))

	if openBg:isVisible() then
		return
	end

	if touched then
		self:playAnim(ANIM_TAG.OpenBag)
	else
		openBg:setVisible(true)
	end

	--玩家头像
	local bankerItem = self._scene:getUserByChair(self._scene.m_wBankerUser)
	if bankerItem then

		openBg:getChildByName("Text_name"):setString(stringLen(bankerItem.szNickName, 9).."的红包")

		local headBG = openBg:getChildByName("img_face")
		headBG:removeChildByTag(5)

		local head = g_var(PopupInfoHead):createNormal(bankerItem, 66)
		headBG:addChild(head)
		head:setPosition(43,43)
		head:setTag(5)
	else
		openBg:setVisible(false)
	end
end

function GameViewLayer:resetGameView()
	self.isReset = true
	self.m_getBagUserNum = 0
	self.btn_bag:setVisible(true)

	self._rootNode:getChildByName("timeClock"):setVisible(false)

  -- 庄家红包信息清理
	self._rootNode:getChildByName("Text_3"):setString("")
	self._rootNode:getChildByName("Text_redCount"):setString("")
	self._rootNode:getChildByName("Text_BoomNm"):setString("")
	--玩家头像
	local bankerNode = self._rootNode:getChildByName("Node_1")
	local bankerName = bankerNode:getChildByName("Text_23"):setString("")
	local headBG = bankerNode:getChildByName("img_face_23")
	headBG:removeChildByTag(5)
	
	for i=1, 10 do
		local bg = self._rootNode:getChildByName("bg")
		bg:getChildByName("player_bg_"..i):setVisible(false)

		self._rootNode:removeChildByTag(TAG_ATLAS + i)
	end
	self._rootNode:removeChildByTag(TAG_ATLAS)

	local openBg = self._rootNode:getChildByName("hbao_open")
	openBg:setVisible(false)
	
	if tolua.cast(self.obj_touchMe, "cc.Node") then
		self.obj_touchMe:removeFromParent()
	end
	if tolua.cast(self.obj_OpenBag, "cc.Node") then
		self.obj_OpenBag:removeFromParent()
	end
	if tolua.cast(self.obj_Thunder, "cc.Node") then
		self.obj_Thunder:removeFromParent()
	end
end
----------------------------------------------------------------------------------------
function GameViewLayer:playAnim(tag)
	if tag == ANIM_TAG.TouchMe then
		local btn = self._rootNode:getChildByName("Button_1")
	    self.obj_touchMe = SpineObj:create("spine/shouzhi/skeleton.json", "spine/shouzhi/skeleton.atlas", true)
        self.obj_touchMe:setPosition(btn:getContentSize().width/3,btn:getContentSize().height/2)
        btn:addChild(self.obj_touchMe)

    elseif tag == ANIM_TAG.OpenBag then
    	local openBg = self._rootNode:getChildByName("hbao_open"):setVisible(false)
	    self.obj_OpenBag = SpineObj:create("spine/qianghongbao/skeleton.json", "spine/qianghongbao/skeleton.atlas", false)
        self.obj_OpenBag:setPosition(openBg:getPositionX() - 2,openBg:getPositionY()+13)
        self._rootNode:addChild(self.obj_OpenBag)
        self.obj_OpenBag:runAction(cc.Sequence:create(cc.DelayTime:create(2),
	        	cc.CallFunc:create(function() 
		        		self.obj_OpenBag:destroy()
		        		openBg:setVisible(true)
	        		 end)))

    elseif tag == ANIM_TAG.Thunder then
	    self.obj_Thunder = SpineObj:create("spine/zhonglei/skeleton.json", "spine/zhonglei/skeleton.atlas", false)
        self.obj_Thunder:setPosition(667,375)
        self._rootNode:addChild(self.obj_Thunder)
	end
end


-------------------------------------------------------------------------------------------
--玩家列表
function GameViewLayer:showUserListPanel( )
	self._UserView:setVisible(true)
	local btn = self._UserView:getChildByName("Button_close")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
            	ExternalFun.playClickEffect()
    			self._UserView:setVisible(false)
    			self._UserTable:removeFromParent()
    			self._UserTable = nil
            end
        end)

	local cellNode = self._UserView:getChildByName("cell")
	cellNode:setVisible(false)
	if nil == self._UserTable then
	    local tableView = cc.TableView:create(cc.size(980, 530))
		tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
		tableView:setPosition(cc.p(175,66))
		tableView:setDelegate()
		tableView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
		tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
		tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
		tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
		tableView:reloadData()
		self._UserView:addChild(tableView)
		self._UserTable = tableView
	end
	dump(self._scene.m_UserList,"self._scene.m_UserList",9)
end

function GameViewLayer.cellSizeForTable( view, idx )
	return 980,130
end

function GameViewLayer:getAllUser()
	local users = self._scene.m_UserList
	if #users > 1 then
		table.sort(users, function(a,b) 
			if a ~= nil and b ~= nil then
				return a.lScore > b.lScore
			end
				return true
			end)
	end
	return users
end

function GameViewLayer:numberOfCellsInTableView( view )
	local num = #self:getAllUser()
	print("**********************numberOfCellsInTableView",num, math.ceil(num/2))
	
	return math.ceil(num/2)
end

function GameViewLayer:tableCellAtIndex( view, idx )
	print("**********************tableCellAtIndex",idx)
	local cell = view:dequeueCell()
	local userlist = self:getAllUser()
	-- if nil == useritem then
 --        return cell 
 --    end

	if cell == nil then
		cell = cc.TableViewCell:new()
		for i = 1, 2 do
			local item = userlist[idx*2 + i]

			local userBg = cc.Sprite:create("image/cellUser.png")
			cell:addChild(userBg, 1, 10+i)
			userBg:setPosition(242 + 496*(i-1), 64)

			local name = cc.Label:createWithTTF("","fonts/round_body.ttf", 24)
			userBg:addChild(name, 1, 6)
			name:setAnchorPoint(0, 0.5)
			name:setPosition(242,88)

			local score = cc.Label:createWithTTF("","fonts/round_body.ttf", 24)
			userBg:addChild(score, 1, 7)
			score:setAnchorPoint(0, 0.5)
			score:setPosition(290,43)

			local headBg = cc.Sprite:create("image/headBG.png")
			userBg:addChild(headBg, 1, 8)
			headBg:setPosition(190,63)

			

			if item then
				name:setString(stringLen(item.szNickName, 15))

				local num = item.lScore / self._scene.nRatio
				num = math.floor(num/0.01)/100
				if num > 10000 then
					num = math.floor(num/10000/0.01)/100 .. "万" --string.format( "%0.2f万",num/10000)
				end
				score:setString(num)

				local head = HeadSprite:createNormal(item, 70)
				if head then
					headBg:addChild(head, 1, 9)
					head:setPosition(43,45)
				end
			else
				userBg:setVisible(false)
			end
		end
	else
		for i = 1, 2 do
			local item = userlist[idx*2 + i]

			if item then
				local userBg = cell:getChildByTag( 10+i)
				userBg:setVisible(true)

				local name = userBg:getChildByTag( 6)
				name:setString(stringLen(item.szNickName, 15))

				local score = userBg:getChildByTag( 7)
				local num = item.lScore / self._scene.nRatio
				num = math.floor(num/0.01)/100
				if num > 10000 then
					num = math.floor(num/10000/0.01)/100 .. "万" --string.format( "%0.2f万",num/10000)
				end
				score:setString(num)

				local headBg = userBg:getChildByTag( 8)
				headBg:removeChildByTag(9)
				local head = HeadSprite:createNormal(item, 66)
				if head then
					headBg:addChild(head, 1, 9)
					head:setPosition(43,43)
				end
			else
				local userBg = cell:getChildByTag( 10+i)
				userBg:setVisible(false)

				local name = userBg:getChildByTag( 6)
				name:setString("")

				local score = userBg:getChildByTag( 7)
				score:setString("")

				local headBg = userBg:getChildByTag( 8)
				headBg:removeChildByTag(9)
			end
		end
	end

	return cell
end

---------------------------------------------银行消息处理-----------------------------------------------
function GameViewLayer:onBankSuccess()

    local bank_success = self:getParentNode().bank_success
    if nil == bank_success then
        return
    end
    GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore
    GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

    if tolua.cast(self.m_bankLayer, "cc.Node") then
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

    showToast(self, bank_fail.szDescribeString, 2)
end

function GameViewLayer:onBankUserInfo(info)
	if self.m_bankLayer == nil or not tolua.cast(self.m_bankLayer, "cc.Node") then
		self.m_bankLayer = g_var(BankLayer):create(self._scene):addTo(self._rootNode, GameViewLayer.LAYER_POPVIEWS)
	end
    local wRevenueTake = info.wRevenueTake or 10
    BANK_BTN_NUM = info.lBankOPButtonArray[1]
    if tolua.cast(self.m_bankLayer, "cc.Node") then
    	self.m_bankLayer:refreshBank(wRevenueTake, info.lBankOPButtonArray[1])
    end
end

---------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------------------------------------------


function GameViewLayer:SetClockType(timetype) --设置倒计时

	local RollPanel = self._rootNode:getChildByName("Panel_roll")
	local typeImage = RollPanel:getChildByName("time_type")
	typeImage:setVisible(true)
	if timetype == g_var(cmd).CLOCK_FREE then
		self:updateScore(self.m_scoreUser)
		typeImage:loadTexture("game_res/tubiao42.png")
	elseif timetype == g_var(cmd).CLOCK_ADDGOLD then
		typeImage:loadTexture("game_res/tubiao41.png")
	else
		typeImage:loadTexture("game_res/tubiao40.png")
	end
end


function GameViewLayer:setClockTypeIsVisible(visible) --倒计时类型

	local RollPanel = self._rootNode:getChildByName("Panel_roll")
	local typeImage = RollPanel:getChildByName("time_type")
	typeImage:setVisible(visible)
end
----------------------------------------------------------------------------------------------------------------------------------------
function GameViewLayer:TouchUserInfo()  --点击用户头像显示信息
	
end

--------------------------------------------------------------

--------------------------------------------------------------
--倒计时
function GameViewLayer:createClockView(time,viewtype)
	if nil ~= self.m_pClock then
		self.m_pClock:removeFromParent()
		self.m_pClock = nil
	end

	local RollPanel = self._rootNode:getChildByName("Panel_roll")
	if viewtype == 0 then --转盘界面
		self.m_pClock = cc.LabelAtlas:create(string.format("%d",time),"game_res/shuzi3.png",130,108,string.byte("0"))
		self.m_pClock:setAnchorPoint(0.5,0.5)
		self.m_pClock:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
		RollPanel:addChild(self.m_pClock, TAG_ZORDER.CLOCK_ZORDER)
	else  --下注界面
		local addview = self._rootNode:getChildByName("add_rect")
		self.m_pClock = cc.LabelAtlas:create(string.format("%d",time),"game_res/shuzi4.png",17,21,string.byte("0"))
		self.m_pClock:setAnchorPoint(0.5,0.5)
		self.m_pClock:setPosition(258,75)
		addview:addChild(self.m_pClock)
	end
end

function GameViewLayer:UpdataClockTime(clockTime)
	local clockTip = self._rootNode:getChildByName("timeClock")
	
	if nil ~= clockTip then
		clockTip:setString(string.format("抢红包剩余时间%d秒",clockTime))
		clockTip:setVisible(clockTime>0)
	end

	if clockTime <= 0 then
    	if tolua.cast(self.obj_touchMe, "cc.Node") then
    		self.obj_touchMe:removeFromParent()
    	end
    end
end

function GameViewLayer:LogicTimeZero()  --倒计时0处理

	local RollPanel = self._rootNode:getChildByName("Panel_roll")
	local typeImage = RollPanel:getChildByName("time_type")
	typeImage:setVisible(false)

	if nil ~= self.m_pClock then
		self._scene:KillGameClock()
		self.m_pClock:removeFromParent()
		self.m_pClock = nil
	end

	if self.m_cbGameStatus == g_var(cmd).GS_GAME_FREE then
		self:removeAction()
		self:restData()
		self:RollDisAppear()
		self:AddViewSlipToShow()
	elseif self.m_cbGameStatus ==  g_var(cmd).GS_PLACE_JETTON then
		self:AddViewSlipToHidden()

	elseif self.m_cbGameStatus == g_var(cmd).GS_GAME_END then

	    local info  = self._scene._info
	    self:ShowBankerInfo(info)

		self:SetEndView(true)
		
		--隐藏时间类型
 		self:setClockTypeIsVisible(false)


 		--self:


 		if self._scene.m_bAllowJoin then
 			--插入记录
	    	self:addRcord(self.endindex)
 		end

 		--移除倒计时
		if nil ~= self.m_pClock then
			self._scene:KillGameClock()
			self.m_pClock:removeFromParent()
			self.m_pClock = nil
		end
		

	end
end

--------------------------------------------------------------

------------------------------------------------------------------------------------------------------------
--动画



-----------------------------------------------------------------------------------------------------------------

--用户聊天
function GameViewLayer:userChat(wChairId, chatString)
	if chatString and #chatString > 0 then
		self._chatLayer:showGameChat(false)
		local bubbleIdx = 1
		if wChairId ~= self._scene:GetMeChairID() then
			if self.chatBubble[2]:isVisible() then
				bubbleIdx = 3
			else
				bubbleIdx = 2
			end
		end
		--取消上次
		if self.chatDetails[bubbleIdx] then
			self.chatDetails[bubbleIdx]:stopAllActions()
			self.chatDetails[bubbleIdx]:removeFromParent()
			self.chatDetails[bubbleIdx] = nil
		end

		--创建label
		local limWidth = 24*12
		local labCountLength = cc.Label:createWithTTF(chatString,"fonts/round_body.ttf", 24)
		if labCountLength:getContentSize().width > limWidth then
			self.chatDetails[bubbleIdx] = cc.Label:createWithTTF(chatString,"fonts/round_body.ttf", 24, cc.size(limWidth, 0))
		else
			self.chatDetails[bubbleIdx] = cc.Label:createWithTTF(chatString,"fonts/round_body.ttf", 24)
		end
		self.chatDetails[bubbleIdx]:setColor(cc.c3b(255, 255, 255))
		self.chatDetails[bubbleIdx]:move(posChat[bubbleIdx].x + 20,posChat[bubbleIdx].y + 30)
		self.chatDetails[bubbleIdx]:setAnchorPoint(cc.p(0, 0))
		self.chatDetails[bubbleIdx]:addTo(self, 3)

	    --改变气泡大小
		self.chatBubble[bubbleIdx]:setContentSize(self.chatDetails[bubbleIdx]:getContentSize().width+38, self.chatDetails[bubbleIdx]:getContentSize().height + 54)
			:setVisible(true)
		--动作
	    self.chatDetails[bubbleIdx]:runAction(cc.Sequence:create(
	    	cc.DelayTime:create(3),
	    	cc.CallFunc:create(function(ref)
	    		self.chatDetails[bubbleIdx]:removeFromParent()
				self.chatDetails[bubbleIdx] = nil
				self.chatBubble[bubbleIdx]:setVisible(false)
	    	end)))
    end
end

--用户表情
local showNext = false
function GameViewLayer:onUserExpression(chatdata, wChairId)
	local bubbleIdx = 1
	print(wChairId,self._scene:GetMeChairID())
	if wChairId ~= self._scene:GetMeChairID() then
		if showNext then
			bubbleIdx = 3
		else
			bubbleIdx = 2
			showNext = true
		end
	end

	local idx = chatdata.wItemIndex
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", idx))
    local frames = {}
    local actionTime = 0.2
    for i=1,32 do
        local strName = string.format("emoj%d_%d.png",idx, i)
        print("browChatSprName--------------------->", strName)
        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
        if spriteFrame then
            table.insert(frames, spriteFrame)
        else
            break
        end
    end
    local animation = cc.Animation:createWithSpriteFrames(frames, actionTime)
    animation:setLoops(-1)
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", idx))
    local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
    spr:setPosition(posChat[bubbleIdx].x + 20, posChat[bubbleIdx].y)
    self._rootNode:addChild(spr, 101, 1)
    spr:runAction(cc.Animate:create(animation))
    self._rootNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0), cc.CallFunc:create(function()
    	spr:removeFromParent()
    	showNext = false
    end)))
end

function GameViewLayer:onUserVoiceEnded(viewId)
	if self.chatDetails[viewId] then
	    self.chatDetails[viewId]:removeFromParent()
	    self.chatDetails[viewId] = nil
	    self.chatBubble[1]:setVisible(false)
	end
end


----------------------------------------------------------------------------------------------------------------------
function GameViewLayer:onTouchBegan(touch, event)
	print("hongbao onTouchBegan...")

	return true

end

function GameViewLayer:onTouchMoved(touch, event)

	print("hongbao onTouchMoved...")

end

function GameViewLayer:onTouchEnded(touch, event )

	print("hongbao onTouchEnded...")
    local pos = touch:getLocation() 
    local m_spBg = self._rootNode:getChildByName("menuBG")
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        m_spBg:setVisible(false)
        self._rootNode:getChildByName("Button_menu"):getChildByName("Image_1"):loadTexture("image/btn_menuDown.png")
    end
end
-----------------------------------------------------------------------------------------------------------------------

function GameViewLayer:playEffect( file )
	if not GlobalUserItem.bVoiceAble then
		return
	end

	AudioEngine.playEffect(file)
end


function GameViewLayer:playBackGroundMusic(cbStatus)
	if not GlobalUserItem.bVoiceAble then
		return
	end

	-- if cbStatus == g_var(cmd).GS_PLACE_JETTON then --下注
	-- 	AudioEngine.stopMusic()
	-- 	self:playEffect("sound_res/PLEASE_ADD_GOLD.wav")
	-- elseif cbStatus == g_var(cmd).GS_GAME_END  then
		AudioEngine.stopMusic()
		AudioEngine.playMusic("sound/BGM.mp3",true)
	-- elseif cbStatus == g_var(cmd).GS_GAME_FREE or cbStatus == g_var(cmd).GS_GAME_FREE then
	-- 	AudioEngine.stopMusic()
	-- 	AudioEngine.playMusic("sound_res/BACK_GROUND_FREE.wav",false)	
	-- end
end

return GameViewLayer
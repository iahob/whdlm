--
-- Author: zhong
-- Date: 2016-07-25 15:19:39
--
--[[
* 通用带弹窗显示个人信息的头像类
]]

local PopupInfoHead = class("PopupInfoHead", cc.Node)
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")
local PopupInfoLayer = require(appdf.EXTERNAL_SRC .. "PopupInfoLayer")

local POP_LAYERNAME = "pop_headinfo_layer"
local POP_LAYERNAME_GAME = "pop_headinfo_layer_game"
function PopupInfoHead:ctor( )
	ExternalFun.registerNodeEvent(self)
	self.m_head = nil
	self.m_bIsGamePop = true
	self.m_szCsbFile = ""
	self.m_GetPos = nil

	self.m_userInfo = nil
	-- 信息通知
	self.m_listener = cc.EventListenerCustom:create(yl.RY_PLAYERUSER_NOTIFY,handler(self, self.onResultUserInfo))
    	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function PopupInfoHead:createNormal( useritem ,headSize )
	--dump(useritem, "玩家信息", 10)
	local sf = PopupInfoHead.new()
	local head = HeadSprite:createNormal(useritem, headSize)
	sf.m_useritem = useritem
	sf.m_head = head
	if nil ~= head then
		sf:addChild(head)
	end	
	return sf, head
end

function PopupInfoHead:createClipHead( useritem, headSize, clippingfile )
	local sf = PopupInfoHead.new()
	local head = HeadSprite:createClipHead(useritem, headSize, clippingfile)
	sf.m_useritem = useritem
	sf.m_head = head
	if nil ~= head then
		sf:addChild(head)
	end	
	return sf, head
end

--设置是否是游戏界面弹窗
function PopupInfoHead:setIsGamePop( var )
	self.m_bIsGamePop = var
end

-- 设置弹窗资源
function PopupInfoHead:setPopCsbFile( var )
	self.m_szCsbFile = var
end

--更新头像
function PopupInfoHead:updateHead(useritem)
	self.m_useritem = useritem
	if nil ~= self.m_head then
		self.m_head:updateHead(useritem)
	end	
end


function PopupInfoHead:setPropInfo(param)
	self.m_pParam = param or {}
end

--[[
* 是否设置游戏头像点击显示详细信息
* [bEnable] 是否设置
* [popPos] 弹窗位置,以锚点为(0,0)计算而来的位置
* [anr] 弹窗锚点,默认为(0,0)
]]
function PopupInfoHead:enableInfoPop( bEnable, popPos, anr, popParent)
	local function funCall( )
		self:onTouchHead()
	end
	self.m_popPos = popPos or self:getInfoLayerPos()
	self.m_popAnchor = anr or self:getInfoLayerAnchor()
	self.popParent = popParent or nil

	self.m_GetPos = self.m_popPos

	if nil ~= self.m_head then
		self.m_head:registerInfoPop(bEnable, funCall)
	end

	if bEnable then
		
	else
		local infoLayer = nil
		if self.m_bIsGamePop then
			infoLayer = cc.Director:getInstance():getRunningScene():getChildByName(POP_LAYERNAME_GAME)
		else
			infoLayer = cc.Director:getInstance():getRunningScene():getChildByName(POP_LAYERNAME)
		end
		if nil ~= infoLayer then
			infoLayer:removeFromParent()
		end
	end
end

--头像框
function PopupInfoHead:enableHeadFrame( bEnable, frameparam )
	if nil ~= self.m_head then
		self.m_head:enableHeadFrame(bEnable, frameparam)
	end
end

function PopupInfoHead:onTouchHead(  )
	local infoLayer = nil
	local name = ""

	if nil == self.m_useritem then
		return
	end
	--判断是否进入防作弊房间
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(self.m_useritem.dwUserID)
	if bAntiCheat then
		return
	end
	if self.m_bIsGamePop then
		name = POP_LAYERNAME_GAME
		infoLayer = cc.Director:getInstance():getRunningScene():getChildByName(POP_LAYERNAME_GAME)
		if nil ~= self.popParent then
			local runningScene = cc.Director:getInstance():getRunningScene()
			local posWorld = self.popParent:convertToWorldSpace(self.m_GetPos)
	    	local posNode = runningScene:convertToNodeSpace(posWorld)
	    	--print("坐标转化-----------------------=============》", posWorld.x, posWorld.y, posNode.x, posNode.y)
	    	self.m_popPos = posNode
	    	if nil ~= infoLayer then
	    		infoLayer:removeFromParent()
	    		infoLayer = nil
	    	end
		end
	else
		name = POP_LAYERNAME
		infoLayer = cc.Director:getInstance():getRunningScene():getChildByName(POP_LAYERNAME)
		if nil ~= self.popParent then
			local runningScene = cc.Director:getInstance():getRunningScene()
			local posWorld = self.popParent:convertToWorldSpace(self.m_GetPos)
		    	local posNode = runningScene:convertToNodeSpace(posWorld)
		    	self.m_popPos = posNode
		    	if nil ~= infoLayer then
		    		infoLayer:removeFromParent()
		    		infoLayer = nil
		    	end
		end
		
	end 
	if nil ~= infoLayer then
    		infoLayer:removeFromParent()
    		infoLayer = nil
    	end


		
	infoLayer = PopupInfoLayer:create(self, self.m_bIsGamePop, self.m_szCsbFile,self.m_pParam)
	if nil ~= self.popParent then
		local runningScene = cc.Director:getInstance():getRunningScene()
		local posWorld = self.popParent:convertToWorldSpace(self.m_GetPos)
    	local posNode = runningScene:convertToNodeSpace(posWorld)
    	--print("坐标转化-----------------------=============》", posWorld.x, posWorld.y, posNode.x, posNode.y)
    	self.m_popPos = posNode
	end
	local runningScene = cc.Director:getInstance():getRunningScene()
	if nil ~= runningScene then
		runningScene:addChild(infoLayer)
		infoLayer:setName(name)
	end


	if nil ~= infoLayer and nil ~= self.m_useritem then
		infoLayer:showLayer(true)
		infoLayer:refresh(self.m_useritem, self.m_popPos, self.m_popAnchor)
	end
end

function PopupInfoHead:queryUserLociton(dwUserID)
	FriendMgr:getInstance():sendQueryUserLocation(dwUserID)
end

function PopupInfoHead:onResultUserInfo( event )
	local nearuser = event.msg
	--dump(nearuser, "nearuser", 6)
	if nil ~= nearuser  and nearuser.dwUserID == self.m_useritem.dwUserID then
		--self.m_latitude = nearuser.dLatitude
		--self.m_longitude = nearuser.dLongitude
		--self.m_placeName = (nearuser.szPlaceName == "") and "暂未获取到地址" or nearuser.szPlaceName
		self.m_userInfo = nearuser
	end
end



function PopupInfoHead:getInfoLayerPos(  )
	local size = cc.Director:getInstance():getWinSize()
	return cc.p(size.width * 0.11, size.height * 0.26)
end

function PopupInfoHead:getInfoLayerAnchor(  )
	return cc.p(0, 0)
end

function PopupInfoHead:onExit()
	--强行移除layer，在关闭程序的时候会异常
	local infoLayer = nil
	if self.m_bIsGamePop then
		name = POP_LAYERNAME_GAME
		infoLayer = cc.Director:getInstance():getRunningScene():getChildByName(POP_LAYERNAME_GAME)
	else
		name = POP_LAYERNAME
		infoLayer = cc.Director:getInstance():getRunningScene():getChildByName(POP_LAYERNAME)
	end
	if nil ~= infoLayer and nil ~= infoLayer.hide then
		infoLayer:hide()
	end
end
return PopupInfoHead
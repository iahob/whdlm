--
-- Author: zhong
-- Date: 2016-07-25 10:19:18
--
--游戏头像
local HeadSprite = class("HeadSprite", cc.Sprite)
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--自定义头像存储规则
-- path/face/userid/custom_+customid.ry
--头像缓存规则
-- uid_custom_cusomid
local FACEDOWNLOAD_LISTENER = "face_notify_down"
local FACERESIZE_LISTENER = "face_resize_notify"
local URLREQUEST_LISTENER = "face_urlhttp_request"
--全局通知函数
cc.exports.g_FaceDownloadListener = function (ncode, msg, filename)
	local event = cc.EventCustom:new(FACEDOWNLOAD_LISTENER)
	event.code = ncode
	event.msg = msg
	event.filename = filename

	cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

--
cc.exports.g_FaceResizeListener = function(oldpath, newpath)
	local event = cc.EventCustom:new(FACERESIZE_LISTENER)
	event.oldpath = oldpath
	event.newpath = newpath

	cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

local SYS_HEADSIZE = 96
function HeadSprite.checkData(useritem)
	useritem = useritem or {}
	useritem.dwUserID = useritem.dwUserID or 0
	useritem.dwCustomID = useritem.dwCustomID or 0
	useritem.wFaceID = useritem.wFaceID or 0
	if useritem.wFaceID > 199 then
		useritem.wFaceID = 0
	end
	useritem.cbMemberOrder = useritem.cbMemberOrder or 0

	return useritem
end

function HeadSprite:ctor( )
	self.m_spRender = nil
	self.m_downListener = nil
	self.m_resizeListener = nil
	self.m_httpUrlListener = nil
	self.m_onBackgroundEnterListener = cc.EventListenerCustom:create(appdf.CLIENT_NOTIFY,handler(self, self.onClientNotify))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_onBackgroundEnterListener, self)

	local this = self
	--注册事件
	local function onEvent( event )
		if event == "exit" then
			this:onExit()
		elseif event == "enterTransitionFinish" then
			this:onEnterTransitionFinish()
        end
	end
	self:registerScriptHandler(onEvent)

	self.m_headSize = 96
	self.m_useritem = nil
	self.listener = nil
	self.m_bEnable = false
	--是否头像
	self.m_bFrameEnable = false
	--头像配置
	self.m_tabFrameParam = {}
	-- 是否请求
	self.m_bSendRequest = false
end

--创建普通玩家头像
function HeadSprite:createNormal( useritem ,headSize)
	if nil == useritem then
		--return
	end
	useritem = HeadSprite.checkData(useritem)
	local sp = HeadSprite.new()
	sp.m_headSize = headSize
	local spRender = sp:initHeadSprite(useritem)
	if nil ~= spRender then
		sp:addChild(spRender)
		local selfSize = sp:getContentSize()
		spRender:setPosition(cc.p(selfSize.width * 0.5, selfSize.height * 0.5))
		return sp
	end
	
	return nil
end

--创建裁剪玩家头像
function HeadSprite:createClipHead( useritem, headSize, clippingfile )
	if nil == useritem then
		--return
	end
	useritem = HeadSprite.checkData(useritem)

	local sp = HeadSprite.new()
	sp.m_headSize = headSize
	local spRender = sp:initHeadSprite(useritem)
	if nil == spRender then
		return nil
	end 

	--创建裁剪
	local strClip = "plaza/plaza_sp_cliphead.png"
	if nil ~= clippingfile then
		strClip = clippingfile
	end
	local clipSp = nil
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strClip)
	if nil ~= frame then
		clipSp = cc.Sprite:createWithSpriteFrame(frame)
	else
		clipSp = cc.Sprite:create(strClip)
	end
	if nil ~= clipSp then
		--裁剪
		local clip = cc.ClippingNode:create()
		clip:setStencil(clipSp)
		clip:setAlphaThreshold(0.05)
		clip:addChild(spRender)
		local selfSize = sp:getContentSize()
		clip:setPosition(cc.p(selfSize.width * 0.5, selfSize.height * 0.5))
		sp:addChild(clip)
		return sp
	end

	return nil
end

function HeadSprite:updateHead( useritem )
	if nil == useritem then
		return nil
	end
	useritem = HeadSprite.checkData(useritem)
	self.m_useritem = useritem

	--判断是否进入防作弊房间
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(useritem.dwUserID)

	--更新头像框
	if self.m_bFrameEnable and false == bAntiCheat then
		local vipIdx = self.m_useritem.cbMemberOrder or 0

		--根据会员等级配置
		local vipIdx = self.m_useritem.cbMemberOrder or 0
		local framestr = string.format("sp_frame_%d_0.png", vipIdx)
		local deScale = 0.72

		local framefile = self.m_tabFrameParam._framefile or framestr
		local scaleRate = self.m_tabFrameParam._scaleRate or deScale
		local zorder = self.m_tabFrameParam._zorder or 1
		self:updateHeadFrame(framefile, scaleRate):setLocalZOrder(zorder)
	end

	--系统头像
	local faceid = useritem.wFaceID or 0
	if true == bAntiCheat then
		faceid = 0
	else
		local render = self:getHeadSprite(useritem)
		if nil ~= render then
			local frame = render:getSpriteFrame()
			if self:updateHeadByFrame(frame) then
				return self.m_spRender
			end
		end
	end
	local str = string.format("Avatar%d.png", faceid)
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
	if nil ~= frame then
		self.m_spRender:setSpriteFrame(frame)
		self:setContentSize(self.m_spRender:getContentSize())		
	end
	self.m_fScale = self.m_headSize / SYS_HEADSIZE
	self:setScale(self.m_fScale)

	return self.m_spRender
end

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
function HeadSprite:updateHeadByFrame(frame)
	if nil == frame then
		return false
	end

	if nil == self.m_spRender then
		self.m_spRender = cc.Sprite:createWithSpriteFrame(frame)
	else
		self.m_spRender:setSpriteFrame(frame)
	end
	local contentsize = self.m_spRender:getContentSize()
	local scaleRate = math.max(contentsize.width, contentsize.height)
	self.m_spRender:setScale(SYS_HEADSIZE / scaleRate)
	
	self:setContentSize(cc.size(SYS_HEADSIZE, SYS_HEADSIZE))
	self.m_fScale = self.m_headSize / SYS_HEADSIZE
	self:setScale(self.m_fScale)
	return true
end

--允许个人信息弹窗/点击头像触摸事件
function HeadSprite:registerInfoPop( bEnable, fun )
	self.m_bEnable = bEnable
	self.m_fun = fun

	if bEnable then
		--触摸事件
		self:registerTouch()
	else
		if nil ~= self.listener then
			local eventDispatcher = self:getEventDispatcher()
			eventDispatcher:removeEventListener(self.listener)
			self.listener = nil
		end
	end
end

--头像框
--[[
frameparam = 
{
	--框文件
	_framefile 
	--缩放值
	_scaleRate
	--位置比例
	_posPer{}
	-- z顺序
	_zorder
}
]]
function HeadSprite:enableHeadFrame( bEnable, frameparam )
	if nil == self.m_useritem then
		return
	end
	self.m_bFrameEnable = bEnable
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(self.m_useritem.dwUserID)

	if false == bEnable or bAntiCheat then
		if nil ~= self.m_spFrame then
			self.m_spFrame:removeFromParent()
			self.m_spFrame = nil
		end
		return
	end	
	local vipIdx = self.m_useritem.cbMemberOrder or 0

	--根据会员等级配置
	local vipIdx = self.m_useritem.cbMemberOrder or 0
	local framestr = string.format("sp_frame_%d_0.png", vipIdx)
	local deScale = 0.72

	frameparam = frameparam or {}
	self.m_tabFrameParam = frameparam
	local framefile = frameparam._framefile or framestr
	local scaleRate = frameparam._scaleRate or deScale
	local zorder = frameparam._zorder or 1
	self:updateHeadFrame(framefile, scaleRate)
	if nil ~= self.m_spFrame then
		self.m_spFrame:setLocalZOrder(zorder)
	end
end

--更新头像框
function HeadSprite:updateHeadFrame(framefile, scaleRate)
	local spFrame = nil	
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framefile)
	if nil == frame then
		spFrame = cc.Sprite:create(framefile)
		frame = (spFrame ~= nil) and spFrame:getSpriteFrame() or nil		
	end
	if nil == frame then
		return nil
	end

	if nil == self.m_spFrame then
		local selfSize = self:getContentSize()
		self.m_spFrame = cc.Sprite:createWithSpriteFrame(frame)
		local positionRate = self.m_tabFrameParam._posPer or cc.p(0.5, 0.64)
		self.m_spFrame:setPosition(selfSize.width * positionRate.x, selfSize.height * positionRate.y)
		self:addChild(self.m_spFrame)
	else
		self.m_spFrame:setSpriteFrame(frame)
	end
	self.m_spFrame:setScale(scaleRate)
	return self.m_spFrame
end

function HeadSprite:initHeadSprite( useritem )
	self.m_useritem = useritem
	local isThirdParty = useritem.bThirdPartyLogin or false	
	--print(string.format("HeadSprite:initHeadSprite ==> nick[%s] customid[%d]", useritem.szNickName, useritem.dwCustomID))
	
	--系统头像
	local faceid = useritem.wFaceID or 0
	--判断是否进入防作弊房间
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(useritem.dwUserID)
	if bAntiCheat then
		--直接使用系统头像
		faceid = 0
	else
		local render = self:getHeadSprite(useritem)
		if nil ~= render then
			return render 
		end
	end
	
	local str = string.format("Avatar%d.png", faceid)
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
	if nil ~= frame then
		self.m_spRender = cc.Sprite:createWithSpriteFrame(frame)
		self:setContentSize(self.m_spRender:getContentSize())
	end	
	self.m_fScale = self.m_headSize / SYS_HEADSIZE
	self:setScale(self.m_fScale)

	return self.m_spRender
end

function HeadSprite:haveCacheOrLocalFile(framename, filepath, bmpfile)
	--判断是否有缓存
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
	if nil ~= frame then
		if self:updateHeadByFrame(frame) then
			return true, self.m_spRender
		end
	else
		--判断是否有本地文件
		local path = filepath
		if cc.FileUtils:getInstance():isFileExist(path) then
			local customframe = nil
			if bmpfile then 
				customframe = createSpriteFrameWithBMPFile(path)
			else
				local sp = cc.Sprite:create(path)
				if nil ~= sp then
					customframe = sp:getSpriteFrame()
				end
			end
			if nil ~= customframe then
				--缓存帧
				local framename = self.m_useritem.dwUserID .. "_custom_" .. self.m_useritem.dwCustomID .. ".ry"    										
				cc.SpriteFrameCache:getInstance():addSpriteFrame(customframe, framename)
				--customframe:retain()
				if self:updateHeadByFrame(customframe) then
					return true, self.m_spRender
				end
			end
		end
	end
	return false
end

--下载头像
function HeadSprite:downloadFace(url, path, filename, onDownLoadSuccess)
	local downloader = CurlAsset:createDownloader("g_FaceDownloadListener",url)			
	if false == cc.FileUtils:getInstance():isDirectoryExist(path) then
		cc.FileUtils:getInstance():createDirectory(path)
	end			

	local function eventCustomListener(event)
        if nil ~= event.filename and 0 == event.code then
        	if nil ~= onDownLoadSuccess 
        		and type(onDownLoadSuccess) == "function" 
        		and nil ~= event.filename 
        		and type(event.filename) == "string" then
        		onDownLoadSuccess(event.filename)
        	end        	
        end
	end
	if nil ~= self.m_downListener then
		self:getEventDispatcher():removeEventListener(self.m_downListener)
		self.m_downListener = nil
	end
	self.m_downListener = cc.EventListenerCustom:create(FACEDOWNLOAD_LISTENER,eventCustomListener)
	self:getEventDispatcher():addEventListenerWithFixedPriority(self.m_downListener, 1)
	downloader:downloadFile(path, filename)
end

function HeadSprite:registerTouch( )
	local this = self
	local function onTouchBegan( touch, event )
		return this:isVisible() and this:isAncestorVisible(this) and this.m_bEnable
	end

	local function onTouchEnded( touch, event )
		local pos = touch:getLocation()
        pos = this:convertToNodeSpace(pos)
        local rec = cc.rect(0, 0, this:getContentSize().width, this:getContentSize().height)
        if true == cc.rectContainsPoint(rec, pos) then
            if type(this.m_fun) == "function" then
            	this.m_fun()
            end
        end        
	end

	local listener = cc.EventListenerTouchOneByOne:create()
	self.listener = listener
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
end

function HeadSprite:isAncestorVisible( child )
	if nil == child then
		return true
	end
	local parent = child:getParent()
	if nil ~= parent and false == parent:isVisible() then
		return false
	end
	return self:isAncestorVisible(parent)
end

function HeadSprite:onExit( )
	if nil ~= self.listener then
		local eventDispatcher = self:getEventDispatcher()
		eventDispatcher:removeEventListener(self.listener)
		self.listener = nil
	end

	if nil ~= self.m_downListener then
		self:getEventDispatcher():removeEventListener(self.m_downListener)
		self.m_downListener = nil
	end

	if nil ~= self.m_resizeListener then
		self:getEventDispatcher():removeEventListener(self.m_resizeListener)
		self.m_resizeListener = nil
	end

	if nil ~= self.m_httpUrlListener then
		self:getEventDispatcher():removeEventListener(self.m_httpUrlListener)
		self.m_httpUrlListener = nil
	end

	if nil ~= self.m_onBackgroundEnterListener then
		self:getEventDispatcher():removeEventListener(self.m_onBackgroundEnterListener)
		self.m_httpUrlListener = nil
	end
end

function HeadSprite:onEnterTransitionFinish()
	if self.m_bEnable and nil == self.listener then
		self:registerTouch()
	end
end

function HeadSprite:onClientNotify( event )
	local what = event.what
    local msg = event.msg
    if what == yl.CLIENT_MSG_BACKGROUND_ENTER then
    	print("HeadSprite:onClientNotify 后台切换")
    	self:updateHead( self.m_useritem )
    end
end

-- 获取头像
-- param[useritem] 用户数据
-- param[useCustomIDSave] 使用customid存储
function HeadSprite:getHeadSprite( useritem, useCustomIDSave )
	local this = self
	local isThirdParty = useritem.bThirdPartyLogin or false
	--系统头像
	local faceid = useritem.wFaceID or 0
	if isThirdParty and nil ~= useritem.szThirdPartyUrl and string.len(useritem.szThirdPartyUrl) > 0 then
		local filename = string.gsub(useritem.szThirdPartyUrl, "[/.:+]", "") .. ".png"
		if useCustomIDSave then
			filename = useritem.dwUserID .. "_custom_" .. useritem.dwCustomID .. ".ry"
		end

		--判断是否有缓存或者本地文件
		local framename = filename
		local path = device.writablePath .. "face/" .. useritem.dwUserID
		local filepath = path .. "/" .. filename
		local bHave, spRender = self:haveCacheOrLocalFile(framename, filepath, false) 
		if bHave then
			return spRender
		else
			print("HeadSprite 本地没有第三方头像文件")
			--保存头像信息
            local infotable = {}
			--判断是否有旧头像
			local infofile = path .. "/face.ry"
			if cc.FileUtils:getInstance():isFileExist(infofile) then
				local oldfile = cc.FileUtils:getInstance():getStringFromFile(infofile)
				local ok, datatable = pcall(function()
					return cjson.decode(oldfile)
				end)
				local vfilepath = ""
				if ok and type(datatable) == "table" then
					for k ,v in pairs(datatable) do
						if useCustomIDSave then
							-- 第三方头像, 使用 customid 规则存储, 则移除文件名不同且包含custom字段的文件
							if v ~= filename and string.match(v, "custom") then
								print("HeadSprite 第三方使用 custom 规则, 移除 ==> ", v)
								vfilepath = path .. "/" .. v
								cc.Director:getInstance():getTextureCache():removeTextureForKey(vfilepath)
								cc.FileUtils:getInstance():removeFile(vfilepath)
								cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(v)
							else
								table.insert(infotable, v)
							end
						else
							-- 第三方头像, 不是使用customid 规则存储, 则移除文件名不同且不包含custom字段的文件
							if v ~= filename and not string.match(v, "custom") then
								print("HeadSprite 第三方, 移除 ==> ", v)
								vfilepath = path .. "/" .. v
								cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(v)
								cc.Director:getInstance():getTextureCache():removeTextureForKey(vfilepath)
								cc.FileUtils:getInstance():removeFile(vfilepath)
							elseif v ~= filename then
								table.insert(infotable, v)
							end
						end
					end
				end
			end	

			--网络请求
			local url = useritem.szThirdPartyUrl
			self:downloadFace(url, path, filename, function(downloadfile)
				local selffile = filename
				if selffile == downloadfile then
					--print("HeadSprite downloadFace ==> ", path, url)
            		--导入新的图片信息
					table.insert(infotable, downloadfile)
            		local jsonStr = cjson.encode(infotable)
					cc.FileUtils:getInstance():writeStringToFile(jsonStr, infofile)
            		local filepath = path .. "/" .. selffile
					--清理旧资源	
					cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(framename)
					local customSp = cc.Sprite:create(filepath)
					if nil ~= customSp then
						local customframe = customSp:getSpriteFrame()
						--缓存帧  										
						cc.SpriteFrameCache:getInstance():addSpriteFrame(customframe, framename)
						if this and this.updateHeadByFrame and tolua.cast(this,"cc.Node") then
							this:updateHeadByFrame(customframe)
						end
					end
            	end
			end)			
		end
	elseif nil ~= useritem.dwCustomID and 0 ~= useritem.dwCustomID then
		--判断是否有缓存或者本地文件
		local framename = useritem.dwUserID .. "_custom_" .. useritem.dwCustomID .. ".ry"
		local filepath = device.writablePath .. "face/" .. useritem.dwUserID .. "/" .. framename

		local bHave, spRender = self:haveCacheOrLocalFile(framename, filepath)
		if bHave then
			return spRender
		else
			if self.m_bSendRequest then
				print("HeadSprite 已经发送请求")
				return
			end
			self.m_bSendRequest = true

			print("HeadSprite 本地没有 custom 头像文件")
			local path = device.writablePath .. "face/" .. useritem.dwUserID
			local filename = framename
			--保存头像信息
	        local infotable = {}
			--判断是否有旧头像
			local infofile = path .. "/face.ry"
			if cc.FileUtils:getInstance():isFileExist(infofile) then
				local oldfile = cc.FileUtils:getInstance():getStringFromFile(infofile)
				local ok, datatable = pcall(function()
					return cjson.decode(oldfile)
				end)
				local vfilepath = ""
				if ok and type(datatable) == "table" then
					for k ,v in pairs(datatable) do
						if v ~= filename and string.match(v, "custom") then
							print("custom 移除 ==> ", v)
							vfilepath = path .. "/" .. v
							cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(v)
							cc.Director:getInstance():getTextureCache():removeTextureForKey(vfilepath)
							cc.FileUtils:getInstance():removeFile(vfilepath)
						elseif v ~= filename then
							table.insert(infotable, v)
						end
					end
				end
			end
			local jsonStr = cjson.encode(infotable)
			cc.FileUtils:getInstance():writeStringToFile(jsonStr, infofile)

			-- 签名
			local formatdate = os.date("%Y%m%d%H%M%S", os.time())
			--print("userid sign time -- ", formatdate)
			local sign = md5(useritem.dwUserID .. "szwhkj56dt90gfpjskdw3p4qm" .. formatdate)
			--print("userid sign value -- ", sign)
			local signparam = "userid=" .. useritem.dwUserID .. "&time=" .. formatdate .. "&sign=" .. sign

			-- 网络监听
			local httpReq = function( event )
				local dwuserid = event.dwuserid
				if not this.m_useritem then
					return 
				end
				--print("event id, self id", dwuserid ,this.m_useritem.dwUserID)
				if dwuserid == this.m_useritem.dwUserID then
					this.m_bSendRequest = false
					--print("HeadSprite httpReq ", this.m_useritem.szNickName)
					local faceurl = event.faceurl
					this.m_useritem.bThirdPartyLogin = true
					this.m_useritem.szThirdPartyUrl = faceurl
					this:getHeadSprite(this.m_useritem, true)
				end
			end
			if nil ~= self.m_httpUrlListener then
				self:getEventDispatcher():removeEventListener(self.m_httpUrlListener)
				self.m_httpUrlListener = nil
			end
    		self.m_httpUrlListener = cc.EventListenerCustom:create(URLREQUEST_LISTENER,httpReq)
			self:getEventDispatcher():addEventListenerWithFixedPriority(self.m_httpUrlListener, 1)
			-- 网络请求
			local param = "customid=" .. useritem.dwCustomID .. "&" .. signparam
			--logFunc(param, true)
			appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/UserFace.ashx", "GET", param, function(jstable, jsdata)
				--dump(jstable, "HeadSprite:getHeadSprite")
				if type(jstable) == "table" then
					local data = jstable["data"]
					if type(data) == "table" then
						local valid = data["valid"]
						if valid then
							local faceurl = data["FaceUrl"]
							local dwuserid = tonumber(data["UserID"])
							if type(dwuserid) == "number" and type(faceurl) == "string" and string.len(faceurl) > 0 then
								local event = cc.EventCustom:new(URLREQUEST_LISTENER)
								event.faceurl = faceurl
								event.dwuserid = dwuserid
								cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
							end
						end
					end
				end
			end)
		end
	end
	return nil
end

--缓存头像
function HeadSprite.loadAllHeadFrame()
end

function HeadSprite.unloadAllHead()
end

--获取系统头像数量
function HeadSprite.getSysHeadCount()
	return 200
end

--缓存头像
HeadSprite.loadAllHeadFrame()

return HeadSprite
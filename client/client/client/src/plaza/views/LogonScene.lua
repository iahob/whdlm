--[[
  登录界面
      2015_12_03 C.P
      功能：登录/注册
--]]
if not yl then
	appdf.req(appdf.CLIENT_SRC.."plaza.models.yl")
end
if not GlobalUserItem then
	appdf.req(appdf.CLIENT_SRC.."plaza.models.GlobalUserItem")
end

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local privatemgr = ""
if cc.PLATFORM_OS_WINDOWS == targetPlatform then
    privatemgr = "client/src/plaza/models/private/models/PriRoom.lua" .. yl.WIN_SRC_SUFFIX
else
    privatemgr = "client/src/plaza/models/private/models/PriRoom.lua"
end
if cc.FileUtils:getInstance():isFileExist(privatemgr) then
    if not PriRoom then
        appdf.req(appdf.CLIENT_SRC .. "plaza.models.private.models.PriRoom")
        PriRoom:getInstance()
    end
end

--加载比赛模块
local Matchmgr = ""
if cc.PLATFORM_OS_WINDOWS == targetPlatform then
    Matchmgr = "client/src/plaza/models/match/plaza/src/models/MatchRoom.lua" .. yl.WIN_SRC_SUFFIX
else
    Matchmgr = "client/src/plaza/models/match/plaza/src/models/MatchRoom.lua"
end
if cc.FileUtils:getInstance():isFileExist(Matchmgr) then
    if not MatchRoom then
        appdf.req(appdf.CLIENT_SRC.."plaza.models.match.plaza.src.models.MatchRoom")
        MatchRoom:getInstance()
    end
end

-- 录像回放
local gamevideo = ""
if cc.PLATFORM_OS_WINDOWS == targetPlatform then
    gamevideo = "client/src/plaza/models/video/models/GameVideo.lua" .. yl.WIN_SRC_SUFFIX
else
    gamevideo = "client/src/plaza/models/video/models/GameVideo.lua"
end
if cc.FileUtils:getInstance():isFileExist(gamevideo) then
    if not GameVideo then
        appdf.req(appdf.CLIENT_SRC.."plaza.models.video.models.GameVideo")
        GameVideo:getInstance()
    end
end

local LogonScene = class("LogonScene", cc.load("mvc").ViewBase)
local ClientPopWait = appdf.req(appdf.EXTERNAL_SRC .. "ClientPopWait")
local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")
local LogonLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.LogonLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
appdf.req(appdf.CLIENT_SRC.."plaza.models.FriendMgr")

-- 网络监听
local HTTPNET_LISTENER = "__logonscene_httpnet_listener__"
-- logindata
local MSG_LOGINDATA = 101
-- logindatalater
local MSG_LOGINDATA_LATER = 102

--全局处理lua错误
cc.exports.g_LuaErrorHandle = function ()
	cc.exports.bHandlePopErrorMsg = true
	if isDebug() then
		print("debug return")
		return true
	else
		print("release return")
		return false
	end
end

-- 全局系统数据(wifi/信号/电量/ios客户端打开链接)
cc.exports.g_SystemInfo = function( info )
	print("onGetSystemInfo")
    local event = cc.EventCustom:new(appdf.CLIENT_NOTIFY)
    event.msg = info
    if info == "ios_openurl" then
        event.what = yl.CLIENT_MSG_IOS_OPENURL
    else
        event.what = yl.CLIENT_MSG_SYSTEMINFO
    end	
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

-- log函数
function logFunc( logmsg, bExtraData )
	bExtraData = bExtraData or false
	local msg = ""
	if type(logmsg) == "table" then
		local ok = false
		ok, msg = xpcall(function()
            return cjson.encode(logmsg)
        end, function(errormsg)
        	LogAsset:getInstance():logData(debug.traceback(),bExtraData)
        end)
	elseif type( logmsg ) == "string" then
		msg = logmsg
	end
	if nil ~= msg and "" ~= msg then
		LogAsset:getInstance():logData(msg, bExtraData)
	end
end

local postCacheTable = {}
local getCacheTable = {}
-- 统计函数
function analysisFunc( param, method )
	if nil == param then
		dump(param, "参数为空", 6)
		return
	end
	method = method or "get"
	if method == "get" or method == "GET" then
		table.insert(getCacheTable, param)
		if 10 == #getCacheTable then
			print("get 提交")
			local ok = false
			local msg = ""
			ok, msg = pcall(function()
	            return cjson.encode(getCacheTable)
	        end)
	        if ok then
	        	local url = "http://172.16.4.140/test_analysis_get.php?analysis=" .. msg
				LogAsset:getInstance():analysisDataByGet(url)
	        end
	        getCacheTable = {}
		end
	elseif method == "post" or method == "POST" then
		table.insert(postCacheTable, param)
		if 10 == #postCacheTable then
			print("post 提交")
			local ok = false
			local msg = ""
			ok, msg = pcall(function()
	            return cjson.encode(postCacheTable)
	        end)
	        if ok then
	        	msg = "analysis=" .. msg
	        	local url = "http://172.16.4.140/test_analysis.php"
				LogAsset:getInstance():analysisDataByPost(url, msg)
	        end
			postCacheTable = {}
		end
	end
end

--加载配置
function LogonScene.onceExcute()
	local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
	--文件日志
	LogAsset:getInstance():init(MultiPlatform:getInstance():getExtralDocPath(), true, true)
	--配置微信
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.WECHAT, yl.WeChat)
	--配置支付宝
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.ALIPAY, yl.AliPay)
	--配置竣付通
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.JFT, yl.JFT)
	--配置分享
	MultiPlatform:getInstance():configSocial(yl.SocialShare)
	--配置高德
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.AMAP, yl.AMAP)
	-- 验证key
	GlobalUserItem.packageMd5Key = md5(yl.VALIDATE)--md5(MultiPlatform:getInstance():getClientPackageName())
	-- 监听系统信息

	-- 共用资源
	cc.SpriteFrameCache:getInstance():addSpriteFrames("public/public.plist")
    local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/public.plist")
    local framesDict = dict["frames"]
    if nil ~= framesDict and type(framesDict) == "table" then
        for k,v in pairs(framesDict) do
            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
            if nil ~= frame then
                frame:retain()
            end
        end
    end
    -- loading动画
    AnimationMgr.loadAnimationFromFrame("ani_loading_%d.png", 0, 80, yl.COMMON_LOADING_ANI)
end
LogonScene.onceExcute()

-- 初始化界面
function LogonScene:onCreate()
    local this = self
	-- 触摸屏蔽
    ExternalFun.popupTouchFilter(30, true)
	ExternalFun.registerNodeEvent(self)
    local logonCallBack = function (result,message)
		this:onLogonCallBack(result,message)
	end
	self._logonFrame = LogonFrame:create(self,logonCallBack)

	--背景
    display.newSprite("background.png")
        :move(yl.WIDTH/2,yl.HEIGHT/2)
        :addTo(self)
	--读取配置
	GlobalUserItem.LoadData()
	--背景音乐
	ExternalFun.playPlazzBackgroudAudio()
    if GlobalUserItem.bVoiceAble then
        AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
    else
        AudioEngine.setMusicVolume(0)
    end
    
    if GlobalUserItem.bSoundAble then
        AudioEngine.setEffectsVolume(GlobalUserItem.nSound/100.00) 
    else
        AudioEngine.setEffectsVolume(0) 
    end

	-- 资源更新
	self.m_tabUpdateRes = {}
	--创建登录界面
	self._logonLayer = LogonLayer:create(self)
		:move(0,0)
		:addTo(self)

	-- 资源更新列表
	self.m_tabUpdateRes = {}
	self.m_tabUpdateRes["list"] = {}
end

local FIRST_LOGIN = true
-- 进入场景而且过渡动画结束时候触发。
function LogonScene:onEnterTransitionFinish()
    self.m_httpnetListener = cc.EventListenerCustom:create(HTTPNET_LISTENER,handler(self, self.onHttpReceiveEvent))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_httpnetListener, self)

    if FIRST_LOGIN then
        -- 存储原始游戏列表
        --GlobalUserItem.m_tabOriginGameList = self:getApp()._gameList
        -- 登陆数据
        self:getLoginData()
    else
        self._logonLayer:onGetNotice(GlobalUserItem.tabSystemNotice, false)
        self._logonLayer:autoLogon(false)
    end
	
    -- 解除屏蔽
    ExternalFun.dismissTouchFilter()
end

function LogonScene:onExit()
	if self._logonFrame:isSocketServer() then
		self._logonFrame:onCloseSocket()
        self._logonFrame = nil
	end
    if nil ~= self.m_httpnetListener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_httpnetListener)
        self.m_httpnetListener = nil
    end
end

function LogonScene:onHttpReceiveEvent( event )
    print("--- LogonScene:onHttpReceiveEvent ---")
    if nil == event then
        return
    end
    local jstable = event.jstable 
    if type(jstable) ~= "table" then
        dump(jstable, "LogonScene onHttpReceiveEvent param jstable invalid", 6)
        return
    end
    local msgWhat = event.msgWhat
    if msgWhat == MSG_LOGINDATA then
        self:_getLoginData(jstable)
    elseif msgWhat == MSG_LOGINDATA_LATER then
        self:_getLoginDataLater(jstable)
    end
end

-- 
function LogonScene:getLoginData()
	self:showPopWait()
    local this = self
    local url = yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx"
    local param = "action=GetMobileLoginData&"  .. GlobalUserItem:urlUnLoginSignParam()
    --logFunc(param, true)
	appdf.onHttpJsionTable(url, "GET", param, function(jstable, jsdata)
        dump(jstable, "getLoginData", 6)
        --logFunc(jstable, true)
 
        local event = cc.EventCustom:new(HTTPNET_LISTENER)
        event.sender = this
        event.jstable = jstable
        event.url = url
        event.msgWhat = MSG_LOGINDATA
        event.param = param
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)

        GlobalUserItem.setAwardData(clone(jstable.data.activityList), clone(jstable.data.systemNotice))
	end)
end

function LogonScene:_getLoginData(jstable)
    local this = self
    local msg = nil
    if type(jstable) == "table" then
        local data = jstable["data"]
        msg = jstable["msg"]
        dump(data, "_getLoginData" ,2)
        if type(data) == "table" then
            local valid = data["valid"]
            if valid then
                httpValid = true
                -- 系统配置
                GlobalUserItem.tabSystemConfig = data["systemConfig"]
                if nil ~= GlobalUserItem.tabSystemConfig.RealNameAuthentPresent and GlobalUserItem.tabSystemConfig.RealNameAuthentPresent > 0 then
                    GlobalUserItem.bEnableIdentify = true
                end

                if data["shareUrl"] and data["shareUrl"] ~= "" then
                    yl.PRIVATE_SHARE_URL = data["shareUrl"].."/Mobile/RoomLink.aspx"
                    yl.GROUP_SHARE_URL                    = data["shareUrl"] .. "/Mobile/GroupShareLink.aspx"
                end

                -- 客服信息
                GlobalUserItem.tabServiceCache = data["customerService"] or {}
                
                -- 系统公告
                local list = data["systemNotice"]
                if type(list) == "table" and #list > 0 then
                    table.sort(list, function(a, b)
                        return a.NoticeID < b.NoticeID
                    end)
                    GlobalUserItem.tabSystemNotice = list
                    this._logonLayer:onGetNotice(GlobalUserItem.tabSystemNotice, FIRST_LOGIN)
                else
                    -- 自动登录
                    this._logonLayer:autoLogon(FIRST_LOGIN)
                end

                --俱乐部信息
                GlobalUserItem.tabClubConfig = data["Groupconfig"] or {}

                -- 广告配置
                this:onGetAdConfig(data)
            end
        end
    end
    FIRST_LOGIN = false

    this:dismissPopWait()
    if type(msg) == "string" and "" ~= msg then
        showToast(this, msg, 1)
    end
    if not httpValid then
        -- 自动登录
        this._logonLayer:autoLogon(FIRST_LOGIN)
    end
end

function LogonScene:onGetSystemConfig( data )

end

function LogonScene:onGetAdConfig( data )
	local config = {}
	-- 广告位
	local adlist = data["adsList"] or {}
	config["adsList"] = {}
	-- 弹窗广告
	local popList = data["adsAlertList"] or {}
	config["adsAlertList"] = {}

	local adconfig = "ads/ad_config.json"
    local data = cc.FileUtils:getInstance():getStringFromFile(adconfig)
    local ok, dataTab =  pcall(function()
        return cjson.decode(data)
    end)
	self.m_tabUpdateRes["config"] = ""
	self.m_tabUpdateRes["configpath"] = device.writablePath .. "client/res/ads/ad_config.json"

    local oldad = {}
    local oldpop = {}
    if ok then
    	local oldconfig = dataTab["adsList"] or {}
    	oldad = oldconfig["list"] or {}
    	
    	oldconfig = dataTab["adsAlertList"] or {}
    	oldpop = oldconfig["list"] or {}
    end
    -- 轮播广告
    self:getUpdateList(oldad, adlist, 0, config)
    -- 弹窗广告
    self:getUpdateList(oldpop, popList, 1, config)

    GlobalUserItem.tabAdConfig = config
	local ok = false
	ok, self.m_tabUpdateRes["config"] = xpcall(function()
        return cjson.encode(config)
    end, function(errormsg)
    	LogAsset:getInstance():logData(debug.traceback(),true)
    end)
end

--微信登陆
function LogonScene:thirdPartyLogin(plat)
    local this = self
	self._tThirdData = {}
	self:showPopWait()
	self:runAction(cc.Sequence:create(cc.DelayTime:create(15), cc.CallFunc:create(function()
			this:dismissPopWait()
			this._logonFrame:onCloseSocket()
			this._logonLayer:onLogonFail()
		end)))
	local function loginCallBack ( param )
		this:dismissPopWait()
		if type(param) == "string" and string.len(param) > 0 then
			local ok, datatable = pcall(function()
				return cjson.decode(param)
			end)
			if ok and type(datatable) == "table" then
				local account = datatable["unionid"] or ""
				local nick = datatable["screen_name"] or ""
				this._szHeadUrl = datatable["profile_image_url"] or ""
				local gender = datatable["gender"] or ""
				local nGender = yl.GENDER_FEMALE
				if gender == "男" then
					nGender = yl.GENDER_MANKIND
				end
				this._Operate = this._logonFrame.OP_OTHERLOGIN
				this._tThirdData = 
				{
					szAccount = account,
					szNick = nick,
					cbGender = nGender,
					platform = yl.PLATFORM_LIST[plat],
				}
				print("微信登陆 ==> ", gender)

				this._logonLayer:onLogon()
				this._logonFrame:onLoginByThirdParty(account, nick, nGender, yl.PLATFORM_LIST[plat], this._szHeadUrl)
				return
			end
		end
		this._logonLayer:onLogonFail()
	end
	MultiPlatform:getInstance():thirdPartyLogin(plat, loginCallBack,self._logonLayer.index)
end

--登录注册回调
function LogonScene:onLogonCallBack(result,message)
	if result == logincmd.SUB_MB_LIST_FINISH then --成功
		--本地保存
		if self._Operate == self._logonFrame.OP_OTHERLOGIN then 				--微信登陆
			GlobalUserItem.tabAccountInfo.szThirdPartyUrl = self._szHeadUrl
            GlobalUserItem.md5Passwd = "D1FD5495E7B727081497CFCE780B6456" --md5(WHYK@foxuc.cn)
			GlobalUserItem.tabAccountInfo.bThirdPartyLogin = true
			GlobalUserItem.thirdPartyData = self._tThirdData
		else
			GlobalUserItem.bAutoLogon = self._bAuto
			GlobalUserItem.bSavePassword = self._bSave
			GlobalUserItem.onSaveAccountConfig()
		end
		self:enterPlaza()
	elseif result == -1 then --失败
		if type(message) == "string" and message ~= "" then
			showToast(self,message,2,cc.c4b(250,0,0,255));
		end
		self._logonLayer:onLogonFail()
	elseif result == 10 then --重复绑定
	end
end

--显示等待
function LogonScene:showPopWait()
	if not self._popWait  then
		self._popWait = ClientPopWait.PopWait:create()
			:show(self,"请稍候！")
	end
end

--关闭等待
function LogonScene:dismissPopWait()
	if self._popWait ~= nil then
		self._popWait:dismiss()
		self._popWait = nil
	end
end

function LogonScene:enterPlaza()
    local this = self
	self._logonLayer:onLogonFinish(cc.CallFunc:create(function()
		if yl.HTTP_SUPPORT then
            this:getLoginDataLater()
		else
			this:_enterPlaza()
		end
	end))
end

function LogonScene:_enterPlaza()
    GlobalUserItem.m_tabOriginGameList =clone(GlobalUserItem.gamelist)


    --整理代理游戏列表
    if table.nums(self._logonFrame.m_angentServerList) > 0 then
        self:arrangeGameList(self._logonFrame.m_angentServerList)
    else
        self:getApp()._gameList = GlobalUserItem.m_tabOriginGameList
    end
    -- 游戏列表
    local gamelist = self:getApp()._gameList
    dump(gamelist, "jack", 6)
    if 1 == #gamelist or 0 ~= GlobalUserItem.tabAccountInfo.dwLockKindID then
        local tmpkind = tonumber(gamelist[1]._KindID) or 0
        if 0 ~= tmpkind then
            GlobalUserItem.nCurGameKind = tmpkind
        end
    end
    -- 是否单个
    --GlobalUserItem.bSingleGame = ( 1 == #self:getApp()._gameList )

	-- 检查资源
	if not self:updateGameResource() then
		--进入游戏列表
		self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
		FriendMgr:getInstance():reSetAndLogin()
	end
end

-- 登陆成功数据
function LogonScene:getLoginDataLater()
    local this = self
    -- 请求推广配置
    local url = yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx"
    local param = "action=getmobileloginlater&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(url, "GET", param, function(jstable, jsdata)
        --dump(jstable, "getLoginDataLater", 6)
        local event = cc.EventCustom:new(HTTPNET_LISTENER)
        event.sender = this
        event.jstable = jstable
        event.url = url
        event.msgWhat = MSG_LOGINDATA_LATER
        event.param = param
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
    end)
end

function LogonScene:_getLoginDataLater( jstable )
    local this = self
    local msg = nil
    if type(jstable) == "table" then
        local data = jstable["data"]
        dump(jstable, "登录时获取数据", 5)
        msg = jstable["msg"]
        if type(data) == "table" then
            local valid = data["valid"]
            if valid then
                -- 推广信息
                local spreadinfo = {}
                -- 推广好友
                spreadinfo.friendcount = data["friendcount"] or 0
                -- 推广链接
                spreadinfo.sharelink = data["sharelink"] or yl.HTTP_URL

                GlobalUserItem.isOpenGroupView = data["isOpenGroupView"] or 0

                -- 推广礼包
                spreadinfo.list = data["spreadlist"] or {}
                GlobalUserItem.tabSpreadInfo = spreadinfo
                if type(GlobalUserItem.tabSpreadInfo["list"]) == "table" and #GlobalUserItem.tabSpreadInfo["list"] > 0 then
                    table.sort(GlobalUserItem.tabSpreadInfo["list"], function(a,b)
                        return a.SpreadNum < b.SpreadNum
                    end)
                end
                -- 屏蔽登陆即领取
                --[[-- 奖励判断
                local validcount = GlobalUserItem.tabSpreadInfo["friendcount"] or 0
                -- 礼包配置
                for k,v in pairs(GlobalUserItem.tabSpreadInfo["list"]) do
                    -- 有效推广大于礼包人数、未领取
                    if validcount >= v.SpreadNum and not v.Flag then
                        -- 加入推送列表
                        local reward = {}
                        -- 奖励类型
                        reward.ntype = yl.REWARD_LIST.SPREAD_REWARD
                        -- 奖励标识
                        reward.nconfigid = v.ConfigID
                        -- 奖励钻石
                        reward.ldiamond = v.PresentDiamond or 0
                        -- 奖励道具
                        reward.npropertyid = v.PresentPropID or 0
                        -- 道具数量
                        reward.npropertycount = v.PresentPropNum or 0
                        -- 推广人数
                        local count = v.SpreadNum or 0
                        -- 奖励内容
                        if 0 ~= reward.npropertyid then
                            reward.sztips = "恭喜您成功推广" .. count .. "个玩家, 获赠" .. reward.ldiamond .. "钻石和" .. reward.npropertycount .. "个大喇叭!"
                        else
                            reward.sztips = "恭喜您成功推广" .. count .. "个玩家, 获赠" .. reward.ldiamond .. "钻石!"
                        end
                        table.insert(GlobalUserItem.tabRewardList, reward)
                        break
                    end
                end]]

                -- 排行榜信息
                -- local list = data["ranklist"]
                -- if type(list) == "table" and #list > 0 then
                --     local tmplist = {}
                --     -- 整理列表
                --     for k,v in pairs(list) do
                --         if nil == tmplist[v.TypeID] then
                --             tmplist[v.TypeID] = {}
                --         end
                --         table.insert(tmplist[v.TypeID], v)
                --     end
                --     for k,v in pairs(tmplist) do
                --         -- 为空判断
                --         if 0 ~= #v then
                --             -- 加入推送列表
                --             local reward = {}
                --             -- 奖励类型
                --             reward.ntype = yl.REWARD_LIST.RANK_REWARD
                --             reward.tablisttype = k
                --             reward.tablist = v
                --             table.insert(GlobalUserItem.tabRewardList, reward)
                --         end
                --     end
                -- end

                --排行榜奖励
                local list = data["wealthrank"]
                if type(list) == "table" and #list > 0 then
                    for i=1,#list do
                        local rewardInfo = {}
                        rewardInfo.rank = list[i]["RankNum"]
                        rewardInfo.dwUserID = list[i]["UserID"]
                        rewardInfo.wFaceID = list[i]["FaceID"]
                        rewardInfo.dwGameID = list[i]["GameID"]
                        rewardInfo.lScore = list[i]["Gold"]
                        rewardInfo.lDiamond = list[i]["Diamond"]
                        rewardInfo.szNickName = list[i]["NickName"]
                        rewardInfo.lRankValue = list[i]["RankValue"] --有效值（金币数/胜局）
                        rewardInfo.DateID = list[i]["DateID"]           --日期
                        rewardInfo.TypeID = list[i]["TypeID"]           --类型
                        
                        local faceurl = list[i]["FaceUrl"] or ""
                        if type(faceurl) == "string" and "" ~= faceurl then
                            rewardInfo.bThirdPartyLogin = true
                            rewardInfo.szThirdPartyUrl = faceurl
                        end
                        
                        table.insert(GlobalUserItem.tabWeathRewardList, rewardInfo)
                    end
                end

                local list = data["wincountrank"]
                if type(list) == "table" and #list > 0 then
                    for i=1,#list do
                        local rewardInfo = {}
                        rewardInfo.rank = list[i]["RankNum"]
                        rewardInfo.dwUserID = list[i]["UserID"]
                        rewardInfo.wFaceID = list[i]["FaceID"]
                        rewardInfo.dwGameID = list[i]["GameID"]
                        rewardInfo.lScore = list[i]["Gold"]
                        rewardInfo.lDiamond = list[i]["Diamond"]
                        rewardInfo.szNickName = list[i]["NickName"]
                        rewardInfo.lRankValue = list[i]["RankValue"]  --有效值（金币数/胜局）
                        rewardInfo.DateID = list[i]["DateID"]           --日期
                        rewardInfo.TypeID = list[i]["TypeID"]           --类型

                        local faceurl = list[i]["FaceUrl"] or ""
                        if type(faceurl) == "string" and "" ~= faceurl then
                            rewardInfo.bThirdPartyLogin = true
                            rewardInfo.szThirdPartyUrl = faceurl
                        end
                        
                        table.insert(GlobalUserItem.tabWinTimesRewardList, rewardInfo)
                    end
                end
                --dump(GlobalUserItem.tabWinTimesRewardList, "@@@@@@tabWinTimesRewardList", 7)

                -- 注册奖励
                local bRegGrant = data["hasGrant"]--tonumber(data["registergrant"])
                if nil ~= bRegGrant and bRegGrant then
                    -- 加入推送列表
                    local reward = {}
                    -- 奖励类型
                    reward.ntype = yl.REWARD_LIST.REGISTER_REWARD
                    -- 奖励钻石
                    reward.ldiamond = tonumber(data["grantDiamond"])
                    -- 奖励金币
                    reward.lgold = tonumber(data["grantGold"])
                    -- 奖励内容
                    local str = ""
                    if reward.ldiamond > 0 then
                        str = str..reward.ldiamond.."钻石 "
                    end
                    if reward.lgold > 0 then
                        str = str..reward.lgold.."金币 "
                    end

                    reward.sztips = "恭喜您获得 " .. str .. "注册奖励!"
                    table.insert(GlobalUserItem.tabRewardList, reward)
                end

                local rankReward = data["rankconfig"]
                if type(rankReward) == "table" and #rankReward > 0 then

                    for i=1,#rankReward do
                        local rewardInfo = {}
                        rewardInfo.typeID = rankReward[i]["TypeID"]
                        rewardInfo.rankType = rankReward[i]["RankType"]
                        rewardInfo.Gold = rankReward[i]["Gold"]
                        rewardInfo.Diamond = rankReward[i]["Diamond"]
                        if rewardInfo.typeID == 1 then
                            table.insert(GlobalUserItem.tabWeathRewardConfigList, rewardInfo)
                        elseif rewardInfo.typeID == 2 then
                            table.insert(GlobalUserItem.tabWinTimesRewardConfigList, rewardInfo)
                        end
                    end
                end
                --dump(GlobalUserItem.tabWeathRewardConfigList, "@@@@@@tabWeathRewardConfigList", 7)
                --dump(GlobalUserItem.tabWinTimesRewardConfigList, "@@@@@@tabWinTimesRewardConfigList", 7)
            end
        end
    end

    if type(msg) == "string" and "" ~= msg then
        showToast(this, msg, 2)
    else
        this:_enterPlaza()
    end
end

function LogonScene:updateGameResource()
	self:getApp()._updateGroup = {}
	local needUpdate = false
	-- 单游戏更新
	local gamelist = self:getApp()._gameList
    
    print("updateGameResource----------------------------------->", #gamelist, GlobalUserItem.tabAccountInfo.dwLockKindID, GlobalUserItem.nCurGameKind)
    if (#gamelist == 1 and yl.SINGLE_GAME_MODOLE) or 0 ~= GlobalUserItem.tabAccountInfo.dwLockKindID then
        GlobalUserItem.nCurGameKind = GlobalUserItem.tabAccountInfo.dwLockKindID
        for k,v in pairs(gamelist) do
            local kindid = tonumber(v._KindID)
            if kindid == tonumber(GlobalUserItem.nCurGameKind) then
                local updategame = v
                local version = tonumber(self:getApp():getVersionMgr():getResVersion(kindid))
                if not version or updategame._ServerResVersion > version then
                    print("update game")

                    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
                    if cc.PLATFORM_OS_WINDOWS == targetPlatform and yl.DISABLE_WIN_UPDATE then
                        print("LogonScene win32 跳过更新")
                        self:getApp()._version:setResVersion(updategame._ServerResVersion, updategame._KindID)
                        needUpdate = false
                    else
                        --进入更新场景
                        local updateinfo = {}
                        updateinfo.localconfig = device.writablePath.."game/" .. updategame._Module.."res/filemd5List.json"
                        updateinfo.remoteconfig = self:getApp()._updateUrl .. "/game/".. updategame._Module.."res/filemd5List.json"
                        updateinfo.downurl = self:getApp()._updateUrl .. "/game/" .. updategame._Type .. "/"
                        updateinfo.downpath = device.writablePath .. "game/" .. updategame._Type .. "/"
                        local targetPlatform = cc.Application:getInstance():getTargetPlatform()
                        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
                            updateinfo.downpath = device.writablePath .. "download/game/" .. updategame._Type .. "/"
                        end
                        updateinfo.ismd5 = true
                        updateinfo.updateversion = true
                        updateinfo.remoteversion = updategame._ServerResVersion
                        updateinfo.updatekindid = updategame._KindID
                        table.insert(self:getApp()._updateGroup, updateinfo)
                        needUpdate = true
                    end
                end
            end
        end
    end
	-- 非md5控制更新
	if 0 ~= #self.m_tabUpdateRes["list"] then
		local updateinfo = {}
		updateinfo["list"] = self.m_tabUpdateRes["list"]
		updateinfo["config"] = self.m_tabUpdateRes["config"]
		updateinfo["localconfig"] = self.m_tabUpdateRes["configpath"]
		updateinfo.ismd5 = false
		updateinfo.updateversion = false

		table.insert(self:getApp()._updateGroup, updateinfo)
		needUpdate = true
	end


	--进入更新场景
	if needUpdate then
		self:getApp():enterSceneEx(appdf.CLIENT_SRC .. "plaza.views.ResUpdateScene", "FADE", 1)
	end

	return needUpdate
end

function LogonScene:getUpdateList( oldlist, newlist, nType, config )
	local savepath = ""
	if 0 == nType then
		savepath = device.writablePath .. "client/res/ads/"
	elseif 1 == nType then
		savepath = device.writablePath ..  "client/res/ads/alert/"
	end
	local list = {}
	-- 资源删除
	for k,v in pairs(oldlist) do
		local del = true
		for newk, newv in pairs(newlist) do
			local filename = ExternalFun.getFileName(newv.ResourceURL)
			if filename == v.res then
				del = false
				break
			end
		end
		if del then
			local delpath = savepath .. v.res
			if cc.FileUtils:getInstance():isFileExist(delpath) then
				print("removefile:", v.res)
				cc.FileUtils:getInstance():removeFile(delpath)
			end
		end
	end
	
	local count = 0
	-- 下载判断
	for newk, newv in pairs(newlist) do
		local filename = ExternalFun.getFileName(newv.ResourceURL)
		local needupdate = true
		for oldk,oldv in pairs(oldlist) do
			if oldv.res == filename then
				needupdate = false
			end
		end
		if needupdate then
			print("updatefile:", filename)
			-- 先移除
			local delpath = savepath .. filename
			if cc.FileUtils:getInstance():isFileExist(delpath) then
				cc.FileUtils:getInstance():removeFile(delpath)
			end
			
			local updateitem = {}
			updateitem["savepath"] = savepath
			updateitem["name"] = filename
			updateitem["downloadurl"] = newv.ResourceURL -- yl.IMAGE_URL .. newv.ResourceURL
			table.insert(self.m_tabUpdateRes["list"], updateitem)
		end

		local configitem = {}
		configitem["SortID"] = newv.SortID
		configitem["res"] = filename
		configitem["link"] = newv.LinkURL
		table.insert(list, configitem)

		count = count + 1
	end
	local list2 = {}
	list2.count = count
	list2.list = list
	if 0 == nType then
		config["adsList"] = list2
	elseif 1 == nType then
		config["adsAlertList"] = list2
	end
end

--整理游戏列表
--[[
serverList = 
{
    KindID = {KindID, SortID},
    KindID2 = {KindID2, SortID},
}
]]
function LogonScene:arrangeGameList(serverList) 
    local originList = GlobalUserItem.m_tabOriginGameList
    local newList = {}
    for k,v in pairs(originList) do
        local serverGame = serverList[tonumber(v._KindID)]
        if nil ~= serverGame then
            v._SortId = serverGame.SortID
            table.insert(newList, v)
        end
    end
    table.sort(newList, function(a, b)
        return a._SortId < b._SortId
    end)
    self:getApp()._gameList = newList
end

return LogonScene
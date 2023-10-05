local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
GlobalUserItem = GlobalUserItem or {}

-- 原始游戏列表
GlobalUserItem.m_tabOriginGameList = {}
-- 排行榜数据
GlobalUserItem.m_tabCacheRankListData = nil
-- 包名md5验证key
GlobalUserItem.packageMd5Key = ""
-- 允许房卡
GlobalUserItem.bEnableRoomCard = true
-- 下载管理
GlobalUserItem.tabUpdater = {}
-- 广告配置
GlobalUserItem.tabAdConfig = {}
-- 客户信息缓存	
GlobalUserItem.tabServiceCache =
{
	Phone = "",
	WeiXin = "",
	QQ = "",
	Link = "",
} 	
-- 系统配置		
GlobalUserItem.tabSystemConfig =
{
	IsOpenMall = 0, -- 是否开启商城
  	IsPayBindSpread = 0, -- 充值是否需要绑定推广人
  	BindSpreadPresent = 0, -- 绑定推广人后钻石奖励
  	RankingListType = 0, -- 排行榜类型(0 无  1 财富排行榜  2消耗排行榜  3财富和消耗 4 战绩  5 财富和战绩  6 消耗和战绩  7 全部)
  	PayChannel = 0, -- 充值渠道( 0-无，1-微信官方，2-支付宝官方，3-微信和支付宝官方)
  	DiamondBuyPropCount = 10, -- 钻石兑换道具比例
  	RealNameAuthentPresent = 0, -- 实名认证奖励
  	EffectiveFriendGame = 0, -- 有效好友需玩房卡游戏局数
  	IOSNotStorePaySwitch = 1, -- 非appstore包是否开启第三方充值(1表示不开启)
  	GoldBuyPropCount = 0, -- 金币购买道具消耗数值
  	EnjoinInsure = 1, -- 0表示开启银行服务,1表示关闭
  	TransferStauts = 1, --0表示开启转账服务,1表示关闭
  	MobileBattleRecordMask = 0, --
  	IsOpenGameSignIn = 0, 	--是否开启每日签到
  	IsOpenRedemptionCenter = 0, 	--兑换中心开馆 0标识开启，1标识关闭
}
-- 系统公告
GlobalUserItem.tabSystemNotice = 
{
	
}
-- 激活认证
GlobalUserItem.bEnableIdentify 							= false

GlobalUserItem.tabClubConfig = 
{
	MaxMenberCount = 0, -- 最大
  	MaxCreateGroupCount = 0, -- 最大创建亲友圈个数
  	CreateGroupTakeIngot = 0, -- 创建亲友圈需要携带
  	CreateGroupDeductIngot = 0, -- 创建亲友圈需要消耗
  	MaxJoinGroupCount = 0, 	-- 最大参与个数
  	GroupPayType = 0, -- 俱乐部支付方式
  	GroupPayTypeCChange = 0, -- 俱乐部支付方式
  	GroupRoomType = 0, -- 俱乐部支持创房类型
}

--重置数据
function GlobalUserItem.reSetData()
	GlobalUserItem.bVistor 									= nil		
	GlobalUserItem.bWeChat									= false
	
	-- 账户信息
	GlobalUserItem.tabAccountInfo 							=  				-- 账号信息 = CMD_MB_LogonSuccess
	{
		szThirdPartyUrl = "",												-- 第三方头像url
		bThirdPartyLogin = false,											-- 是否第三方登陆
	}		
	-- md5密码
	GlobalUserItem.md5Passwd 								= ""	
	-- 附加信息
	GlobalUserItem.tabPlusInfo								=  				
	{
		bQueryInfo = false,													-- 已请求数据
		szSign = "此人很懒，没有签名", 										-- 签名
		cbMemberOrder = 0,
		szQQNumber = "", 													-- qq号码
		szEmailAddress = "",												-- 邮箱地址
		szSeatPhone = "",													-- 座机
		szMobilePhone = "",													-- 手机
		szTrueName = "",													-- 真实姓名
		szAddress = "",														-- 联系地址
		szPassportID = "",													-- 身份ID
		szSpreaderAccount = "" 												-- 推广员
	}			
	GlobalUserItem.thirdPartyData 							= {}			-- 第三方登陆数据
	
	GlobalUserItem.LogonTime								= 0				-- 登陆时间
	GlobalUserItem.nLargeTrumpetCount						= 0				-- 大喇叭数量

	-- 当前游戏信息
	GlobalUserItem.nCurGameKind								= 0			-- 游戏标识
	GlobalUserItem.nCurRoomIndex 							= -1 			-- 房间索引

	GlobalUserItem.roomlist 								= {}			-- 房间列表

	-- 音效配置
	GlobalUserItem.bVoiceAble								= true 			-- 背景音乐
	GlobalUserItem.bSoundAble								= true 			-- 游戏音效
	GlobalUserItem.nSound									= 100 			-- 音效音量
	GlobalUserItem.nMusic									= 100 			-- 音乐音量
	
	GlobalUserItem.m_tabEnterGame							= nil 			--保存进入游戏的数据
	GlobalUserItem.dwServerRule								= 0				--房间规则
	GlobalUserItem.bEnterGame								= false 		--进入游戏
	GlobalUserItem.bIsAngentAccount							= false 		--是否是代理商帐号	

	GlobalUserItem.bJftPay									= false
	GlobalUserItem.bShopPay 								= false
	GlobalUserItem.bPrivateRoom 							= false 		-- 私人房
	GlobalUserItem.bWaitQuit 								= false 		-- 等待退出
	GlobalUserItem.bAutoConnect 							= true 			-- 是否自动断线重连(游戏切换到主页再切换回)

	GlobalUserItem.cbLockMachine 							= 0 			-- 是否锁定设备
	GlobalUserItem.szIpAdress 								= "" 			-- ip地址
	GlobalUserItem.tabCoordinate 							= {lo = 360.0, la = 360.0} 		-- 坐标
	GlobalUserItem.bUpdateCoordinate 						= false 		-- 是否更新坐标
	GlobalUserItem.szLocation								= "暂未获取到位置" 			-- ip地址

	GlobalUserItem.szCopyRoomId 							= "" 			-- 复制房间id 	

	GlobalUserItem.bVideo 									= false 		-- 回放
	GlobalUserItem.dwVideoUserID							= 0				-- 回放主视图玩家
 		
	GlobalUserItem.bSpreadTipShow 							= false 			-- 推广提示显示	
	GlobalUserItem.tabSpreadInfo 							= {} 			-- 推广配置

	GlobalUserItem.tabRewardList 							= {}			-- 奖励列表
	GlobalUserItem.bSingleGame 								= false 		-- 单个游戏

	GlobalUserItem.bPrivateGoldRoom							= false			--金币私人房

	GlobalUserItem.tabWeathRewardList 						= {}			-- 财富奖励列表
	GlobalUserItem.tabWinTimesRewardList 					= {}			-- 赢局奖励列表

	GlobalUserItem.tabWinTimesRewardConfigList				= {}			-- 财富奖励配置列表
	GlobalUserItem.tabWeathRewardConfigList					= {}			-- 赢局奖励配置列表


	GlobalUserItem.tabDailyInfo								= {}			-- 签到信息
	GlobalUserItem.tabDailyRewards							= {}			-- 签到免费礼物列表
	GlobalUserItem.tabCumulativeRewards						= {}			-- 签到累计礼物列表

	GlobalUserItem.tabPropertyItem							= {}			-- 道具列表

	GlobalUserItem.cbMatchType 							= -1 			-- -1：没有比赛，0:定时赛，1：即时赛，
	GlobalUserItem.bIsNeedConnect							= false
	GlobalUserItem.m_tabGameFrame							= nil
	GlobalUserItem.gamelist								= {}    --游戏列表

	GlobalUserItem.dwCurRoomGroupID							= 0
	GlobalUserItem.tabCurGroupInfo							= {} --当前俱乐部的信息
	GlobalUserItem.tabGroupUserInfo							= {} --我在当前俱乐部的信息
	GlobalUserItem.bLookOn								= false


end

--读取配置
function GlobalUserItem.LoadData()
	--声音设置
	GlobalUserItem.bVoiceAble = cc.UserDefault:getInstance():getBoolForKey("vocieable",true)
	GlobalUserItem.bSoundAble = cc.UserDefault:getInstance():getBoolForKey("soundable", true)
	--音量设置
	GlobalUserItem.nSound = cc.UserDefault:getInstance():getIntegerForKey("soundvalue",50)
	GlobalUserItem.nMusic = cc.UserDefault:getInstance():getIntegerForKey("musicvalue",50)

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

	--账号密码
	local sp = ""
	local tmpInfo = readByDecrypt(sp.."user_gameconfig.plist","code_1")
	--检验
	if tmpInfo ~= nil and #tmpInfo >32 then
		local md5Test = string.sub(tmpInfo,1,32)
		tmpInfo = string.sub(tmpInfo,33,#tmpInfo)
		if md5Test ~= md5(tmpInfo) then
			print("test:"..md5Test.."#"..tmpInfo)
			tmpInfo = nil
		end
	else
		tmpInfo = nil
	end

	if tmpInfo ~= nil then
		GlobalUserItem.szAccount = tmpInfo
		tmpInfo = readByDecrypt(sp.."user_gameconfig.plist","code_2")
		--检验
		if tmpInfo ~= nil and #tmpInfo >32 then
			local md5Test = string.sub(tmpInfo,1,32)
			tmpInfo = string.sub(tmpInfo,33,#tmpInfo)
			if md5Test ~= md5(tmpInfo) then
				print("test:"..md5Test.."#"..tmpInfo)
				tmpInfo = nil
			end
		else
			tmpInfo = nil
		end
		if tmpInfo ~= nil then
			GlobalUserItem.szPassword = tmpInfo
			GlobalUserItem.bSavePassword = true
		else
			GlobalUserItem.szPassword = ""
			GlobalUserItem.bSavePassword = false
		end
	else
		GlobalUserItem.szAccount = ""
		GlobalUserItem.szPassword = ""
		GlobalUserItem.bAutoLogon = false
		GlobalUserItem.bSavePassword = false
	end
end
GlobalUserItem.reSetData()

--保存配置参数
function GlobalUserItem.onSaveAccountConfig()
	local szSaveInfo = ""
	if GlobalUserItem.szAccount ~= nil and #GlobalUserItem.szAccount >0 then
	 	szSaveInfo = md5(GlobalUserItem.szAccount)..GlobalUserItem.szAccount
	end
	local sp = ""
	saveByEncrypt(sp.."user_gameconfig.plist","code_1",szSaveInfo)
	szSaveInfo = ""
	if GlobalUserItem.bSavePassword == true and GlobalUserItem.szPassword ~= nil and #GlobalUserItem.szPassword >0 then
		szSaveInfo = md5(GlobalUserItem.szPassword)..GlobalUserItem.szPassword
	end
	saveByEncrypt(sp.."user_gameconfig.plist","code_2",szSaveInfo)
	cc.UserDefault:getInstance():setBoolForKey("autologon",GlobalUserItem.bAutoLogon)
end

function GlobalUserItem.setSoundAble(able)
	GlobalUserItem.bSoundAble = able
	if true == able then
		GlobalUserItem.setEffectsVolume(50)
		AudioEngine.setEffectsVolume(GlobalUserItem.nSound/100.00)
	else
		GlobalUserItem.setEffectsVolume(0)
		AudioEngine.setEffectsVolume(0)
	end
	cc.UserDefault:getInstance():setBoolForKey("soundable",GlobalUserItem.bSoundAble)
end

function GlobalUserItem.setVoiceAble(able)
	GlobalUserItem.bVoiceAble = able
	if GlobalUserItem.bVoiceAble == true then
		GlobalUserItem.setMusicVolume(50)
		AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
	else		
		GlobalUserItem.setMusicVolume(0)
		AudioEngine.setMusicVolume(0)
	end
	cc.UserDefault:getInstance():setBoolForKey("vocieable",GlobalUserItem.bVoiceAble)
end

function GlobalUserItem.setMusicVolume(music) 
	local tmp = music 
	if tmp >100 then
		tmp = 100
	elseif tmp < 0 then
		tmp = 0
	end
	AudioEngine.setMusicVolume(tmp/100.0) 
	GlobalUserItem.nMusic = tmp
	cc.UserDefault:getInstance():setIntegerForKey("musicvalue",GlobalUserItem.nMusic)
end

function GlobalUserItem.setEffectsVolume(sound) 
	local tmp = sound 
	if tmp >100 then
		tmp = 100
	elseif tmp < 0 then
		tmp = 0
	end
	AudioEngine.setEffectsVolume(tmp/100.00) 
	GlobalUserItem.nSound = tmp
	cc.UserDefault:getInstance():setIntegerForKey("soundvalue",GlobalUserItem.nSound)
end

--查询房间数目(普通房間)
function GlobalUserItem.GetGameRoomCount(nKindID)
	local checkKind
	if not nKindID then
		checkKind = GlobalUserItem.nCurGameKind
	else
		checkKind = tonumber(nKindID)
	end
	for i = 1,#GlobalUserItem.roomlist do
		local list = GlobalUserItem.roomlist[i]
		if tonumber(list[1]) == checkKind then
			if not list[2] then
				break
			end
			local nCount = 0
			local serverList = list[2]
			for k,v in pairs(serverList) do
				if v.wServerType ~= yl.GAME_GENRE_PERSONAL and v.wServerType ~= yl.GAME_GENRE_MATCH then
					nCount = nCount + 1
				end				
			end
			return nCount
		end
	end
	return 0
end




--获取房间信息(普通房间)
function GlobalUserItem.GetGameRoomInfo(wServerID, nKindID)
	local checkKind
	if not nKindID then
		checkKind = GlobalUserItem.nCurGameKind
	else
		checkKind = tonumber(nKindID)
	end
	for i = 1,#GlobalUserItem.roomlist do
		local list = GlobalUserItem.roomlist[i]
		if tonumber(list[1]) == checkKind then
			if not list[2] then
				break
			end
			local nCount = 0
			local serverList = list[2]
			for k,v in pairs(serverList) do
				if v.wServerID == wServerID and v.wServerType ~= yl.GAME_GENRE_PERSONAL and v.wServerType ~= yl.GAME_GENRE_MATCH then
					return v
				end				
			end
		end
	end
	return nil
end

--查询房间信息
function GlobalUserItem.GetRoomInfo(index,nKindID)
	local checkKind 
	if not nKindID then
		checkKind = GlobalUserItem.nCurGameKind
	else
		checkKind = tonumber(nKindID)
	end
	if not checkKind then
		print("not checkKind")
		return nil
	end

	local roomIndex = index
	if not roomIndex then
		 roomIndex = GlobalUserItem.nCurRoomIndex
	end
	if not roomIndex then 
		print("not roomIndex")
		return nil
	end
	if roomIndex <1 then
		print("roomIndex <1")
		return nil
	end
	for i = 1,#GlobalUserItem.roomlist do
		local list = GlobalUserItem.roomlist[i]
		if tonumber(list[1]) == tonumber(checkKind) then
			local listinfo = list[2]
			if not listinfo then
				print("not listinfo")
				return nil
			end
			if roomIndex > #listinfo then 
				print("roomIndex > #listinfo")
				return nil
			end
			return listinfo[roomIndex]
		end
	end
end

-- 非房卡房間號
function GlobalUserItem.normalRoomIndex(nKindID)
	if not nKindID then 
		print("not kindid")
		return nil
	end
	for i = 1,#GlobalUserItem.roomlist do
		local list = GlobalUserItem.roomlist[i]
		if tonumber(list[1]) == tonumber(nKindID) then
			local listinfo = list[2]
			if not listinfo then
				print("not listinfo")
				return nil
			end
			for k,v in pairs(listinfo) do
				if v.wServerType ~= yl.GAME_GENRE_PERSONAL then
					return v._nRoomIndex
				end	
			end
			return nil
		end
	end
end

--查询房间数目
function GlobalUserItem.GetRoomCount(nKindID)
	local checkKind
	if not nKindID then
		checkKind = GlobalUserItem.nCurGameKind
	else
		checkKind = tonumber(nKindID)
	end
	for i = 1,#GlobalUserItem.roomlist do
		local list = GlobalUserItem.roomlist[i]
		if tonumber(list[1]) == checkKind then
			if not list[2] then
				break
			end
			local nCount = 0
			local serverList = list[2]
			for k,v in pairs(serverList) do
				nCount = nCount + 1			
			end
			return nCount
		end
	end
	return 0
end

function GlobalUserItem:readScore(dataBuffer)
    if self._int64 == nil then
       self._int64 = Integer64.new():retain()
    end
    dataBuffer:readscore(self._int64)
    return self._int64:getvalue()
end

-- 签名参数(未登陆)
function GlobalUserItem:urlUnLoginSignParam()
	local formatdate = os.date("%Y%m%d%H%M%S", os.time())
	print("unlogin sign time -- ", formatdate)
	local sign = md5("szwhkj56dt90gfpjskdw3p4qm" .. formatdate)
	print("unlogin sign value -- ", sign)
	return "time=" .. formatdate .. "&sign=" .. sign
end

-- 签名参数(gameid)
function GlobalUserItem:urlGameIdSignParam()
	local formatdate = os.date("%Y%m%d%H%M%S", os.time())
	print("gameid sign time -- ", formatdate)
	local sign = md5(GlobalUserItem.tabAccountInfo.dwGameID .. "szwhkj56dt90gfpjskdw3p4qm" .. formatdate)
	print("gameid sign value -- ", sign)
	return "gameid=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&time=" .. formatdate .. "&sign=" .. sign
end

-- userid 签名
function GlobalUserItem:useridSign( userid )
	local formatdate = os.date("%Y%m%d%H%M%S", os.time())
	print("userid sign time -- ", formatdate)
	local sign = md5(userid .. "szwhkj56dt90gfpjskdw3p4qm" .. formatdate)
	print("userid sign value -- ", sign)
	return formatdate, sign
end

-- 签名参数(userid)
function GlobalUserItem:urlUserIdSignParam()
	local formatdate = os.date("%Y%m%d%H%M%S", os.time())
	print("userid sign time -- ", formatdate)
	local sign = md5(GlobalUserItem.tabAccountInfo.dwUserID .. "szwhkj56dt90gfpjskdw3p4qm" .. formatdate)
	print("userid sign value -- ", sign)
	return "userid=" .. GlobalUserItem.tabAccountInfo.dwUserID .. "&time=" .. formatdate .. "&sign=" .. sign
end

--是否是防作弊
function GlobalUserItem.isAntiCheat()
	return (bit:_and(GlobalUserItem.dwServerRule, yl.SR_ALLOW_AVERT_CHEAT_MODE) ~= 0)
end

--防作弊是否有效(是否进入了游戏)
function GlobalUserItem.isAntiCheatValid(userid)
	if false == GlobalUserItem.bEnterGame then
		return false
	end

	--自己排除
	if userid == GlobalUserItem.tabAccountInfo.dwUserID then
		return false
	end
	return GlobalUserItem.isAntiCheat()
end

function GlobalUserItem.todayCheck(date)
	if nil == date then
		return false
	end
	local curDate = os.date("*t")
	local checkDate = os.date("*t", date)
	if curDate.year == checkDate.year and curDate.month == checkDate.month and curDate.day == checkDate.day then
		return true
	end
	return false
end

function GlobalUserItem.setTodayFirstAction(key, value)
	cc.UserDefault:getInstance():setStringForKey(key, value .. "")
	cc.UserDefault:getInstance():flush()
end

--当日首次充值
function GlobalUserItem.isFirstPay()
	local everyDayPay = cc.UserDefault:getInstance():getStringForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayPay", "nil")
	if "nil" ~= everyDayPay then
		local n = tonumber(everyDayPay)
		return not GlobalUserItem.todayCheck(n)
	end
	return true
end

function GlobalUserItem.setTodayPay()
	if GlobalUserItem.isFirstPay() then
		GlobalUserItem.setTodayFirstAction(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayPay", os.time())
	end	
end

--当日首次分享
function GlobalUserItem.isFirstShare()
	local everyDayShare = cc.UserDefault:getInstance():getStringForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayShare", "nil")
	if "nil" ~= everyDayShare then
		local n = tonumber(everyDayShare)
		return not GlobalUserItem.todayCheck(n)
	end
	return true
end

function GlobalUserItem.setTodayShare()
	if GlobalUserItem.isFirstShare() then
		GlobalUserItem.setTodayFirstAction(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayShare", os.time())
	end	
end

-- 当日首页广告
function GlobalUserItem.isShowAdNotice()
	local everyDayAdNotice = cc.UserDefault:getInstance():getStringForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayNoAdNotice", "nil")
	if "nil" ~= everyDayAdNotice then
		local n = tonumber(everyDayAdNotice)
		return not GlobalUserItem.todayCheck(n)
	end
	return true
end

function GlobalUserItem.setTodayNoAdNotice( noAds )
	if noAds then
		GlobalUserItem.setTodayFirstAction(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayNoAdNotice", os.time())	
	else
		GlobalUserItem.setTodayFirstAction(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayNoAdNotice", "nil")
	end
end

-- 当日引导
function GlobalUserItem.isTodayGuide()
	local todayGuide = cc.UserDefault:getInstance():getStringForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayIdentifyGuide", "nil")
	if "nil" ~= todayGuide then
		local n = tonumber(todayGuide)
		return GlobalUserItem.todayCheck(n)
	end
	return false
end

function GlobalUserItem.isIdentifyGuide()
	if GlobalUserItem.isTodayGuide() then
		local guideCount = cc.UserDefault:getInstance():getIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "todayIdentifyGuide", 0)
		if 3 == guideCount then
			return true
		end
	end
	return false
end

function GlobalUserItem.setTodayGuide()
	if not GlobalUserItem.isTodayGuide() then
		cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "todayIdentifyGuide", 1)
		GlobalUserItem.setTodayFirstAction(GlobalUserItem.tabAccountInfo.dwUserID .. "everyDayIdentifyGuide", os.time())	
	else
		local guideCount = cc.UserDefault:getInstance():getIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "todayIdentifyGuide", 0)
		cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID .. "todayIdentifyGuide", guideCount + 1)
		cc.UserDefault:getInstance():flush()
	end
end

--判断是否是代理商帐号
function GlobalUserItem.isAngentAccount(nottip)
	nottip = nottip or false
	if GlobalUserItem.bIsAngentAccount then
		local runScene = cc.Director:getInstance():getRunningScene()
		if nil ~= runScene and not nottip then
			showToast(runScene, "您是代理商帐号，无法操作！", 2)
		end
		return true
	end
	return false
end

--设置是否绑定账号
function GlobalUserItem.setBindingAccount()
	cc.UserDefault:getInstance():setBoolForKey("isBingdingAccount", true)
end

--获取是否绑定账号
function GlobalUserItem.getBindingAccount()
	return cc.UserDefault:getInstance():getBoolForKey("isBingdingAccount", false)
end

--判断是否能修改信息
function GlobalUserItem.notEditAble(nottip)
	nottip = nottip or false
	if --[[GlobalUserItem.bVistor or]] GlobalUserItem.bWeChat then
		local runScene = cc.Director:getInstance():getRunningScene()
		if nil ~= runScene and not nottip then
			local str = ""
			if GlobalUserItem.bVistor then
				str = "游客登录无法修改信息"
			elseif GlobalUserItem.bWeChat then
				str = "微信登录无法修改信息"
			end
			showToast(runScene, str, 2)
		end
		return true
	end
	return false
end

-- 无定位数据
function GlobalUserItem.noCoordinateData()
	if nil == GlobalUserItem.tabCoordinate 
		or nil == GlobalUserItem.tabCoordinate.la 
		or 360.0 == GlobalUserItem.tabCoordinate.la
		or nil == GlobalUserItem.tabCoordinate.lo
		or 360.0 == GlobalUserItem.tabCoordinate.lo then
		return true
	end
	return false
end

-- 是否认证
function GlobalUserItem.isRealAuth()
	if ExternalFun.stringLen(GlobalUserItem.tabPlusInfo.szPassportID) > 0
        and ExternalFun.stringLen(GlobalUserItem.tabPlusInfo.szTrueName) > 0 then
        return true
    end
    return false
end

--活动数据
function GlobalUserItem.setAwardData(activityData, systemNotice)
	--dump(activityData, "#GlobalUserItem.setAwardData=======")
	GlobalUserItem._awardList = activityData
	GlobalUserItem._systemNotice = systemNotice
end

--活动数据
function GlobalUserItem.getAwardData()
	-- print("GlobalUserItem.setAwardData=======")
	return GlobalUserItem._awardList or {}, GlobalUserItem._systemNotice
end
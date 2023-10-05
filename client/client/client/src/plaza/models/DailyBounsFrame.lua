local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local DailyBounsFrame = class("DailyBounsFrame",BaseFrame)

function DailyBounsFrame:ctor(view,callbcak)
	DailyBounsFrame.super.ctor(self,view,callbcak)
	self.isInfoEnd = true
end
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 查询签到
local CHECKIN_QUERY = loginCMD.SUB_GP_CHECKIN_QUERY
-- 签到
local CHECKIN_DONE = loginCMD.SUB_GP_CHECKIN_DONE
-- 领取
local CHECKIN_GETCYCREWARD = loginCMD.SUB_GP_CHECKIN_GETCYCREWARD
DailyBounsFrame.QUERY_USERINFO = QUERY_USERINFO


--连接结果
function DailyBounsFrame:onConnectCompeleted()
	print("onConnectCompeleted------------------>", self._oprateCode)
	if self._oprateCode == CHECKIN_QUERY then	 		--查询签到
		print("sendQueryDailyInfo------------------>", self._oprateCode)
		self:sendQueryDailyInfo()
	elseif self._oprateCode == CHECKIN_DONE then 		-- 签到
		self:sendDailyBouns()
	elseif self._oprateCode == CHECKIN_GETCYCREWARD then -- 领取奖励
		self:sendGetDailyPrize()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知操作模式！")
		end		
	end
end

--网络信息(短连接)
function DailyBounsFrame:onSocketEvent(main,sub,pData)
	local isClose = true
	if main == loginCMD.MDM_GP_USER_SERVICE then 				-- 用户服务
		if sub == loginCMD.SUB_GP_CHECKIN_INFO then				--签到信息
			isClose = false
			self:onSubGetDailyInfo(pData)
		elseif sub == loginCMD.SUB_GP_CHECKIN_RESULT then		--领取结果
			self:onSubOperateResult(pData)
		elseif sub == loginCMD.SUB_GP_CHECKIN_INFO_END then		--签到信息结束
			isClose = true
			self.isInfoEnd = true
		end
	end
	if isClose then
		self:onCloseSocket()
	end
end

--网络消息(长连接)
function DailyBounsFrame:onGameSocketEvent(main,sub,pData)
	
end

function DailyBounsFrame:onSubGetDailyInfo(pData)
	if self.isInfoEnd then
		self.isInfoEnd = false

		GlobalUserItem.tabDailyInfo = {}
		GlobalUserItem.tabDailyRewards = {}
		GlobalUserItem.tabCumulativeRewards = {}
	end

	print("onSubGetDailyInfo------->", pData:getlen())
	local cmd_table = ExternalFun.read_netdata( loginCMD.CMD_GP_CheckInInfo, pData)
	GlobalUserItem.tabDailyInfo = cmd_table

	local numPrize = cmd_table.nPackCount
	for i=1,numPrize do
		local cmd_table = ExternalFun.read_netdata( loginCMD.tagSignInReward, pData)

		if 0 == cmd_table.nPlatformKindID or 1 == cmd_table.nPlatformKindID then
			if 0 == cmd_table.cbType then
				table.insert(GlobalUserItem.tabDailyRewards, cmd_table)
				table.sort(GlobalUserItem.tabDailyRewards, function (a, b)
					-- body
					return a.nSortID < b.nSortID
				end)
			elseif 1 == cmd_table.cbType then
				table.insert(GlobalUserItem.tabCumulativeRewards, cmd_table)
				table.sort(GlobalUserItem.tabCumulativeRewards, function (a, b)
					-- body
					return a.nSortID < b.nSortID
				end)
			end
		end
	end
	if nil ~= self._callBack and numPrize > 0 then
		self._callBack(1, "请求信息成功！")
	end	
end

function DailyBounsFrame:onSubOperateResult(pData)
	local cmd_table = ExternalFun.read_netdata(loginCMD.CMD_GP_CheckInResult, pData)
	dump(cmd_table, "onSubOperateResult", 7)
	if not cmd_table.bSuccessed then
	    local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, cmd_table.szDescribeString, 2)
            return
        end   
	end
	if nil ~= self._callBack and cmd_table.bSuccessed then
		if 0 == cmd_table.cbType then
			local param = {}
			param.nPackageID = cmd_table.nPackageID
			--更新信息,签到次数加1
			GlobalUserItem.tabDailyInfo.wSeriesDate = GlobalUserItem.tabDailyInfo.wSeriesDate +1
			GlobalUserItem.tabDailyInfo.bTodayChecked = true

			self._callBack(2, cmd_table.szDescribeString, param)

		elseif 1 == cmd_table.cbType then
			--更新信息
			for i=1, #GlobalUserItem.tabCumulativeRewards do
				if GlobalUserItem.tabCumulativeRewards[i].nPackageID == cmd_table.nPackageID then
					GlobalUserItem.tabDailyInfo.nGotPackageID[1][i] = GlobalUserItem.tabCumulativeRewards[i].nNeedDay
				end
			end
			self._callBack(3, cmd_table.szDescribeString)
		end

		--更新金钱
        -- +钻石
        local ldiamond = cmd_table.lDiamond
        GlobalUserItem.tabAccountInfo.lDiamond = ldiamond

        -- +金币
        local lScore = cmd_table.lScore
        GlobalUserItem.tabAccountInfo.lUserScore = lScore

        -- +奖券
        local nAwardsTickets = cmd_table.nAwardsTickets
        GlobalUserItem.tabAccountInfo.nAwardsTickets = nAwardsTickets

        

        -- 通知更新        
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY) 
        eventListener.obj = yl.RY_MSG_USERWEALTH
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
	end	
end

 
-- 发送查询信息
function DailyBounsFrame:sendQueryDailyInfo()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_CheckInQueryInfo)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_CHECKIN_QUERY)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass, yl.LEN_MD5)
	buffer:pushint(1)
	if nil == buffer or (not self:sendSocketData(buffer) and nil ~= self._callBack) then
		self._callBack(-1,"查询信息失败！")
	end
end

--发送免费签到
function DailyBounsFrame:sendDailyBouns()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_CheckInDone)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_CHECKIN_DONE)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass, yl.LEN_MD5)
	buffer:pushstring(GlobalUserItem.szMachine, yl.LEN_MACHINE_ID)
	buffer:pushint(1)

	if nil == buffer or (not self:sendSocketData(buffer) and nil ~= self._callBack) then
		self._callBack(-1,"免费签到失败！")
	end
end

-- 发送领取签到
function DailyBounsFrame:sendGetDailyPrize()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_CheckInGetCycReward)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_CHECKIN_GETCYCREWARD)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushdword(self._GiftID)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass, yl.LEN_MD5)
	--buffer:pushint(1)

	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送领取累计签到失败！")
	end
end

function DailyBounsFrame:onGetDailyInfo()
	self._oprateCode = CHECKIN_QUERY
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function DailyBounsFrame:onGetPrizeDaily()
	--记录模式
	self._oprateCode = CHECKIN_DONE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 累计奖励
function DailyBounsFrame:onGetCumulativeRewards(nGiftID)
	self._GiftID = nGiftID
	self._oprateCode = CHECKIN_GETCYCREWARD
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end
return DailyBounsFrame
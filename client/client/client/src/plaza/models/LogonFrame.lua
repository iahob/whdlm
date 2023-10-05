--[[
	登录模块
]]

local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")

local LogonFrame = class("LogonFrame",BaseFrame)
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local cmdstruct = appdf.req(appdf.HEADER_SRC .. "CMD_Struct")

LogonFrame.OP_OTHERLOGIN = logincmd.SUB_MB_LOGON_OTHERPLATFORM

LogonFrame.OP_GETONLINE = logincmd.SUB_MB_GET_ONLINE

LogonFrame.OP_GETOFFLINE = logincmd.SUB_MB_PERSONAL_GET_OFFLINE_INFO

function LogonFrame:ctor(view,callback)
	LogonFrame.super.ctor(self,view,callback)
	self._plazaVersion = appdf.VersionValue(6,7,0,1)
	self._stationID = yl.STATION_ID
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	local tmp = yl.DEVICE_TYPE_LIST[targetPlatform]
	self._deviceType = tmp or yl.DEVICE_TYPE
	self._szMachine = MultiPlatform:getInstance():getMachineId()
	self._szDeviceToken = MultiPlatform:getInstance():getDeviceToken()

	self.m_angentServerList = {}
	self._tempAllRoom = {}

	self._wServerCount = 0
	self._wOnlineServerID = {}
	self._OnlineInfo = {}
end

--连接结果
function LogonFrame:onConnectCompeleted()	
	if self._logonMode == LogonFrame.OP_OTHERLOGIN then
		self:sendThirdPartyLogin()
	elseif self._logonMode == LogonFrame.OP_GETONLINE then
		self:sendGetOnline()
	elseif self._logonMode == LogonFrame.OP_GETOFFLINE then
		self:sendGetOffLineInfo()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知登录模式！")
		end		
	end	
end

--网络信息
function LogonFrame:onSocketEvent(main,sub,pData)
	if (main == logincmd.MDM_MB_LOGON) then --登录命令
		self:onSubLogonEvent(sub,pData)
	elseif main == logincmd.MDM_MB_SERVER_LIST then --房间列表
		self:onRoomListEvent(sub,pData)
	elseif main == logincmd.MDM_GP_PROPERTY_LIST then --房间列表
		self:onPropertyListEvent(sub,pData)
	elseif main == logincmd.MDM_MB_PERSONAL_SERVICE then --离线信息
		self:onUserOffLineResult(sub,pData)
	end
end

--玩家信息
function LogonFrame:onSubLogonEvent(sub,pData)
	local sub =pData:getsub()
	--登录成功读取用户信息
	if sub == logincmd.SUB_MB_LOGON_SUCCESS then
		self:onLogonSuccess(pData)
	--登录失败
	elseif sub == logincmd.SUB_MB_LOGON_FAILURE then
		print("=========== 登录失败 ===========")
		-- CMD_MB_LogonFailure
		local cmdtable = ExternalFun.read_netdata(logincmd.CMD_MB_LogonFailure, pData)
		if 10 == cmdtable.lResultCode then
			GlobalUserItem.setBindingAccount()
			if nil ~= self._callBack then
				self._callBack(10,cmdtable.szDescribeString)
			end
		end
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,cmdtable.szDescribeString)
		end		
	--更新APP
	elseif sub == logincmd.SUB_MB_UPDATE_NOTIFY then
		local cbMustUpdate = pData:readbyte()
		local cbAdviceUpdate = pData:readbyte()
		local dwCurrentVersion = pData:readdword()
		print("update_notify:"..cbMustUpdate.."#"..cbAdviceUpdate.."#"..dwCurrentVersion)
		local tmpV = appdf.ValuetoVersion(dwCurrentVersion)
		print(tmpV.p.."."..tmpV.m.."."..tmpV.s.."."..tmpV.b)
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"版本信息错误！")
		end		
	end
end

--房间信息
function LogonFrame:onRoomListEvent(sub,pData)
	if sub == logincmd.SUB_MB_LIST_FINISH then	--列表完成
		if self._logonMode == LogonFrame.OP_OTHERLOGIN then 		--微信登陆
			GlobalUserItem.bVistor  = false
			GlobalUserItem.bWeChat = true
		end
		-- 整理列表
		--dump(self._tempAllRoom, "比赛信息", 6)
		for k,v in pairs(self._tempAllRoom) do
			table.sort(v, function(a, b)
				return a.wSortID < b.wSortID
			end)
			
			for i = 1, #v do
				local matchRoomList = {}
				v[i]._nRoomIndex = i
				if v[i].wServerType == yl.GAME_GENRE_MATCH then
					table.insert(matchRoomList, v[i])
				end
				if MatchRoom then
					if table.nums(matchRoomList)~=0 then
						table.insert(MatchRoom:getInstance().m_tabMatchRoomList, matchRoomList)
					end
					
				end
			end
			for i = 1, #v do
				v[i]._nRoomIndex = i
			end

			local roomlist = {}
			--记录游戏ID
			table.insert(roomlist,k)
			table.insert(roomlist,v)
			if PriRoom then
				PriRoom:getInstance().m_tabPriRoomList[k] = v
			end

			--加入缓存
			table.insert(GlobalUserItem.roomlist,roomlist)
		end

		if MatchRoom then
			MatchRoom:getInstance():onSubMatchRuleGroup()
		end

		--登录完成
		self:onCloseSocket() --无状态
		if nil ~= self._callBack then
			self._callBack(logincmd.SUB_MB_LIST_FINISH)
		end		
		
	elseif sub == logincmd.SUB_MB_LIST_SERVER then	--列表数据
		self:onSubRoomListInfo(pData)
	elseif sub == logincmd.SUB_MB_LIST_MATCH then     --比赛数据
		self:onSubMatchListInfo(pData)
	elseif sub == logincmd.SUB_MB_LIST_LUA_GAME then --游戏列表
		self:onSubGameList(pData)
	elseif sub == logincmd.SUB_MB_AGENT_KIND then 	--代理列表
		self:onSubAngentListInfo(pData)
	elseif sub == logincmd.SUB_MB_SERVER_ONLINE then 	--房间在线人数
		self:onSubOnlineInfo(pData)
		if nil ~= self._callBack then
			self._callBack(logincmd.SUB_MB_SERVER_ONLINE)
		end		
	end
end

--道具信息
function LogonFrame:onPropertyListEvent(sub,pData)
	if sub == logincmd.SUB_GP_QUERY_PROPERTY_LIST_RESULT then	--列表完成
		local len = pData:getlen()
		if (len - math.floor(len/ 377) *377) ~= 0 then
			print("PropertyList_len_error"..len)
			return
		end
		local itemcount =  math.floor(len/377)
		print("=============== onPropertyListEvent ================ ", itemcount)
		
		--读取房间信息
		print("itemcount is ================",itemcount)
		for i = 1,itemcount do
			local item = ExternalFun.read_netdata(logincmd.tagPropertyInfo, pData)
			--dump(item, "onSubRoomListInfo", 10)
			if not item then 
				break
			end
			table.insert(GlobalUserItem.tabPropertyItem, item)

		end
	end
end

function LogonFrame:onUserOffLineResult(sub,pData)
	if sub == logincmd.SUB_MB_PERSONAL_GET_OFFLINE_INFO_RESULT then
		local cmd_data = ExternalFun.read_netdata(logincmd.CMD_GP_GetOffLineInfoResult, pData)
		GlobalUserItem.tabAccountInfo.dwLockServerID = cmd_data.dwServerId or 0
		--dump(cmd_data,"===========================11111111111111")
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(10,"")
		end
	end
	
end


-- 登陆成功
function LogonFrame:onLogonSuccess( pData )
	print("=========== 登录成功 ===========")
	-- 账号信息
	local cmd_table = ExternalFun.read_netdata( logincmd.CMD_MB_LogonSuccess, pData )
	--dump(cmd_table, "CMD_MB_LogonSuccess")
	print(string.format("LogonFrame:onLogonSuccess lockkind[%d], lockserver[%d]", cmd_table.dwLockKindID, cmd_table.dwLockServerID))
	GlobalUserItem.tabAccountInfo = cmd_table
	-- 记录锁表
	if 0 ~= GlobalUserItem.tabAccountInfo.dwLockKindID then
		GlobalUserItem.nCurGameKind = GlobalUserItem.tabAccountInfo.dwLockKindID
	end
	-- 附加信息
	local curlen = pData:getcurlen()
	local datalen = pData:getlen()
	print("*** curlen-", curlen)
	print("*** datalen-", datalen)
	while curlen < datalen do
		tmpSize = pData:readword()
		tmpCmd = pData:readword()
		if not tmpSize or not tmpCmd then
		 	break
		end

		print("*** tmpSize-", tmpSize)
		print("*** tmpCmd-", tmpCmd)
		if tmpCmd == logincmd.DTP_GP_GROUP_INFO then 			-- 社团信息
			print(pData:readstring(tmpSize/2))
		elseif tmpCmd == logincmd.DTP_GP_MEMBER_INFO then 		-- 会员信息
			GlobalUserItem.tabPlusInfo.cbMemberOrder = pData:readbyte()
			for i=1,8 do
				print("member- ", pData:readword())
			end
		elseif tmpCmd == logincmd.DTP_GP_UNDER_WRITE then 			-- 个性签名
			GlobalUserItem.tabPlusInfo.szSign = pData:readstring(tmpSize/2)
			if not GlobalUserItem.tabPlusInfo.szSign then
				GlobalUserItem.tabPlusInfo.szSign = "此人很懒，没有签名"
			end
			print("szSign ", GlobalUserItem.tabPlusInfo.szSign)
		elseif tmpCmd == logincmd.DTP_GP_STATION_URL then 		-- 主页信息
			print(pData:readstring(tmpSize/2))
		elseif tmpCmd == logincmd.DTP_GP_UI_SPREADER then
			GlobalUserItem.tabPlusInfo.szSpreaderAccount = pData:readstring(tmpSize/2) or ""
			print("szSpreaderAccount ", GlobalUserItem.tabPlusInfo.szSpreaderAccount)
		elseif tmpCmd == 0 then
			break
		else
			for i = 1, tmpSize do
				if not pData:readbyte() then
					break
				end
			end
		end
		curlen = pData:getcurlen()
	end
	GlobalUserItem.szMachine = self._szMachine
	GlobalUserItem.szIpAdress = MultiPlatform:getInstance():getClientIpAdress() or ""

	--重置房间
	GlobalUserItem.roomlist = {}
	if PriRoom then
		PriRoom:getInstance().m_tabPriModeGame = {}
		PriRoom:getInstance().m_tabGoldPriModeGame = {}
		PriRoom:getInstance().m_tabScorePriModeGame = {}
		
	end

	if MatchRoom then
		MatchRoom:getInstance():reSet()
	end
	self._tempAllRoom = {}
end

--房间列表
function LogonFrame:onSubRoomListInfo(pData)
	--计算房间数目
	local len = pData:getlen()
	print("=============== onSubRoomListInfo ================")
	print("onSubRoomListInfo:"..len)
	if (len - math.floor(len/yl.LEN_GAME_SERVER_ITEM)*yl.LEN_GAME_SERVER_ITEM) ~= 0 then
		print("roomlist_len_error"..len)
		return
	end
	local itemcount =  math.floor(len/yl.LEN_GAME_SERVER_ITEM)
	print("=============== onSubRoomListInfo ================ ", itemcount)
	
	--读取房间信息
	print("itemcount is ================",itemcount)
	for i = 1,itemcount do
		local item = ExternalFun.read_netdata(cmdstruct.tagGameServer, pData)
		--dump(item, "onSubRoomListInfo", 10)
		if not item then 
			break
		end
		if nil == self._tempAllRoom[item.wKindID] then
			self._tempAllRoom[item.wKindID] = {}
		end

		if item.wServerType == yl.GAME_GENRE_PERSONAL then
			if GlobalUserItem.bEnableRoomCard then
				if PriRoom then
					PriRoom:getInstance().m_tabPriModeGame[item.wKindID] = true
					if 1 == item.dwSurportType then
						PriRoom:getInstance().m_tabGoldPriModeGame[item.wKindID] = true
					elseif 0 == item.dwSurportType then
						PriRoom:getInstance().m_tabScorePriModeGame[item.wKindID] = true
					end
				end				
				table.insert(self._tempAllRoom[item.wKindID], item)
			end
		else
			table.insert(self._tempAllRoom[item.wKindID], item)
		end		
		if item.wServerType == yl.GAME_GENRE_MATCH and MatchRoom then
			print("item.wKindID..",item.wKindID)
			MatchRoom:getInstance().m_tabMatchModeGame[item.wKindID] = true
		end 
	end
end

function LogonFrame:onSubMatchListInfo(pData)
	if MatchRoom then
		MatchRoom:getInstance():onSubMatchRuleList(pData)
	end
end

function LogonFrame:onSubGameList(pData)
	local len = pData:getlen()
	print(len.."=================")
	
	if len%143 == 0 then
		
		for i=1,math.floor(len/143) do
			local cmd_data = ExternalFun.read_netdata(logincmd.tagGameItem, pData)
			for k ,v in pairs(GlobalUserItem.gamelist) do
				if v and v._KindID == cmd_data.wKindID then
					table.remove(GlobalUserItem.gamelist,k)
					break
				end
			end

			local gameinfo = {}
			    gameinfo._KindID = cmd_data.wKindID
			    gameinfo._KindName = string.lower(cmd_data.szModuleName) .. "."
			    gameinfo._Module = string.gsub(gameinfo._KindName, "[.]", "/")
			    gameinfo._KindVersion = cmd_data.dwClientVersion
			    gameinfo._ServerResVersion = cmd_data.dwResVersion
			    gameinfo._Type = gameinfo._Module
			    gameinfo._GameName = cmd_data.szKindName
			    --检查本地文件是否存在
			    local path = device.writablePath .. "game/" .. gameinfo._Module
			    gameinfo._Active = cc.FileUtils:getInstance():isDirectoryExist(path)
			    local e = string.find(gameinfo._KindName, "[.]")
			    if e then
			    	gameinfo._Type = string.sub(gameinfo._KindName,1,e - 1)
			    end
			    -- 排序
			    gameinfo._SortId = cmd_data.wSortID

			table.insert(GlobalUserItem.gamelist,gameinfo)
		end
          end 
          

end


function LogonFrame:onSubAngentListInfo(pData)
	self.m_angentServerList = {}
	--计算数目
	local len = pData:getlen()
	print("=============== onSubAngentListInfo ================")
	print("onSubAngentListInfo:"..len)
	if (len - math.floor(len/4)*4) ~= 0 then
		print("angentlist_len_error"..len)
		return
	end
	print("=============== onSubAngentListInfo ================")
	local itemcount =  math.floor(len/4)
	
	--读取房间信息
	for i = 1,itemcount do
		local kind = pData:readword()
		local sort = pData:readword()
		
		self.m_angentServerList[kind] = {KindID = kind, SortID = sort}
	end
end

function LogonFrame:onLoginByThirdParty(szAccount, szNick, cbgender, platform, szHeadUrl)
	--数据保存
	self._szAccount = szAccount
	self._szNickName = szNick
	self._cbLoginGender = cbgender
	self._cbPlatform = platform
	self._szMobilePhone = "0123456789"
	self._szHeadUrl = szHeadUrl

	--记录模式
	self._logonMode = LogonFrame.OP_OTHERLOGIN
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:sendThirdPartyLogin()
	local cmddata = ExternalFun.create_netdata(logincmd.CMD_MB_LogonOtherPlatform)
	cmddata:setcmdinfo(logincmd.MDM_MB_LOGON,logincmd.SUB_MB_LOGON_OTHERPLATFORM)

	cmddata:pushword(yl.INVALID_WORD)
	cmddata:pushdword(self._plazaVersion)
	cmddata:pushbyte(self._deviceType)
	cmddata:pushbyte(self._cbLoginGender)
	cmddata:pushbyte(self._cbPlatform)
	cmddata:pushstring(self._szAccount,yl.LEN_USER_UIN)
	cmddata:pushstring(self._szNickName,yl.LEN_NICKNAME)
	cmddata:pushstring(self._szNickName,yl.LEN_COMPELLATION)
	cmddata:pushstring(self._szMachine,yl.LEN_MACHINE_ID)
	cmddata:pushstring(self._szMobilePhone,yl.LEN_MOBILE_PHONE)
	cmddata:pushstring(self._szDeviceToken,yl.LEN_DEVICE_TOKEN)
	cmddata:pushstring(self._szHeadUrl,yl.LEN_FACE_URL)

	cmddata:pushbyte(3)
	if not self:sendSocketData(cmddata) and nil ~= self._callBack then
		self._callBack(-1,"发送微信登录失败！")
	end
end

function LogonFrame:onGetOnline(wServerCount, wOnlineServerID)
		--数据保存
	self._wServerCount = wServerCount
	for  i =1, self._wServerCount do
		self._wOnlineServerID[i] = wOnlineServerID[i]
	end
	
	--记录模式
	self._logonMode = LogonFrame.OP_GETONLINE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:onGetOffLineInfo()

	--记录模式
	self._logonMode = LogonFrame.OP_GETOFFLINE
	
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end



function LogonFrame:sendGetOffLineInfo()
	local cmddata =  ExternalFun.create_netdata()--ExternalFun.create_netdata(logincmd.CMD_MB_GetOnline)
	cmddata:setcmdinfo(logincmd.MDM_MB_PERSONAL_SERVICE,logincmd.SUB_MB_PERSONAL_GET_OFFLINE_INFO)
	cmddata:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)

	if not self:sendSocketData(cmddata) and nil ~= self._callBack then
		self._callBack(-1,"发送请求人数失败！")
	end
end


function LogonFrame:sendGetOnline()
	local cmddata =  ExternalFun.create_netdata()--ExternalFun.create_netdata(logincmd.CMD_MB_GetOnline)
	cmddata:setcmdinfo(logincmd.MDM_MB_SERVER_LIST,logincmd.SUB_MB_GET_ONLINE)

	cmddata:pushword(self._wServerCount)
	for i=1, self._wServerCount do
		cmddata:pushword(self._wOnlineServerID[i])
	end

	if not self:sendSocketData(cmddata) and nil ~= self._callBack then
		self._callBack(-1,"发送请求人数失败！")
	end
end

function LogonFrame:onSubOnlineInfo(pData)
	self._OnlineInfo = {}
	local count = pData:readword()
	for i=1,count do
		local info = {}
		local cmd_table = ExternalFun.read_netdata( logincmd.tagOnlineInfo, pData )
		info.wServerID = cmd_table.wID
		info.dwOnlineCount = cmd_table.dwOnlineCount
		info.dwAndroidCount = cmd_table.dwAndroidCount
		info.dwSetCount = cmd_table.dwSetCount

		table.insert(self._OnlineInfo, info)
	end
end

return LogonFrame
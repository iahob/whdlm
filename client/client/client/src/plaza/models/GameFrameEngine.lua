local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local GameFrameEngine = class("GameFrameEngine",BaseFrame)

local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local cmdstruct = appdf.req(appdf.HEADER_SRC .. "CMD_Struct")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")


function GameFrameEngine:ctor(view,callbcak)
	GameFrameEngine.super.ctor(self,view,callbcak)
	self._kindID = 0
	self._kindVersion = 0

	-- 断网对话框
	self.m_netQuery = nil

	-- 短连接服务
	self._shotFrame = nil

	-- 是否发送 option 消息
	self.m_bSendOption = false

	self:onInitData()
end

function GameFrameEngine:setKindInfo(id,version)
	self._kindID = id 
	self._kindVersion = version
	return self
end

function GameFrameEngine:setScene( scene )
	self._scene = scene
end

function GameFrameEngine:onInitData()
	-- 房间信息
	self._wTableCount = 0
	self._wChairCount = 0
	self._wServerType = 0
	self._dwServerRule = 0
	self._UserList = {}
	self._tableUserList = {}
	self._tableStatus = {}
	self._delayEnter = false

	self._wTableID	 	= yl.INVALID_TABLE
	self._wChairID	 	= yl.INVALID_CHAIR
	self._cbTableLock	= 0
	self._cbGameStatus 	= 0
	self._cbAllowLookon	= 0
	self.bChangeDesk = false
	self.bEnterAntiCheatRoom = false 		--进入防作弊房间
	GlobalUserItem.bWaitQuit = false 		-- 退出等待
	self._tabVoiceMsgQueue = {}
	self._bPlayVoiceRecord = false
	self._nPlayVoiceId = nil


	self:removeNetQuery()
	self.m_bSendOption = false
end

function GameFrameEngine:removeNetQuery()
	if nil ~= self.m_netQuery then
		self.m_netQuery:removeFromParent()
		self.m_netQuery = nil
	end
end

function GameFrameEngine:setEnterAntiCheatRoom( bEnter )
	self.bEnterAntiCheatRoom = bEnter
end

--连接房间
function GameFrameEngine:onLogonRoom()
	self._roomInfo = GlobalUserItem.GetRoomInfo()

	if not self._roomInfo and nil ~= self._callBack then
		self._callBack(-1,"获取房间信息失败！")
		return
	end
	print("登录房间："..self._roomInfo.szServerAddr.."#"..self._roomInfo.wServerPort)

	if not self:onCreateSocket(self._roomInfo.szServerAddr,self._roomInfo.wServerPort) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end
--连接结果
function GameFrameEngine:onConnectCompeleted()
	if nil ~= self._socket then
		self._socket:setovertime(30000)
	end
	if nil ~= self._scene then
		self._scene.m_bIsReConnet = false
	end
	local dataBuffer = ExternalFun.create_netdata(game_cmd.CMD_GR_LogonMobile)
	--初始化参数
	self:onInitData()
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_LOGON,game_cmd.SUB_GR_LOGON_MOBILE)
	dataBuffer:pushword(self._kindID)
	dataBuffer:pushdword(self._kindVersion)
	dataBuffer:pushbyte(1)--lua端标识
	dataBuffer:pushword(0x0011)
	dataBuffer:pushword(255)
	dataBuffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	dataBuffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass,yl.LEN_MD5)
	dataBuffer:pushstring(GlobalUserItem.tabAccountInfo.md5Passwd,yl.LEN_PASSWORD)
	dataBuffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
	if GlobalUserItem.dwCurRoomGroupID ~=0 then
		dataBuffer:pushdword(GlobalUserItem.dwCurRoomGroupID)
	else
		dataBuffer:pushdword(ClubFrame:getInstance().m_nCurrentClub or 0)
	end

	if not self:sendSocketData(dataBuffer) and nil ~= self._callBack then
		self._callBack(-1,"发送登录失败！")
	end
end

--网络信息
function GameFrameEngine:onSocketEvent(main,sub,dataBuffer)
	--登录信息
	if main == game_cmd.MDM_GR_LOGON then
		self:onSocketLogonEvent(sub,dataBuffer)
	--配置信息
	elseif main == game_cmd.MDM_GR_CONFIG then
		self:onSocketConfigEvent(sub,dataBuffer)
	--用户信息
	elseif main == game_cmd.MDM_GR_USER then
		self:onSocketUserEvent(sub,dataBuffer)
	--状态信息
	elseif main == game_cmd.MDM_GR_STATUS then
		self:onSocketStatusEvent(sub,dataBuffer)
	elseif main == game_cmd.MDM_GF_FRAME then
		self:onSocketFrameEvent(sub,dataBuffer)
		if MatchRoom and GlobalUserItem.bMatch then
			if sub == MatchRoom.CMD_MATCH.SUB_GR_MATCH_WAIT_TIP or sub == MatchRoom.CMD_MATCH.SUB_GR_MATCH_RESULT 
				or game_cmd.SUB_GF_SYSTEM_MESSAGE == sub or sub == MatchRoom.CMD_MATCH.SUB_GR_MATCH_INFO then
				print("比赛框架系统消息")
				MatchRoom:getInstance():getNetFrame():onGameSocketEvent(main,sub,dataBuffer)
			end
		end
	elseif main == game_cmd.MDM_GF_GAME then
		if self._viewFrame and self._viewFrame.onEventGameMessage then
			self._viewFrame:onEventGameMessage(sub,dataBuffer)
		end
	elseif main == game_cmd.MDM_GR_INSURE then
		if self._viewFrame and self._viewFrame.onSocketInsureEvent then
			self._viewFrame:onSocketInsureEvent(sub,dataBuffer)
		end

		-- 短连接服务
		if nil ~= self._shotFrame and nil ~= self._shotFrame.onGameSocketEvent then
			self._shotFrame:onGameSocketEvent(main,sub,dataBuffer)
		end
	elseif main == game_cmd.MDM_GR_PROPERTY then --道具信息

		self:onSocketPropEvent(sub,dataBuffer)

	else
		-- 短连接服务
		if nil ~= self._shotFrame and nil ~= self._shotFrame.onGameSocketEvent then
			self._shotFrame:onGameSocketEvent(main,sub,dataBuffer)
		end
		-- 私人房
		if PriRoom then
			PriRoom:getInstance():getNetFrame():onGameSocketEvent(main,sub,dataBuffer)
		end

		--比赛场
		if MatchRoom then
			MatchRoom:getInstance():getNetFrame():onGameSocketEvent(main,sub,dataBuffer)
		end
	end


	
end

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
--网络错误
function GameFrameEngine:onSocketError(pData)
	if self._threadid == nil then
		return
	end
	--[[local cachemsg = cjson.encode(self.m_tabCacheMsg)
	if nil ~= cachemsg then
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			logFunc(cachemsg or "", true)
		else
			buglyReportLuaException(cachemsg or "", debug.traceback())
		end	
	end]]
	if self._scene and self._scene.m_bIsReConnet == false then
		
		self._scene:dismissPopWait()
	end
	
	self:onCloseSocket()
	self:removeNetQuery()
	local curTag = nil
	-- 当前游戏场景
	if nil ~= self._scene and nil ~= self._scene._sceneLayer then
		curTag = self._scene._sceneLayer:getCurrentTag()
	end
	if curTag ~= yl.SCENE_GAME then
		print("GameFrameEngine:onSocketError 非游戏场景")
		if self._callBack ~= nil then
			if not pData then
				self._callBack(-1,"网络断开！")
			elseif type(pData) == "string" then
				self._callBack(-1,pData)
			else
				local errorcode = pData:readword()
				if errorcode == nil then
					self._callBack(-1,"网络断开！")
				elseif errorcode == 6 then
					self._callBack(-1,"长时间无响应，网络断开！")
				elseif errorcode == 3 then
					self._callBack(-1,"网络连接超时, 请重试!")
					-- 切换地址
					if nil ~= yl.SERVER_LIST[yl.CURRENT_INDEX] then
						yl.LOGONSERVER = yl.SERVER_LIST[yl.CURRENT_INDEX]
					end
					yl.CURRENT_INDEX = yl.CURRENT_INDEX + 1
					if yl.CURRENT_INDEX > yl.TOTAL_COUNT then
						yl.CURRENT_INDEX = 1
					end		
				else
					--self._callBack(-1,"网络错误, " .. errorcode)			
				end
			end
		end
		return
	end
	logFunc("网络出现错误记录", true)

	local gameLayer = self._scene._sceneLayer:getCurrentLayer()
	if gameLayer then
		if gameLayer.KillGameClock then
			gameLayer:KillGameClock()
			gameLayer:stopAllActions()
		end
	end

	-- 尝试重连
	if nil ~= self._scene and  self._scene.m_bIsReConnet == false  then

		self._scene.m_bIsReConnet = true
		self._scene:showGameSeverReConnect()

	end

end

function GameFrameEngine:onSocketLogonEvent(sub,dataBuffer)
	--登录完成
	if sub == game_cmd.SUB_GR_LOGON_FINISH then	
		self:onSocketLogonFinish()
	-- 登录成功
	elseif sub == game_cmd.SUB_GR_LOGON_SUCCESS then
		local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_LogonSuccess, dataBuffer)
	--登录失败
	elseif sub == game_cmd.SUB_GR_LOGON_FAILURE then
		local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_LogonFailure, dataBuffer)
		dump(cmd_table, "SUB_GR_LOGON_FAILURE", 10)	

		local errorCode = cmd_table.lErrorCode
		local msg = cmd_table.szDescribeString
		local serverID = cmd_table.wServerID
		local KindID = cmd_table.wKindID

		-- local errorCode = dataBuffer:readint()
		-- local msg = dataBuffer:readstring()
		-- print("登录房间失败:"..errorCode.."#"..msg)
		self:onCloseSocket()
		if errorCode == 100 then --已经在其他房间
			if self._scene then
				self._scene:dismissPopWait()
		        self.m_Query = QueryDialog:create(msg, function(ok)
		            if ok then
		            	GlobalUserItem.nCurGameKind = KindID

		            	self._scene:logonRoomByLockServerID(serverID)
		            end
		            self.m_Query = nil
		        end, nil, QueryDialog.QUERY_SURE_CANCEL)
		        :setCanTouchOutside(false)
		        :addTo(self._scene)
		    end
		else
			if nil ~= self._callBack then
				self._callBack(-1,"进入房间失败:"..msg)
			end	
		end
	
	--升级提示
	elseif sub == game_cmd.SUB_GR_UPDATE_NOTIFY then
		if nil ~= self._callBack then
			self._callBack(-1,"版本信息错误")
		end		
	end
end




--登录完成
function GameFrameEngine:onSocketLogonFinish()
	if self._delayEnter == true then
		return
	end	

	local myUserItem   =  self:GetMeUserItem()
	if not myUserItem and nil ~= self._callBack then
		self._callBack(-1,"获取自己信息失败！")
		return
	end
	print("onSocketLogonFinish--------------->", GlobalUserItem.bPrivateRoom)
	if GlobalUserItem.bPrivateRoom and PriRoom then		
		PriRoom:getInstance():onLoginPriRoomFinish()
	elseif MatchRoom and GlobalUserItem.bMatch then
		MatchRoom:getInstance():onLoginMatchRoomFinish()		
	else
		if self._wTableID ~= yl.INVALID_TABLE then
			if self._viewFrame and self._viewFrame.onEnterTable then
				self._viewFrame:onEnterTable()
				--showToast(self._viewFrame,"找到游戏桌子，正在获取场景中...",1)
			end
			self:SendGameOption()
		else
			--[[if self._viewFrame and self._viewFrame.onEnterRoom then
				self._viewFrame:onEnterRoom()
			end]]
		end
	end

	-- 清理锁表
    GlobalUserItem.tabAccountInfo.dwLockServerID = 0
    GlobalUserItem.tabAccountInfo.dwLockKindID = 0
end

--房间配置
function GameFrameEngine:onSocketConfigEvent(sub,dataBuffer)
	--房间配置
	if sub == game_cmd.SUB_GR_CONFIG_SERVER then
		-- CMD_GR_ConfigServer
		self._wTableCount  		= dataBuffer:readword()
		self._wChairCount  		= dataBuffer:readword()
		self._wServerType  		= dataBuffer:readword()
		self._dwServerRule 		= dataBuffer:readdword()
		GlobalUserItem.dwServerRule = self._dwServerRule

		--是否进入防作弊
		self:setEnterAntiCheatRoom(GlobalUserItem.isAntiCheat())
		print("房间配置[table:"..self._wTableCount.."][chair:"..self._wChairCount.."][type:"..self._wServerType.."][rule:"..self._dwServerRule.."]")
	--配置完成
	elseif sub == game_cmd.SUB_GR_CONFIG_FINISH then

	end
end

function GameFrameEngine:GetTableCount()
	return self._wTableCount
end

function GameFrameEngine:GetChairCount()
	return self._wChairCount
end

function GameFrameEngine:GetServerType()
	return self._wServerType
end

function GameFrameEngine:GetServerRule()
	return self._dwServerRule
end

--房间取款准许
function GameFrameEngine:OnRoomAllowBankTake()
	return bit:_and(self._dwServerRule,0x00010000) ~= 0
end

--房间存款准许
function GameFrameEngine:OnRoomAllowBankSave()
	return bit:_and(self._dwServerRule,0x00040000) ~= 0
end

--游戏取款准许
function GameFrameEngine:OnGameAllowBankTake()
	return bit:_and(self._dwServerRule,0x00020000) ~= 0
end

--游戏存款准许
function GameFrameEngine:OnGameAllowBankSave()
	return bit:_and(self._dwServerRule,0x00080000) ~= 0
end

function GameFrameEngine:IsAllowAvertCheatMode( )
	return bit:_and(self._dwServerRule, yl.SR_ALLOW_AVERT_CHEAT_MODE) ~= 0
end

--是否更新大厅游戏币
function GameFrameEngine:IsAllowPlazzScoreChange()
	return (self._wServerType ~= yl.GAME_GENRE_SCORE) and (self._wServerType ~= yl.GAME_GENRE_EDUCATE)
end

--游戏赠送准许
function GameFrameEngine:OnGameAllowBankTransfer()
	return false
end

--用户信息
function GameFrameEngine:onSocketUserEvent(sub,dataBuffer)
	--等待分配
	if sub == game_cmd.SUB_GR_USER_WAIT_DISTRIBUTE then
		--showToast(self._viewFrame, "正在进行分组,请稍后...", 3)
		print("正在进行分组,请稍后...")
	--用户进入
	elseif sub == game_cmd.SUB_GR_USER_ENTER then
		self:onSocketUserEnter(dataBuffer)
	--用户积分
	elseif sub == game_cmd.SUB_GR_USER_SCORE then
		self:onSocketUserScore(dataBuffer)
	--用户状态
	elseif sub == game_cmd.SUB_GR_USER_STATUS then
		self:onSocketUserStatus(dataBuffer)
	--请求失败
	elseif sub == game_cmd.SUB_GR_REQUEST_FAILURE then	
		self:onSocketReQuestFailure(dataBuffer)
	end
end

--用户进入
function GameFrameEngine:onSocketUserEnter(dataBuffer)
	local userItem = ExternalFun.read_netdata(cmdstruct.tagMobileUserInfoHead, dataBuffer) --UserItem:create()
	userItem.dwDistance = nil

	--自己判断
	local bMySelfInfo = (userItem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID)
	
	-- if bMySelfInfo and userItem.wTableID ~= yl.INVALID_TABLE then
	-- 	for i = 1, self._wChairCount do
	-- 		-- 查询其他玩家信息
	-- 		if i -1 ~= userItem.wChairID then
	-- 			self:QueryUserInfo(userItem.wTableID, i-1)
	-- 		end
	-- 	end
	-- end
	
	--非法过滤
	if not self._UserList[GlobalUserItem.tabAccountInfo.dwUserID]  then
		if	bMySelfInfo == false then
			print("还未有自己信息，不处理其他用户信息")
			return
		end
	else 
		if bMySelfInfo == true then
			print("GameFrameEngine:onSocketUserEnter 已有自己信息，不再次处理自己信息")
			return
		end
	end

	-- 附加包
	local curlen = dataBuffer:getcurlen()
	local datalen = dataBuffer:getlen()
	local tmpSize 
	local tmpCmd
	while curlen<datalen do
		tmpSize = dataBuffer:readword()
		tmpCmd = dataBuffer:readword()
		if not tmpSize or not tmpCmd then
		 	break
		end
		if tmpCmd == game_cmd.DTP_GR_NICK_NAME then
			userItem.szNickName 	= dataBuffer:readstring(tmpSize/2)

			if not userItem.szNickName or (self:IsAllowAvertCheatMode() == true and userItem.dwUserID ~=  GlobalUserItem.tabAccountInfo.dwUserID) then
				userItem.szNickName = "游戏玩家"
			end
		elseif tmpCmd == game_cmd.DTP_GR_UNDER_WRITE then
			userItem.szSign = dataBuffer:readstring(tmpSize/2)
			if not userItem.szSign or (self:IsAllowAvertCheatMode() == true and userItem.dwUserID ~=  GlobalUserItem.tabAccountInfo.dwUserID) then
				userItem.szSign = "此人很懒，没有签名"
			end
		elseif tmpCmd == 0 then
			break
		else
			for i = 1, tmpSize do
				if not dataBuffer:readbyte() then
					break
				end
			end
		end
		curlen = dataBuffer:getcurlen()
	end
	print("GameFrameEngine enter ==> ", userItem.szNickName, userItem.dwIpAddress, userItem.dwDistance, userItem.lScore)

	--添加/更新到缓存
	local bAdded = false
	local item = self._UserList[userItem.dwUserID] 
	if self._UserList[userItem.dwUserID] ~= nil then
		--如果不为空，取药清掉之前的信息
		-- 记录旧状态
		local oldstatus = {}
		oldstatus.wTableID = item.wTableID
		oldstatus.wChairID = item.wChairID
		oldstatus.cbUserStatus = item.cbUserStatus

		bAdded = true
		--更新信息
		item.cbUserStatus = userItem.cbUserStatus
		item.wTableID = userItem.wTableID
		item.wChairID = userItem.wChairID

		item.szNickName=userItem.szNickName
		item.lScore=userItem.lScore

		--清除旧桌子椅子记录
		if oldstatus.wTableID ~= yl.INVALID_TABLE then
			--新旧桌子不同 新旧椅子不同
			if (oldstatus.wTableID ~= item.wTableID) or (oldstatus.wChairID ~= item.wChairID) then
				self:onUpDataTableUser(oldstatus.wTableID, oldstatus.wChairID, nil)
			end
		end
	end
	if not bAdded then
		self._UserList[userItem.dwUserID] = userItem
	end

	-- local info = {}
	-- info.wTableID = self._UserList[userItem.dwUserID].wTableID
	-- info.wChairID = self._UserList[userItem.dwUserID].wChairID
	-- info.oldcbUserStatus = self._UserList[userItem.dwUserID].cbUserStatus
	-- info.newcbUserStatus = self._UserList[userItem.dwUserID].cbUserStatus
	-- info.name = self._UserList[userItem.dwUserID].szNickName
	-- info.des = "用户进入"

	-- local jsonStr = cjson.encode(info)
 --    LogAsset:getInstance():logData(jsonStr, true)

	--记录自己桌椅号
	if userItem.dwUserID ==  GlobalUserItem.tabAccountInfo.dwUserID then
		self._wTableID = userItem.wTableID
		self._wChairID = userItem.wChairID
	end

	if userItem.wTableID ~= yl.INVALID_TABLE  and userItem.cbUserStatus ~= yl.US_LOOKON then
		self:onUpDataTableUser(userItem.wTableID,userItem.wChairID,userItem)

		if self._viewFrame and self._viewFrame.onEventUserEnter then
			self._viewFrame:onEventUserEnter(userItem.wTableID,userItem.wChairID,userItem)
		end
	end

	if bMySelfInfo == true and self._delayEnter == true then
		self._delayEnter = false
		--self:onSocketLogonFinish()
	end

end
--用户积分
function GameFrameEngine:onSocketUserScore(dataBuffer)
	
	local dwUserID = dataBuffer:readdword()

	local int64 = Integer64.new()
	local item = self._UserList[dwUserID]
	if  item ~= nil then
		local cmd_table = ExternalFun.read_netdata( cmdstruct.tagMobileUserScore, dataBuffer)
		--更新数据
		item.lScore = cmd_table.lScore
		item.dBeans =  cmd_table.dBeans
		item.dwWinCount = cmd_table.dwWinCount
		item.dwLostCount = cmd_table.dwLostCount
		item.dwDrawCount = cmd_table.dwDrawCount
		item.dwFleeCount = cmd_table.dwFleeCount
		item.dwExperience = cmd_table.dwExperience

		print("更新用户 uid, name, score", dwUserID, item.szNickName, item.lScore)
		--自己信息
		if item.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID and self:IsAllowPlazzScoreChange() then
			print("更新游戏币")
			GlobalUserItem.lUserScore = item.lScore
			GlobalUserItem.dUserBeans = item.dBeans
		end

		--通知更新界面
		if self._wTableID ~= yl.INVALID_TABLE and self._viewFrame and self._viewFrame.onEventUserScore  then
			self._viewFrame:onEventUserScore(item)
		end
	end  
end

--用户状态
function GameFrameEngine:onSocketUserStatus(dataBuffer)
	--读取信息
	local dwUserID 		= dataBuffer:readdword()
	local newstatus = ExternalFun.read_netdata( cmdstruct.tagUserStatus, dataBuffer )

	--获取自己
	local myUserItem  =  self:GetMeUserItem()

	local useritem = self._UserList[dwUserID]

	



	--未找到自己
	if not myUserItem then
		print(" GameFrameEngine:未找到自己, 查询自己信息 ")
		if newstatus.wTableID ~= yl.INVALID_TABLE then
			self._delayEnter = true
			self:QueryUserInfo(newstatus.wTableID,newstatus.wChairID)
			return
		end

		--非法信息
		self:onCloseSocket()
		if nil ~= self._callBack then
			--self._callBack(-1,"用户信息获取不正确,请重新登录！")
		end		
		return
	end	


		--过滤观看
	if newstatus.cbUserStatus == yl.US_LOOKON then

		if dwUserID == GlobalUserItem.tabAccountInfo.dwUserID  and myUserItem then

			useritem.cbUserStatus = newstatus.cbUserStatus
			useritem.wTableID = newstatus.wTableID
			useritem.wChairID = newstatus.wChairID

			self._wTableID = newstatus.wTableID
			self._wChairID = newstatus.wChairID


			if self._viewFrame and self._viewFrame.onEnterTable then
				self._viewFrame:onEnterTable()
			end

			self:SendGameOption()
			if self._viewFrame and self._viewFrame.onEventUserStatus then
			 	self._viewFrame:onEventUserStatus(useritem,newstatus)
			end

		end
		return
	end


	--自己判断
	local bMySelfInfo = (dwUserID == myUserItem.dwUserID)
	


	--找不到用户
	if useritem == nil then
		--当前桌子用户
		if newstatus.wTableID ~= yl.INVALID_TABLE then
			--虚拟信息
			-- local newitem = {}
			-- newitem.szNickName = "游戏玩家"
			-- newitem.dwUserID = dwUserID
			-- newitem.cbUserStatus = cbUserStatus
			-- newitem.wTableID = newstatus.wTableID
			-- newitem.wChairID = newstatus.wChairID

			-- self._UserList[dwUserID] = newitem
			-- self:onUpDataTableUser(newitem.wTableID,newitem.wChairID,newitem)
			--发送查询
			
			self:QueryUserInfo(newstatus.wTableID,newstatus.wChairID)

		end
		return
	end

	-- 记录旧状态
	local oldstatus = {}
	oldstatus.wTableID = useritem.wTableID
	oldstatus.wChairID = useritem.wChairID
	oldstatus.cbUserStatus = useritem.cbUserStatus
	--更新信息
	useritem.cbUserStatus = newstatus.cbUserStatus
	useritem.wTableID = newstatus.wTableID
	useritem.wChairID = newstatus.wChairID


	--清除旧桌子椅子记录
	if oldstatus.wTableID ~= yl.INVALID_TABLE then
		--新旧桌子不同 新旧椅子不同
		if (oldstatus.wTableID ~= newstatus.wTableID) or (oldstatus.wChairID ~= newstatus.wChairID) then

			self:onUpDataTableUser(oldstatus.wTableID, oldstatus.wChairID, nil)
		end
	end

	--新桌子记录
	if newstatus.wTableID ~= yl.INVALID_TABLE then
		self:onUpDataTableUser(newstatus.wTableID, newstatus.wChairID, useritem)
	end


	--自己状态
	if bMySelfInfo == true then
		GlobalUserItem.bLookOn = false
		self._wTableID = newstatus.wTableID
		self._wChairID = newstatus.wChairID

		--离开
		if newstatus.cbUserStatus == yl.US_NULL then
			print("自己离开")
			dump(newstatus)
			if self._viewFrame and self._viewFrame.onExitRoom and not GlobalUserItem.bWaitQuit then
				self._viewFrame:onExitRoom()
			end
		--起立
		elseif newstatus.cbUserStatus == yl.US_FREE and oldstatus.cbUserStatus > yl.US_FREE then
			print("自己起立")
			if self._viewFrame and self._viewFrame.onExitTable and not GlobalUserItem.bWaitQuit then
				if self.bEnterAntiCheatRoom then
					print("防作弊换桌")
					self:OnResetGameEngine()
				elseif MatchRoom and GlobalUserItem.bMatch then
					print("比赛场初始化")
					self:OnResetGameEngine()
				elseif not self.bChangeDesk  then
					self._viewFrame:onExitTable()
					
				else
					--self.bChangeDesk = false
					self:OnResetGameEngine()
				end
			end
		--坐下
		elseif newstatus.cbUserStatus >yl.US_FREE and (oldstatus.cbUserStatus <yl.US_SIT or oldstatus.cbUserStatus ==yl.US_LOOKON) then
			print("自己坐下")
			
			if self._viewFrame and self._viewFrame.onEnterTable then
				self._viewFrame:onEnterTable()
			end
			--showToast(self._viewFrame,"找到游戏桌子，正在获取场景中...",1)
			if self.bChangeDesk ==  true then
				self.bChangeDesk = false
				self.m_bSendOption = false
			end

			self:SendGameOption()
			if self._viewFrame and self._viewFrame.onEventUserStatus then
			 	self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		elseif newstatus.wTableID ~= yl.INVALID_TABLE and self.bChangeDesk == true then
			print("换位")
			if self._viewFrame and self._viewFrame.onEnterTable then
				self._viewFrame:onEnterTable()
			end
			--showToast(self._viewFrame,"找到游戏桌子，正在获取场景中...",1)
			self:SendGameOption()
			if self._viewFrame and self._viewFrame.onEventUserStatus then
				self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		else 
			print("自己新状态:"..newstatus.cbUserStatus)
			if self._viewFrame and self._viewFrame.onEventUserStatus then
				self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		end 
	--他人状态
	else
		--更新用户
		if oldstatus.wTableID ~= yl.INVALID_TABLE or newstatus.wTableID ~= yl.INVALID_TABLE then
			if self._viewFrame and self._viewFrame.onEventUserStatus then
				self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		end
		--删除用户
		if newstatus.cbUserStatus == yl.US_NULL or newstatus.cbUserStatus == yl.US_FREE then
			self:onRemoveUser(dwUserID)
		end
	end
end

--请求失败
function GameFrameEngine:onSocketReQuestFailure(dataBuffer)
	local cmdtable = ExternalFun.read_netdata(game_cmd.CMD_GR_RequestFailure, dataBuffer)
	--dump(cmdtable)
	if cmdtable.lErrorCode == 1000  then  --旁观请求坐下失败
		self.bChangeDesk = false
		if self._viewFrame then
			showToast(self._viewFrame,cmdtable.szDescribeString,2)
		end
		return 
	end
	if self._viewFrame and self._viewFrame.onReQueryFailure then
		self._viewFrame:onReQueryFailure(cmdtable.lErrorCode,cmdtable.szDescribeString)
 	else
 		dump(cmdtable, "onSocketReQuestFailure", 6)
 		print("not viewframe or not onReQueryFailure")
	end
	
	if self.bChangeDesk == true  then
		self.bChangeDesk = false
		if  self._viewFrame and self._viewFrame.onExitTable and not GlobalUserItem.bWaitQuit then
			self._viewFrame:onExitTable()
		end
	end
	-- 清理锁表
	GlobalUserItem.tabAccountInfo.dwLockServerID = 0
	GlobalUserItem.tabAccountInfo.dwLockKindID = 0
end

--状态信息
function GameFrameEngine:onSocketStatusEvent(sub,dataBuffer)
	if sub == game_cmd.SUB_GR_TABLE_INFO then
		print("SUB_GR_TABLE_INFO")
		local wTableCount = dataBuffer:readword()
		for i = 1, wTableCount do
			self._tableStatus[i] = ExternalFun.read_netdata( cmdstruct.tabTableStatus, dataBuffer )
		end
		if not GlobalUserItem.bPrivateRoom and not GlobalUserItem.bMatch then
			if self._viewFrame and self._viewFrame.onEnterRoom then
				self._viewFrame:onEnterRoom()
			end
		end		

		if self._viewFrame and self._viewFrame.onGetTableInfo then
			self._viewFrame:onGetTableInfo()
		end
	elseif sub == game_cmd.SUB_GR_TABLE_STATUS then	--桌子状态		
		local wTableID = dataBuffer:readword() + 1		
		self._tableStatus[wTableID] = ExternalFun.read_netdata( cmdstruct.tabTableStatus, dataBuffer )

		print("SUB_GR_TABLE_STATUS ==> ", wTableID--[[ .. " ==> " .. self._tableStatus[wTableID].cbPlayStatus]])
		if self._viewFrame and self._viewFrame.upDataTableStatus then
			self._viewFrame:upDataTableStatus(wTableID)
		end
	end
end

--框架信息
function GameFrameEngine:onSocketFrameEvent(sub,dataBuffer)
	--游戏状态
	if sub == game_cmd.SUB_GF_GAME_STATUS then
		self._cbGameStatus = dataBuffer:readword()
		self._cbAllowLookon = dataBuffer:readword()
	--游戏场景
	elseif sub == game_cmd.SUB_GF_GAME_SCENE then
		 print("2222222222222222222222222222222222222222222222222222222222222222222222222222222")
		if self._viewFrame and self._viewFrame.onEventGameScene then
			self._viewFrame:onEventGameScene(self._cbGameStatus,dataBuffer)
		else
			print("game scene did not respon")
			if not self._viewFrame then
				print ("viewframe is nl")
			else
				print("onEventGameScene is ni viewframe is"..self._viewFrame:getTag())
			end
		end
	--系统消息
	elseif sub == game_cmd.SUB_GF_SYSTEM_MESSAGE then
		self:onSocketSystemMessage(dataBuffer)
	--动作消息
	elseif sub == game_cmd.SUB_GF_ACTION_MESSAGE then
		self:onSocketActionMessage(dataBuffer)
	--用户聊天
	elseif sub == game_cmd.SUB_GF_USER_CHAT then
		local chat = ExternalFun.read_netdata(game_cmd.CMD_GF_S_UserChat, dataBuffer)
		--获取玩家昵称
		local useritem = self._UserList[chat.dwSendUserID]
		if not  useritem then
			return
		end
		if self._wTableID == yl.INVALID_CHAIR or self._wTableID ~= useritem.wTableID then
			return
		end

		chat.szNick = useritem.szNickName
		GameChatLayer.addChatRecordWith(chat)
		        if nil ~= self._viewFrame and nil ~= self._viewFrame.addChatRecordWithGame then
		            self._viewFrame:addChatRecordWithGame(chat)
		        end 

		if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserChat then
			-- 播放声音
			local idx = GameChatLayer.compareWithText(chat.szChatString)
			if nil ~= idx then
				local sound_path = "sound/" .. useritem.cbGender .. "_" .. idx .. ".mp3"
				if GlobalUserItem.bSoundAble then
					AudioEngine.playEffect(cc.FileUtils:getInstance():fullPathForFilename(sound_path),false)
				end
			end
			self._viewFrame:onUserChat(chat,useritem.wChairID)
		end
	--用户表情
	elseif sub == game_cmd.SUB_GF_USER_EXPRESSION then
		local expression = ExternalFun.read_netdata(game_cmd.CMD_GF_S_UserExpression, dataBuffer)
		--获取玩家昵称
		local useritem = self._UserList[expression.dwSendUserID]

		if not  useritem then
			return
		end
		if self._wTableID == yl.INVALID_CHAIR or self._wTableID ~= useritem.wTableID then
			return
		end
		
		expression.szNick = useritem.szNickName

		GameChatLayer.addChatRecordWith(expression, true)
	        if nil ~= self._viewFrame and nil ~= self._viewFrame.addChatRecordWithGame then
	            self._viewFrame:addChatRecordWithGame(expression, true)
	        end 
			if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserExpression then
				self._viewFrame:onUserExpression(expression,useritem.wChairID)
			end
		-- 用户语音
		elseif sub == game_cmd.SUB_GF_USER_VOICE then
			AudioRecorder:getInstance():saveRecordFile(dataBuffer, function(uid, tid, spath)
				local msgTab = {}
				msgTab.uid = uid
				msgTab.tid = tid
				msgTab.spath = spath
				table.insert(self._tabVoiceMsgQueue, msgTab)
				
				self:popVocieMsg()
	        end)
    	--- 通知玩家准备
    	elseif sub == game_cmd.SUB_GF_NOTICE_READ then
    		if nil ~= self._viewFrame and nil ~= self._viewFrame.onGetNoticeReady then
			self._viewFrame:onGetNoticeReady()
		end
	elseif sub == game_cmd.SUB_GF_GAME_USER_COMMOND then
		local cmd_data = ExternalFun.read_netdata(game_cmd.CMD_GF_S_UserCommond, dataBuffer)
		if nil ~= self._viewFrame and nil ~= self._viewFrame.onEventShowBegin then
			self._viewFrame:onEventShowBegin(cmd_data.dwWaitting)
		end	
	end
end

--系统消息
function GameFrameEngine:onSocketSystemMessage(dataBuffer)
	local wType = dataBuffer:readword()
	local wLength = dataBuffer:readword()
	local szString = dataBuffer:readstring()
	print("系统消息#"..wType.."#"..szString)
	local bCloseRoom = bit:_and(wType,yl.SMT_CLOSE_ROOM)
	local bCloseGame = bit:_and(wType,yl.SMT_CLOSE_GAME)
	local bCloseLink = bit:_and(wType,yl.SMT_CLOSE_LINK)
	local bSystemTip = bit:_and(wType,yl.SMT_EJECT)
	if self._viewFrame and bSystemTip ~= 0 then
		showToast(self._viewFrame,szString,2)
	end
	print("bCloseRoom ==> ", bCloseRoom)                
	print("bCloseGame ==> ", bCloseGame)
	print("bCloseLink ==> ", bCloseLink)
	if bCloseRoom ~= 0 or bCloseGame ~= 0 or bCloseLink ~=0 then
		if 515 == wType or 501 == wType then
			if self._viewFrame and self._viewFrame.onSystemMessage then
				self._viewFrame:onSystemMessage(wType, szString)
			end
			
		else
			self:setEnterAntiCheatRoom(false)
			if self._viewFrame and self._viewFrame.onExitRoom and not GlobalUserItem.bWaitQuit then
				if MatchRoom then
					self:onCloseSocket()
					return
				end
				self._viewFrame:onExitRoom()
			else
				self:onCloseSocket()
			end
		end
	end
end

function GameFrameEngine:onSocketPropEvent(sub,dataBuffer)
	if sub ==  game_cmd.SUB_GP_LIST_PROPERTY then
		local len = dataBuffer:getlen()
		if (len - math.floor(len/ 377) *377) ~= 0 then
			print("PropertyList_len_error"..len)
			return
		end
		local itemcount =  math.floor(len/377)
		
		--读取房间信息
		print("itemcount is ================",itemcount)
		for i = 1,itemcount do
			local item = ExternalFun.read_netdata(game_cmd.tagPropertyItem, dataBuffer)
			--dump(item, "onSubRoomListInfo", 10)
			if not item then 
				break
			end
			if (item.cbPlatformKind ==0 or  item.cbPlatformKind ==1) and item.wKind ==yl.PROP_INTERACT then
				if #GlobalUserItem.tabPropertyItem > 0 then
					for k,v in pairs(GlobalUserItem.tabPropertyItem) do
						if v then
							if v.dwPropID ==  item.dwPropID then
								break
							elseif k ==#GlobalUserItem.tabPropertyItem then
								table.insert(GlobalUserItem.tabPropertyItem, item)
							end
						end
					end
				else
					table.insert(GlobalUserItem.tabPropertyItem, item)
				end
				
			end
		end
		--assert(false)
		--收到道具信息后，查询自身道具数量
		self:QueryUserProp()
	elseif sub ==  game_cmd.SUB_GR_PROPERTY_BACKPACK_RESULT then
		local cmd_data = ExternalFun.read_netdata(game_cmd.CMD_GR_S_BackpackProperty, dataBuffer)
		if cmd_data.dwCount > 0 then
			for i=1,cmd_data.dwCount do
				local item = ExternalFun.read_netdata(game_cmd.tagBackpackProperty, dataBuffer)
				for k,v in pairs(GlobalUserItem.tabPropertyItem) do
					if v and v.dwPropID ==item.Property.wIndex then
						v.cbPropCount = item.nCount
					end
				end
			end
		end
	elseif sub ==  game_cmd.SUB_GR_GAME_PROPERTY_BUY_RESULT then
		local cmd_data = ExternalFun.read_netdata(game_cmd.CMD_GR_S_PropertyBuyResult, dataBuffer)
		if self.dwPropRecvUserID then
			self:SendUseProp(self.dwPropRecvUserID,cmd_data.dwPropID,1)
		end
	elseif sub ==  game_cmd.SUB_GR_GAME_PROPERTY_FAILURE then
		local cmd_data = ExternalFun.read_netdata(game_cmd.CMD_GR_PropertyFailure, dataBuffer)
		if self._viewFrame then
			showToast(self._viewFrame,cmd_data.szDescribeString,2)
		end
	elseif sub ==  game_cmd.SUB_GR_PROPERTY_USE_RESULT then
		local cmd_data = ExternalFun.read_netdata(game_cmd.CMD_GR_S_PropertyUse, dataBuffer)
		for k,v in pairs(GlobalUserItem.tabPropertyItem) do
			if v and v.dwPropID ==cmd_data.dwPropID then
				v.cbPropCount = cmd_data.dwRemainderPropCount
			end
		end
		if self._viewFrame then
			local sound_path = "sound/prop_" .. cmd_data.dwPropID .. ".mp3"
				if GlobalUserItem.bSoundAble then
					local time = 0.7
					if cmd_data.dwPropID ==3 then
						time = 0.45
					elseif  cmd_data.dwPropID == 4 then
						time = 1
					end
					self._viewFrame:runAction(cc.Sequence:create(cc.DelayTime:create(time),cc.CallFunc:create(function ( ... )
						AudioEngine.playEffect(cc.FileUtils:getInstance():fullPathForFilename(sound_path),false)
					end)))
					
				end

			self._viewFrame:onEventUserPropAction(cmd_data.dwUserID,cmd_data.dwRecvUserID,cmd_data.dwPropID)
		end
	else

	end
end

--系统动作
function GameFrameEngine:onSocketActionMessage(dataBuffer)
	local wType = dataBuffer:readword()
	local wLength = dataBuffer:readword()
	local nButtonType = dataBuffer:readint()
	local szString = dataBuffer:readstring()
	print("系统动作#"..wType.."#"..szString)
		
	local bCloseRoom = bit:_and(wType,yl.SMT_CLOSE_ROOM)
	local bCloseGame = bit:_and(wType,yl.SMT_CLOSE_GAME)
	local bCloseLink = bit:_and(wType,yl.SMT_CLOSE_LINK)

	if self._viewFrame then
		--showToast(self._viewFrame,szString,2,cc.c3b(250,0,0))
	end
	if bCloseRoom ~= 0 or bCloseGame ~= 0 or bCloseLink ~=0 then
		self:setEnterAntiCheatRoom(false)
		if self._viewFrame and self._viewFrame.onExitRoom and not GlobalUserItem.bWaitQuit then
			if MatchRoom then
				self:onCloseSocket()
				return
			end
			self._viewFrame:onExitRoom()
		else
			self:onCloseSocket()
		end
	end
end


--更新桌椅用户
function GameFrameEngine:onUpDataTableUser(tableid,chairid,useritem)
	local id = tableid + 1
	local idex = chairid + 1
	if not self._tableUserList[id]  then
		self._tableUserList[id] = {}
	end
	if useritem then
		self._tableUserList[id][idex] = useritem.dwUserID
	else
		self._tableUserList[id][idex] = nil
	end
end

--获取桌子用户
function GameFrameEngine:getTableUserItem(tableid,chairid)
	
	local id = tableid + 1
	local idex = chairid + 1
	if self._tableUserList[id]  then
		local userid = self._tableUserList[id][idex] 
		if userid then
			return clone(self._UserList[userid])
		end
	end
end

function GameFrameEngine:getTableInfo(index)
	if index > 0  then
		return self._tableStatus[index]
	end
end

--获取自己游戏信息
function GameFrameEngine:GetMeUserItem()
	if GlobalUserItem.bVideo then
		return self._UserList[GlobalUserItem.dwVideoUserID]
	else
		return self._UserList[GlobalUserItem.tabAccountInfo.dwUserID]
	end
end

--获取游戏状态
function GameFrameEngine:GetGameStatus()
	return self._cbGameStatus
end

--设置游戏状态
function GameFrameEngine:SetGameStatus(cbGameStatus)
	self._cbGameStatus = cbGameStatus
end

--获取桌子ID
function GameFrameEngine:GetTableID()
	return self._wTableID
end

--获取椅子ID
function GameFrameEngine:GetChairID()
	return self._wChairID
end

--移除用户
function GameFrameEngine:onRemoveUser(dwUserID)
	self._UserList[dwUserID] = nil
end

--坐下请求
function GameFrameEngine:SitDown(table ,chair, password, dwpasswd)
	local dataBuffer = CCmd_Data:create(74)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_USER, game_cmd.SUB_GR_USER_SITDOWN)
	dataBuffer:pushword(table)
	dataBuffer:pushword(chair)
	self._reqTable = table
	self._reqChair = chair
	if password then
		dataBuffer:pushstring(password,yl.LEN_PASSWORD)
	else
		dataBuffer:pushstring("",yl.LEN_PASSWORD)
	end
	dataBuffer:pushdword(dwpasswd)

	--记录坐下信息
	if nil ~= GlobalUserItem.m_tabEnterGame and type(GlobalUserItem.m_tabEnterGame) == "table" then
		print("update game info")
		GlobalUserItem.m_tabEnterGame.nSitTable = table
		GlobalUserItem.m_tabEnterGame.nSitChair = chair
	end
	return self:sendSocketData(dataBuffer)
end

--换椅子
function GameFrameEngine:QueryChangeChair(table ,chair)
	self.bChangeDesk = true
	local dataBuffer = CCmd_Data:create(74)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_USER, game_cmd.SUB_GR_USER_CHANGE_CHAIR)
	dataBuffer:pushword(table)
	dataBuffer:pushword(chair)
	self._reqTable = table
	self._reqChair = chair
	dataBuffer:pushstring("",yl.LEN_PASSWORD)
	
	dataBuffer:pushdword(dwpasswd)

	--记录坐下信息
	if nil ~= GlobalUserItem.m_tabEnterGame and type(GlobalUserItem.m_tabEnterGame) == "table" then
		print("update game info")
		GlobalUserItem.m_tabEnterGame.nSitTable = table
		GlobalUserItem.m_tabEnterGame.nSitChair = chair
	end
	return self:sendSocketData(dataBuffer)
end


function GameFrameEngine:LookOn(table ,chair)
	local dataBuffer = CCmd_Data:create()
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_USER, game_cmd.SUB_GR_USER_LOOKON)
	dataBuffer:pushword(table)
	dataBuffer:pushword(chair)
	self._reqTable = table
	self._reqChair = chair
	--记录坐下信息
	if nil ~= GlobalUserItem.m_tabEnterGame and type(GlobalUserItem.m_tabEnterGame) == "table" then
		print("update game info")
		GlobalUserItem.m_tabEnterGame.nSitTable = table
		GlobalUserItem.m_tabEnterGame.nSitChair = chair
	end
	return self:sendSocketData(dataBuffer)

end

--查询用户
function GameFrameEngine:QueryUserInfo(table ,chair)
	local dataBuffer = CCmd_Data:create(4)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_USER,game_cmd.SUB_GR_USER_CHAIR_INFO_REQ)
	dataBuffer:pushword(table)
	dataBuffer:pushword(chair)
	return self:sendSocketData(dataBuffer)
end


--查询道具信息
function GameFrameEngine:QueryUserProp()
	local dataBuffer = CCmd_Data:create(8)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_BACKPACK)
	dataBuffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	dataBuffer:pushdword(0)--查询所有道具
	return self:sendSocketData(dataBuffer)
end


--换位请求
function GameFrameEngine:QueryChangeDesk()
	self.bChangeDesk = true
	local dataBuffer = CCmd_Data:create(0)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_USER,game_cmd.SUB_GR_USER_CHAIR_REQ)
	return self:sendSocketData(dataBuffer)
end

--起立请求
function GameFrameEngine:StandUp(bForce)
	local dataBuffer = CCmd_Data:create(5)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_USER,game_cmd.SUB_GR_USER_STANDUP)
	dataBuffer:pushword(self:GetTableID())
	dataBuffer:pushword(self:GetChairID())
	dataBuffer:pushbyte(not bForce and 0 or 1)
	return self:sendSocketData(dataBuffer)
end


--购买互动道具
function GameFrameEngine:SendBuyProp(dwRecvUserID,dwPropID,wCount,payType)
	self.dwPropRecvUserID = dwRecvUserID

	local dataBuffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_GamePropertyBuy)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_GAME_PROPERTY_BUY)
	dataBuffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	dataBuffer:pushdword(dwPropID)
	dataBuffer:pushbyte(payType)
	dataBuffer:pushdword(wCount)
	dataBuffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
	dataBuffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	return self:sendSocketData(dataBuffer)
end


--使用互动道具
function GameFrameEngine:SendUseProp(dwRecvUserID,dwPropID,wCount)
	local dataBuffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_PropertyUse)
	dataBuffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_USE)
	dataBuffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	dataBuffer:pushdword(dwRecvUserID)
	dataBuffer:pushdword(dwPropID)
	dataBuffer:pushword(wCount)

	return self:sendSocketData(dataBuffer)
end


--发送准备
function GameFrameEngine:SendUserReady(dataBuffer)
	local userReady = dataBuffer
	if not userReady then
		userReady = CCmd_Data:create(0)
	end
	userReady:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_USER_READY)
	return self:sendSocketData(userReady)
end

--房主动态开始游戏
function GameFrameEngine:SendUserBegin(dataBuffer)
	local userBegin = dataBuffer
	if not userBegin then
		userBegin = CCmd_Data:create(0)
	end
	userBegin:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_COMMOND_START)
	return self:sendSocketData(userBegin)
end


--场景规则
function GameFrameEngine:SendGameOption()
	if self.m_bSendOption then
		print("不重发 GameOption 消息")
		return
	end
	self.m_bSendOption = true
	print("GameFrameEngine:SendGameOption")
	local dataBuffer = CCmd_Data:create(9)
	dataBuffer:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_GAME_OPTION)
	dataBuffer:pushbyte(0)
	dataBuffer:pushdword(appdf.VersionValue(6,7,0,1))
	dataBuffer:pushdword(self._kindVersion)
	return self:sendSocketData(dataBuffer)
end

--加密桌子
function GameFrameEngine:SendEncrypt(pass)
	local passlen = string.len(pass) * 2 --14--(ExternalFun.stringLen(pass)) * 2
	print("passlen ==> " .. passlen)
	local len = passlen + 4 + 13--(sizeof game_cmd.CMD_GR_UserRule)
	print("len ==> " .. len)
	local cmddata = CCmd_Data:create(len)
	cmddata:setcmdinfo(game_cmd.MDM_GR_USER, game_cmd.SUB_GR_USER_RULE)
	cmddata:pushbyte(0)
	cmddata:pushword(0)
	cmddata:pushword(0)
	cmddata:pushint(0)
	cmddata:pushint(0)
	cmddata:pushword(passlen)
	cmddata:pushword(game_cmd.DTP_GR_TABLE_PASSWORD)	
	cmddata:pushstring(pass, passlen / 2)

	return self:sendSocketData(cmddata)
end

--发送文本聊天 game_cmd.CMD_GF_C_UserChat
--[msg] 聊天内容
--[tagetUser] 目标用户
function GameFrameEngine:sendTextChat( msg, tagetUser , color)
	if type(msg) ~= "string" then
		print("聊天内容异常")
		return false, "聊天内容异常!"
	end
	--敏感词判断
	if true == ExternalFun.isContainBadWords(msg) then
		print("聊天内容包含敏感词汇")
		return false, "聊天内容包含敏感词汇!"
	end
	msg = msg .. "\0"

	tagetUser = tagetUser or yl.INVALID_USERID
	color = color or 16777215 --appdf.ValueToColor( 255,255,255 )
	local msgLen = string.len(msg)
	local defineLen = yl.LEN_USER_CHAT * 2

	local cmddata = CCmd_Data:create(266 - defineLen + msgLen * 2)
	cmddata:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_USER_CHAT)
	cmddata:pushword(msgLen)
	cmddata:pushdword(color)
	cmddata:pushdword(tagetUser)
	cmddata:pushstring(msg, msgLen)

	return self:sendSocketData(cmddata)
end

--发送表情聊天 game_cmd.CMD_GF_C_UserExpressio
--[idx] 表情图片索引
--[tagetUser] 目标用户
function GameFrameEngine:sendBrowChat( idx, tagetUser )
	tagetUser = tagetUser or yl.INVALID_USERID

	local cmddata = CCmd_Data:create(6)
	cmddata:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_USER_EXPRESSION)
	cmddata:pushword(idx)
	cmddata:pushdword(tagetUser)

	return self:sendSocketData(cmddata)
end

function GameFrameEngine:OnResetGameEngine()
	if self._viewFrame and self._viewFrame.OnResetGameEngine then
		self._viewFrame:OnResetGameEngine()
	end
end

function GameFrameEngine:popVocieMsg()
	if self._bPlayVoiceRecord then
		return
	end

	local msgTab = self._tabVoiceMsgQueue[1]
	if type(msgTab) == "table" then
		local uid = msgTab.uid
		local spath = msgTab.spath

		--获取玩家
		local useritem = self._UserList[uid]
		if nil ~= useritem then
			-- -- 播放开始
			-- if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserVoiceStart then
			-- 	self._viewFrame:onUserVoiceStart(useritem, spath)
			-- end
			-- 播放开始
			local event = cc.EventCustom:new(yl.RY_VOICE_NOTIFY)
		    event.obj = yl.RY_MSG_VOICE_START
		    event.useritem = useritem
		    event.spath = spath
		    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)

			local tmpNode = nil
			local runScene = cc.Director:getInstance():getRunningScene()
			if nil ~= runScene then
				local child = runScene:getChildByName("__gameframeengine_voice_node__")
				if nil ~= child then
					child:removeFromParent()
				end
				tmpNode = cc.Node:create()
				tmpNode:setName("__gameframeengine_voice_node__")
				runScene:addChild(tmpNode)

				tmpNode:runAction(cc.Sequence:create(cc.DelayTime:create(VoiceRecorderKit.MAX_RECORD_TIME + 2), cc.CallFunc:create(function()
					print("delay remove")
					tmpNode:removeFromParent()
					if nil ~= self._nPlayVoiceId then
						ccexp.AudioEngine:stop(self._nPlayVoiceId)
						self._nPlayVoiceId = nil
					end 
					self:onVoiceFinish(useritem, spath)
				end)))
			end

			self._nPlayVoiceId = VoiceRecorderKit.startPlayVoice(spath)
			if -1 ~= self._nPlayVoiceId then
				AudioRecorder:getInstance():setFinishCallBack(self._nPlayVoiceId, function(voiceid, filename)
	                print("play over ", filename)
	                if nil ~= tmpNode and nil ~= tmpNode:getParent() then
	                	tmpNode:stopAllActions()
	                	tmpNode:removeFromParent()
	                end
	                self:onVoiceFinish(useritem, spath)    
	            end)
				self._bPlayVoiceRecord = true
			else
				print("录音播放异常")
				self:onVoiceFinish(useritem, spath) 
			end
		end
	end
	table.remove(self._tabVoiceMsgQueue, 1)
end

function GameFrameEngine:onVoiceFinish(useritem, spath)
	if type(spath) == "string" then
		cc.FileUtils:getInstance():removeFile(spath)
	end
	VoiceRecorderKit.finishPlayVoice()
    self._nPlayVoiceId = nil

    -- 录音结束
    -- if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserVoiceEnded then
    -- 	self._viewFrame:onUserVoiceEnded(useritem, spath)
    -- end   
    local event = cc.EventCustom:new(yl.RY_VOICE_NOTIFY)
    event.obj = yl.RY_MSG_VOICE_END
    event.useritem = useritem
    event.spath = spath 
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)    
    self:setPlayingVoice(false)     
end

function GameFrameEngine:setPlayingVoice( bPlaying )
	self._bPlayVoiceRecord = bPlaying
	if false == bPlaying then
		self:popVocieMsg()
	end 
end

function GameFrameEngine:clearVoiceQueue()
	self._tabVoiceMsgQueue = {}
	self._bPlayVoiceRecord = false
	if nil ~= self._nPlayVoiceId then
		ccexp.AudioEngine:stop(self._nPlayVoiceId)
        VoiceRecorderKit.finishPlayVoice()
        self._nPlayVoiceId = nil
	end
	if nil ~= AudioRecorder:getInstance().clear then
		AudioRecorder:getInstance():clear()
	end
end
return GameFrameEngine
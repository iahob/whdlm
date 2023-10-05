local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local ModifyFrame = class("ModifyFrame",BaseFrame)

function ModifyFrame:ctor(view,callbcak)
	ModifyFrame.super.ctor(self,view,callbcak)
end
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 修改信息
local MODIFY_USERINFO = loginCMD.SUB_GP_MODIFY_INDIVIDUAL
-- 修改头像
local MODIFY_SYSTEM_FACE = loginCMD.SUB_GP_SYSTEM_FACE_INFO
-- 查询信息
local QUERY_USERINFO = loginCMD.SUB_GP_QUERY_INDIVIDUAL
ModifyFrame.QUERY_USERINFO = QUERY_USERINFO
-- 绑定推广员
local INPUT_SPREADER = 12--loginCMD.SUB_GP_MODIFY_INDIVIDUAL 与修改个人信息命令一致, 需要做区分
ModifyFrame.INPUT_SPREADER = INPUT_SPREADER
-- 实名认证
local OP_REALAUTH = loginCMD.SUB_GP_REAL_AUTH_QUERY
ModifyFrame.OP_REALAUTH = OP_REALAUTH

--连接结果
function ModifyFrame:onConnectCompeleted()
	if self._oprateCode == MODIFY_USERINFO then	 		--修改信息
		self:sendModifyUserInfo()
	elseif self._oprateCode == QUERY_USERINFO then 		-- 查询信息
		self:sendQueryUserInfo()
	elseif self._oprateCode == INPUT_SPREADER then 		-- 填写推广员
		self:sendInputSpreader()
	elseif self._oprateCode == OP_REALAUTH then 		-- 认证
		self:sendRealAuth()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知操作模式！")
		end		
	end
end

--网络信息(短连接)
function ModifyFrame:onSocketEvent(main,sub,pData)
	if main == loginCMD.MDM_GP_USER_SERVICE then 				-- 用户服务
		if sub == loginCMD.SUB_GP_OPERATE_SUCCESS then
			self:onSubOperateSuccess(pData)
		elseif sub == loginCMD.SUB_GP_OPERATE_FAILURE then
			self:onSubOperateFailure(pData)
		elseif sub == loginCMD.SUB_GP_USER_INDIVIDUAL then  	-- 信息
			self:onSubQueryUserInfoResult(pData)
		elseif sub == loginCMD.SUB_GP_INDIVIDUAL_RESULT then
			local cmd_table = ExternalFun.read_netdata(loginCMD.CMD_GP_IndividuaResult, pData)
			dump(cmd_table, "CMD_GP_IndividuaResult")
			if cmd_table.bSuccessed then
				GlobalUserItem.tabPlusInfo.szPassportID = self._szPassportID
				GlobalUserItem.tabPlusInfo.szTrueName = self._szCompellation

				-- 更新钻石
				GlobalUserItem.tabAccountInfo.lDiamond = cmd_table.lCurrDiamond
				--通知更新        
                local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                eventListener.obj = yl.RY_MSG_USERWEALTH
                cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
			end
			if nil ~= self._callBack then
				self._callBack(self._oprateCode,cmd_table)
			end
		elseif sub == loginCMD.SUB_GP_BIND_SPREADER_RESULT then
			local cmd_table = ExternalFun.read_netdata(loginCMD.CMD_GP_BindSpreaderResult, pData)
			dump(cmd_table, "CMD_GP_BindSpreaderResult")
			-- 更新钻石
			GlobalUserItem.tabAccountInfo.lDiamond = cmd_table.dwDiamond
			GlobalUserItem.tabPlusInfo.szSpreaderAccount = self._SpreaderID
			--通知更新        
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
            eventListener.obj = yl.RY_MSG_USERWEALTH
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
            if nil ~= self._callBack then
				self._callBack(self._oprateCode,cmd_table.szDescribeString)
			end
		end
	end
	self:onCloseSocket()
end

--网络消息(长连接)
function ModifyFrame:onGameSocketEvent(main,sub,pData)
	
end

function ModifyFrame:onSubOperateSuccess(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	if self._oprateCode == MODIFY_USERINFO then
		GlobalUserItem.tabAccountInfo.cbGender = self._cbGender
		GlobalUserItem.tabAccountInfo.szNickName = self._szNickname
		GlobalUserItem.tabPlusInfo.szSign = self._szSign
	elseif self._oprateCode == INPUT_SPREADER then
		GlobalUserItem.tabPlusInfo.szSpreaderAccount = self._SpreaderID
	end
	if nil ~= self._callBack then
		self._callBack(self._oprateCode,szDescribe)
	end	
end

function ModifyFrame:onSubOperateFailure(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	print("ModifyFrame:onSubOperateFailure ==> ", szDescribe)
	if nil ~= self._callBack then
		self._callBack(-1,szDescribe)
	end	
end

function ModifyFrame:onSubUserFaceInfoResult( pData )
	local wFaceId = pData:readword()
	local dwCustomId = pData:readdword()

	GlobalUserItem.wFaceID = wFaceId
	GlobalUserItem.dwCustomID = dwCustomId

	if nil ~= self._callBack then
		self._callBack(yl.SUB_GP_USER_FACE_INFO,"头像修改成功")
	end	
end

function ModifyFrame:_onSubQueryUserInfoResult( pData )
	local cmd_table = ExternalFun.read_netdata(loginCMD.CMD_GP_UserIndividual, pData)
	-- 附加包
	local curlen = pData:getcurlen()
	local datalen = pData:getlen()
	print("*** curlen-"..curlen)
	print("*** datalen-"..datalen)

	local tmpSize = 0
	local tmpCmd = 0
	while curlen < datalen do
		tmpSize = pData:readword()
		tmpCmd = pData:readword()
		if not tmpSize or not tmpCmd then
		 	break
		end
		print("*** tmpSize-"..tmpSize)
		print("*** tmpCmd-"..tmpCmd)

		if tmpCmd == loginCMD.DTP_GP_UI_SPREADER then -- 推广
			GlobalUserItem.tabPlusInfo.szSpreaderAccount = pData:readstring(tmpSize/2) or ""
			print("szSpreaderAccount ", GlobalUserItem.tabPlusInfo.szSpreaderAccount)
		elseif tmpCmd == loginCMD.DTP_GP_UI_QQ then
			GlobalUserItem.tabPlusInfo.szQQNumber = pData:readstring(tmpSize/2) or ""
			print("qq ", GlobalUserItem.tabPlusInfo.szQQNumber)
		elseif tmpCmd == loginCMD.DTP_GP_UI_EMAIL then
			GlobalUserItem.tabPlusInfo.szEmailAddress = pData:readstring(tmpSize/2) or ""
			print("email ", GlobalUserItem.tabPlusInfo.szEmailAddress)
		elseif tmpCmd == loginCMD.DTP_GP_UI_SEAT_PHONE then
			GlobalUserItem.tabPlusInfo.szSeatPhone = pData:readstring(tmpSize/2) or ""
			print("szSeatPhone ", GlobalUserItem.tabPlusInfo.szSeatPhone)
		elseif tmpCmd == loginCMD.DTP_GP_UI_MOBILE_PHONE then
			GlobalUserItem.tabPlusInfo.szMobilePhone = pData:readstring(tmpSize/2) or ""
			print("szMobilePhone ", GlobalUserItem.tabPlusInfo.szMobilePhone)
		elseif tmpCmd == loginCMD.DTP_GP_UI_COMPELLATION then
			GlobalUserItem.tabPlusInfo.szTrueName = pData:readstring(tmpSize/2) or ""
			print("szTrueName ", GlobalUserItem.tabPlusInfo.szTrueName)
		elseif tmpCmd == loginCMD.DTP_GP_UI_DWELLING_PLACE then
			GlobalUserItem.tabPlusInfo.szAddress = pData:readstring(tmpSize/2) or ""
			print("szAddress ", GlobalUserItem.tabPlusInfo.szAddress)
		elseif tmpCmd == loginCMD.DTP_GP_UI_PASSPORTID then
			GlobalUserItem.tabPlusInfo.szPassportID = pData:readstring(tmpSize/2) or ""
			print("szPassportID ", GlobalUserItem.tabPlusInfo.szPassportID)
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
	-- 标记已请求
	GlobalUserItem.tabPlusInfo.bQueryInfo = true
end

function ModifyFrame:onSubQueryUserInfoResult( pData )
	self:_onSubQueryUserInfoResult(pData)
	if nil ~= self._callBack then
		self._callBack(QUERY_USERINFO)
	end
end

function ModifyFrame:sendModifyUserInfo()
	local ModifyUserInfo = CCmd_Data:create(817)

	ModifyUserInfo:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_MODIFY_INDIVIDUAL)
	ModifyUserInfo:pushbyte(self._cbGender)
	ModifyUserInfo:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	ModifyUserInfo:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)

	------
	--附加信息
	-- 昵称
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_NICKNAME)
	ModifyUserInfo:pushstring(self._szNickname,yl.LEN_NICKNAME)
	-- 签名
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_UNDER_WRITE)
	ModifyUserInfo:pushstring(self._szSign,yl.LEN_UNDER_WRITE)
	-- qq
	ModifyUserInfo:pushword(32)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_QQ)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szQQNumber,yl.LEN_QQ)
	-- email
	ModifyUserInfo:pushword(66)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_EMAIL)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szEmailAddress,yl.LEN_EMAIL)
	-- 座机
	ModifyUserInfo:pushword(66)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_SEAT_PHONE)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szSeatPhone,yl.LEN_SEAT_PHONE)
	-- 手机
	ModifyUserInfo:pushword(24)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_MOBILE_PHONE)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szMobilePhone,yl.LEN_MOBILE_PHONE)
	-- 真实姓名
	ModifyUserInfo:pushword(32)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_COMPELLATION)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szTrueName,yl.LEN_COMPELLATION)
	-- 联系地址
	ModifyUserInfo:pushword(256)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_DWELLING_PLACE)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szAddress,yl.LEN_DWELLING_PLACE)
	-- 身份证
	ModifyUserInfo:pushword(38)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_PASSPORTID)
	ModifyUserInfo:pushstring(GlobalUserItem.tabPlusInfo.szPassportID,yl.LEN_PASS_PORT_ID)
	--附加信息
	------

	if not self:sendSocketData(ModifyUserInfo) and nil ~= self._callBack then
		self._callBack(-1,"发送修改资料失败！")
	end
end

-- 发送查询信息
function ModifyFrame:getSendQueryUserInfoBuffer()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_QueryIndividual)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_QUERY_INDIVIDUAL)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass, yl.LEN_MD5)
	return buffer
end
function ModifyFrame:sendQueryUserInfo()
	local buffer = self:getSendQueryUserInfoBuffer()
	if nil == buffer or (not self:sendSocketData(buffer) and nil ~= self._callBack) then
		self._callBack(-1,"查询信息失败！")
	end
end

-- 发送填推广员
function ModifyFrame:sendInputSpreader()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_BindSpreader)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_BIND_SPREADER)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
	buffer:pushdword(self._SpreaderID)

	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送修改资料失败！")
	end
end

function ModifyFrame:sendRealAuth()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_RealAuth)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_REAL_AUTH_QUERY)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass, yl.LEN_MD5)
	buffer:pushstring(self._szCompellation, yl.LEN_COMPELLATION)
	buffer:pushstring(self._szPassportID, yl.LEN_PASS_PORT_ID)
	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送修认证失败！")
	end
end

function ModifyFrame:onModifyUserInfo(cbGender,szNickname,szSign)
	self._oprateCode = MODIFY_USERINFO
	self._cbGender = cbGender
	if szNickname == nil then
		self._szNickname = GlobalUserItem.tabAccountInfo.szNickName
	else
		self._szNickname = szNickname
	end
	self._szSign = szSign

	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function ModifyFrame:onModifySystemHead( wFaceId )
	--数据保存
	self._wFaceId = wFaceId
	self._dwUserId = GlobalUserItem.tabAccountInfo.dwUserID
	self._szMachine = GlobalUserItem.szMachine

	--记录模式
	self._oprateCode = MODIFY_SYSTEM_FACE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 信息查询
function ModifyFrame:onQueryUserInfo()
	--记录模式
	self._oprateCode = QUERY_USERINFO
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 发送填写推广员
function ModifyFrame:onBindSpreader( nSpreaderID )
	self._SpreaderID = nSpreaderID
	--记录模式
	self._oprateCode = INPUT_SPREADER
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 发送认证
function ModifyFrame:onRealAuth( szName, szIdcard )
	self._szCompellation = szName
	self._szPassportID = szIdcard
	--记录模式
	self._oprateCode = OP_REALAUTH
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

return ModifyFrame
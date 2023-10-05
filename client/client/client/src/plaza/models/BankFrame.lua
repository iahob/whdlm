local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local BankFrame = class("BankFrame",BaseFrame)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

function BankFrame:ctor(view,callbcak)
	BankFrame.super.ctor(self,view,callbcak)
	self._view = view
end

--银行刷新
BankFrame.OP_BANK_REFRESH = 0
--银行存款
BankFrame.OP_SAVE_SCORE = 1
--银行取款
BankFrame.OP_TAKE_SCORE = 2
--银行赠送
BankFrame.OP_SEND_SCORE = 3
--银行开通
BankFrame.OP_ENABLE_BANK = 4
--银行资料
BankFrame.OP_GET_BANKINFO = 5
--查询用户
BankFrame.OP_QUERY_USER = 6
-- 长连接开通银行
BankFrame.OP_ENABLE_BANK_GAME = 7

-- 修改银行密码
BankFrame.OP_EDIT_PASSWORD = 8

--连接结果
function BankFrame:onConnectCompeleted()
	print("BankFrame:onConnectCompeleted oprateCode="..self._oprateCode)

	if self._oprateCode == BankFrame.OP_SAVE_SCORE then			--存入
		self:sendSaveScore()
	elseif self._oprateCode == BankFrame.OP_TAKE_SCORE then 		--取出
		self:sendTakeScore()
	elseif self._oprateCode == BankFrame.OP_SEND_SCORE then			--赠送
		self:sendTransferScore()
	elseif self._oprateCode == BankFrame.OP_ENABLE_BANK then  		--开通
		self:sendEnableBank()
	elseif self._oprateCode == BankFrame.OP_GET_BANKINFO then 		--获取银行信息
		self:sendGetBankInfo()
	elseif self._oprateCode == BankFrame.OP_QUERY_USER then
		self:sendQueryUserInfo()
	elseif self._oprateCode == BankFrame.OP_EDIT_PASSWORD then
		self:sendEditPassWord()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知操作模式！")
		end		
	end

end

--网络信息(短连接)
function BankFrame:onSocketEvent(main,sub,pData)
	local bCloseSocket = true
	if main == logincmd.MDM_GP_USER_SERVICE then --用户服务
		if sub == logincmd.SUB_GP_USER_INSURE_INFO then
			self:onSubGetBankInfo(pData)
		elseif sub == logincmd.SUB_GP_USER_INSURE_SUCCESS then
			self:onSubInsureSuccess(pData)
		elseif sub == logincmd.SUB_GP_USER_INSURE_FAILURE then
			self:onSubInsureFailue(pData)
		elseif sub == logincmd.SUB_GP_USER_INSURE_ENABLE_RESULT then
			self:onSubEnableBankResult(pData)
			bCloseSocket = false
		elseif sub == logincmd.SUB_GP_QUERY_USER_INFO_RESULT then
		elseif sub == logincmd.SUB_GP_OPERATE_SUCCESS then
			local lResultCode = pData:readint()
			local szDescribe = pData:readstring()

			local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
                showToast(runScene, szDescribe, 2)
            end 
		elseif sub == logincmd.SUB_GP_OPERATE_FAILURE then
			local lResultCode = pData:readint()
			local szDescribe = pData:readstring()

			local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
                showToast(runScene, szDescribe, 2)
            end  
		else
			local message = string.format("未知命令码：%d-%d",main,sub)
			if nil ~= self._callBack then
				self._callBack(-1,message);
			end			
		end
	end

	if main == logincmd.MDM_GP_USER_SERVICE and sub == logincmd.SUB_GP_QUERY_USER_INFO_RESULT then
		self:onUserInfoResult(pData)
	else
		if bCloseSocket then
			self:onCloseSocket()
		end		
	end
end

--网络消息(长连接)
function BankFrame:onGameSocketEvent(main,sub,pData)
	if main == game_cmd.MDM_GR_INSURE then
		print("GameSocket Bank #" .. main .. "# #" .. sub .. "#")
		if sub == game_cmd.SUB_GR_USER_INSURE_INFO then 				--银行资料
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_UserInsureInfo, pData)
			GlobalUserItem.tabAccountInfo.lUserScore = cmd_table.lUserScore
			GlobalUserItem.tabAccountInfo.lUserInsure = cmd_table.lUserInsure
			if nil ~= self._callBack then
				self._callBack(BankFrame.OP_GET_BANKINFO,cmd_table)
			end
			--通知更新        
			local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
		    eventListener.obj = yl.RY_MSG_USERWEALTH
		    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
		elseif sub == game_cmd.SUB_GR_USER_INSURE_SUCCESS then 			--银行成功
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_UserInsureSuccess, pData)
			GlobalUserItem.tabAccountInfo.lUserScore= cmd_table.lUserScore
			GlobalUserItem.tabAccountInfo.lUserInsure = cmd_table.lUserInsure
			--通知更新        
			local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
		    eventListener.obj = yl.RY_MSG_USERWEALTH
		    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

		    if nil ~= self._callBack then
				self._callBack(1,"操作成功！")
			end
		elseif sub == game_cmd.SUB_GR_USER_INSURE_FAILURE then 			--银行失败
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_UserInsureFailure, pData)
			if nil ~= self._callBack then
				self._callBack(-1,cmd_table.szDescribeString)
			end
		elseif sub == game_cmd.SUB_GR_USER_TRANSFER_USER_INFO then 		--用户资料
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_UserTransferUserInfo, pData)
			self._tabTarget.opTargetAcconts = cmd_table.szAccounts
			self._tabTarget.opTargetID = cmd_table.dwTargerUserID

			local buffer = ExternalFun.create_netdata(game_cmd.CMD_GP_C_TransferScoreRequest)
			buffer:setcmdinfo(game_cmd.MDM_GR_INSURE,game_cmd.SUB_GR_TRANSFER_SCORE_REQUEST)
			buffer:pushbyte(game_cmd.SUB_GR_TRANSFER_SCORE_REQUEST)
			buffer:pushscore(self._lOperateScore)		
			buffer:pushstring(cmd_table.szAccounts,yl.LEN_ACCOUNTS)
			buffer:pushstring(md5(self._szPassword),yl.LEN_PASSWORD)
			buffer:pushstring("",yl.LEN_TRANS_REMARK)
			buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass,yl.LEN_PASSWORD)
			self._oprateCode = BankFrame.OP_SEND_SCORE

			if not self._gameFrame:sendSocketData(buffer) then
				self._callBack(-1,"发送转账失败！")
			end
		elseif sub == game_cmd.SUB_GR_USER_INSURE_ENABLE_RESULT then 	--开通结果
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_UserInsureEnableResult, pData)
			dump(cmd_table, "CMD_GR_S_UserInsureEnableResult", 6)

			GlobalUserItem.tabAccountInfo.cbInsureEnabled = cmd_table.cbInsureEnabled
			if nil ~= self._callBack then
				self._callBack(BankFrame.OP_ENABLE_BANK_GAME,cmd_table.szDescribeString)
			end
		end
	end
end

function BankFrame:onSubEnableBankResult(pData)
	GlobalUserItem.tabAccountInfo.cbInsureEnabled = pData:readbyte()

	--开通失败，断网
	if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
		self:onCloseSocket()
	end
	
	local szTipString = pData:readstring()
	if nil ~= self._callBack then
		self._callBack(BankFrame.OP_ENABLE_BANK,szTipString)
	end	
end

function BankFrame:onSubInsureSuccess(pData)
	local dwUserID = pData:readdword()
	if dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
		GlobalUserItem.tabAccountInfo.lUserScore= GlobalUserItem:readScore(pData)
		GlobalUserItem.tabAccountInfo.lUserInsure = GlobalUserItem:readScore(pData)
		if nil ~= self._callBack then
			self._callBack(1,"操作成功！")
		end

		--通知更新        
		local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
	    eventListener.obj = yl.RY_MSG_USERWEALTH
	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
	end
end

function BankFrame:onSubInsureFailue(pData)
	local lError = pData:readint()
	local szError = pData:readstring()
	if nil ~= self._callBack then
		self._callBack(-1,szError)
	end	
end

--获取到银行资料
function BankFrame:onSubGetBankInfo( pData )
	local cmdtable = ExternalFun.read_netdata(logincmd.CMD_GP_UserInsureInfo, pData)
	GlobalUserItem.tabAccountInfo.lUserScore = cmdtable.lUserScore
	GlobalUserItem.tabAccountInfo.lUserInsure = cmdtable.lUserInsure
	if nil ~= self._callBack then
		self._callBack(BankFrame.OP_GET_BANKINFO,cmdtable)
	end	
	--通知更新        
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function BankFrame:onUserInfoResult(pData)
	local cmdtable =  ExternalFun.read_netdata(logincmd.CMD_GP_UserTransferUserInfo, pData)
	self._target = cmdtable.dwTargetGameID--cmdtable.szAccounts
	self._tabTarget.opTargetAcconts = cmdtable.szAccounts
	self._tabTarget.opTargetID = cmdtable.dwTargetGameID
	self._view:dismissPopWait()

	local str = "赠送金币收取"..self._view.m_tabBankConfigInfo.wRevenueTransfer.."‰手续费，您确定要给["..cmdtable.szAccounts.."],ID:"..cmdtable.dwTargetGameID.."赠送金币吗？"
	local sureDialog = QueryDialog:create(str, function(ok)
		if ok == true then
			self._view:showPopWait()
			self._oprateCode = BankFrame.OP_SEND_SCORE
		    self:sendTransferScore()
		else
		    self:onCloseSocket()
	    end
	end)
	:setCanTouchOutside(false)
	:addTo(self._view)
end

--开通
function BankFrame:sendEnableBank()
	local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_UserEnableInsure)
	local bankpass = md5(self._szPassword)
	local machine  = GlobalUserItem.tabAccountInfo.szMachine
	buffer:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_USER_ENABLE_INSURE)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.md5Passwd,33)
	buffer:pushstring(bankpass,33)
	buffer:pushstring(machine,33)

	--发送失败
	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送开通失败！")
	end
end

--存入
function BankFrame:sendSaveScore()
	local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_UserSaveScore)
	buffer:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_USER_SAVE_SCORE)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushscore(self._lOperateScore)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szMachine,yl.LEN_PASSWORD)

	--发送失败
	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送存款失败！")
	end
end

--取出
function BankFrame:sendTakeScore()
	local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_UserTakeScore)
	buffer:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_USER_TAKE_SCORE)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushscore(self._lOperateScore)
	buffer:pushstring(md5(self._szPassword),yl.LEN_PASSWORD)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szMachine,yl.LEN_PASSWORD)

	--发送失败
	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送取款失败！")
	end
end

--发送赠送
function BankFrame:sendTransferScore()
	local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_UserTransferScore)
	buffer:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_USER_TRANSFER_SCORE)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushscore(self._lOperateScore)
	buffer:pushstring(md5(self._szPassword),yl.LEN_PASSWORD)
	buffer:pushdword(self._target)
	buffer:pushstring(GlobalUserItem.tabAccountInfo.szMachine,yl.LEN_MACHINE_ID)	
	buffer:pushstring(self._remark,yl.LEN_TRANS_REMARK)

	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送赠送失败！")
	end
end

--发送查询银行数据
function BankFrame:sendGetBankInfo(  )
	local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_QueryInsureInfo)
	buffer:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_QUERY_INSURE_INFO)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)

	--发送失败
	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送查询失败！")
	end
end

--发送修改密码
function BankFrame:sendEditPassWord( )
	local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_ModifyInsurePass)
	buffer:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_MODIFY_INSURE_PASS)
	buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	buffer:pushstring(md5(self._newPassWord),yl.LEN_PASSWORD)
	buffer:pushstring(md5(self._oldPassWord),yl.LEN_PASSWORD)

	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"发送修改密码失败")

	end
end

--发送查询用户信息
function BankFrame:sendQueryUserInfo( )
	local cmd = CCmd_Data:create(65)
	cmd:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_QUERY_USER_INFO_REQUEST)
	cmd:pushbyte(0)
	cmd:pushstring(self._target, 32)

	if not self:sendSocketData(cmd) and nil ~= self._callBack then
		self._callBack(-1,"发送查询失败！")

	end
end

function BankFrame:onSaveScore(lScore)
	--操作记录
	self._oprateCode = BankFrame.OP_SAVE_SCORE
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_SaveScoreRequest)
		buffer:setcmdinfo(game_cmd.MDM_GR_INSURE,game_cmd.SUB_GR_SAVE_SCORE_REQUEST)
		buffer:pushbyte(game_cmd.SUB_GR_SAVE_SCORE_REQUEST)
		buffer:pushscore(lScore)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送存款失败！")
		end
	else
		--参数记录
		self._lOperateScore = lScore
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

function BankFrame:onTakeScore(lScore,szPassword)
	--操作记录
	self._oprateCode = BankFrame.OP_TAKE_SCORE

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_TakeScoreRequest)
		buffer:setcmdinfo(game_cmd.MDM_GR_INSURE,game_cmd.SUB_GR_TAKE_SCORE_REQUEST)
		buffer:pushbyte(game_cmd.SUB_GR_TAKE_SCORE_REQUEST)
		buffer:pushscore(lScore)
		buffer:pushstring(md5(szPassword),yl.LEN_PASSWORD)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送取款失败！")
		end
	else
		--参数记录
		self._lOperateScore = lScore
		self._szPassword = szPassword
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

function BankFrame:onTransferScore(lScore, target, szPassword, byID, szRemark)
	self._tabTarget = {}
	self._tabTarget.opTime = os.time()
	self._tabTarget.opScore = lScore
	self._tabTarget.opTargetID = target

	--参数记录
	self._lOperateScore = lScore
	self._target = target
	self._byID	= byID
	self._szPassword = szPassword
	self._remark = szRemark

	--操作记录
	self._oprateCode = BankFrame.OP_QUERY_USER

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		print("游戏服务器不处理转账")
		--[[local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_QueryUserInfoRequest)
		buffer:setcmdinfo(game_cmd.MDM_GR_INSURE,game_cmd.SUB_GR_QUERY_USER_INFO_REQUEST)
		buffer:pushbyte(game_cmd.SUB_GR_QUERY_USER_INFO_REQUEST)
		buffer:pushbyte(0)
		buffer:pushstring(self._target, yl.LEN_ACCOUNTS)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送转账失败！")
		end]]
	else		
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

function BankFrame:onEnableBank(szPassword)
	--操作记录
	self._oprateCode = BankFrame.OP_ENABLE_BANK

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_EnableInsureRequest)
		buffer:setcmdinfo(game_cmd.MDM_GR_INSURE,game_cmd.SUB_GR_ENABLE_INSURE_REQUEST)
		buffer:pushbyte(game_cmd.SUB_GR_ENABLE_INSURE_REQUEST)
		buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)		
		buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
		buffer:pushstring(md5(szPassword),yl.LEN_PASSWORD)
		buffer:pushstring(GlobalUserItem.tabAccountInfo.szMachine,yl.LEN_MACHINE_ID)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送开通失败！")
		end
	else
		--参数记录
		self._szPassword = szPassword
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--查询银行资料
function BankFrame:onGetBankInfo()
	--操作记录
	self._oprateCode = BankFrame.OP_GET_BANKINFO
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_QueryInsureInfoRequest)
		buffer:setcmdinfo(game_cmd.MDM_GR_INSURE,game_cmd.SUB_GR_QUERY_INSURE_INFO)
		buffer:pushbyte(game_cmd.SUB_GR_QUERY_INSURE_INFO)
		buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送查询失败！")
		end
	else
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--修改银行密码
function BankFrame:onEditPassWord(newPassWord, oldPassWord)
	self._newPassWord= newPassWord
	self._oldPassWord = oldPassWord

	self._oprateCode = BankFrame.OP_EDIT_PASSWORD
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end


return BankFrame
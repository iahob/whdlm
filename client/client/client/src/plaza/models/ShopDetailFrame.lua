--[[
	商城接口
	2016_07_05 Ravioyla
]]

local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local ShopDetailFrame = class("ShopDetailFrame",BaseFrame)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 道具购买
local OP_PROPERTY_BUY = logincmd.SUB_GP_PROPERTY_BUY
-- 道具使用
local OP_PROPERTY_USE = logincmd.SUB_GP_PROPERTY_USE
-- 获取背包
local OP_PROPERTY_BAG = logincmd.SUB_GP_QUERY_BACKPACKET

function ShopDetailFrame:ctor(view,callbcak)
	ShopDetailFrame.super.ctor(self,view,callbcak)
end

--连接结果
function ShopDetailFrame:onConnectCompeleted() 

	print("============ShopDetail onConnectCompeleted============")
	print("ConnectCompeleted oprateCode="..self._oprateCode)

	if self._oprateCode == OP_PROPERTY_BUY then	--购买
		self:sendPropertyBuy()
	else
		print("self:onCloseSocket() 1")
		self:onCloseSocket()
		self._callBack(-1,"未知操作模式！")
	end

end

--网络信息(短连接)
function ShopDetailFrame:onSocketEvent(main,sub,pData)
	print("============ ShopDetail onSocketEvent ============")
	print("socket event:"..main.."#"..sub)

	if main == logincmd.MDM_GP_PROPERTY then --道具命令
		if sub == logincmd.SUB_GP_PROPERTY_BUY_RESULT then 				--购买
			self:onSubPropertyBuyResult(pData)	
		elseif sub == logincmd.SUB_GP_PROPERTY_USE_RESULT then 			--使用
			self:onSubPropertyUseResult(pData)
		elseif sub == logincmd.SUB_GP_PROPERTY_FAILURE then 			--失败
			self:onSubPropertyFailure(pData)
		elseif sub == logincmd.SUB_GP_QUERY_BACKPACKET_RESULT then 		--背包查询
			self:onSubQueryBag(pData)
		else
			local message = string.format("未知命令码：%d-%d",main,sub)
			self._callBack(-1,message)
		end
	end

	self:onCloseSocket()
end

--网络消息(长连接)
function ShopDetailFrame:onGameSocketEvent(main,sub,pData)
	if main == game_cmd.MDM_GR_PROPERTY then
		print("GameSocket ShopDetail #" .. main .. "# #" .. sub .. "#")
		if sub == game_cmd.SUB_GR_GAME_PROPERTY_BUY_RESULT then 			-- 道具购买
			self:onSubPropertyBuyResult(pData)
		elseif sub == game_cmd.SUB_GR_PROPERTY_USE_RESULT then 				-- 道具使用
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_PropertyUse, pData)
			dump(cmd_table, "CMD_GR_S_PropertyUse", 5)

			GlobalUserItem.lUserScore = cmd_table.Score
			local tmpOrder = cmd_table.cbMemberOrder
			if tmpOrder > GlobalUserItem.cbMemberOrder then
				GlobalUserItem.cbMemberOrder = tmpOrder
			end
			if nil ~= self._callBack then
				self._callBack(2,cmd_table.szNotifyContent)
			end
			--通知更新        
			local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
		    eventListener.obj = yl.RY_MSG_USERWEALTH
		    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
		elseif sub == game_cmd.SUB_GR_PROPERTY_FAILURE then 				-- 道具失败
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_PropertyFailure, pData)
			if nil ~= self._callBack then
				self._callBack(0,cmd_table.szDescribeString)
			end
		elseif sub == game_cmd.SUB_GR_PROPERTY_BACKPACK_RESULT then 		-- 背包查询
			self:onSubQueryBag(pData)
		elseif sub == game_cmd.SUB_GR_GAME_PROPERTY_FAILURE then
			self:onSubPropertyFailure(pData)
		end
	end
end

-- CMD_LogonServer CMD_GP_S_PropertySuccess
function ShopDetailFrame:onSubPropertyBuyResult(pData)
	print("============ ShopDetailFrame:onSubPropertyBuyResult ============")
	local cmdtable = ExternalFun.read_netdata(logincmd.CMD_GP_S_PropertySuccess, pData)

	--判断是大喇叭
	if cmdtable.dwPropID == yl.LARGE_TRUMPET then
		GlobalUserItem.nLargeTrumpetCount = GlobalUserItem.nLargeTrumpetCount + cmdtable.dwPropCount
	end

	-- 更新钻石
	GlobalUserItem.tabAccountInfo.lDiamond = cmdtable.lDiamond
	GlobalUserItem.tabAccountInfo.lUserScore = cmdtable.lScore
	--通知更新        
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

    local tips = cmdtable.szNotifyContent
    if nil ~= self._callBack then
    	self._callBack(OP_PROPERTY_BUY,tips)
    end
	return true
end

function ShopDetailFrame:onSubPropertyUseResult(pData)
	print("============ ShopDetailFrame:onSubPropertyUseResult ============")
end

function ShopDetailFrame:onSubPropertyFailure(pData)
	print("============ ShopDetailFrame:onSubPropertyFailure ============")
	local code = pData:readdword()
	local szTip = pData:readstring()

	if nil ~= self._callBack then
		self._callBack(-1,szTip)
	end
end

-- CMD_LogonServer CMD_GP_S_BackpackProperty
function ShopDetailFrame:onSubQueryBag(pData)
	print("============ ShopDetailFrame:onSubQueryBag ============")
	local userID = pData:readdword()
	local status = pData:readdword()
	local count = pData:readdword()
	local list = {}
	for i=1,count do
		local item = {}
		--道具信息
		item._count = pData:readint()
		--game_cmd.tagPropertyInfo
		item._index = pData:readword()
		item._kind  = pData:readword()
		item._shop  = pData:readword()
		item._area  = pData:readword()
		item._service = pData:readword()
		item._commend = pData:readword()
		item._multiple= pData:readword()
		item._cbSuportMobile = pData:readbyte()
		--销售价格
		item._gold  = GlobalUserItem:readScore(pData)
		item._bean  = pData:readdouble()
		item._ingot = GlobalUserItem:readScore(pData) 
		item._loveliness = GlobalUserItem:readScore(pData)
		--赠送内容
		item._sendLoveliness = GlobalUserItem:readScore(pData)
		item._recvLoveliness = GlobalUserItem:readScore(pData)
		item._resultGold = GlobalUserItem:readScore(pData)

		item._name = pData:readstring(32) or ""
		item._info = pData:readstring(128) or ""

		table.insert(list,item)
		--如果是大喇叭
		if item._index == yl.LARGE_TRUMPET then
			GlobalUserItem.nLargeTrumpetCount = item._count
		end
	end

	if nil ~= self._callBack then
		self._callBack(yl.SUB_GP_QUERY_BACKPACKET_RESULT,list)
	end
end

--道具购买
function ShopDetailFrame:sendPropertyBuy()
	local PropertyBuy = ExternalFun.create_netdata(logincmd.CMD_GP_PropertyBuy)
	PropertyBuy:setcmdinfo(logincmd.MDM_GP_PROPERTY,logincmd.SUB_GP_PROPERTY_BUY)
	PropertyBuy:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
	PropertyBuy:pushdword(self._id)
	PropertyBuy:pushbyte(self._consumeType)
	PropertyBuy:pushdword(self._itemCount)
	PropertyBuy:pushstring(GlobalUserItem.md5Passwd, yl.LEN_PASSWORD)
	PropertyBuy:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
	print("sendPropertyBuy", GlobalUserItem.md5Passwd, self._consumeType)

	--发送失败
	if not self:sendSocketData(PropertyBuy) and nil ~= self._callBack then
		self._callBack(-1,"发送道具购买失败！")
	end
end

--道具使用(非连接)
function ShopDetailFrame:sendPropertyUseNoConnect(id,count)
	print("============ ShopDetailFrame:sendPropertyUseNoConnect ============")
	local PropertyUse = CCmd_Data:create(14)
	PropertyUse:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_PROPERTY_USE)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(id)
	PropertyUse:pushword(count)

	--发送失败
	if not self:sendSocketData(PropertyUse) and nil ~= self._callBack then
		self._callBack(-1,"发送道具使用失败！")
	end
end


--道具使用(正常)
function ShopDetailFrame:sendPropertyUse()
	local PropertyUse = CCmd_Data:create(14)
	PropertyUse:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_PROPERTY_USE)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(self._id)
	PropertyUse:pushword(self._itemCount)

	--发送失败
	if not self:sendSocketData(PropertyUse) and nil ~= self._callBack then
		self._callBack(-1,"发送道具使用失败！")
	end
end

--获取背包
function ShopDetailFrame:sendQueryBag()
	local QueryBag = CCmd_Data:create(8)
	QueryBag:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_QUERY_BACKPACKET)
	QueryBag:pushdword(GlobalUserItem.dwUserID)
	QueryBag:pushdword(0)

	--发送失败
	if not self:sendSocketData(QueryBag) and nil ~= self._callBack then
		self._callBack(-1,"发送背包查询失败！")
	end
end

--道具购买
function ShopDetailFrame:onPropertyBuy(count,id, consumeType)	
	self._itemCount = count
	self._id = id
	self._consumeType = consumeType

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_GamePropertyBuy)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_GAME_PROPERTY_BUY)
		buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
		buffer:pushdword(self._id)
		buffer:pushdword(self._itemCount)
		buffer:pushbyte(self._consumeType)
		buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
		buffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
		
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送购买失败！")
		end
	else
		--操作记录
		self._oprateCode = OP_PROPERTY_BUY
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--道具使用
function ShopDetailFrame:onPropertyUse(id,count)	
	self._itemCount = count
	self._id = id

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_PropertyUse)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_USE)
		buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
		buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
		buffer:pushdword(self._id)
		buffer:pushword(self._itemCount)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送使用失败！")
		end
	else
		--操作记录
		self._oprateCode = 1
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--获取背包
function ShopDetailFrame:onQueryBag()
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_BackpackProperty)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_BACKPACK)
		buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
		buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送查询失败！")
		end
	else
		--操作记录
		self._oprateCode = 2

		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

return ShopDetailFrame
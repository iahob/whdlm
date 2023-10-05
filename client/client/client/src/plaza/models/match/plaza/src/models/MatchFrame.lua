--
-- Author: zhouweixiang
-- Date: 2017-02-13 11:45:35
--
--比赛网络消息处理
local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local MatchFrame = class("MatchFrame",BaseFrame)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local cmd_match = appdf.req(MatchRoom.MODULE.MATCHHEADER .. "CMD_Match")
local struct_match = appdf.req(MatchRoom.MODULE.MATCHHEADER .. "Struct_Match")
local cmd_match_game = cmd_match.game
local cmd_match_define = cmd_match.define
local LEN_MATCH_GAME_SERVER_ITEM			= 672			--比赛房间长度
function MATCHGAMESERVER(code)
    return { m = cmd_match_game.MDM_GR_MATCH, s = code }
end

function MATCHCMSYSTEM(code)
	return { m = cmd_match_define.MDM_CM_SYSTEM, s = code }
end

function MatchFrame:ctor(view,callbcak)
    MatchFrame.super.ctor(self,view,callbcak)
end

--比赛数据解析
function MatchFrame:onSubMatchRuleList( pData)
	local len = pData:getlen()
	print("=============== onSubMatchListInfo ================")
	print("onSubMatchListInfo=================================:"..len)
	if (len - math.floor(len/LEN_MATCH_GAME_SERVER_ITEM)*LEN_MATCH_GAME_SERVER_ITEM) ~= 0 then
		print("matchlist_len_error"..len)
		return
	end

	local itemcount =  math.floor(len/LEN_MATCH_GAME_SERVER_ITEM)
	for i=1,itemcount do
		local cmddata = clone(ExternalFun.read_netdata(struct_match.tagGameMatch, pData))
		local bytelen = 512
		if cmddata.cbMatchType == cmd_match_define.MATCH_TYPE_LOCKTIME then
			cmddata.cbMatchRule = clone(ExternalFun.read_netdata(struct_match.tagLockTimeMatch, pData))
			bytelen = bytelen - cmd_match_define.LEN_MATCH_LOCKTIME


		elseif cmddata.cbMatchType == cmd_match_define.MATCH_TYPE_IMMEDIATE then
			cmddata.cbMatchRule = clone(ExternalFun.read_netdata(struct_match.tagImmediateMatch, pData))
			bytelen = bytelen - cmd_match_define.LEN_MATCH_IMMEDIATE
			--dump(cmddata.cbMatchRule,"cmddata.cbMatchRule1",6)
			
		end
		for i=1,bytelen do
			local testdata = pData:readbyte()
		end
		
		table.insert(MatchRoom:getInstance().m_tabAllMatchRule, cmddata)
		
	end
	 --dump(MatchRoom:getInstance().m_tabAllMatchRule,"m_tabAllMatchRule==========fdsfsfs=========",9)
	 --assert(false)
	 -- LogAsset:getInstance():logData("m_tabAllMatchRule:")
	 -- LogAsset:getInstance():logData(cjson.encode(MatchRoom:getInstance().m_tabAllMatchRule))
	
end

-- 发送报名
function MatchFrame:sendJoinMatch()
	local nKind = GlobalUserItem.nCurGameKind
	if not MatchRoom:getInstance():MatchRoomCheckKindResVersion(nKind) then
		return
	end
    local buffer = CCmd_Data:create(1)
    buffer:setcmdinfo(cmd_match_game.MDM_GR_MATCH,cmd_match_game.SUB_GR_MATCH_FEE)
    buffer:pushbyte(1)
    if not self._gameFrame:sendSocketData(buffer) then
        self._callBack(MATCHGAMESERVER(-1),"发送报名失败！")
    end
end

--取消报名
function MatchFrame:sendLeaveMatch()
	local buffer = CCmd_Data:create(0)
    buffer:setcmdinfo(cmd_match_game.MDM_GR_MATCH,cmd_match_game.SUB_GR_LEAVE_MATCH)
    if not self._gameFrame:sendSocketData(buffer) then
    	self._callBack(MATCHGAMESERVER(-1),"取消报名失败！")
    end
end

--网络消息(长连接)
function MatchFrame:onGameSocketEvent( main,sub,pData )
	if cmd_match_game.MDM_GR_MATCH == main then  
		if cmd_match_game.SUB_GR_MATCH_RULE == sub then --比赛消息
		 	self:onSubMatchRule(pData)
		elseif cmd_match_game.SUB_GR_MATCH_FEE == sub then --报名费用
			self:onSubMatchFee(pData)
		elseif cmd_match_game.SUB_GR_MATCH_NUM == sub then --等待人数
			self:onSubMatchNum(pData)
		elseif cmd_match_game.SUB_GR_LEAVE_MATCH == sub then --退出比赛

		elseif cmd_match_game.SUB_GR_MATCH_STATUS == sub then --比赛状态
			self:onSubMatchStatus(pData)
		elseif cmd_match_game.SUB_GR_MATCH_USTATUS == sub then --比赛状态
			self:onSubMatchUStatus(pData)
		elseif cmd_match_game.SUB_GR_MATCH_DESC == sub then --比赛描述
			self:onSubMatchDesc(pData)
		elseif cmd_match_game.SUB_GR_MATCH_GOLDUPDATE == sub then --游戏币更新
			self:onSubMatchGoldUpdate(pData)
		elseif cmd_match_game.SUB_GR_MATCH_PROMOTION == sub then --一轮结束后，晋级的会收到这个消息
			self:onSubMatchPromotion(pData )
		end
	elseif cmd_match_define.MDM_CM_SYSTEM == main then
		local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_CM_SystemMessage, pData)
		if GlobalUserItem.bMatch then
			self._callBack(MATCHCMSYSTEM(-1), cmdtable.szString)
		end
		dump(cmdtable, "desciption", 10)
	elseif cmd_match_define.MDM_GF_FRAME == main then
		pData:resetread()
		if cmd_match_game.SUB_GR_MATCH_WAIT_TIP == sub then --等待提示
			self:onSubMatchTip(pData)
		elseif cmd_match_game.SUB_GR_MATCH_RESULT == sub then --比赛结果
			self:onSubMatchResult(pData)
		elseif cmd_match_game.SUB_GR_MATCH_INFO == sub then --比赛信息
			self:onSubMatchInfo(pData)
		elseif cmd_match_define.SUB_GF_SYSTEM_MESSAGE == sub then
			local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_CM_SystemMessage, pData)
			dump(cmdtable, "系统提示", 10)
			-- LogAsset:getInstance():logData((GlobalUserItem.tabAccountInfo.szNickName or "nil ").."系统提示")
			-- LogAsset:getInstance():logData(cjson.encode(cmdtable))
			local bmatchResult = bit:_and(cmdtable.wType,cmd_match_define.SMT_MATCHRESULT)

			local bCloseGame = bit:_and(cmdtable.wType,cmd_match_define.SMT_CLOSE_ROOM)

			dump({cmdtable.wType,cmd_match_define.SMT_CLOSE_ROOM,bCloseGame},"============111")
			if bCloseGame ~=0 then
				self._callBack(MATCHCMSYSTEM(cmd_match_define.SMT_CLOSE_ROOM), cmdtable.szString)
				return
			end

			print("是否比赛结果", bmatchResult)
			if bmatchResult ~= 0 then
				-- self._gameFrame.bMatch = false
				-- GlobalUserItem.bWaitQuit = true
				-- local resulttable = {}
				-- resulttable.wMatchRank = tonumber(string.match(cmdtable.szString, "%d+"))
				-- dump(resulttable, "比赛结果", 10)
				-- self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_RESULT), "", resulttable)
				
			else
				if cmdtable.wType == 17 then --?
					self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_WAIT_TIP), "", cmdtable)
					return
				end
				self._callBack(MATCHCMSYSTEM(-1), cmdtable.szString)
			end
		end
	end
end

--比赛信息
function MatchFrame:onSubMatchInfo( pData )
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_Match_Info, pData)
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_INFO), "", cmdtable)
	dump(cmdtable, "比赛信息", 10)
end

--比赛结果
function MatchFrame:onSubMatchResult( pData )

	self._gameFrame.bMatch = false
	GlobalUserItem.bWaitQuit = true
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_MatchResult, pData)
	dump(cmdtable, "比赛结果", 10)
	-- LogAsset:getInstance():logData(cjson.encode(cmdtable))
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_RESULT), "", cmdtable)

end

--比赛提示
function MatchFrame:onSubMatchTip( pData )
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_Match_Wait_Tip, pData)

	self._gameFrame.bMatch = true
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_WAIT_TIP), "", cmdtable)
	-- LogAsset:getInstance():logData("onSubMatchTip")
	-- LogAsset:getInstance():logData(cjson.encode(cmdtable))
	dump(cmdtable, "比赛提示", 10)
end

--登录游戏比赛规则
function MatchFrame:onSubMatchRule( pData )
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_Match_Rule, pData)
	local bytelen = 512
	if cmdtable.cbMatchType == cmd_match_define.MATCH_TYPE_LOCKTIME then
		cmdtable.cbMatchRule = ExternalFun.read_netdata(struct_match.tagLockTimeMatch, pData)
	elseif cmdtable.cbMatchType == cmd_match_define.MATCH_TYPE_IMMEDIATE then
		cmdtable.cbMatchRule = ExternalFun.read_netdata(struct_match.tagImmediateMatch, pData)
		--dump(cmdtable.cbMatchRule,"cmdtable.cbMatchRule2",6)
		
	end
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_RULE), "", cmdtable)
	--dump(cmdtable, "游戏比赛规则", 10)
end

--比赛费用
function MatchFrame:onSubMatchFee( pData )
	print("比赛费用"..pData:getlen())
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_Match_Fee, pData)
	--dump(cmdtable, "比赛费用", 10)
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_FEE), "", cmdtable)
end

--游戏比赛报名人数更新
function MatchFrame:onSubMatchNum( pData )
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_Match_Num, pData)
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_NUM), "", cmdtable)
	--dump(cmdtable, "等待人数", 10)
end

--比赛状态 
function MatchFrame:onSubMatchStatus( pData )
	local cmdtable = pData:readbyte()
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_STATUS), "", cmdtable)
	dump(cmdtable, "比赛状态", 10)
end

--比赛状态
function MatchFrame:onSubMatchUStatus( pData )
	local cmdtable = pData:readbyte()
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_USTATUS), "", cmdtable)
	dump(cmdtable, "用户状态", 10)
end

--比赛描述
function MatchFrame:onSubMatchDesc( pData )
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_MatchDesc, pData)
	dump(cmdtable, "比赛描述", 10)
end

--游戏币更新
function MatchFrame:onSubMatchGoldUpdate( pData )
	local cmdtable = ExternalFun.read_netdata(cmd_match_game.CMD_GR_MatchGoldUpdate, pData)
	dump(cmdtable, "游戏币更新", 10)
end

function MatchFrame:onSubMatchPromotion(pData )
	local curRank=pData:readword()
	-- LogAsset:getInstance():logData("curRank "..(curRank or " nil"))
	self._callBack(MATCHGAMESERVER(cmd_match_game.SUB_GR_MATCH_PROMOTION), "",curRank)
end

return MatchFrame
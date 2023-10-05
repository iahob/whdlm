--
-- Author: zhong
-- Date: 2017-06-01 15:38:47
--
-- 启动查询队列
local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local QueryQueueFrame = class("QueryQueueFrame",BaseFrame)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 约战header
local cmd_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "CMD_Private")
local define_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "Define_Private")
local struct_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "Struct_Private")
local cmd_pri_login = cmd_private.login
-- 

-- 登陆服header
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
--

-- net
local PriFrame = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PriFrame")          -- 约战网络
local ModifyFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.ModifyFrame")       -- 信息修改

-- 

local TAG_START             = 100
local enumTable = 
{
    "OP_ERROR",                 -- 查询错误
    "OP_ROOMPARAM",             -- 约战配置
    "OP_QUERYTRUMPET",          -- 查询喇叭
    "OP_QUERYUSERINFO",         -- 查询附加
    "OP_END",                   -- 查询结束
}
local OP_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
QueryQueueFrame.OP_ENUM = OP_ENUM

function QueryQueueFrame:ctor(view,callback)
    QueryQueueFrame.super.ctor( self, view, callback )
    -- 当前查询
    self.m_nQueryIdx = 0
    -- 查询数量
    self.m_nQueryCount = 0
    -- 查询队列
    self.m_tabQueryQueue = {}
end

function QueryQueueFrame:addToQueryList( param )
    if type(param) ~= "table" then
        dump(param, "QueryQueueFrame:addToQueryList 参数非法", 6)
        return
    end
    table.insert(self.m_tabQueryQueue, param)
end

function QueryQueueFrame:startQuery()
    self.m_nQueryCount = #self.m_tabQueryQueue
    if 0 == self.m_nQueryCount then
        if nil ~= self._callBack and type(self._callBack) == "function" then
            self._callBack(OP_ENUM.OP_END)
            return false
        end
    end
    local param = self.m_tabQueryQueue[1]
    -- 记录模式
    self._oprateCode = param.nOprateCode
    -- 地址
    local server = param.szServer
    -- 端口
    local port = param.nPort
    
    if self:isSocketServer() then
        print("QueryQueueFrame已连接")
        self:onConnectCompeleted()
    else
        if not self:onCreateSocket(server, port) and nil ~= self._callBack then
            self._callBack( OP_ENUM.OP_ERROR, "建立连接失败！")
            return false
        end
    end
    return true
end

function QueryQueueFrame:checkQueryList()
    self:startQuery()
end

--连接结果
function QueryQueueFrame:onConnectCompeleted()
    print("QueryQueueFrame:onConnectCompeleted opCode ==> ", self._oprateCode)
    if self._oprateCode == OP_ENUM.OP_ROOMPARAM then                -- 约战配置
        local buffer = PriFrame:getSendQueryRoomParamBuffer()
        if nil == buffer or (not self:sendSocketData(buffer) and nil ~= self._callBack) then
            self._callBack(OP_ENUM.OP_ERROR,"发送请求配置失败!")
        end
    elseif self._oprateCode == OP_ENUM.OP_QUERYTRUMPET then         -- 查询喇叭
        local buffer = ExternalFun.create_netdata(logincmd.CMD_GP_PropertyQuerySingle)
        buffer:setcmdinfo(logincmd.MDM_GP_PROPERTY,logincmd.SUB_GP_QUERY_SINGLE)
        buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
        buffer:pushdword(yl.LARGE_TRUMPET)
        buffer:pushstring(GlobalUserItem.tabAccountInfo.szDynamicPass, yl.LEN_PASSWORD)
        if not self:sendSocketData(buffer) and nil ~= self._callBack then
            self._callBack(-1,"发送背包查询失败!")
        end
    elseif self._oprateCode == OP_ENUM.OP_QUERYUSERINFO then        -- 查询附加
        local buffer = ModifyFrame:getSendQueryUserInfoBuffer()
        if nil == buffer or (not self:sendSocketData(buffer) and nil ~= self._callBack) then
            self._callBack(-1,"发送查询附加信息失败!")
        end
    end
end

--网络信息(短连接)
function QueryQueueFrame:onSocketEvent(main,sub,pData)    
    -- 是否等待下一个消息 
    local needWait = false     
    if logincmd.MDM_GP_USER_SERVICE == main then
        needWait = self:onUserService(sub, pData)
    elseif cmd_pri_login.MDM_MB_PERSONAL_SERVICE == main then
        needWait = self:onPrivateService(sub, pData)
    elseif logincmd.MDM_GP_PROPERTY == main then
        needWait = self:onPropertyService(sub, pData)
    end

    if not needWait then
        table.remove(self.m_tabQueryQueue, 1)
        if not self:startQuery() then
            self:onCloseSocket()
        end
    end
end

--网络消息(长连接)
function QueryQueueFrame:onGameSocketEvent(main,sub,pData)
    
end

-- 用户服务
function QueryQueueFrame:onUserService( sub, pData )
    if logincmd.SUB_GP_USER_INDIVIDUAL == sub then                              -- 附加信息
        ModifyFrame:_onSubQueryUserInfoResult(pData)
        return false
    end
end

-- 约战服务
function QueryQueueFrame:onPrivateService( sub, pData )
    if cmd_pri_login.SUB_MB_PERSONAL_PARAMETER == sub then                      -- 房间属性
        PriFrame:_onSubParameter(pData)
        -- 保持连接
        return true
    elseif cmd_pri_login.SUB_MB_PERSONAL_FEE_PARAMETER == sub then              -- 约战配置
        PriFrame:_onSubFeeParameter(pData)
        print("QueryQueueFrame:onPrivateService fee list call back")
        if nil ~= self._callBack then
            return self._callBack(OP_ENUM.OP_ROOMPARAM)
        end
    end
    return false
end

-- 道具服务
function QueryQueueFrame:onPropertyService( sub, pData )
    if logincmd.SUB_GP_QUERY_SINGLE_RESULT == sub then                          -- 喇叭查询
        print("============ QueryQueueFrame:onSubQuery ============")
        local param = ExternalFun.read_netdata( logincmd.CMD_GP_PropertyQuerySingleResult, pData )
        print("大喇叭数量:",param.dwItemCount)
        GlobalUserItem.nLargeTrumpetCount = param.dwItemCount
    end
    return false
end

return QueryQueueFrame
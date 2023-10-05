--
-- Author: zhong
-- Date: 2016-12-28 16:16:34
--
-- 私人房网络处理
local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local ClubFrame = class("ClubFrame",BaseFrame)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopWait = appdf.req(appdf.BASE_SRC.."app.views.layer.other.PopWait")

local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")



ClubFrame.OP_LOGINSERVER = cmd_Club.SUB_GA_LOGON_SERVER                     -- 登录服务
ClubFrame.OP_SEARCHGROUP = cmd_Club.SUB_GA_SEARCH_GROUP                     -- 查询群组
ClubFrame.OP_CREATEGROUP = cmd_Club.SUB_GA_CREATE_GROUP                     -- 创建群组
ClubFrame.OP_UPDATEGROUP = cmd_Club.SUB_GA_UPDATE_GROUP                     -- 更新群组
ClubFrame.OP_DELETEGROUP = cmd_Club.SUB_GA_DELETE_GROUP                     -- 删除群组
ClubFrame.OP_DELETEMEMBER = cmd_Club.SUB_GA_DELETE_MEMBER                   -- 删除成员
ClubFrame.OP_APPLYREQUEST = cmd_Club.SUB_GA_APPLY_REQUEST                   -- 申请请求
ClubFrame.OP_APPLYRESPOND = cmd_Club.SUB_GA_APPLY_RESPOND                   -- 申请响应
ClubFrame.OP_APPLYDELETE = cmd_Club.SUB_GA_APPLY_DELETE                     --申请删除

ClubFrame.OP_ENTERGROUP = cmd_Club.SUB_GA_ENTER_GROUP                       -- 进入群组
ClubFrame.OP_LEAVEGROUP = cmd_Club.SUB_GA_LEAVE_GROUP                     -- 离开群组

local function LOGICSERVICE (code)
    return { m = cmd_Club.MDM_GA_LOGIC_SERVICE, s = code }
end
local function GROUPSERVICE(code)
    return { m = cmd_Club.MDM_GA_GROUP_SERVICE, s = code }
end

ClubFrame._instance = nil
function ClubFrame:getInstance( )
    if nil == ClubFrame._instance then
        print("new instance")
        ClubFrame._instance = ClubFrame:create(nil,function (code,result)
        end)
    end
    return ClubFrame._instance
end

function ClubFrame:ctor(view,callbcak)
    --local obj= ClubFrame.new()
    ClubFrame.super.ctor(self,view,callbcak)
    self:init()
    --return obj
end

function ClubFrame:init()
    self.m_Delegate = nil;


    self.m_CallBack = nil;
    self.m_FriendTab = {}

    --加载框
    self._popWait = nil

    --断线自动重连次数
    self.m_nAutoLoginCount = 5

    --申请信息列表
    self.m_tabApplyMsg = {}
    --回应信息列表
    self.m_tabApplyRespondMsg = {}
    --我的俱乐部列表
    self.m_tabGroupList = {}

    --我在各俱乐部的信息
    self.m_tabMyInfo ={}

    --俱乐部信息（存储所有俱乐部信息，退出时清除）
    self.m_tabGroupItem = {}
    --俱乐部成员信息
    self.m_tabGroupMember = {}

    --游戏玩法列表
    self.m_tabGroupConfig ={}

    self.m_tabScoreRecord = {}

    --俱乐部房间列表
    self.m_tabRoomInfoList = {}
    self.isRoomListSuccess = true  --房间列表传输完成

    self.m_nCurrentClub = 0

    self.m_cRoomView =nil --俱乐部桌子界面

    self.m_tabCurDateRecord = {} --所有成员

    self.m_tabMyDateRecord = {} --仅自己

    self.m_tabClubDateRecord = {} --俱乐部单日

    self.m_cbManagerRight = 0

    self.m_cbCaptainRight = 0

    self.m_tabClubCommon ={}

    self.m_tabRoomDetail =nil

    self.m_tabCaptainList = nil

    self.m_tabTeamDayRecord = nil

    self.m_tabTeamerRecord = nil    

    self.m_tabScoreBox = nil --积分提前记录

    self.m_nCreateRoomNum = 0 --创建房间数
    self.m_nConsumeIngot = 0 --消耗房卡数

    self.m_tabBattleInfo = {}

    self.m_curQueryClub = 0

    self.m_bBattleGameCreate = nil --百人游戏创建，玩法创建后直接创建房间

    self.m_bEnterAfterJion = false --分享链接加入的，直接进入俱乐部

    self.m_tabClubRollMsg = {}

    self.m_szClubNoticeMsg = ""

    self.m_cbClubCurPlayMode = 0   --0,积分；1，金币

    self.m_selectGameList = {} --当前显示的游戏列表

    self._scene = nil

    --接受回调，响应场景
    self._viewFrame = nil

end

--进入新的俱乐部时，需要刷新的数据
function ClubFrame:OnResetClubData()
    self.m_tabClubRollMsg = {}
    self.m_tabApplyMsg ={}
end

function ClubFrame:contentAndLogin()
    
    self:onCreateSocket(yl.LOGONSERVER,yl.CLUBPORT)

end


--连接结果
function ClubFrame:onConnectCompeleted()

    if nil ~= self._socket then
        self._socket:setovertime(30000)
    end

        --申请信息列表
    self.m_tabApplyMsg = {}
    --回应信息列表
    self.m_tabApplyRespondMsg = {}
    --我的俱乐部列表
    self.m_tabGroupList = {}

    self.m_nAutoLoginCount = 5
    
    -- 发送登录
    self:sendLoginServer()
end



--网络错误
function ClubFrame:onSocketError(pData) 



    self:onCloseSocket()
    if not pData then
        print("网络断开！")
    elseif type(pData) == "string" then
        print(pData)
    else
        local errorcode = pData:readword()
        if errorcode == nil then
            print("网络断开！")
        elseif errorcode == 3 then
            print("网络连接超时, 请重试!")
            -- 切换地址
            if nil ~= yl.SERVER_LIST[yl.CURRENT_INDEX] then
                yl.LOGONSERVER = yl.SERVER_LIST[yl.CURRENT_INDEX]
            end
            yl.CURRENT_INDEX = yl.CURRENT_INDEX + 1
            if yl.CURRENT_INDEX > yl.TOTAL_COUNT then
                yl.CURRENT_INDEX = 1
            end
        else
            -- print("网络错误，code："..errorcode) 
            -- local runScene = cc.Director:getInstance():getRunningScene()
            -- if nil ~= runScene then
            --     showToast(runScene, "网络错误，code："..errorcode, 2)
            -- end         
        end
    end

    if self._scene and self._scene.m_bIsReConnet  == false  then
        self._scene:dismissPopWait()
        
        self._scene:showClubSeverReConnect()
    end

end



--重置并进行网络连接
function ClubFrame:reSetAndLogin( )

    self:contentAndLogin()

end

--重置并断开网络连接
function ClubFrame:reSetAndDisconnect(  )
    self.m_nAutoLoginCount = 0
    self:onCloseSocket()
    --清空好友数据
    self.m_FriendTab = {}

    --申请信息列表
    self.m_tabApplyMsg = {}
    --回应信息列表
    self.m_tabApplyRespondMsg = {}
    --我的俱乐部列表
    self.m_tabGroupList = {}

    --俱乐部信息（存储所有俱乐部信息，退出时清除）
    self.m_tabGroupItem = {}
    --俱乐部成员信息
    self.m_tabGroupMember = {}


    self.m_nCurrentClub = 0

    self.m_nCreateRoomNum = 0 --创建房间数
    self.m_nConsumeIngot = 0 --消耗房卡数

    self.m_tabBattleInfo = {}

    --接受回调，响应场景
    self._viewFrame = nil
end


-- 设置网络消息处理层
function ClubFrame:setViewFrame(viewFrame)
    self._viewFrame = viewFrame
end


function ClubFrame:setClientScene(scene)
    self._scene = scene
end


-----------------------------------消息处理-------------------------------------

-- 发送登录服务
function ClubFrame:sendLoginServer()
    print("ClubFrame:sendLoginServer")
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_LogonServer)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_LOGON_SERVER)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送登录服务失败！")
        self:dismissPopWait()
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, "连接俱乐部服务器失败，请返回大厅重新进入", 2)
        end
    end
end

-- 发送查询群组
function ClubFrame:sendSearchGroup(groupID)
    print("ClubFrame:sendSearchGroup")
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_SearchGroup)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SEARCH_GROUP)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(groupID)
    if not self:sendSocketData(buffer) then
        print("发送查询房间失败！")
    end
end

-- 请求创建群组
function ClubFrame:sendCreateGroup(tabInfo)
    print("ClubFrame:getSendQueryRoomParamBuffer")
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_CreateGroup)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_CREATE_GROUP)

    dump(tabInfo, "sendCreateGroup", 10)

    buffer:pushbyte(tabInfo.cbPayType)
    buffer:pushbyte(tabInfo.cbCurrencyKind)
    buffer:pushbyte(tabInfo.cbBattleMask)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(tabInfo.dwStationID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    buffer:pushstring(tabInfo.szGroupName,cmd_Club.LEN_GROUP_NAME)
    buffer:pushstring(tabInfo.szGroupIntroduce,cmd_Club.LEN_GROUP_INTRODUCE)

    if not self:sendSocketData(buffer) then
        print("发送创建房间失败！")
    end
end

--更新群组
function ClubFrame:sendUpdateGroup(buffer)
    if nil == buffer or (not self:sendSocketData(buffer)) then
        print("发送请求配置失败！")
    end
end

-- 删除群组
function ClubFrame:sendDeleteGroup(groupID, stationID)
    print("ClubFrame:sendDeleteGroup")
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_DeleteGroup)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_DELETE_GROUP)
    buffer:pushdword(groupID)
    buffer:pushdword(stationID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送查询房间列表失败！")
    end
end

-- 删除组员
function ClubFrame:sendDeleteMember(groupID, tabMenber)
    print("ClubFrame:sendDeleteMember")
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_DELETE_MEMBER)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    buffer:pushdword(#tabMenber)
    for i=1,#tabMenber do
            buffer:pushdword(tabMenber[i])
    end

    if not self:sendSocketData(buffer) then
        print("发送解散房间失败！")
    end
end

-- 申请请求
function ClubFrame:sendApplyRequest(groupID, cbApplyType,lInvitationCode)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_ApplyRequest)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_APPLY_REQUEST)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushbyte(cbApplyType)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    buffer:pushscore(lInvitationCode or 0)
    if not self:sendSocketData(buffer) then
        print("发送查询参与列表失败！")
    end
end

-- 申请响应
function ClubFrame:sendApplyRespond(msgID, groupID, applyerID, applyStatus)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_ApplyRespond)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_APPLY_RESPOND)
    buffer:pushdword(msgID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    buffer:pushdword(groupID)
    buffer:pushdword(applyerID)
    buffer:pushbyte(applyStatus)

    if not self:sendSocketData(buffer) then
        print("发送解散房间失败！")
    end
end

-- 申请删除
function ClubFrame:sendApplyDelete( msgID, msgType )
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_ApplyDelete)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_APPLY_DELETE)
    buffer:pushdword(msgID)
    buffer:pushbyte(msgType)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送申请删除失败！")
    end
end

-- 请求详情
function ClubFrame:sendQueryBattleInfo(groupID)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_QueryBattle)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_BATTLE)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送请求详情失败！")
    end
end

-- 修改统计
function ClubFrame:sendSetSettleInfo(groupID, userID, setNum)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_SetSettle)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SETTLE_BATTLE)
    buffer:pushdword(groupID)
    buffer:pushdword(userID)
    buffer:pushdword(setNum)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送修改统计失败！")
    end
end

--请求约战总数
function ClubFrame:sendQueryBattleTotal(groupID)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_QueryBattle)
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_TOTAL_BATTLE)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送请求约战总数失败！")
    end
end



--更新群组
function ClubFrame:sendGroupConfigInfo(buffer)
    if nil == buffer or (not self:sendSocketData(buffer)) then
        print("发送玩法信息失败！")
    end
end

--上下分
function ClubFrame:sendChangeClubScore(groupID,score,flag,tabPlayer)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_PRESENT_SCORE)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushscore(score)
    buffer:pushbyte(flag)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    buffer:pushint(#tabPlayer)
    for i=1,#tabPlayer do
        buffer:pushdword(tabPlayer[i].UserInfo.dwUserID)
    end

    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end

--查询上下分记录
function ClubFrame:sendQueryScoreRecord(groupID,dwQueryUserID)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_PRESENT)
    buffer:pushdword(groupID)
    buffer:pushdword(dwQueryUserID or GlobalUserItem.tabAccountInfo.dwUserID)

    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end


--修改玩法名
function ClubFrame:sendModifyConfigName(tabConfig)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_MODIFY_CONFIG)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID) --玩家ID
    buffer:pushdword(tabConfig.dwGroupID) --俱乐部ID
    buffer:pushdword(tabConfig.dwConfigID) --玩法ID
    buffer:pushstring(tabConfig.szConfigName,13)  --玩法名
    buffer:pushbyte(tabConfig.cbPlayMode) --结算模式0积分，1勋章
    buffer:pushscore(tabConfig.lCellScore) --底分设置
    buffer:pushdword(tabConfig.dwDrawCountLimit)
    buffer:pushdword(tabConfig.dwDrawTimeLimit)
    buffer:pushword(tabConfig.wJoinGamePeopleCount)    --游戏人数
    buffer:pushbyte(0)
    buffer:pushbyte(0)
    for i=1,100 do
        buffer:pushbyte(tabConfig.cbGameRule[1][i])
    end
    buffer:pushbyte(tabConfig.cbGameMode)
    buffer:pushscore(tabConfig.lEnterScore)
    buffer:pushscore(tabConfig.lLeaveScore)
    buffer:pushbyte(tabConfig.cbTaxMode)   
    buffer:pushdword(tabConfig.dwTaxCount)  
    buffer:pushbyte(tabConfig.cbLessScoreDismiss)  
    
    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end




--删除玩法
function ClubFrame:sendDeleteGroupConfig(groupID,configId)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_DELETE_CONFIG)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(groupID)
    buffer:pushdword(configId)
    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end


--设置权限
function ClubFrame:sendChangeRight(dwGroupID,dwMemberID,nRight,cbMode)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_MODIFY_RIGHT)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwMemberID)
    buffer:pushint(nRight)   
    buffer:pushbyte(cbMode)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end



--查询创建
function ClubFrame:sendQueryCreate(groupID,configId)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_CREATE)
    buffer:pushdword(groupID)
    buffer:pushdword(configId)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)

    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end

--查询日统计
function ClubFrame:sendQueryDateRecord(groupID,dateTime)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_DAYRECORD)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dateTime)

    if not self:sendSocketData(buffer) then
        print("发送查询日统计失败！")
    end
end


--查询勋章模式
function ClubFrame:sendQueryFeeMode(groupID)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_MEDALMODE)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)

    if not self:sendSocketData(buffer) then
        print("发送删除玩法失败！")
    end
end


--设置跑马灯
function ClubFrame:sendRollMsg(groupID,cbFlag,str)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_ROLLMSG)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushbyte(cbFlag)
    buffer:pushstring(str,33)
    if not self:sendSocketData(buffer) then
        print("发送跑马灯失败！")
    end
end




--设置公告
function ClubFrame:sendNoticeMsg(groupID,str)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_EDIT_NOTICE)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(str,109)
    print("========sendNoticeMsg=========")
    if not self:sendSocketData(buffer) then
        print("发送公告失败！")
    end
end


--设置大联盟状态
function ClubFrame:sendClubStatus(groupID,flag)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_GROUP_STATUS)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)
    buffer:pushbyte(flag)

    if not self:sendSocketData(buffer) then
        print("发送大联盟状态失败！")
    end
end


--进出设置
function ClubFrame:sendInOutSet(groupID,cbInMode,cbInTybe,cbOutMode)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_INOUT)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushbyte(cbInMode)
    buffer:pushbyte(cbInTybe)   
    buffer:pushbyte(cbOutMode)

    if not self:sendSocketData(buffer) then
        print("发送进出设置失败！")
    end
end

--权限设置
function ClubFrame:sendRightSet(groupID,mgRight,cpRight,dwTargetID)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_RIGHT_MANAGEMENT)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwTargetID or 0)
    for i=1,30 do
        if mgRight[i] then
            buffer:pushbyte(mgRight[i])
        else
            buffer:pushbyte(0)
        end
    end

    for i=1,30 do
        if cpRight[i] then
            buffer:pushbyte(cpRight[i])
        else
            buffer:pushbyte(0)
        end
    end

    if not self:sendSocketData(buffer) then
        print("发送进出设置失败！")
    end
end


--勋章设置
function ClubFrame:sendXunzhangSet(groupID,tabData)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_MEDAL)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    for i=1, 5 do
        buffer:pushbyte(tabData[i])
    end

    if not self:sendSocketData(buffer) then
        print("发送进出设置失败！")
    end
end

--空桌设置
function ClubFrame:sendFreeTableSet(groupID,cbFreeTableLimit)
    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_FREE_TABLE)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushbyte(cbFreeTableLimit)

    if not self:sendSocketData(buffer) then
        print("发送进出设置失败！")
    end
end


--大赢家设置
function ClubFrame:sendMaxWinSet(groupID,cbShow)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_BIGWINNER_SHOW)
    buffer:pushdword(groupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushbyte(cbShow)


    if not self:sendSocketData(buffer) then
        print("发送进出设置失败！")
    end
end


--查询玩家当日战绩
function ClubFrame:sendQueryUserDetail(dwGroupID,dwTargetUserID,dwDateID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_USER_DETAIL)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwTargetUserID)    
    buffer:pushdword(dwDateID)


    if not self:sendSocketData(buffer) then
        print("发送查询单日战绩失败！")
    end
end


--查询俱乐部当日战绩
function ClubFrame:sendQueryClubDetail(dwGroupID,dwDateID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_GROUP_DETAIL)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwDateID)

    if not self:sendSocketData(buffer) then
        print("发送查询俱乐部单日战绩失败！")
    end
end


--设置点赞
function ClubFrame:sendSetStatisticFlag(dwGroupID,dwTargetUserID,dwDateID,cbFlag)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_FLAG)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwTargetUserID)
    buffer:pushdword(dwDateID)
    buffer:pushbyte(cbFlag)

    if not self:sendSocketData(buffer) then
        print("发送点赞设置失败！")
    end
end

--解散桌子
function ClubFrame:sendDismissTable(dwGroupID,dwRoomID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_DISMISS_CLUB_TABLE)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwRoomID)

    if not self:sendSocketData(buffer) then
        print("发送点赞设置失败！")
    end
end



--设置快速开始玩法
function ClubFrame:sendFastConfig(dwGroupID,dwConfigID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_FAVORITE_CONFIG)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwConfigID)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--设置快速开始玩法
function ClubFrame:sendModifyGroupName(dwGroupID,szGroupName)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_MODIFY_GROUP_NAME)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(szGroupName,cmd_Club.LEN_GROUP_NAME)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end



--房间详情
function ClubFrame:sendQueryRoomDetail(dwGroupID,dwRoomID,dwDateID,endTime)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_ROOM_DETAIL)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwRoomID)
    buffer:pushdword(dwDateID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushword(endTime.wYear)
    buffer:pushword(endTime.wMonth)
    buffer:pushword(endTime.wDayOfWeek)
    buffer:pushword(endTime.wDay)
    buffer:pushword(endTime.wHour)
    buffer:pushword(endTime.wMinute)
    buffer:pushword(endTime.wSecond)
    buffer:pushword(endTime.wMilliseconds)



    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--查询队长列表
function ClubFrame:sendQueryCaptainList(dwGroupID,dwUserID,dwCaptainID,dwDateID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_CAPTAIN_LIST)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwUserID)
    buffer:pushdword(dwCaptainID)    
    buffer:pushdword(dwDateID)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--调配队员
function ClubFrame:sendSetMemberChange(dwGroupID,dwUserID,dwCaptainID,tabMemberID,cbMode)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_ADD_CAPTAIN_TEAMER)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwUserID)
    buffer:pushdword(dwCaptainID)
    --buffer:pushdword(dwMemberID)
    buffer:pushbyte(cbMode)
    buffer:pushint(#tabMemberID)
    for i=1,#tabMemberID do
        buffer:pushdword(tabMemberID[i])
    end

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

-- --查询小队日统计
-- function ClubFrame:sendQueryTeamRecord(dwGroupID,dwQueryUserID,dwDateID)

--     local buffer = CCmd_Data:create()
--     buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_TEAM_DAY_RECORD)
--     buffer:pushdword(dwGroupID)
--     buffer:pushdword(dwQueryUserID)
--     buffer:pushdword(dwDateID)


--     if not self:sendSocketData(buffer) then
--         print("发送玩法设置失败！")
--     end
-- end


--查询小队成员日统计
function ClubFrame:sendQueryTeamRecord(dwGroupID,dwQueryUserID,dwCaptainID,dwDateID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_TEAMER_RECORD)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwQueryUserID)
    buffer:pushdword(dwCaptainID)
    buffer:pushdword(dwDateID)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end


--隐私设置
function ClubFrame:sendQueryPrivacy(dwGroupID,dwUserID,cbShowGroupID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_PRIVACY)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwUserID)
    buffer:pushdword(cbShowGroupID)--0隐藏ID，1显示ID

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--设置队长点位
function ClubFrame:sendQueryCaptainPoint(dwGroupID,dwUserID,dwCaptainID,wPoint)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_SET_CAPTAIN_POINT)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(dwUserID)
    buffer:pushdword(dwCaptainID)
    buffer:pushword(wPoint)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--获取提取记录
function ClubFrame:sendQueryOpenBox(dwGroupID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_OPEN_BOX)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end


--提取积分
function ClubFrame:sendPickUpScore(dwGroupID,lScore)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_GET_AWARDSCORE)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushscore(lScore)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--查询邀请码
function ClubFrame:sendQueryCode(dwGroupID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_INVITATION_CODE)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--绑定邀请码
function ClubFrame:sendBindInvitationCode(dwGroupID,lInvitationCode)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_BLIND_INVITATION_CODE)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushscore(lInvitationCode)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end


function ClubFrame:sendQueryClubScore(dwGroupID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_SCORE)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end


function ClubFrame:sendQueryTableBattleInfo(dwGroupID,dwServerID,dwTableID)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_QUERY_ROOM_BATTLE_INFO)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(dwServerID)
    buffer:pushdword(dwTableID)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end

--更新资料
function ClubFrame:sendModifyInfo(dwGroupID,cbType,strMsg)

    local buffer = CCmd_Data:create()
    buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_MODIFY_INFO)
    buffer:pushdword(dwGroupID)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)

    local flag ={0,0,0}
    flag[cbType] = 1
    for i=1,3 do
        buffer:pushbyte(flag[i])
    end
    local str = {"","",""}
    str[cbType] = strMsg
    buffer:pushstring(str[1],yl.LEN_NICKNAME)
    buffer:pushstring(str[2],yl.LEN_NICKNAME)
    buffer:pushstring(str[3],12)

    if not self:sendSocketData(buffer) then
        print("发送玩法设置失败！")
    end
end


------------------------------------------------------------------------------------------------------
------------------------------------MDM_GA_GROUP_SERVICE----------------------------------------------

-- 进入群组
function ClubFrame:sendEnterGroup(groupID)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_EnterGroup)
    buffer:setcmdinfo(cmd_Club.MDM_GA_GROUP_SERVICE,cmd_Club.SUB_GA_ENTER_GROUP)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(groupID)

    if not self:sendSocketData(buffer) then
        print("发送进入群组失败！")
    end
end

-- 退出群组
function ClubFrame:sendExitGroup(groupID)
    local buffer = ExternalFun.create_netdata(cmd_Club.SUB_GA_LeaveGroup)
    buffer:setcmdinfo(cmd_Club.MDM_GA_GROUP_SERVICE,cmd_Club.SUB_GA_LEAVE_GROUP)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(groupID)

    if not self:sendSocketData(buffer) then
        print("发送退出群组失败！")
    end
end


------------------------------------------------------------------------------------------------------
------------------------------------MDM_GA_BATTLE_SERVICE----------------------------------------------
--请求桌子
function ClubFrame:sendGetTableList(wKindID, groupID)
    local buffer = ExternalFun.create_netdata(cmd_Club.CMD_GA_QueryTable)
    buffer:setcmdinfo(cmd_Club.MDM_GA_BATTLE_SERVICE,cmd_Club.SUB_GA_QUERY_TABLE)
    buffer:pushword(wKindID)
    buffer:pushdword(groupID)

    if not self:sendSocketData(buffer) then
        print("发送请求桌子失败！")
    end
end

--获取玩家列表
function ClubFrame:sendGetTableUserList(groupID, dwPersonalRoomID)
    local buffer = ExternalFun.create_netdata(cmd_Club.CMD_GA_GetTableUserList)
    buffer:setcmdinfo(cmd_Club.MDM_GA_BATTLE_SERVICE,cmd_Club.SUB_GA_GET_TABLE_USERLIST)
    buffer:pushdword(groupID)
    buffer:pushdword(dwPersonalRoomID)

    if not self:sendSocketData(buffer) then
        print("发送获取玩家列表失败！")
    end
end


--网络信息(长连接)
function ClubFrame:onSocketEvent( main,sub,pData )
    print("ClubFrame:onSocketEvent ==> " .. main .. "##" .. sub)
    local dissmissPop = true
    if cmd_Club.MDM_GA_LOGIC_SERVICE == main then
        if cmd_Club.SUB_GA_APPLY_MESSAGE == sub then                           -- 请求信息
            self:onSubApplyMessageList(pData)
        elseif cmd_Club.SUB_GA_LOGON_FAILURE == sub then                           -- 请求信息
            local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_OperateFailure, pData)
            local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
                showToast(runScene, "大联盟登录失败！".. cmd_table.lErrorCode, 2)
            end
        elseif cmd_Club.SUB_GA_SEARCH_RESULT == sub then                       -- 搜索结果
            self:onSubSerchGroupResult(pData)
        elseif cmd_Club.SUB_GA_WEALTH_UPDATE == sub then                        -- 更新玩家金钱
            self:onSubUpdataWealth(pData)
        elseif cmd_Club.SUB_GA_APPLY_DEL_RESULT == sub then                     -- 删除申请信息
            self:onSubDeleteMessage(pData)
        elseif cmd_Club.SUB_GA_OPERATE_FAILURE == sub then                  --操作失败
            self:onSubOperateFailure(pData)
        elseif cmd_Club.SUB_GA_OPERATE_SUCCESS == sub then                  --操作成功
            self:onSubOperateSuccess(pData)
        elseif cmd_Club.SUB_GA_SYSTEM_MESSAGE == sub then                  --系统消息
            print("SUB_GA_SYSTEM_MESSAGE")
        elseif cmd_Club.SUB_GA_QUERY_BATTLE_RESULT == sub then                        --请求详情结果
            self:onSubGetBattleInfo(pData)
        elseif cmd_Club.SUB_GA_SETTLE_BATTLE_RESULT == sub then                        --设置结果
            self:onSubSetSettleInfo(pData)
        elseif cmd_Club.SUB_GA_TOTAL_RESULT == sub then                        --设置结果
            self:onSubGetBattleTotal(pData)
        elseif cmd_Club.SUB_GA_PRESENT_SUCCESS == sub then 
            self:onSubChangeScore(pData)
        elseif cmd_Club.SUB_GA_QUERY_PRESENT_LIST == sub then 
            self:onSubQueryScoreRecord(pData)
        elseif cmd_Club.SUB_GA_QUERY_CREATE_RESULT == sub then
            self:onSubQueryCreateResult(pData)
        elseif cmd_Club.SUB_GA_QUERY_DAYRECORD_LIST == sub then
            self:onSubQueryDateRecordResult(pData)
        elseif cmd_Club.SUB_GA_MEDAL_MODE == sub then
            self:onSubQueryFeeModeResult(pData)
        elseif cmd_Club.SUB_GA_DAY_DETAIL_LIST == sub then
            self:onSubQueryDayDetailResult(pData)
        elseif cmd_Club.SUB_GA_GROUP_DAY_DETAIL_LIST == sub then
            self:onSubQueryClubDayDetailResult(pData)
        elseif cmd_Club.SUB_GA_FAVORITE_CONFIG == sub then       
            self:onSubSetFavoriteConfigResult(pData)
        elseif cmd_Club.SUB_GA_ROOM_DETAIL_INFO == sub then       
            self:onSubRoomDetail(pData)
        elseif cmd_Club.SUB_GA_CAPTAIN_LIST == sub then         
            self:onSubCapTainList(pData)
        elseif cmd_Club.SUB_GA_TEAM_DAY_RECORD == sub then         
            self:onSubTeamDayRecord(pData)
        elseif cmd_Club.SUB_GA_TEAMER_DAY_RECORD == sub then         
            self:onSubTeamerDayRecord(pData)
        elseif cmd_Club.SUB_GA_SET_CAPTAIN_POINT_RESULT == sub then         
            self:onSubCaptainPointResult(pData)
        elseif cmd_Club.SUB_GA_BOX_INFO == sub then        
            self:onSubBoxInfo(pData)            
        elseif cmd_Club.SUB_GA_GETBOXSCORE_RESULT == sub then        
            self:onSubPickUpScoreResult(pData)    
        elseif cmd_Club.SUB_GA_INVITATION_CODE == sub then        
            self:onSubInvitationCodeResult(pData)  
        elseif cmd_Club.SUB_GA_BLIND_CODE_RES == sub then        
            self:onSubBlindInvitationCodeResult(pData)  
        elseif cmd_Club.SUB_GA_QUERY_SCORE_RES == sub then        
            self:onSubClubScoreResult(pData)  
        elseif cmd_Club.SUB_GA_QUERY_TABLE_RES == sub then        
            self:onSubTableBattleInfoResult(pData)  
        end
    elseif cmd_Club.MDM_GA_GROUP_SERVICE == main then 
        if cmd_Club.SUB_GA_ENTER_FAILURE == sub then                           -- 请求加入失败
            self:onSubEnterFailure(pData)
        elseif cmd_Club.SUB_GA_GROUP_ITEM == sub then                           -- 请求加入成功，返回群组信息
            self:onSubEnterSuccess(pData)
        elseif cmd_Club.SUB_GA_GROUP_MEMBER == sub then                     --下发成员信息
            self:onSubGroupMenber(pData)
        elseif cmd_Club.SUB_GA_ENTER_SUCCESS == sub then                     --成功
            self:onSubGroupEnterSuccess(pData)
        elseif cmd_Club.SUB_GA_QUERY_PROPERTY == sub then                      -- 登录成功下发群组信息
            self:onSubGroupList(pData)
        elseif cmd_Club.SUB_GA_QUERY_PROPERTY_END == sub then
            self:onSubLogonEnd(pData)
        elseif cmd_Club.SUB_GA_GROUP_PROPERTY == sub then                      -- 创建群成功或者进入俱乐部成功
            self:onSubCreateGroupSuccess(pData)
        elseif cmd_Club.SUB_GA_GROUP_UPDATE == sub then                         -- 更新群组
            self:onSubUpdateGroup(pData)
        elseif cmd_Club.SUB_GA_GROUP_DELETE == sub then                         -- 删除群组
            self:onSubDeleteGroup(pData)
        elseif cmd_Club.SUB_GA_MEMBER_DELETE == sub then                         -- 删除成员
            self:onSubDeleteMember(pData)
        elseif cmd_Club.SUB_GA_MEMBER_UPDATE == sub then                         -- 成员属性更新
            self:onSubUpdateMember(pData)

        elseif cmd_Club.SUB_GA_MEMBER_INSERT == sub then                         -- 添加成员
            self:onSubInsertMember(pData)
        elseif cmd_Club.SUB_GA_GETREPOND == sub then                            -- 服务器回应，不带内容
            print("get Socket respond!!!!!")
        elseif cmd_Club.SUB_GA_LOGON_SUCCESS == sub then                            --登陆成功 服务器回应，不带内容
            self:onSubLoginGroupSuccess(pData)
        elseif cmd_Club.SUB_AG_GROUP_EXTRA_INFO == sub then                            --俱乐部消息刷新
            self:onSubUpdateGroupExtra(pData)
        elseif cmd_Club.SUB_GA_BATTLE_CONFIG == sub then
            self:onSubBattleConfig(pData)
        elseif cmd_Club.SUB_GA_CONFIG_APPEND == sub then
            self:onSubAddConfig(pData)
        elseif cmd_Club.SUB_GA_CONFIG_MODIFY == sub then
            self:onSubModifyConfig(pData)
        elseif cmd_Club.SUB_GA_CONFIG_DELETE == sub then
            self:onSubDeleteConfig(pData)
        elseif cmd_Club.SUB_GA_RIGHT_CHANGE == sub then
            self:onSubRightChange(pData)
        elseif cmd_Club.SUB_GA_ROLL_MSG == sub then
            self:onSubRollMsg(pData)
        elseif cmd_Club.SUB_GA_GROUP_NOTICE == sub then
            self:onSubNoticeMsg(pData)
        elseif cmd_Club.SUB_GA_GROUP_STATUS == sub then
            self:onSubGroupStatus(pData)
        elseif cmd_Club.SUB_GA_INOUT_MODE == sub then
            self:onSubInOutSet(pData)     
        elseif cmd_Club.SUB_GA_FREE_TABLE_LIMIT == sub then      
            self:onSubFreeTableSet(pData)    
        elseif cmd_Club.SUB_GA_BIGWINNER_SHOW_MODE == sub then
            self:onSubMaxWinSet(pData)    
        elseif cmd_Club.SUB_GA_SET_MANAGER_RIGHT == sub then
            self:onSubRightSet(pData) 
        elseif cmd_Club.SUB_GA_XUNZHANG_MODE == sub then   
            self:onSubXunZhangSet(pData)   
        elseif cmd_Club.SUB_GA_MEMBER_STATUS == sub then   
            self:onSubMemberStatus(pData)   
        elseif cmd_Club.SUB_GA_SET_FLAG_RESULT == sub then   
            self:onSubSetFlagResult(pData)          
        elseif cmd_Club.SUB_GA_SET_CLEAR_MEDAL == sub then   
            self:onSubScoreClean(pData) 
        elseif cmd_Club.SUB_GA_GROUP_NAME == sub then   
            self:onSubGroupName(pData)
        elseif cmd_Club.SUB_GA_ADD_TEAMER == sub then  
            self:onSubAddTeamer(pData)
        elseif cmd_Club.SUB_GA_GROUP_PRIVACY == sub then  
            self:onSubGroupPrivacy(pData)
        elseif cmd_Club.SUB_GA_GROUP_COMMON_INFO == sub then  
            self:onSubGroupCommonInfo(pData)

        end
    elseif cmd_Club.MDM_GA_BATTLE_SERVICE == main then 
        if cmd_Club.SUB_GA_TABLE_LIST == sub then                               -- 查询桌子
            self:onSubGetTableList(pData)
        elseif cmd_Club.SUB_GA_TABLE_USERLIST == sub then                       -- 查询桌子玩家
            self:onSubGetTableUserList(pData)
        elseif cmd_Club.SUB_GA_OPERATE_FAILED == sub then                       -- 操作失败
            self:onSubRoomListOpFail(pData)
        elseif cmd_Club.SUB_GA_TABLE_LIST_FINISH == sub then                       -- 列表完成
            self:onSubGetTableListFinish(pData)
        elseif cmd_Club.SUB_GA_USER_TABLE_STATUS == sub then                       -- 用户状态
            self:onSubGetTableUserStata(pData)
        elseif cmd_Club.SUB_GA_TABLE_START == sub then                              -- 桌子开始
            self:onSubGetTableStatus(pData)
        elseif cmd_Club.SUB_GA_TABLE_CHANGEBANKER == sub then                       -- 切换庄家
            self:onSubGetTableChangeBanker(pData)
        elseif cmd_Club.SUB_GA_TABLE_CREATE == sub then                             -- 桌子创建
            self:onSubGetTableCreate(pData)
        elseif cmd_Club.SUB_GA_TABLE_DISMISS == sub then                            -- 桌子解散
            self:onSubGetTableDismiss(pData)
        end
    end

    if dissmissPop then
        self:dismissPopWait()
    end   
end

--申请信息列表
function ClubFrame:onSubApplyMessageList(pData)
    local msgType =  pData:readbyte()
    local msgCount =  pData:readword()
    --print("onSubApplyMessageList", msgCount, msgType)

    for i=1, msgCount do
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMApplyMessage, pData)
        --dump(cmd_table, "onSubApplyMessageList", 10)

            --先删除之前的
            for i,v in ipairs(self.m_tabApplyRespondMsg) do
                if v.dwMessageID == cmd_table.dwMessageID then
                    table.remove(self.m_tabApplyRespondMsg, i)
                    break
                end
            end
            --if cmd_table.cbMessageStatus ~= cmd_Club.APPLY_STATUS_REFUSE and cmd_table.cbMessageStatus ~= cmd_Club.APPLY_STATUS_AGREE then
                table.insert(self.m_tabApplyRespondMsg, 1,cmd_table)
            --end

    end

    --dump(self.m_tabApplyRespondMsg, "onSubApplyMessageList1111", 10)

    if self.m_cRoomView and self.m_cRoomView.updateMsgRedPoint and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateMsgRedPoint()
    end

    
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubApplyMessageList then
        self._viewFrame:onSubApplyMessageList()
    end
end

--登录下发我的俱乐部列表
function ClubFrame:onSubGroupList(pData)
    local userID =  pData:readdword()
    local GroupCount =  pData:readword()
    print("ClubFrame:onSubGroupList-->", GroupCount)

     for i=1, GroupCount do
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupProperty, pData)
        local cmd_extra = ExternalFun.read_netdata(cmd_Club.tagIMGroupExtraInfo, pData) --附加信息
        cmd_table.cmd_extra = cmd_extra
        dump(cmd_table, "onSubGroupList", 10)
        for k,v in pairs(self.m_tabGroupList) do
            if v and v.dwGroupID == cmd_table.dwGroupID then
                table.remove(self.m_tabGroupList, k)
                break
            end
        end
        table.insert(self.m_tabGroupList, cmd_table)
    end

end

function ClubFrame:onSubLogonEnd(pData)

        --如果是断线重连
        if self.m_nCurrentClub and self.m_nCurrentClub ~= 0 then

            self._scene:enterClub(self.m_nCurrentClub)
        end

        self._scene:dismissPopWait()
        self._scene.m_bIsReConnet = false

        --大厅界面刷新俱乐部显示
        if nil ~= self._viewFrame and nil ~= self._viewFrame.updateClubIcon then
            self._viewFrame:updateClubIcon({obj=yl.RY_CULB_MEMBER_NOTIFY})
        end

        if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubGroupList then
            self._viewFrame:onSubGroupList()
        end

end


-- 俱乐部搜索结果
function ClubFrame:onSubSerchGroupResult(pData)
    local GroupCount =  pData:readword()
    print("onSubSerchGroupResult", GroupCount)
    local tabGroupList = {}
        for i=1, GroupCount do
            local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupProperty, pData)
            if nil ~= cmd_table then
                table.insert(tabGroupList, cmd_table)
                dump(cmd_table, "onSubSerchGroupResult", 6)
            end
            

        end
   if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubSerchGroupResult then
        self._viewFrame:onSubSerchGroupResult(tabGroupList)
    end
end

--操作上失败，显示消息
function ClubFrame:onSubOperateFailure(pData)
    --展示错误信息
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_OperateFailure, pData)
    print("onSubOperateFailure",cmd_table.lErrorCode, cmd_table.szErrorDescribe)
    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, cmd_table.szErrorDescribe, 2)
    end
    if ( cmd_table.lErrorCode == 110 or cmd_table.lErrorCode == 111 )and nil ~= self._viewFrame and nil ~= self._viewFrame._scene.popShopLayer then
                self._viewFrame._scene:popShopLayer(cmd_table.lErrorCode == 110 and 1 or 2)
    end
end
--操作成功，显示消息
function ClubFrame:onSubOperateSuccess(pData)
    --展示信息
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_OperateSuccess, pData)
    dump(cmd_table,"onSubOperateSuccess")

    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, cmd_table.szOperateNotice, 2)
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onRemoveJionLayer then
        self._viewFrame:onRemoveJionLayer()
    end
end

function ClubFrame:onSubGetBattleInfo(pData)
    --先读取总体数据
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_BattleResult, pData)
    self.m_nCreateRoomNum = cmd_table.dwCreateCount --创建房间数
    self.m_nConsumeIngot = cmd_table.lConsumeIngot --消耗房卡数

    self.m_tabBattleInfo = {}
    local infoNum = cmd_table.dwRecordCount
    for i=1,infoNum do
         local cmd_table = ExternalFun.read_netdata(cmd_Club.tagClubSettleBattleInfo, pData)
         table.insert(self.m_tabBattleInfo, cmd_table)
    end
    dump(self.m_tabBattleInfo, "onSubGetBattleInfo", 6)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubGetBattleInfo then
        self._viewFrame:onSubGetBattleInfo()
    end

end


function ClubFrame:onSubChangeScore(pData)

    local dwGroupID = pData:readdword()
    local dwPresentID = pData:readdword()    
    local cbType = pData:readbyte()
    local nCount = pData:readint()
    for i=1, nCount do
        local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_PresentSuccess, pData)
        if cmd_table.cbSuccess == 1 then
            for i,v in ipairs(self.m_tabGroupMember) do
                   --先判断俱乐部
                   if v.groupID == dwGroupID then
                        local tabMember = v.memberInfo
                        for j=1, #tabMember do
                            if tabMember[j].UserInfo.dwUserID == cmd_table.dwPresentUserID then
                                    tabMember[j].MemberInfo.lScore = cmd_table.lCurScore
                            end
                        end
                        break
                   end
            end
        end
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
        self._viewFrame:onRefreshInfo()

    end

    if self.m_cRoomView and self.m_cRoomView.updateClubData and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end

    if dwPresentID == GlobalUserItem.tabAccountInfo.dwUserID then
            self:sendQueryClubScore(self.m_nCurrentClub)
    end

end

function ClubFrame:onSubQueryScoreRecord(pData)

    local dwGroupID = pData:readdword()
    local nCount = pData:readint()
    local bListEnd = pData:readbool()

    for i=1, nCount do
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMPresentItem, pData)   
        table.insert(self.m_tabScoreRecord,cmd_table)
    end

    if bListEnd and nil ~= self._viewFrame and nil ~= self._viewFrame.onRecordRefresh then
        self._viewFrame:onRecordRefresh()
    end

end

function ClubFrame:onSubGetBattleTotal(pData)
    --读取总体数据
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_BattleResult, pData)
    self.m_nCreateRoomNum = cmd_table.dwCreateCount --创建房间数
    self.m_nConsumeIngot = cmd_table.lConsumeIngot --消耗房卡数
    print("ClubFrame:onSubGetBattleTotal", cmd_table.dwCreateCount, cmd_table.lConsumeIngot)
end

function ClubFrame:onSubQueryCreateResult(pData)

    local dwGroupID = pData:readdword()
    local dwConfigID = pData:readdword()    
    local nMaxNullTableCount = pData:readint()
    local bCanCreate = pData:readbool()
    local dwRoomID = pData:readdword() 


    if nil ~= self._viewFrame and nil ~= self._viewFrame.onGetCreateInfo then
        self._viewFrame:onGetCreateInfo(bCanCreate,dwConfigID,dwRoomID)
    end

end


function ClubFrame:onSubQueryDateRecordResult(pData)

    local dwGroupID = pData:readdword()
    local nCount = pData:readint()
    local bListEnd = pData:readbool()
    
    for i=1,nCount do  
            local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMMemberDayRecord, pData)

            self.m_tabCurDateRecord[cmd_table.dwMemberID] = clone(cmd_table)
            --table.insert(self.m_tabCurDateRecord,)

    end


    if bListEnd and nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshDateRecord then
        self._viewFrame:onRefreshDateRecord(self.m_tabCurDateRecord)
    end

end



function ClubFrame:onSubQueryFeeModeResult(pData)

    local dwGroupID = pData:readdword()
    local cbMedalMode = pData:readbyte()--0关闭，1开启

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onFeeResult then
        self._viewFrame:onFeeResult(cbMedalMode)
    end

end




function ClubFrame:onSubQueryDayDetailResult(pData)

    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_QueryMyDayRecord, pData)

    self.m_tabMyDateRecord.allData = clone(cmd_table)
    self.m_tabMyDateRecord.cellData = self.m_tabMyDateRecord.cellData or {}


    for i=1, cmd_table.nCount do
           -- local dwRoomID = pData:readdword() 
        local dayData = ExternalFun.read_netdata(cmd_Club.tagIMMyDayRecord, pData)
        table.insert(self.m_tabMyDateRecord.cellData,dayData)

    end
    if cmd_table.bListEnd and  nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshDateRecord then
        self._viewFrame:onRefreshDateRecord(self.m_tabMyDateRecord)
    end

end


function ClubFrame:onSubQueryClubDayDetailResult(pData)

    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_QueryClubDayRecord, pData)
    self.m_tabClubDateRecord.allData = clone(cmd_table)
    self.m_tabClubDateRecord.cellData = self.m_tabClubDateRecord.cellData or {}
  

    for i=1, cmd_table.nCount do
        local dayData = ExternalFun.read_netdata(cmd_Club.tagIMClubDayRecord, pData)
        table.insert(self.m_tabClubDateRecord.cellData,dayData)
    end

    if cmd_table.bListEnd and  nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshDateRecord then
        self._viewFrame:onRefreshDateRecord(self.m_tabClubDateRecord)
    end

end


--设置快速开始结果
function ClubFrame:onSubSetFavoriteConfigResult(pData)

    local dwGroupID = pData:readdword()
    local dwMemberID = pData:readdword()
    local dwConfigID = pData:readdword()

    GlobalUserItem.tabGroupUserInfo.MemberInfo.dwFavoriteConfig = dwConfigID

    if self.m_cRoomView  and  tolua.cast(self.m_cRoomView, "cc.Node") and self.m_cRoomView.updateFastBtn then

        self.m_cRoomView:updateFastBtn(true)
    end

end

--房间详情
function ClubFrame:onSubRoomDetail(pData)

    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_QueryRoomDetail_List, pData)
    
    self.m_tabRoomDetail = self.m_tabRoomDetail or clone(cmd_table)
    self.m_tabRoomDetail.turnData = self.m_tabRoomDetail.turnData or {}

    for i=1,cmd_table.nListCount do
            local turnData = ExternalFun.read_netdata(cmd_Club.tagIMRoomTurnDetail, pData)
            self.m_tabRoomDetail.turnData[turnData.nTurnIndex] = clone(turnData)
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
        self._viewFrame:onRefreshInfo(self.m_tabRoomDetail)
    end

end


--小队列表
function ClubFrame:onSubCapTainList(pData)


    local cmd_table1 = ExternalFun.read_netdata(cmd_Club.SUB_GA_CaptainTotalData, pData)

    self.m_tabCaptainList = self.m_tabCaptainList or clone(cmd_table1)
    self.m_tabCaptainList.list = self.m_tabCaptainList.list or {}

    for i=1,cmd_table1.nCount do
            local cmd_table2 = ExternalFun.read_netdata(cmd_Club.tagIMCaptainList, pData)
            table.insert(self.m_tabCaptainList.list,cmd_table2)
    end

    if cmd_table1.bListEnd and nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshDateRecord then
        self._viewFrame:onRefreshDateRecord(self.m_tabCaptainList)
    end

end

-- --所有小队日统计
-- function ClubFrame:onSubTeamDayRecord(pData)

--     local dwGroupID = pData:readdword()
--     local nCount = pData:readint()
--     local bListEnd = pData:readbool()

--     self.m_tabTeamDayRecord = self.m_tabTeamDayRecord or {}

--     for i=1,nCount do
--             local cmd_table = ExternalFun.read_netdata(cmd_Club.tagTeamDayRecord, pData)
--             table.insert(self.m_tabTeamDayRecord,cmd_table)
--     end

--     if bListEnd and nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
--         self._viewFrame:onRefreshInfo(self.m_tabTeamDayRecord)
--     end

-- end


--小队成员日统计
function ClubFrame:onSubTeamerDayRecord(pData)

    local dwGroupID = pData:readdword()
    local nCount = pData:readint()
    local bListEnd = pData:readbool()

    self.m_tabTeamerRecord = self.m_tabTeamerRecord or {}

    for i=1,nCount do
            local cmd_table = ExternalFun.read_netdata(cmd_Club.tagTeamerDayRecord, pData)
            table.insert(self.m_tabTeamerRecord,cmd_table)
    end

    if bListEnd and nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshDateRecord then
        self._viewFrame:onRefreshDateRecord(self.m_tabTeamerRecord)
        self.m_tabTeamerRecord = {}
    end

end


--队长点位设置
function ClubFrame:onSubCaptainPointResult(pData)

    local dwGroupID = pData:readdword()
    local dwCaptainID = pData:readdword()
    local wPoint = pData:readword()


        for i,v in ipairs(self.m_tabGroupMember) do
               --先判断俱乐部
               if v.groupID == dwGroupID then
                    local tabMember = v.memberInfo
                    for i=1, #tabMember do
                        if tabMember[i].UserInfo.dwUserID == dwCaptainID then
                                tabMember[i].MemberInfo.nAwardPoint = wPoint
                        end
                    end
                    break
               end
        end

    local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, "设置点位成功！", 2)
        end
    -- if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
    --     self._viewFrame:onRefreshInfo(self.m_tabTeamDayRecord)
    -- end

end

--积分宝箱
function ClubFrame:onSubBoxInfo(pData)

    local cmd_tableHead = ExternalFun.read_netdata(cmd_Club.SUB_GA_BoxInfo, pData)
    self.m_tabScoreBox = self.m_tabScoreBox or clone(cmd_tableHead)
    self.m_tabScoreBox.list = self.m_tabScoreBox.list or {}

    for i=1, cmd_tableHead.nCount do
            local cmd_tableCell = ExternalFun.read_netdata(cmd_Club.tagIMAwardBoxRecord, pData)
            table.insert(self.m_tabScoreBox.list,cmd_tableCell)
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
        self._viewFrame:onRefreshInfo(self.m_tabScoreBox)
    end

end

--提取积分结果
function ClubFrame:onSubPickUpScoreResult(pData)

     local cmd_tableHead = ExternalFun.read_netdata(cmd_Club.SUB_GA_BoxInfo, pData)
    -- self.m_tabScoreBox = self.m_tabScoreBox or clone(cmd_tableHead)
    -- self.m_tabScoreBox.list = self.m_tabScoreBox.list or {}

    -- for i=1, cmd_tableHead.nCount do
    --         local cmd_tableCell = ExternalFun.read_netdata(cmd_Club.tagIMAwardBoxRecord, pData)
    --         table.insert(self.m_tabScoreBox.list,cmd_tableCell)
    -- end
    dump(cmd_tableHead)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onTransitionInFinish then
        self._viewFrame:onTransitionInFinish()
    end

end

--查询邀请码结果
function ClubFrame:onSubInvitationCodeResult(pData)

     local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_InvitationCode, pData)

    if nil ~= self._viewFrame and nil ~= self._viewFrame.updateCode then
        self._viewFrame:updateCode(cmd_table.lInvitationCode)
    end

end


--绑定邀请码结果
function ClubFrame:onSubBlindInvitationCodeResult(pData)

    local dwGroupID = pData:readdword()
    local dwUserID = pData:readdword()  --玩家ID
    local dwCaptainID = pData:readdword() --队长ID

    dump(dwUserID.."===,"..dwCaptainID)
        for i,v in ipairs(self.m_tabGroupMember) do
               --先判断俱乐部
               if v.groupID == dwGroupID then
                    local tabMember = v.memberInfo
                    for i=1, #tabMember do
                        if tabMember[i].UserInfo.dwUserID == dwUserID then
                                tabMember[i].MemberInfo.dwCaptainID = dwCaptainID
                                tabMember[i].MemberInfo.nMemberRight = cmd_Club.AUTHORITY_TEAMMEMBER
                                break
                        end
                    end
                    break
               end
        end

        if dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
            GlobalUserItem.tabGroupUserInfo.MemberInfo.nMemberRight = cmd_Club.AUTHORITY_TEAMMEMBER
        end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.updateBlind then
        self._viewFrame:updateBlind()
    end

end

function ClubFrame:onSubClubScoreResult(pData)


    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_QueryScoreResult, pData)

    if nil ~= self._viewFrame and nil ~= self._viewFrame.updateClubScore then
        self._viewFrame:updateClubScore(cmd_table.lScore)
    end

end

function ClubFrame:onSubTableBattleInfoResult(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_QueryTableBattleResult, pData)

    if nil ~= self._viewFrame and nil ~= self._viewFrame.updateTableBattleInfo then
        self._viewFrame:updateTableBattleInfo(cmd_table)
    end

end


function ClubFrame:onSubSetSettleInfo(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagClubSettleBattleInfo, pData)
    for i,v in ipairs(self.m_tabBattleInfo) do
        if v.dwUserID == cmd_table.dwUserID then
            table.remove(self.m_tabBattleInfo, i)
            break
        end
    end
    table.insert(self.m_tabBattleInfo, cmd_table)
    dump(self.m_tabBattleInfo, "onSubSetSettleInfo", 6)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubSetSettleInfo then
        self._viewFrame:onSubSetSettleInfo()
    end
end

--成功创建群
function ClubFrame:onSubCreateGroupSuccess(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupProperty, pData)
    local cmd_extra = ExternalFun.read_netdata(cmd_Club.tagIMGroupExtraInfo, pData) --附加信息
    cmd_table.cmd_extra = cmd_extra

    --dump(cmd_table, "onSubCreateGroupSuccess tagIMGroupProperty", 6)
    --assert(false)
    if self.m_curQueryClub ~= 0 then
            
            self.m_curQueryClub = 0
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
            eventListener.obj = yl.RY_CULB_ENTER_NOTIFY
            eventListener.dwGroupID = cmd_table.dwGroupID
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    end

    --添加俱乐部列表
    --先查找
    for i,v in ipairs(self.m_tabGroupList) do
        if v.dwGroupID == cmd_table.dwGroupID then
            table.remove(self.m_tabGroupList, i)
        end
    end
    table.insert(self.m_tabGroupList, cmd_table)


    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)



    --传递群组信息给俱乐部界面
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubCreateGroupSuccess then
        self._viewFrame:onSubCreateGroupSuccess()
    end
end

function ClubFrame:onSubUpdataWealth(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_WealthUpdate, pData)
    GlobalUserItem.tabAccountInfo.lDiamond = cmd_table.lUserIngot

    if self.m_cRoomView and self.m_cRoomView.updateClubData and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end
end

--更新群组
function ClubFrame:onSubUpdateGroup(pData)
    local groupID = pData:readdword()
    local updateKind = pData:readbyte()
    print("*****************ClubFrame:onSubUpdateGroup  updateKind = ", updateKind)

    local tagGroup = nil
    --更新属性
    for i,v in ipairs(self.m_tabGroupItem) do
        print("获取俱乐部信息", v.groupID, groupID)
        if v.groupID == groupID then
            tagGroup = v
        end
    end

    --更新群组属性
    if bit:_and(updateKind, cmd_Club.UPGROUP_KIND_WEALTH) ~= 0 then
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupWealth, pData)
        if nil ~= tagGroup and nil ~= cmd_table then
            tagGroup.groupItem.GroupWealth = cmd_table
        end
    end
    if bit:_and(updateKind, cmd_Club.UPGROUP_KIND_PROPERTY) ~= 0 then
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupProperty, pData)
        
        if nil ~= tagGroup and nil ~= cmd_table then
            tagGroup.groupItem.GroupProperty = cmd_table

             for k,v in pairs(self.m_tabGroupList) do
                if v and v.dwGroupID == groupID then

                    v.wMemberCount = cmd_table.wMemberCount
                    break
                end
            end
                        -- -- 通知更新    
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
            eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
        end
    end

    if bit:_and(updateKind, cmd_Club.UPGROUP_KIND_BATTLERULE) ~= 0 then
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupBattleRule, pData)
        if nil ~= tagGroup and nil ~= cmd_table then
            tagGroup.groupItem.GroupBattleRule = cmd_table
        end
    end

    if groupID ==  self.m_nCurrentClub then
        GlobalUserItem.tabCurGroupInfo = tagGroup.groupItem
    end



    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubUpdateGroup then
        self._viewFrame:onSubUpdateGroup()
    end
end


--删除群组
function ClubFrame:onSubDeleteGroup(pData)
     local groupID = pData:readdword()  --删除的ID
     print("onSubDeleteGroup", groupID)
     for i,v in ipairs(self.m_tabGroupList) do
        if v.dwGroupID == groupID then
            table.remove(self.m_tabGroupList, i)
            if groupID == self.m_nCurrentClub then

                self.m_nCurrentClub = 0


                --解散时在游戏中，不返回俱乐部大厅
                if nil ~= self._scene and nil ~= self._scene._sceneLayer then
                    local sceneRecord = self._scene._sceneLayer._sceneRecord
                    if sceneRecord[#sceneRecord-1] == yl.SCENE_CLUB_ROOM then
                        table.remove(sceneRecord,#sceneRecord-1)
                    end
                end

                            -- 通知更新    
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
            eventListener.obj = yl.RY_CULB_DELETE_NOTIFY
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
            
            end

            break
        end
     end



     --回调，从我的列表删除该俱乐部
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubDeleteGroup then
        self._viewFrame:onSubDeleteGroup(groupID)
    end
end

--删除成员
function ClubFrame:onSubDeleteMember(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_MemberDelete, pData)
    local groupID = cmd_table.dwGroupID

    local tabMember ={}
    for i=1,cmd_table.nCount do
        local dwMemberID = pData:readdword()
        local cbSuccess = pData:readbyte()
        if cbSuccess == 1 then
                --删除成员
                for i,v in ipairs(self.m_tabGroupMember) do
                    if groupID == v.groupID then
                        --查找成员
                        local tabMember = v.memberInfo
                        for j,v in ipairs(tabMember) do
                            if dwMemberID == v.UserInfo.dwUserID then --找到删除
                                table.remove(tabMember, j)
                                break
                            end
                        end
                    end
                end
                --删除成员的俱乐部人员减少
                for i,v in ipairs(self.m_tabGroupList) do
                    if groupID == v.dwGroupID then
                        v.wMemberCount = v.wMemberCount -1
                    end
                end

                for i,v in ipairs(self.m_tabGroupItem) do
                    if v.groupID == groupID then
                        v.groupItem.GroupProperty.wMemberCount = v.groupItem.GroupProperty.wMemberCount -1
                        GlobalUserItem.tabCurGroupInfo = v.groupItem
                        break
                    end
                end

                table.insert(tabMember,dwMemberID)
        end
    end


    for k,v in pairs(tabMember) do

        --判断是不是我，如果是我，删除俱乐部
        if v == GlobalUserItem.tabAccountInfo.dwUserID then
            for i,v in ipairs(self.m_tabGroupList) do
                if v.dwGroupID == groupID then
                    table.remove(self.m_tabGroupList, i)

                    --被删除时在游戏中，不返回俱乐部大厅
                    if nil ~= self._scene and nil ~= self._scene._sceneLayer then
                        local sceneRecord = self._scene._sceneLayer._sceneRecord
                        if sceneRecord[#sceneRecord-1] == yl.SCENE_CLUB_ROOM then
                            table.remove(sceneRecord,#sceneRecord-1)
                        end
                    end

                    -- 通知更新    
                    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
                    eventListener.obj = yl.RY_CULB_OUT_NOTIFY
                    eventListener.type = cmd_table.cbByte
                    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)



                    break
                end
            end
        end
    end

    --通知，刷新俱乐部信息
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
        self._viewFrame:onRefreshInfo()
    end
end


--更新成员
function ClubFrame:onSubUpdateMember(pData)
    local groupID = pData:readdword()
    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupMember, pData)

        --添加
    for i,v in ipairs(self.m_tabGroupMember) do
        if groupID == v.groupID then
            --查找成员
            local tabMember = v.memberInfo
            for j,v in ipairs(tabMember) do
                if v.MemberInfo.dwMemberID == cmd_table.dwMemberID then
                    v.MemberInfo = clone(cmd_table)
                    break
                end
            end

            if cmd_table.dwMemberID == GlobalUserItem.tabAccountInfo.dwUserID then
                if self.m_cRoomView and self.m_cRoomView.updateClubData and  tolua.cast(self.m_cRoomView, "cc.Node") then
                    self.m_cRoomView:updateClubData()
                end
            end

            break
        end
    end

end



--添加成员
function ClubFrame:onSubInsertMember(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_MemberInsert, pData)
    dump(cmd_table, "onSubInsertMember", 6)
    --回调，添加成员
    local groupID = cmd_table.dwGroupID
    local menberID = cmd_table.wMemberCount

    --添加
    for i,v in ipairs(self.m_tabGroupMember) do
        if groupID == v.groupID then
            --查找成员
            local tabMember = v.memberInfo
            --先查找有没有重复成员
            for j,v in ipairs(tabMember) do
                if v.UserInfo.dwUserID == cmd_table.MemberUser.UserInfo.dwUserID then
                    table.remove(tabMember, j)
                end
            end
            table.insert(tabMember, cmd_table.MemberUser)
        end
    end


    --找到制定俱乐部，人数加一
     for i,v in ipairs(self.m_tabGroupList) do
        if groupID == v.dwGroupID then
            v.wMemberCount = v.wMemberCount +1
        end
    end

    for i,v in ipairs(self.m_tabGroupItem) do
        if v.groupID == groupID then
            v.groupItem.GroupProperty.wMemberCount = v.groupItem.GroupProperty.wMemberCount +1
            GlobalUserItem.tabCurGroupInfo = v.groupItem
            break
        end
    end

    dump(self.m_tabGroupList)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onInsertMember then
        self._viewFrame:onInsertMember()
    end
end
    

function ClubFrame:onSubLoginGroupSuccess(pData)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubLoginGroupSuccess then
        self._viewFrame:onSubLoginGroupSuccess()
    end
end


function ClubFrame:onSubBattleConfig(pData)

    local groupID =  pData:readdword()  --群组ID
    local count = pData:readint()  --数量

    self.m_tabGroupConfig[groupID] ={}
    for i=1,count do
            local cmd_table = ExternalFun.read_netdata(cmd_Club.tagGroupConfigInfo, pData)

            table.insert(self.m_tabGroupConfig[groupID],clone(cmd_table))
    end

end


function ClubFrame:onSubUpdateGroupExtra(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupExtraInfo, pData)
        cmd_table.cmd_extra = cmd_extra
        for k,v in pairs(self.m_tabGroupList) do
            if v and v.dwGroupID == cmd_table.dwGroupID then
                v.cmd_extra = cmd_table

                -- -- 通知更新    
                local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
                eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
                cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                break
            end
        end

end


function ClubFrame:onSubAddConfig(pData)

    local groupID =  pData:readdword()  --群组ID

    self.m_tabGroupConfig[groupID] =self.m_tabGroupConfig[groupID] or {}

    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagGroupConfigInfo, pData)

        for k,v in pairs(self.m_tabGroupConfig[groupID]) do
            if v and v.dwConfigID ==  cmd_table.dwConfigID then
                       table.insert(self.m_tabGroupConfig[groupID],k)
            end
        end

        table.insert(self.m_tabGroupConfig[groupID],clone(cmd_table))


    if self.m_cRoomView and self.m_cRoomView.updateConfigList and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateConfigList()
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onConfigSuccess then
        self._viewFrame:onConfigSuccess()
    end

    if self.m_bBattleGameCreate == cmd_table.dwKindID  then --如果是自己创建的玩法，直接创建桌子
        self.m_bBattleGameCreate = nil
        if self.m_cRoomView and self.m_cRoomView.onGetCreateInfo and  tolua.cast(self.m_cRoomView, "cc.Node") then
            self.m_cRoomView:onGetCreateInfo(true,cmd_table.dwConfigID)
        end
    end

end

function ClubFrame:onSubModifyConfig(pData)

    local groupID =  pData:readdword()  --群组ID

    self.m_tabGroupConfig[groupID] =self.m_tabGroupConfig[groupID] or {}

    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagGroupConfigInfo, pData)
    local index = #self.m_tabGroupConfig[groupID]+1
        for k,v in pairs(self.m_tabGroupConfig[groupID]) do
            if v and v.dwConfigID ==  cmd_table.dwConfigID then

                       self.m_tabGroupConfig[groupID][k] = clone(cmd_table)
                       break
            end
        end
  


    if self.m_cRoomView  and  tolua.cast(self.m_cRoomView, "cc.Node") then

        if self.m_cRoomView.updateConfigList  then
                    self.m_cRoomView:updateConfigList()
        end

        if GlobalUserItem.tabGroupUserInfo.MemberInfo.dwFavoriteConfig == cmd_table.dwConfigID then
            GlobalUserItem.tabGroupUserInfo.MemberInfo.dwFavoriteConfig = 0
            if self.m_cRoomView.updateFastBtn then
                self.m_cRoomView:updateFastBtn()
            end
        end
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onConfigSuccess then
        self._viewFrame:onConfigSuccess()
    end

end

function ClubFrame:onSubDeleteConfig(pData)

    local groupID =  pData:readdword()  --群组ID
    local configID =  pData:readdword()  --玩法ID


    if self.m_tabGroupConfig[groupID] then
        for k,v in pairs(self.m_tabGroupConfig[groupID]) do
            if v and v.dwConfigID ==  configID then
                        table.remove(self.m_tabGroupConfig[groupID],k)
            end
        end

    end

    if self.m_cRoomView  and  tolua.cast(self.m_cRoomView, "cc.Node") then

        if self.m_cRoomView.updateConfigList  then
                    self.m_cRoomView:updateConfigList()
        end

        if GlobalUserItem.tabGroupUserInfo.MemberInfo.dwFavoriteConfig == configID then
            GlobalUserItem.tabGroupUserInfo.MemberInfo.dwFavoriteConfig = 0
            if self.m_cRoomView.updateFastBtn then
                self.m_cRoomView:updateFastBtn()
            end
        end
    end


end


--设置权限
function ClubFrame:onSubRightChange(pData)

    local dwGroupID = pData:readdword()
    local dwMemberID = pData:readdword()
    local nMemberRight = pData:readint()
    for i,v in ipairs(self.m_tabGroupMember) do
       --先判断俱乐部
       if v.groupID == dwGroupID then
            local tabMember = v.memberInfo
            for j=1, #tabMember do
                
                    if tabMember[j].UserInfo.dwUserID == dwMemberID then
                        tabMember[j].MemberInfo.nMemberRight = nMemberRight
                        if nMemberRight == 0 then --如果是普通成员，去掉队长ID
                            tabMember[j].MemberInfo.dwCaptainID = 0
                        end

                        if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
                            self._viewFrame:onRefreshInfo()
                        end

                        if nil ~= self._viewFrame and nil ~= self._viewFrame.updateCurDateRecord then
                            self._viewFrame:updateCurDateRecord()
                        end
                        break
                    end

            end
            break
       end
    end




    if self.m_cRoomView and self.m_cRoomView.updateClubData and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end

end

--跑马灯
function ClubFrame:onSubRollMsg(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_RollMsg, pData)


    self.m_tabClubRollMsg[cmd_table.cbMsgType] = clone(cmd_table)

    dump(cmd_table,"====================dfsfsffdsf")

    if self.m_cRoomView and self.m_cRoomView.updateRollMsg and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateRollMsg(true)
    end

end

--公告
function ClubFrame:onSubNoticeMsg(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_GroupNotice, pData)
    dump(cmd_table,"公告")
    self.m_szClubNoticeMsg = cmd_table.szNotice

end


function ClubFrame:onSubGroupStatus(pData)
    local groupID =  pData:readdword()  --群组ID
    local status =  pData:readbyte()  --状态值0删除，1生效，2冻结
    if groupID == self.m_nCurrentClub then 
        GlobalUserItem.tabCurGroupInfo.GroupProperty.cbGroupStatus = status
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, status==1 and "大联盟已解冻！" or "大联盟已冻结！", 2)
        end
        if nil ~= self._viewFrame and nil ~= self._viewFrame.updateFreezeBtn then
            self._viewFrame:updateFreezeBtn()
        end
    end 
end


--进出设置
function ClubFrame:onSubInOutSet(pData)
    local groupID =  pData:readdword()  --群组ID
    local cbInMode =  pData:readbyte()  --0不需审核，1需审核
    local cbInType =  pData:readbyte()  --0邀请码，1大联盟ID
    local cbOutMode =  pData:readbyte()  --0不需审核，1需审核
    if groupID == self.m_nCurrentClub then 
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbInMode = cbInMode
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbGroupIDShow = cbInType
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbOutMode = cbOutMode
    end 

    for k,v in pairs(self.m_tabGroupList) do
        if v and v.dwGroupID == groupID then
            v.cmd_extra.cbGroupShow = cbInType

            -- 通知更新    
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
            eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
            break
        end
    end

end

--空桌设置
function ClubFrame:onSubFreeTableSet(pData)
    local dwGroupID =  pData:readdword()  --群组ID
    local cbCount =  pData:readbyte()  --空桌数量
    if dwGroupID == self.m_nCurrentClub then 
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbFreeTableLimit = cbCount
    end 
end


--大赢家设置
function ClubFrame:onSubMaxWinSet(pData)
    local dwGroupID =  pData:readdword()  --群组ID
    local cbShowMode =  pData:readbyte()  --0显示，1仅管理员显示
    if dwGroupID == self.m_nCurrentClub then 
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbBigWinnerShow = cbShowMode
    end
end


--权限设置
function ClubFrame:onSubRightSet(pData)

    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_ManagerRight, pData)

    if cmd_table.dwGroupID == self.m_nCurrentClub then 

        if cmd_table.dwTargetID ~= 0 then
            for k,v in pairs(ClubFrame:getInstance().m_tabGroupMember) do
                    if v and v.groupID == cmd_table.dwGroupID then

                        for n,m in pairs(v.memberInfo) do
                                if m and m.UserInfo.dwUserID == cmd_table.dwTargetID then
                                        m.MemberInfo.cbManagerRightFlag = 1
                                        m.MemberInfo.lMemberManagerRight = cmd_table.lManagerRight
                                end
                         end




                    end
             end
        else
            self.m_cbManagerRight = cmd_table.lManagerRight
            self.m_cbCaptainRight = cmd_table.lCaptainRight
        end

    end 


    if self.m_cRoomView and self.m_cRoomView.updateClubData and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end
end


--调配成员
function ClubFrame:onSubAddTeamer(pData)

    local dwGroupID =  pData:readdword()
    local dwUserID =  pData:readdword()
    local dwCaptainID =  pData:readdword()
    --local dwMemberID =  pData:readdword()
    local cbMode =  pData:readbyte()
    local nCount = pData:readint()

    for i=1,nCount do
            local dwMemberID =  pData:readdword()
            local cbSuccess = pData:readbyte()
            if cbSuccess == 1 then 
                for i,v in ipairs(self.m_tabGroupMember) do
                       --先判断俱乐部
                       if v.groupID == dwGroupID then
                            local tabMember = v.memberInfo
                            for i=1, #tabMember do
                                if tabMember[i].UserInfo.dwUserID == dwMemberID then
                                        tabMember[i].MemberInfo.dwCaptainID = cbMode ==1 and dwCaptainID or 0
                                        tabMember[i].MemberInfo.nMemberRight = cbMode ==1 and cmd_Club.AUTHORITY_TEAMMEMBER or cmd_Club.AUTHORITY_MEMBER
                                end
                            end
                            break
                       end
                end
            end
    end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.updateCurDateRecord then
        self._viewFrame:updateCurDateRecord()
    end
end

--隐私设置
function ClubFrame:onSubGroupPrivacy(pData)

    local dwGroupID =  pData:readdword()
    local cbShowGroupID =  pData:readbyte()


    if dwGroupID == self.m_nCurrentClub then 
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbGroupIDShow = cbShowGroupID
    end 


    -- if self.m_cRoomView and self.m_cRoomView.updateClubData and  tolua.cast(self.m_cRoomView, "cc.Node") then
    --     self.m_cRoomView:updateClubData()
    -- end
end


function ClubFrame:onSubGroupCommonInfo(pData)

    local count = pData:getlen()/68

    for i=1,count do
            local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMcommonItem, pData)   
             for k,v in pairs(self.m_tabClubCommon) do
                if v and v.szOptionName == cmd_table.szOptionName then
                    table.remove(self.m_tabClubCommon,k)
                    break
                end
            end

    table.insert(self.m_tabClubCommon,cmd_table)           

    end



    
end



function ClubFrame:isManagerAndRight(right)
    local myMemberInfo = GlobalUserItem.tabGroupUserInfo.MemberInfo
    if bit:_and(myMemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 then
        if myMemberInfo.cbManagerRightFlag ==0 then
            return bit:_and(self.m_cbManagerRight, right) ~= 0
        else
            return bit:_and(myMemberInfo.lMemberManagerRight, right) ~= 0
        end
    end
    return false
end

function ClubFrame:isCaptainAndRight(right)
    local myMemberInfo = GlobalUserItem.tabGroupUserInfo.MemberInfo
    if bit:_and(myMemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
        
        return bit:_and(self.m_cbCaptainRight, right) ~= 0
      
    end
    return false
end

--勋章设置返回
function ClubFrame:onSubXunZhangSet(pData)
    local groupID =  pData:readdword()  
    if groupID == self.m_nCurrentClub then 
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbMedalMode = pData:readbyte()
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbMedalShow = pData:readbyte()
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbCurMedalShow = pData:readbyte()
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbMedalRevenueShow = pData:readbyte()
        GlobalUserItem.tabCurGroupInfo.GroupBattleRule.cbMedalClear = pData:readbyte()
    end 

    if self.m_cRoomView and tolua.cast(self.m_cRoomView, "cc.Node") then

        self.m_cRoomView:updateClubData()
    end
end

--玩家状态
function ClubFrame:onSubMemberStatus(pData)
    local dwUserID =  pData:readdword()  
    local cbStatus =  pData:readbyte()  

    for k,v in pairs(self.m_tabGroupMember) do
            if v and v.groupID == self.m_nCurrentClub then
                    for m,n in pairs(v.memberInfo) do
                            if n and n.UserInfo.dwUserID == dwUserID then
                                    n.MemberInfo.cbStatus = cbStatus
                            end
                     end
            end
     end

    if self.m_cRoomView and tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end


end



--勋章设置返回
function ClubFrame:onSubSetFlagResult(pData)
    local dwGroupID =  pData:readdword()  
    local dwOperateUserID =  pData:readdword()  
    local dwTargetUserID =  pData:readdword()  
    local dwDateID =  pData:readdword()  
    local cbFlag =  pData:readbyte()  

    -- if dwGroupID == self.m_nCurrentClub then 
    --     self.m_tabCurDateRecord[dwTargetUserID].cbFlag = cbFlag
    -- end 

    if  nil ~= self._viewFrame and nil ~= self._viewFrame.onFlagResult then
        self._viewFrame:onFlagResult(dwTargetUserID,cbFlag)
    end
end

--勋章清零
function ClubFrame:onSubScoreClean(pData)
    local dwGroupID =  pData:readdword()  

    for k,v in pairs(self.m_tabGroupMember) do
            if v and v.groupID == dwGroupID then
                    for m,n in pairs(v.memberInfo) do                
                            n.MemberInfo.lScore = 0     
                     end
            end
            
            if dwGroupID ==  self.m_nCurrentClub then
                GlobalUserItem.tabGroupUserInfo.MemberInfo.lScore = 0
            end
     end

    if self.m_cRoomView and tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end

end

--修改名称
function ClubFrame:onSubGroupName(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_GroupName, pData)
    for i,v in ipairs(self.m_tabGroupItem) do
        if v.groupID == cmd_table.dwGroupID then

            v.groupItem.GroupProperty.szGroupName = cmd_table.szGroupName
            break
        end
    end

    for i,v in ipairs(self.m_tabGroupList) do
        if cmd_table.dwGroupID == v.dwGroupID then
            v.szGroupName = cmd_table.szGroupName
        end
    end

    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, "大联盟名字修改成功！", 2)
    end

    if self.m_cRoomView and tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateClubData()
    end

    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

end


--申请消息删除
function ClubFrame:onSubDeleteMessage(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_ApplyDelResult, pData)
    dump(cmd_table, "申请消息删除", 10)
    if not cmd_table.bDelResult then
        --显示错误消息
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, cmd_table.szErrorDescribe, 2)
        end
        return
    end
    --判断类型
    if cmd_table.cbMessageType == cmd_Club.APPLY_MSG_TYPE_REQUEST then
        for i,v in ipairs(self.m_tabApplyMsg) do
            if v.dwMessageID == cmd_table.dwMessageID then --找到删除
                table.remove(self.m_tabApplyMsg, i)
            end
        end
    elseif cmd_table.cbMessageType == cmd_Club.APPLY_MSG_TYPE_RESPOND or cmd_table.cbMessageType ==  cmd_Club.APPLY_MSG_TYPE_DELETE then
        for i,v in ipairs(self.m_tabApplyRespondMsg) do
            if v.dwMessageID == cmd_table.dwMessageID then --找到删除
                table.remove(self.m_tabApplyRespondMsg, i)
            end
        end
    end
    --通知，刷新消息列表
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubDeleteMessage then
        self._viewFrame:onSubDeleteMessage()
    end
end

--请求加入失败
function ClubFrame:onSubEnterFailure(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_EnterFailure, pData)
    print("请求加入失败 onSubEnterFailure",cmd_table.lErrorCode, cmd_table.szErrorDescribe)
    --显示错误消息
    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, cmd_table.szErrorDescribe, 2)
    end
    self:dismissPopWait()
end

--请求加入成功
function ClubFrame:onSubEnterSuccess(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.SUB_GA_GroupItem, pData)
    dump(cmd_table, "onSubEnterSuccess", 6)
    --添加信息
    local tabGroupItem = {}
    tabGroupItem.groupID = cmd_table.GroupItem.GroupProperty.dwGroupID
    tabGroupItem.groupItem = cmd_table.GroupItem

    --先删除之前的
    for i=1,#self.m_tabGroupItem do
        local info = self.m_tabGroupItem[i]
        if info.groupID == tabGroupItem.groupID then
            table.remove(self.m_tabGroupItem, i)
            break
        end
    end
    table.insert(self.m_tabGroupItem, tabGroupItem)
    --显示

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubEnterSuccess then
        self._viewFrame:onSubEnterSuccess()
    end
end

--请求成功下发成员信息
function ClubFrame:onSubGroupMenber(pData)
    local groupID =  pData:readdword()  --群组ID
    local tableGroupMember = {}

    tableGroupMember.memberInfo = {}
    tableGroupMember.groupID = groupID
    --计算数目
    local len = pData:getlen() -4  --4是groupID 的长度
    print("=============== onSubGroupMenber ================")
    print("onSubGroupMenber:"..len)
    local tabLen = 144 + 152
    if (len - math.floor(len/tabLen)*tabLen) ~= 0 then
        print("GroupMenber_len_error"..len)
        return
    end
    local itemcount =  math.floor(len/tabLen)
   
    for i=1, #self.m_tabGroupMember do
        local info = self.m_tabGroupMember[i]
        if info.groupID == tableGroupMember.groupID then
            tableGroupMember = clone(info)
        end
    end

    for i=1, itemcount  do
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagIMGroupMemberUser, pData)

        if nil ~= cmd_table.UserInfo.dwUserID then
            for k,v in pairs(tableGroupMember.memberInfo) do
                if v and v.UserInfo.dwUserID == cmd_table.UserInfo.dwUserID then
                    table.remove(tableGroupMember.memberInfo, k)
                    break
                end
            end
            table.insert(tableGroupMember.memberInfo, cmd_table)
        end
    end
    for i=1, #self.m_tabGroupMember do
        local info = self.m_tabGroupMember[i]
        if info.groupID == tableGroupMember.groupID then
            table.remove(self.m_tabGroupMember, i)
            break
        end
    end

    table.insert(self.m_tabGroupMember, tableGroupMember)

    dump(self.m_tabGroupMember)
    --assert(false)
end

--进入俱乐部成功
function ClubFrame:onSubGroupEnterSuccess(pData)
    --进入成功，回调，切换到俱乐部显示界面
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubGroupEnterSuccess then
        self._viewFrame:onSubGroupEnterSuccess()
    end
end

--------------------------------------------------房间信息------------------
function ClubFrame:onSubGetTableList(pData)
     local groupID = pData:readdword()  --群组ID
     local wTableCount = pData:readword()  --桌子数目
     if self.isRoomListSuccess then
        self.m_tabRoomInfoList = {}
        self.isRoomListSuccess = false
     end
     for i=1,wTableCount do
        local cmd_table = ExternalFun.read_netdata(cmd_Club.tagPersonalTableInfo, pData)
        table.insert(self.m_tabRoomInfoList, cmd_table)
     end
end

function ClubFrame:onSubGetTableListFinish(pData)
    self.isRoomListSuccess = true
    --结束了刷新列表
    if self.m_cRoomView and self.m_cRoomView.updateConfigList and  tolua.cast(self.m_cRoomView, "cc.Node") then
        self.m_cRoomView:updateConfigList()
    end
end

function ClubFrame:onSubGetTableUserList(pData)
     local wUserCount = pData:readword()  --玩家数目

     local tableUserInfo = {}
     for i=1,8 do
        local userID = pData:readdword()  --玩家数目
        print("onSubGetTableUserList", userID)
        if yl.INVALID_DWORD ~= userID then
            table.insert(tableUserInfo, userID)
        end
     end

    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubGetTableUserList then
        self._viewFrame:onSubGetTableUserList(tableUserInfo)
    end
end

function ClubFrame:onSubGetTableCreate(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.tagPersonalTableInfo, pData)
    if nil ~= cmd_table then
        table.insert(self.m_tabRoomInfoList, cmd_table)
        if self.m_cRoomView and self.m_cRoomView.updateConfigList and  tolua.cast(self.m_cRoomView, "cc.Node") then
            self.m_cRoomView:updateConfigList()
        end
    end
end

function ClubFrame:onSubGetTableStatus(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.CMD_GA_TableStart, pData)
    
    for i,v in ipairs(self.m_tabRoomInfoList) do
        if v.dwGroupID == cmd_table.dwGroupID and v.dwPersonalRoomID == cmd_table.dwPersonalRoomID then
            v.bGameStart = cmd_table.bStart
            if self.m_cRoomView and self.m_cRoomView.updateRoomTable and  tolua.cast(self.m_cRoomView, "cc.Node") then
                self.m_cRoomView:updateRoomTable(cmd_table.dwPersonalRoomID)
            end
            break
        end
    end
end

--切换庄家
function ClubFrame:onSubGetTableChangeBanker(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.CMD_GA_BankerChange, pData)
    for i,v in ipairs(self.m_tabRoomInfoList) do
        if v.dwGroupID == cmd_table.dwGroupID and v.dwPersonalRoomID == cmd_table.dwPersonalRoomID then
            v.dwBankerUserID = cmd_table.dwBankerUserID
            if self.m_cRoomView and self.m_cRoomView.updateConfigList and  tolua.cast(self.m_cRoomView, "cc.Node") then
                self.m_cRoomView:updateConfigList()
            end
            break
        end
    end
end

function ClubFrame:onSubGetTableUserStata(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.CMD_GA_UserTableStatus, pData)
    local clubID = cmd_table.dwGroupID
    local roomID = cmd_table.dwPersonalRoomID
    local bSit = cmd_table.bSit
    print("onSubGetTableUserStata", clubID, roomID, bSit)
    for i,v in ipairs(self.m_tabRoomInfoList) do
        if v.dwGroupID == clubID and v.dwPersonalRoomID == roomID then
            if bSit then
                if cmd_table.dwUserID ~=0 and v.dwPlayerUserId[1][cmd_table.wChairID+1] ~= cmd_table.dwUserID then 
                    v.wCurSitPlayer = v.wCurSitPlayer +1
                    v.dwPlayerUserId[1][cmd_table.wChairID+1] = cmd_table.dwUserID
                end
            else
                v.wCurSitPlayer = v.wCurSitPlayer -1
                v.dwPlayerUserId[1][cmd_table.wChairID+1] = 0
            end

            if self.m_cRoomView and self.m_cRoomView.updateRoomTable and  tolua.cast(self.m_cRoomView, "cc.Node") then
                self.m_cRoomView:updateRoomTable(roomID)
            end
            break
        end
    end
end

function ClubFrame:onSubGetTableDismiss(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.CMD_CS_S_DismissClubTable, pData)
    local clubID = cmd_table.dwGroupID
    local roomID = cmd_table.dwPersonalRoomID
    print("onSubGetTableDismiss", clubID, roomID)
    for i,v in ipairs(self.m_tabRoomInfoList) do
        if cmd_table.bAllRoom then 
            if v.dwServerID ==  cmd_table.dwServerID then 
                table.remove(self.m_tabRoomInfoList, i)
            end
        else
            if v.dwGroupID == clubID and v.dwPersonalRoomID == roomID then
                table.remove(self.m_tabRoomInfoList, i)
                
                break
            end
        end
    end

        if self.m_cRoomView and self.m_cRoomView.updateConfigList and  tolua.cast(self.m_cRoomView, "cc.Node") then
            self.m_cRoomView:updateConfigList()
        end
end


function ClubFrame:onSubRoomListOpFail(pData)
    local cmd_table = ExternalFun.read_netdata(cmd_Club.CMD_GA_OperateFailed, pData)
    print("================>", cmd_table.szErrorDescribe)
    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, cmd_table.szErrorDescribe, 2)
    end
end


--显示等待
function ClubFrame:showPopWait(msg)
    local runScene = cc.Director:getInstance():getRunningScene()
    if nil == self._popWait and nil ~= runScene then
        self._popWait = PopWait:create()
        self._popWait:show(runScene,msg)            
    end
end

--关闭等待
function ClubFrame:dismissPopWait()
    if self._popWait then
        self._popWait:dismiss()
        self._popWait = nil
    end
end

return ClubFrame
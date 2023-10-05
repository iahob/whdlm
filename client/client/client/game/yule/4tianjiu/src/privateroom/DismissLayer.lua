local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")
local DismissLayer = class("DismissLayer", cc.Layer)

local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local MAX_TIME = 120
local TAG_START             = 100
local enumTable = 
{
    "TAG_MASK",             -- 遮罩
    "BTN_AGREE",            -- 同意解散
    "BTN_DISAGREE",         -- 不同意
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function DismissLayer:ctor( scene )
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/DismissLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 倒计时
    self.textCountDown = spbg:getChildByName("txt_time")
    self.textCountDown:setString("(0)")
    self.textCountDown:setVisible(false)

    -- 解散请求者
    local tmpclip = spbg:getChildByName("_txt_request")
    self.m_clipRequestUser = ClipText:createClipText(tmpclip:getContentSize(), "", nil, 20)
    self.m_clipRequestUser:setAnchorPoint(tmpclip:getAnchorPoint())
    self.m_clipRequestUser:setPosition(tmpclip:getPosition())
    spbg:addChild(self.m_clipRequestUser)
    tmpclip:removeFromParent()

    -- 玩家列表
    self.nOrder = {}
    self.m_tabUserNick = {}
    self.m_tabUserState = {}
    self.m_tabUserHead = {}
    for i = 1, cmd.GAME_PLAYER do
        -- 昵称
        tmpclip = spbg:getChildByName("_txt_nick_" .. i)
        local clipnick = ClipText:createClipText(tmpclip:getContentSize(), "", nil, 20)
        clipnick:setAnchorPoint(tmpclip:getAnchorPoint())
        clipnick:setPosition(tmpclip:getPosition())
        spbg:addChild(clipnick)
        self.m_tabUserNick[i] = clipnick
        tmpclip:removeFromParent()

        -- 头像
        self.m_tabUserHead[i] = spbg:getChildByName("_head_" .. i)

        -- 状态
        tmpclip = spbg:getChildByName("txt_state_" .. i)
        self.m_tabUserState[i] = tmpclip
    end

    -- 同意
    local btn = spbg:getChildByName("btn_agree")
    btn:setTag(TAG_ENUM.BTN_AGREE)
    btn:addTouchEventListener( touchFunC )
    self.m_btnAgree = btn

    -- 不同意
    btn = spbg:getChildByName("btn_disagree")
    btn:setTag(TAG_ENUM.BTN_DISAGREE)
    btn:addTouchEventListener( touchFunC ) 
    self.m_btnDisagree = btn

    self.wMyChairId = self._scene._gameLayer:GetMeChairID()
    print("创建解散界面获取玩家椅子号", self.wMyChairId)
    self.time = 0
    self._requestUser = nil
    self:setVisible(false)

end

function DismissLayer:onButtonClickedEvent(tag, ref)
    if tag == TAG_ENUM.BTN_AGREE then
        PriRoom:getInstance():getNetFrame():sendRequestReply(1)
        self:requestReply(self._scene._gameLayer:GetMeUserItem(), true)
    elseif tag == TAG_ENUM.BTN_DISAGREE then
        PriRoom:getInstance():getNetFrame():sendRequestReply(0)
        self:requestReply(self._scene._gameLayer:GetMeUserItem(), false)
    end
    self:setButtonVisible(false)
end

function DismissLayer:showLayer(remainTime)
    self.time = remainTime
    if not self._timeId then
        self.textCountDown:setVisible(true)
        self._timeId = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
            self:onUpdateClock()
        end, 1, false)
        self:onUpdateClock()
    end
    self._scene._gameLayer.bIsDismiss = true
    self:setVisible(true)
end

function DismissLayer:hideLayer()
    if self._timeId then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeId)
        self._timeId = nil
    end
    self:setVisible(false)
    self:setButtonVisible(true)
    self.textCountDown:setVisible(false)
end

function DismissLayer:onUpdateClock()
    self.textCountDown:setString("( "..self.time.." )")
    self.time = self.time - 1

    if self.time <= 0 then
        self.textCountDown:setVisible(false)
        if self._timeId then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeId)
            self._timeId = nil
        end
        if self.m_btnAgree:isVisible() then
            -- 默认同意
            self:onButtonClickedEvent(TAG_ENUM.BTN_AGREE, self.m_btnAgree)
        end
    end
end

function DismissLayer:onLeaveGame()
	--cc.UserDefault:getInstance():setIntegerForKey("dismissLayerTime", self.time)
end

function DismissLayer:onExit()
    --正在倒计时
    if self._timeId then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeId)
        self._timeId = nil
    end
end

-- 解散请求
function DismissLayer:requestDismissRoom(requestUser, remaintime)
    if nil == requestUser then
        return
    end
    self._requestUser = requestUser
    --隐藏
    for i = 1, cmd.GAME_PLAYER do
        self.m_tabUserNick[i]:setString("")
        self.m_tabUserState[i]:setString("")
        self.m_tabUserHead[i]:setVisible(false)
    end

    --请求者
    local szreq = requestUser.szNickName or ""
    self.m_clipRequestUser:setString(szreq)

    print("PriRoom:getInstance():getChairCount():",PriRoom:getInstance():getChairCount())
    for i = 1, PriRoom:getInstance():getChairCount() do
        local wChairID = i - 1
        local userItem = self._scene._gameLayer:getUserInfoByChairID(wChairID)
        if userItem then
            local bIsGame = false
            for j=1,#self._scene._gameLayer.m_playingUser do
                if userItem.dwUserID == self._scene._gameLayer.m_playingUser[j] then 
                    bIsGame = true
                    break
                end
            end

            print("userItem ", userItem.szNickName)
            if userItem.dwUserID == requestUser.dwUserID then
                -- 昵称
                local sznick = userItem.szNickName or ""
                self.m_tabUserNick[i]:setString(sznick)

                -- 头像
                local head = PopupInfoHead:createNormal(userItem, 88)
                head:enableInfoPop(false)
                self.m_tabUserHead[i]:addChild(head)
                self.m_tabUserHead[i]:setVisible(true)

                -- 动作
                self.m_tabUserState[i]:setString("申请解散")
            elseif bIsGame then
                -- 昵称
                local sznick = userItem.szNickName or ""
                self.m_tabUserNick[i]:setString(sznick)

                --头像
                local head = PopupInfoHead:createNormal(userItem, 88)
                head:enableInfoPop(false)
                self.m_tabUserHead[i]:addChild(head)
                self.m_tabUserHead[i]:setVisible(true)

                -- 动作
                self.m_tabUserState[i]:setString("正在选择..")

               self.nOrder[i] = i

            end
        end
    end
    self.wMyChairId = self._scene._gameLayer:GetMeChairID()
    print("解散房间请求解散玩家椅子号", requestUser.wChairID, self.wMyChairId)
    if requestUser.wChairID == self.wMyChairId then
        self:setButtonVisible(false)
    else
        self:setButtonVisible(true)
    end
    self:showLayer(remaintime or MAX_TIME)
end

--重连解散界面(cbIsDeal)
function DismissLayer:reStartDismissRoom(requestUserChairId, cbIsDeal, remainTime)
    local myChairId = self._scene._gameLayer:GetMeChairID()
    self.m_clipRequestUser:setString("")
    -- 先隐藏
    for i = 1,cmd.GAME_PLAYER do
        self.m_tabUserNick[i]:setString("")
        self.m_tabUserState[i]:setString("")
        self.m_tabUserHead[i]:setVisible(false)
    end
    -- 再显示
    for i = 1, PriRoom:getInstance():getChairCount() do
        local wChairID = i - 1
        local userItem = self._scene._gameLayer:getUserInfoByChairID(wChairID)
        if userItem then
            print("userItem ", userItem.szNickName)
            if userItem.wChairID == requestUserChairId then
                --请求解散者       
                self._requestUser = userItem
                local sznick = userItem.szNickName or ""
                self.m_clipRequestUser:setString(sznick)

                -- 昵称
                self.m_tabUserNick[i]:setString(sznick)

                -- 头像
                local head = PopupInfoHead:createNormal(userItem, 88)
                head:enableInfoPop(false)
                self.m_tabUserHead[i]:addChild(head)
                self.m_tabUserHead[i]:setVisible(true)

                -- 动作
                self.m_tabUserState[i]:setString("申请解散")

                if userItem.wChairID == myChairId then
                    --隐藏按钮
                    self:setButtonVisible(false)
                else
                    self:setButtonVisible(true)
                end
            else    
                --非请求解散者                                    
                local str = ""
                if cbIsDeal[i] == 0 then
                    str = "正在选择.."
                elseif cbIsDeal[i] == 1 then
                    str = "选择了同意"
                    if userItem.wChairID == myChairId then
                        --隐藏按钮
                        self:setButtonVisible(false)
                    end
                elseif cbIsDeal[i] == 2 then
                    str = "选择了拒绝"
                end

                -- 昵称
                local sznick = userItem.szNickName or ""
                self.m_tabUserNick[i]:setString(sznick)

                -- 头像
                local head = PopupInfoHead:createNormal(userItem, 88)
                head:enableInfoPop(false)
                self.m_tabUserHead[i]:addChild(head)
                self.m_tabUserHead[i]:setVisible(true)

                -- 动作
                self.m_tabUserState[i]:setString(str)

                self.nOrder[i] = i
              
            end
        end
    end

    self:showLayer(remainTime)
end

function DismissLayer:requestReply(replyUser, bAgree)
    local str = (bAgree and "选择了同意" or "选择了拒绝")
    local order = self.nOrder[replyUser.wChairID + 1]
    if order then
        -- 昵称
        local sznick = replyUser.szNickName or ""
        self.m_tabUserNick[order]:setString(sznick)

        -- 头像
        local head = PopupInfoHead:createNormal(replyUser, 88)
        head:enableInfoPop(false)
        self.m_tabUserHead[order]:addChild(head)

        -- 动作
        self.m_tabUserState[order]:setString(str)
    else
        print("Error, request reply")
    end
end

function DismissLayer:setButtonVisible(bVisible)
    self.m_btnAgree:setVisible(bVisible)
    self.m_btnDisagree:setVisible(bVisible)
end

return DismissLayer
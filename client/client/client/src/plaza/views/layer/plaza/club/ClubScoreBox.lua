
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local ClubShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubShareLayer")
local ClubRecordDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRecordDetailLayer")
local ClubScorePickUp = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubScorePickUp")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubScoreBox = class("ClubScoreBox", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_TIQU",
    "BTN_UP",
    "BTN_DOWN",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubScoreBox:ctor( scene, param, level )
    local this = self

    ClubScoreBox.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_scoreBox.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene
    self._param = param
    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo
    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)



    local btn = spbg:getChildByName("btn_ok")
    btn:setTag(TAG_ENUM.BTN_TIQU)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)    


    local btn = spbg:getChildByName("Button_up")
    btn:setTag(TAG_ENUM.BTN_UP)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true) 

    self.m_btnUp = btn


    local btn = spbg:getChildByName("Button_down")
    btn:setTag(TAG_ENUM.BTN_DOWN)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true) 

    self.m_btnDown = btn

    self.m_lastScore = spbg:getChildByName("num1")
    self.m_point = spbg:getChildByName("num2")
    self.m_todayScore = spbg:getChildByName("num3")

    self.m_pageTxt =  spbg:getChildByName("page")
    self.m_pageTxt:setString("0/0")

    self.m_recordList={}
    for i=1,5 do
        self.m_recordList[i] = spbg:getChildByName("Image"..i)
    end


    if bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_TEAMMEMBER) ~= 0 or 
        self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MEMBER then

        self.m_point:setVisible(false)
        self.m_todayScore:setPositionX(295)
    end
    
    self.m_page = 1

    self.m_recordData = {}
    self.m_recordData.list = {}

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:onTransitionInFinish()

end

function ClubScoreBox:onExit()
    ClubScoreBox.super.onExit(self)
end

-- 按键监听
function ClubScoreBox:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        --self:scaleTransitionOut(self.m_spBg)
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_UP then
        if self.m_page > 1 then
            self.m_page = self.m_page - 1
            self:onRefreshInfo()
        end

    elseif tag == TAG_ENUM.BTN_DOWN then
        if self.m_page < math.ceil(#self.m_recordData.list/5) then
            self.m_page = self.m_page + 1
            self:onRefreshInfo()
        end
    elseif tag == TAG_ENUM.BTN_TIQU then
        local pa = ClubScorePickUp:create(self, {todayScore = self.m_recordData.lAwardScore}, ClubScorePickUp.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    end
end

function ClubScoreBox:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubScoreBox:onTransitionInFinish()
    print("===================999999999")
        ClubFrame:getInstance().m_tabScoreBox = nil
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendQueryOpenBox(ClubFrame:getInstance().m_nCurrentClub)
       
end

function ClubScoreBox:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubScoreBox:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubScoreBox:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end


function ClubScoreBox:onRefreshInfo(data)

    self.m_recordData = data or self.m_recordData

    self.m_lastScore:setString(self.m_recordData.lAwardScore.."")
    self.m_point:setString(self.m_recordData.wPoint.."")
    self.m_todayScore:setString(self.m_recordData.lTodayGetScore.."")

    self.m_pageTxt:setString(self.m_page.."/"..(math.ceil(#self.m_recordData.list/5) == 0 and 1 or math.ceil(#self.m_recordData.list/5)))

    for i=1,5 do
        local curIndex = (self.m_page -1)*5 +i
        if curIndex <= #self.m_recordData.list then
            local curData = self.m_recordData.list[curIndex]

            self.m_recordList[i]:getChildByName("time"):setString(curData.DateTime.wYear.."/"..curData.DateTime.wMonth.."/"..curData.DateTime.wDay.."  "..curData.DateTime.wHour..":"..curData.DateTime.wMinute)
            self.m_recordList[i]:getChildByName("num1"):setString(curData.lScore.."")
            self.m_recordList[i]:getChildByName("num2"):setString(curData.lBeforeScore.."")
            self.m_recordList[i]:getChildByName("num3"):setString(curData.lAfterScore.."")
            self.m_recordList[i]:setVisible(true)
        else
            self.m_recordList[i]:setVisible(false)
        end

    end

    if #self.m_recordData.list <= 0 then
        self.m_btnUp:setVisible(false)
        self.m_btnDown:setVisible(false)
        self.m_pageTxt:setVisible(false)
    else
        self.m_btnUp:setVisible(true)
        self.m_btnDown:setVisible(true)
        self.m_pageTxt:setVisible(true)       
    end

end





return ClubScoreBox
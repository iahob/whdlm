--
-- Author: zhong
-- Date: 2017-05-31 14:33:18
--
-- 包含(约战ID输入界面、 带房间列表的ID输入界面)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PrivateIdInputLayer = class("PrivateIdInputLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- ui
local PrivateRecordLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateRecordLayer")          -- 记录列表
local VideoMarkListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "video.VideoMarkListLayer")            -- 战绩
-- ui

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_CLEAR",            -- 清除
    "BTN_DEL",              -- 删除
    "BTN_RECORD",           -- 记录
    "BTN_VIDEO",            -- 回放
    "BTN_SURE",             -- 确定
    "IMAGE_EDIT",           -- 编辑按钮
    "BTN_JOINGAME",         -- 加入

    "BTN_NUM_BEGIN",        -- 数字
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

-- 约战ID输入界面()
local PrivateIdEditLayer = class("PrivateIdEditLayer", TransitionLayer)
PrivateIdInputLayer.PrivateIdEditLayer = PrivateIdEditLayer
function PrivateIdEditLayer:ctor( scene, param, level )
    PrivateIdEditLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("private/PrivateIdEditLayer.csb", self)

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 查看
    btn = spbg:getChildByName("btn_sure")
    btn:setTag(TAG_ENUM.BTN_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setEnabled(false)
    btn:setPressedActionEnabled(true)
    self.m_btnSearch = btn

    self.m_szRoomId = ""
    -- 编辑区
    self.m_editBox = nil
    self.m_bEditInput = false
    -- 编辑内容
    self.m_txtEdit = spbg:getChildByName("txt_edit")
    self.m_txtEdit:setString("")
    -- 编辑按钮
    btn = spbg:getChildByName("editbg")
    btn:setTag(TAG_ENUM.IMAGE_EDIT)
    btn:addTouchEventListener( touchFunC )
    self.m_imageEdit = btn
    self.m_imageEdit:setEnabled(false)

    self:scaleShakeTransitionIn(spbg)
end

function PrivateIdEditLayer:onButtonClickedEvent( tag, ref )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_SURE == tag then
        local roomid = self.m_editBox:getText()
        if string.len(roomid) == 6 and nil ~= tonumber(roomid) then
            self.m_szRoomId = roomid
            self:scaleTransitionOut(self.m_spBg)
        end
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        self.m_editBox:setVisible(true)
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    end
end

function PrivateIdEditLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
        self.m_txtEdit:setVisible(false)
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "return" then
        editbox:setVisible(false)
        self.m_txtEdit:setVisible(true)
        self.m_txtEdit:setString(src)
        self.m_imageEdit:setEnabled(true)
    end
end

function PrivateIdEditLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PrivateIdEditLayer:onTransitionInFinish()
    -- 编辑框
    local tmp = self.m_imageEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 100, tmp:getContentSize().height - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入房间ID")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 30)
            --:setFontColor(cc.c4b(0,0,0,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_spBg:addChild(editbox)         
    self.m_editBox = editbox
    self.m_editBox:setVisible(false)
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_imageEdit:setEnabled(true)
    if type(self._param) == "string" and string.len(self._param) == 6 then
        self.m_editBox:setText(self._param)
        self:onButtonClickedEvent(TAG_ENUM.IMAGE_EDIT, self.m_imageEdit)
    end
    self.m_btnSearch:setEnabled(true)
end

function PrivateIdEditLayer:onTransitionOutFinish()
    if string.len(self.m_szRoomId) == 6 then
        PriRoom:getInstance():getNetFrame():onSearchRoom(self.m_szRoomId)
    end
    self:removeFromParent()
end

-- 带房间列表的ID输入界面(二级弹窗)
function PrivateIdInputLayer:ctor( scene, param, level )
    PrivateIdInputLayer.super.ctor( self, scene, level )

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("private/PrivateIdInputLayer.csb", self)
    self.m_rootLayer = rootLayer

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 记录
    btn = spbg:getChildByName("btn_record")
    btn:setTag(TAG_ENUM.BTN_RECORD)
    btn:addTouchEventListener( touchFunC )

    -- 回放
    btn = spbg:getChildByName("btn_video")
    btn:setTag(TAG_ENUM.BTN_VIDEO)
    btn:addTouchEventListener( touchFunC )


    -- 输入id
    self.m_szRoomId = ""
    self.m_tabTxtRoomId = {}
    for i = 1, 6 do
        local txt = spbg:getChildByName("txt_id_" .. i)
        txt:setString("")
        self.m_tabTxtRoomId[i] = txt
    end
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onNumButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 数字按钮
    for i = 1, 10 do
        local tag = i - 1
        local btn = spbg:getChildByName("btn_num_" .. tag)
        btn:setTag(tag)
        btn:addTouchEventListener(btncallback)
    end

    -- 清空
    btn = spbg:getChildByName("btn_clear")
    btn:setTag(TAG_ENUM.BTN_CLEAR)
    btn:addTouchEventListener( touchFunC )

    -- 删除
    btn = spbg:getChildByName("btn_del")
    btn:setTag(TAG_ENUM.BTN_DEL)
    btn:addTouchEventListener( touchFunC )

    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function PrivateIdInputLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_CLEAR == tag then
        self.m_szRoomId = ""
        for k,v in pairs(self.m_tabTxtRoomId) do
            v:setString("")
        end
    elseif TAG_ENUM.BTN_DEL == tag then
        local roomid = self.m_szRoomId
        local len = string.len(roomid)
        if len > 0 then
            roomid = string.sub(roomid, 1, len - 1)
        end
        len = string.len(roomid)
        for k,v in pairs(self.m_tabTxtRoomId) do
            if k <= len then
                local cur = string.sub(roomid, k, k)
                v:setString(cur)
            else
                v:setString("")
            end
        end
        self.m_szRoomId = roomid
    elseif TAG_ENUM.BTN_RECORD == tag then
        local pr = PrivateRecordLayer:create( self._scene, {from = PrivateRecordLayer.FROM_ENUM.FROM_IDINPUT}, self:getLevel() + 1)
        self._scene:addPopLayer(pr)    

        -- 移除自己
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_VIDEO == tag then
        
        local ml = VideoMarkListLayer:create(self._scene, {}, VideoMarkListLayer.SECOND_LEVEL)
        self._scene:addPopLayer(ml)

        -- 移除自己
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_JOINGAME == tag then
        local roomid = self.m_szRoomId
        if string.len(roomid) == 6 then     
            PriRoom:getInstance():showPopWait()
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
                self:scaleTransitionOut(self.m_spBg)
            end)))
        else
            showToast(self, "请输入房间ID", 2)
        end
    end
end

-- 数字按钮
function PrivateIdInputLayer:onNumButtonClickedEvent( tag, sender )
    local roomid = self.m_szRoomId
    local len = string.len(roomid)
    if len < 6 then
        roomid = roomid .. tag
        len = string.len(roomid)
        for k,v in pairs(self.m_tabTxtRoomId) do
            if k <= len then
                local cur = string.sub(roomid, k, k)
                v:setString(cur)
            else
                v:setString("")
            end
        end
    end
    self.m_szRoomId = roomid

    if string.len(roomid) == 6 then     
        PriRoom:getInstance():showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
            --self:scaleTransitionOut(self.m_spBg)
            PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(self.m_szRoomId))
        end)))
    end
end

function PrivateIdInputLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PrivateIdInputLayer:onTransitionOutFinish()
    self:removeFromParent()
end

return PrivateIdInputLayer
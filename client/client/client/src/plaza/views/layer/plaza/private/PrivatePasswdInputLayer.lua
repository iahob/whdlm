--
-- Author: zhong
-- Date: 2017-06-20 17:33:15
--
-- 物品购买(三级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PrivatePasswdInputLayer = class("PrivatePasswdInputLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_CLEAR",            -- 清除
    "BTN_DEL",              -- 删除
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function PrivatePasswdInputLayer:ctor( scene, param, level )
    PrivatePasswdInputLayer.super.ctor( self, scene, param, level )
    local this = self

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("private/PrivatePasswdInputLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 输入内容
    self.m_txtPasswd = spbg:getChildByName("txt_passwd")
    self.m_txtPasswd:setString("")

    -- 输入id
    self.m_szTextID = ""
    self.m_tabTxtId = {}
    for i = 1, 6 do
        local txt = spbg:getChildByName("txt_id_" .. i)
        txt:setString("")
        self.m_tabTxtId[i] = txt
    end
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onNumButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 数字按钮
    for i = 1, 10 do
        local tag = i - 1
        local btn = spbg:getChildByName("btn_num_" .. tag)
        btn:setTag(tag)
        btn:addTouchEventListener(btncallback)
    end

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    
    -- 清空
    btn = spbg:getChildByName("btn_clear")
    btn:setTag(TAG_ENUM.BTN_CLEAR)
    btn:addTouchEventListener( touchFunC )

    -- 删除
    btn = spbg:getChildByName("btn_del")
    btn:setTag(TAG_ENUM.BTN_DEL)
    btn:addTouchEventListener( touchFunC )

    self:scaleShakeTransitionIn(spbg)
end

function PrivatePasswdInputLayer:onButtonClickedEvent(tag, ref)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        if nil ~= self._scene then
            showToast(self._scene, "密码为空, 请重新输入!", 2)
            self._scene:onKeyBack()
        end
    elseif TAG_ENUM.BTN_CLEAR == tag then
        self.m_szTextID = ""
        for k,v in pairs(self.m_tabTxtId) do
            v:setString("")
        end
    elseif TAG_ENUM.BTN_DEL == tag then
        local sztxt = self.m_szTextID
        local len = string.len(sztxt)
        if len > 0 then
            sztxt = string.sub(sztxt, 1, len - 1)
        end
        len = string.len(sztxt)
        for k,v in pairs(self.m_tabTxtId) do
            if k <= len then
                local cur = string.sub(sztxt, k, k)
                v:setString(cur)
            else
                v:setString("")
            end
        end
        self.m_szTextID = sztxt
    end
end

-- 数字按钮
function PrivatePasswdInputLayer:onNumButtonClickedEvent( tag, sender )
    local sztxt = self.m_szTextID
    local len = string.len(sztxt)
    if len < 6 then
        sztxt = sztxt .. tag
        len = string.len(sztxt)
        for k,v in pairs(self.m_tabTxtId) do
            if k <= len then
                local cur = string.sub(sztxt, k, k)
                v:setString(cur)
            else
                v:setString("")
            end
        end
    end
    self.m_szTextID = sztxt

    if string.len(sztxt) == 6 then        
        PriRoom:getInstance():showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
            self:scaleTransitionOut(self.m_spBg)
        end)))
    end
end

function PrivatePasswdInputLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PrivatePasswdInputLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function PrivatePasswdInputLayer:onTransitionInFinish()
    -- 设置密码
    local passwd = PriRoom:getInstance().m_szPasswd
    if type(passwd) == "string" and string.len(passwd) == 6 then
        self.m_szTextID = passwd
        for k,v in pairs(self.m_tabTxtId) do
            local cur = string.sub(passwd, k, k)
            v:setString(cur)
        end

        PriRoom:getInstance():showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
            self:scaleTransitionOut(self.m_spBg)
        end)))
    end
    -- 重置密码
    PriRoom:getInstance().m_szPasswd = ""
end

function PrivatePasswdInputLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function PrivatePasswdInputLayer:onTransitionOutFinish()
    if string.len(self.m_szTextID) == 6 then
        PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_INPUTPASSWD
        PriRoom:getInstance():getNetFrame():sendEnterPrivateGame(tonumber(self.m_szTextID))
    end
    self:removeFromParent()
end

return PrivatePasswdInputLayer
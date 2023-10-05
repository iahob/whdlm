
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local SuperRobLayer = class("SuperRobLayer", cc.Layer)

function SuperRobLayer:ctor(parent, callback, consume)
    local csbNode = ExternalFun.loadCSB("SuperRobLayer.csb", self)
    self.csbNode = csbNode

    cc.funAddChildListener(csbNode, "img_bg_1/Button_close", function() self:onClickClose() end)
    cc.funAddChildListener(csbNode, "img_bg_1/Button_ok", function() self:onClickOK() end)
    cc.funAddChildListener(csbNode, "img_bg_1/Button_cancel", function() self:onClickCancel() end)
    cc.funAddChildListener(csbNode, "Panel_1", function() self:onClickClose() end)

    self.callback = callback
    csbNode:getChildByName("img_bg_1"):getChildByName("Text_1"):setString("超级抢庄将花费".. consume .. "，确定抢庄？")
end

function SuperRobLayer:destroy()
    if self.csbNode and self.csbNode:getParent() then
        self.csbNode:removeFromParent()
    end
end

function SuperRobLayer:onClickClose()
    self:destroy()
end

function SuperRobLayer:onClickOK()
    if type(self.callback) == "function" then
        self.callback(true)
    end
    self:destroy()
end

function SuperRobLayer:onClickCancel()
    if type(self.callback) == "function" then
        self.callback(false)
    end
    self:destroy()
end

return SuperRobLayer
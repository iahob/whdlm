local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local uiBase = appdf.req(appdf.EXTERNAL_SRC .. "uiBase")
local SpineObj = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.SpineObj")
local SchedulerPool = appdf.req(appdf.EXTERNAL_SRC .. "SchedulerPool")

local uiResult = uiBase:ctor{
    zorder = 1000,
    needMoveInAction = true,
}

function uiResult:ctor(...)
    local o = {}
    self.__index = self
    setmetatable(o, self)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiResult:init(parent)
    local csbNode = cc.CSLoader:createNode("LayerResult.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode

    cc.funAddChildListener(csbNode, "Panel_1", function() self:onClickContinue() end)
    cc.funAddChildListener(csbNode, "Sprite_1/Button_1", function() self:onClickContinue() end)

    self:refresLeftCount(8)

    self._schedulPool = SchedulerPool:create()
    self._schedulPool:loopCall(function() return self:refresLeftCount() end, 1)
end

function uiResult:onClickContinue()
    self:destroy()
end

function uiResult:refresh(num_zhuang, num_self, bankName, isSelfBank)
    local obj
    if num_self >= 0 then
        obj = SpineObj:create("animate/sp_win/skeleton.json", "animate/sp_win/skeleton.atlas", false)
        obj:setPosition(display.width*0.5, 370)
    else
        obj = SpineObj:create("animate/sp_lose/skeleton.json", "animate/sp_lose/skeleton.atlas", false)
        obj:setPosition(display.width*0.5, 400)
    end
    obj:setCompleteCallback(function() obj:destroy() end)
    self.widget:addChild(obj)

    local lab_zhuang = cc.find(self.widget, "Sprite_1/Text_zhuang")
    local lab_self = cc.find(self.widget, "Sprite_1/Text_self")
    if num_zhuang >= 0 then
        num_zhuang = "+" .. num_zhuang
    end
    if num_self >= 0 then
        num_self = "+" .. num_self
    end
    lab_zhuang:setString(string.format("%s:   %s", bankName, num_zhuang))
    lab_self:setString("自己:   " .. num_self)

end

function uiResult:refresLeftCount(left)
    self._left = self._left or left
    if self._left -1 > 0 then
        local lab_self = cc.find(self.widget, "Sprite_1/Button_1/Text_1")
        lab_self:setString(string.format("%s秒", self._left-1))
        self._left = self._left - 1
    else
        self._left = nil
        self:destroy()
        return false
    end
    return true
end

function uiResult:clear()
end

function uiResult:destroy()
    if self._schedulPool then
        self._schedulPool:clearAll()
        self._schedulPool = nil
    end

    if self.widget and not tolua.isnull(self.widget) then
        self.widget:removeFromParent()
        self:onHide()
    end
    self.widget = nil
    self._left = nil
end

function uiResult:onExit()
    if self._schedulPool then
        self._schedulPool:clearAll()
        self._schedulPool = nil
    end
    self._left = nil
end

return uiResult
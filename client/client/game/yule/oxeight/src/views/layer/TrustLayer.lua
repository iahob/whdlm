--
-- Author: luo
-- Date: 2016年12月30日 15:18:32
--
--设置界面
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.CMD_Game")
local TrustLayer = class("TrustLayer", cc.Layer)

local callString = {
    "任意牌型抢一倍",
    "牛八以上抢2倍",
    "牛九以上抢3倍",
    "牛牛以上抢4倍",
}
local betString = {
    "牛八以上最大金额下注",
    "牛九以上最大金额下注",
    "牛牛以上最大金额下注",
}
local CALL_SELEC_TAG = 20
local BET_SELEC_TAG = 30
--构造
function TrustLayer:ctor( parent )
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("game/TrustLayer.csb", self)
    self.m_spBg = self._csbNode:getChildByName("bg_2")

    self._callSelect = {1,1}   -- 叫庄选项，第二个表示下拉框选择
    self._betSelect = {1,1}   -- 下注选项，第二个表示下拉框选择


    local touchCallFun = function(sender, eventType)
        -- if eventType == ccui.TouchEventType.ended then
            local tag = sender:getTag()
            self:onTouchEvent(tag)
        -- else
        --     if sender:isSelected() then
        --         sender:setSelected(true)

        --     end
        -- end
    end

    self.callCheckBox = {}
    local check1 = self._csbNode:getChildByName("CheckBox_1"):setSelected(true)
    self.callCheckBox[1] = check1
        check1:setTag(1)
        check1:addTouchEventListener(touchCallFun)
    local checkTxt1 = self._csbNode:getChildByName("Text_1")
        checkTxt1:setTag(1)
        checkTxt1:addTouchEventListener(touchCallFun)

    local check2 = self._csbNode:getChildByName("CheckBox_1_0"):setSelected(false)
    self.callCheckBox[2] = check2
        check2:setTag(2)
        check2:addTouchEventListener(touchCallFun)


    self.betCheckBox = {}
    for i = 1, 3 do
        local check = self._csbNode:getChildByName("CheckBox_2_"..i)
        table.insert(self.betCheckBox, check)
        check:setTag(10 + i)
        check:addTouchEventListener(touchCallFun)
        if i == 1 then
            check:setSelected(true)
        else
            check:setSelected(false)
        end
        if i ~= 3 then
            txt = self._csbNode:getChildByName("Text_2_"..i)
            txt:setTag(10 + i)
            txt:addTouchEventListener(touchCallFun)
        end
    end

    self.popCallCheck = {}
    local Panel_1 = self._csbNode:getChildByName("Panel_1")
    for i = 1, 4 do
        local check = Panel_1:getChildByName("select_"..i)
        table.insert(self.popCallCheck, check)
        check:setTag(CALL_SELEC_TAG + i)
        check:addTouchEventListener(touchCallFun)
        check:setSelected(false)
        local txt = Panel_1:getChildByName("txt_"..i)
        txt:setTag(CALL_SELEC_TAG + i)
        txt:addTouchEventListener(touchCallFun)
    end

    self.popBetCheck = {}
    local Panel_2 = self._csbNode:getChildByName("Panel_2")
    for i = 1, 3 do
        local check = Panel_2:getChildByName("select_"..i)
        table.insert(self.popBetCheck, check)
        check:setTag(BET_SELEC_TAG + i)
        check:addTouchEventListener(touchCallFun)
        check:setSelected(false)

        local txt = Panel_2:getChildByName("txt_"..i)
        txt:setTag(BET_SELEC_TAG + i)
        txt:addTouchEventListener(touchCallFun)

    end

    Panel_1:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_1:setVisible(false)
        end
    end)

    Panel_2:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_2:setVisible(false)
        end
    end)

    local Image_1 = self._csbNode:getChildByName("Image_1")
    Image_1:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_1:setVisible(true)
        end
    end)

    local Image_2 = self._csbNode:getChildByName("Image_2")
    Image_2:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_2:setVisible(true)
        end
    end)

    local Button_pop1 = self._csbNode:getChildByName("Button_pop1")
    local Panel_1_1 = self._csbNode:getChildByName("Panel_1_1")
    Panel_1_1:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_1_1:setVisible(false)
        end
    end)
    Button_pop1:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_1_1:setVisible(true)
        end
    end)

    local Button_pop2 = self._csbNode:getChildByName("Button_pop2")
    local Panel_2_1 = self._csbNode:getChildByName("Panel_2_1")
    Panel_2_1:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_2_1:setVisible(false)
        end
    end)
    Button_pop2:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            Panel_2_1:setVisible(true)
        end
    end)

    local Button_Close = self._csbNode:getChildByName("Button_Close")
    Button_Close:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            self:setVisible(false)
        end
    end)

    local Button_Trust = self._csbNode:getChildByName("Button_Trust")
    Button_Trust:addTouchEventListener(function ( sender,eventType )
        if eventType == ccui.TouchEventType.ended then
            parent:onButtonClickedEvent(125)   -- 延用之前的自动逻辑
            self:setVisible(false)
        end
    end)

    -- 非发四等五时，隐藏部分
    if parent._scene.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then 
        -- parent._scene.m_tabPrivateRoomConfig.sendCardType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then
    else
        self:hideUI()
    end
end

function TrustLayer:onTouchEvent(tag)
    if tag >= 11 and tag <= 13 then
        print("tag============",tag)
        for k, check in ipairs(self.betCheckBox) do
            if k == tag - 10 then
                check:setSelected(true)
                self._betSelect[1] = k
                if k == 3 then
                    self.popBetCheck[self._betSelect[2]]:setSelected(true)
                else
                    self.popBetCheck[self._betSelect[2]]:setSelected(false)
                end
            else
                check:setSelected(false)
            end
        end
    elseif tag == 1 or tag == 2 then
        for k, check in ipairs(self.callCheckBox) do
            if k == tag then
                check:setSelected(true)
                self._callSelect[1] = k
                if k == 2 then
                    self.popCallCheck[self._callSelect[2]]:setSelected(true)
                else
                    self.popCallCheck[self._callSelect[2]]:setSelected(false)
                end
            else
                check:setSelected(false)
            end
        end
    elseif tag >= 21 and tag <= 24 then
        local Panel_1 = self._csbNode:getChildByName("Panel_1")
        Panel_1:setVisible(false)
        local text = self._csbNode:getChildByName("Image_1"):getChildByName("Text_5")

        for k, check in ipairs(self.popCallCheck) do
            if k == tag - 20 then
                check:setSelected(true)
                self._callSelect[2] = k
                text:setString(callString[k])
                self:onTouchEvent(2)
            else
                check:setSelected(false)
            end
        end

    elseif tag >= 31 and tag <= 33 then
        local Panel_2 = self._csbNode:getChildByName("Panel_2")
        Panel_2:setVisible(false)
        local text = self._csbNode:getChildByName("Image_2"):getChildByName("Text_5")

        for k, check in ipairs(self.popBetCheck) do
            if k == tag - 30 then
                check:setSelected(true)
                self._betSelect[2] = k
                text:setString(betString[k])
                self:onTouchEvent(13)
            else
                check:setSelected(false)
            end
        end
    end

end

function TrustLayer:hideUI()
    self._csbNode:getChildByName("CheckBox_2_3"):setVisible(false)
    self._csbNode:getChildByName("Image_2"):setVisible(false)
    self._csbNode:getChildByName("Button_pop2"):setVisible(false)
    local Panel_1 = self._csbNode:getChildByName("Panel_1")
    for i = 2, 4 do
        local check = Panel_1:getChildByName("select_"..i):setVisible(false)
        local txt = Panel_1:getChildByName("txt_"..i):setVisible(false)
    end
end
--
function TrustLayer:showLayer( bIsAuto )
    self:setVisible(true)
    local Button_Trust = self._csbNode:getChildByName("Button_Trust")    
    local font = Button_Trust:getChildByName("trustFont")
    if bIsAuto then
        font:setTexture("game/cancelTrust.png")
    else
        font:setTexture("game/trustFont.png")
    end
end
--触摸回调
function TrustLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function TrustLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:setVisible(false)
    end
end

return TrustLayer
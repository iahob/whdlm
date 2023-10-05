local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local HelpLayer = class("HelpLayer", cc.Layer)

HelpLayer.BOX = 1


HelpLayer.BT_CLOSE = 10
HelpLayer.TAG_MASK = 11

function HelpLayer:ctor(verstr)
    --加载csb资源
     --注册触摸事件
    local csbNode = ExternalFun.loadCSB("help_res/LayerHelp.csb", self)

    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onBtnClick(sender:getTag(), sender)
        end
    end

    local mask = csbNode:getChildByName("mask")  
    mask:setTouchEnabled(true)
    mask:setTag(HelpLayer.TAG_MASK)
    mask:addTouchEventListener(btnEvent)

    --关闭按钮
    local btn = csbNode:getChildByName("btn_close")
    btn:setTag(HelpLayer.BT_CLOSE)
    btn:addTouchEventListener(btnEvent)

    local boxListener = function (sender,eventType)
        self:onBoxSelectedEvent(sender:getTag(),sender)
    end

    --复选框
    self.tabBox = {}
    for i=1,4 do
        local check = csbNode:getChildByName("CheckBox_".. i)
                        :setTag(HelpLayer.BOX + i)
        check:addEventListener(boxListener)
        self.tabBox[HelpLayer.BOX + i] = check
        if i == 1 then 
            check:setSelected(true) 
            self.selectTab = HelpLayer.BOX + 1 
        end
    end

    --左侧玩法显示
    self.scorllView = csbNode:getChildByName("ScrollView")

    local spTips = cc.Sprite:create("help_res/sp_content1.png")
    self.scorllView:addChild(spTips)
    self.scorllView:setInnerContainerSize(spTips:getContentSize())

    spTips:setAnchorPoint(cc.p(0.5, 0))
    spTips:setPosition(cc.p(375, 0))


end

function HelpLayer:onBtnClick( tag, sender )
    if HelpLayer.BT_CLOSE == tag or HelpLayer.TAG_MASK == tag then
        ExternalFun.playClickEffect()
        self:hideLayer()
    end
end

function HelpLayer:onBoxSelectedEvent( tag, sender )
    print("onBoxSelectedEvent---------->", tag, self.selectTab)
    if self.selectTab == tag then
        sender:setSelected(true)
        return
    end
    sender:setSelected(true)
    self.selectTab = tag

    for k,v in pairs(self.tabBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    
    --更新对应的玩法 
    local spTips = cc.Sprite:create("help_res/sp_content" .. tag - HelpLayer.BOX .. ".png")
    self.scorllView:removeAllChildren()
    self.scorllView:addChild(spTips)

    if spTips:getContentSize().height < 550  then 
        self.scorllView:setInnerContainerSize(cc.size(750, 550))
    else
        self.scorllView:setInnerContainerSize(spTips:getContentSize())
    end
    
    spTips:setAnchorPoint(cc.p(0.5, 1))
    spTips:setPosition(cc.p(375, self.scorllView:getInnerContainerSize().height))
end


function HelpLayer:showLayer()
    self:setVisible(true)
end

function HelpLayer:hideLayer()
    self:setVisible(false)
end

return HelpLayer
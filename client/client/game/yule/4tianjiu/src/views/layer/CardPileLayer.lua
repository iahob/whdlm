local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local CardNode = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.views.layer.CardNode")
local CardPileLayer = class("CardPileLayer", cc.Layer)

CardPileLayer.BT_MASK = 1
CardPileLayer.BT_ClOSE = 1

--构造
function CardPileLayer:ctor(viewParent)
    self.m_parent = viewParent;
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("card/CardPileLayer.csb", self);
    csbNode:setPosition(0,0);
    self._csbNode = csbNode;

    --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(CardPileLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    local ScrollView = csbNode:getChildByName("ScrollView");
    self.scrollViewContent = ScrollView;


    -- local userlist = {1};
    -- self:refreshList( userlist );
    self.m_tabList = {};
    for i = 1, 32 do
        if self.m_parent.cardHeapArr[i] and self.m_parent.cardHeapArr[i] > 0 then
            table.insert(self.m_tabList, self.m_parent.cardHeapArr[i]);
        end
    end
    self:showRankInfo();
end

--展示排行榜
function CardPileLayer:showRankInfo(userlist)
    if self.m_tabList == nil then
        print("未获取到排行信息");
        return;
    end

    self.scrollViewContent:removeAllChildren();
    --获取列表个数
    local listNum = #self.m_tabList;
    
    local num = 0;
    for i = 1, 4 do
        for j = 1, 8 do
            num = (i-1)*8 + (j-1);
            
            if num < listNum then
                local CardNode = CardNode:create();
                CardNode:setPosition(cc.p(-20 + 73*j, 600 - i * 130));
                self.scrollViewContent:addChild(CardNode);
                CardNode:setScale(0.75);
                CardNode:setCardValue(self.m_tabList[num+1]);
            end
        end
    end
end

function CardPileLayer:onExit()
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:removeEventListener(self.listener)
end

function CardPileLayer:onEnterTransitionFinish()
    self:registerTouch()
end

function CardPileLayer:registerTouch()
    -- local function onTouchBegan( touch, event )
    --     return self:isVisible()
    -- end

    -- local function onTouchEnded( touch, event )
    --     local pos = touch:getLocation();
    --     local m_spBg = self.m_spBg
    --     pos = m_spBg:convertToNodeSpace(pos)
    --     local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    --     if false == cc.rectContainsPoint(rec, pos) then
    --         self:setVisible(false)
    --     end        
    -- end

    -- local listener = cc.EventListenerTouchOneByOne:create();
    -- listener:setSwallowTouches(true)
    -- self.listener = listener;
    -- listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN );
    -- listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED );
    -- local eventDispatcher = self:getEventDispatcher();
    -- eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self);
end

function CardPileLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function CardPileLayer:onButtonClickedEvent(tag, sender)
    if CardPileLayer.BT_MASK == tag then
        self:removeFromParent();
    elseif CardPileLayer.BT_ClOSE == tag then
        self:removeFromParent();
    end
end

--触摸回调
function CardPileLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        -- local fadeout = cc.FadeOut:create(0.5)
        -- local call = cc.CallFunc:create(function( )
        --     self:removeFromParent()
        -- end)
        -- self._csbNode:runAction(cc.Sequence:create(fadeout, call))

        self:removeFromParent();
    end
end

function CardPileLayer:onTouchEnded(touch, event)
end

return CardPileLayer
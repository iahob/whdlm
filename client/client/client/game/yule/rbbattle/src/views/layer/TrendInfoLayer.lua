local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local TrendInfoLayer = class("TrendInfoLayer", cc.Layer)

TrendInfoLayer.BT_LEFT = 1;
TrendInfoLayer.BT_MASK = 2;

local invalidValue = 255;

--构造
function TrendInfoLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("trendinfo/TrendInfoLayer.csb", self)
    self._csbNode:setPosition(0,0);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(TrendInfoLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    
end

function TrendInfoLayer:trendInfoFresh(recordData)
    local rbHistory = recordData.cbRBHistory;
    local pokerType = recordData.cbPokerType[1];
    local wlHistory = recordData.cbRB_WL_History[1];

    local percent_1 = self._csbNode:getChildByName("percent_1");
    local percent_2 = self._csbNode:getChildByName("percent_2");

    local node_result = self._csbNode:getChildByName("node_result");

    local redWinNum = 0;
    local blackWinNum = 0;
    for i = 1, #wlHistory do
        local result = node_result:getChildByName("result_" .. i);
        -- result:setTexture("maingame/bottom_8.png");

        if wlHistory[i] == 1 then  -- 红方胜
            redWinNum = redWinNum + 1;

            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/history_red.png");
            result:setSpriteFrame(frame);
        elseif wlHistory[i] == 2 then  -- 黑方胜
            blackWinNum = blackWinNum + 1;

            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/history_black.png");
            result:setSpriteFrame(frame);
        else
            result:setVisible(false);
        end
        
    end

    local redPercent = redWinNum/(redWinNum + blackWinNum)*100;
    local blackPercent = blackWinNum/(redWinNum + blackWinNum)*100;
    percent_1:setString(blackPercent);
    percent_2:setString(redPercent);


    local node_card_road = self._csbNode:getChildByName("node_card_road");

    for i = 1, 20 do
        for j = 1, 6 do
            local card = node_card_road:getChildByName("card_" .. i .. "_" .. j);
            
            local item = rbHistory[i][j];

            if item == 1 then  -- 红方胜
                -- card:setTexture("trendinfo/icon_red.png");
                local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/history_red.png");
                card:setSpriteFrame(frame);
                card:setScale(0.8);
            elseif item == 2 then  -- 黑方胜
                -- card:setTexture("trendinfo/icon_black.png");
                local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game/game/history_black.png");
                card:setSpriteFrame(frame);
                card:setScale(0.8);
            else
                card:setVisible(false);
            end

        end
    end

    local node_type_trend = self._csbNode:getChildByName("node_type_trend");

    for i = 1, 20 do
        local item = pokerType[i];

        local recordTypeStr = "game/cardtype/record_type_1_1.png";
        local typeBg = "game/game/box_balck.png";
        if item == 1 then  
            recordTypeStr = "game/cardtype/record_type_1_1.png";
        elseif item == 2 then --2到8
            recordTypeStr = "game/cardtype/record_type_1_2.png";
        elseif item == 3 then -- 9到a
            recordTypeStr = "game/cardtype/record_type_2_2.png";
            typeBg = "game/game/box_yellow.png";
        elseif item > 3 then
            recordTypeStr = "game/cardtype/record_type_2_" .. item-1 .. ".png";
            typeBg = "game/game/box_yellow.png";
        else
            trend_bg:setVisible(false);
            card_type:setVisible(false);
        end

        local trend_bg = node_type_trend:getChildByName("trend_bg_" .. i);
        -- trend_bg:setTexture("maingame/bottom_8.png");
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(typeBg);
        trend_bg:setSpriteFrame(frame);

        local card_type = node_type_trend:getChildByName("card_type_" .. i);
        -- card_type:setTexture("maingame/bottom_8.png");

        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(recordTypeStr);
        card_type:setSpriteFrame(frame);
    end
    local new_sign = node_type_trend:getChildByName("new_sign");
end

function TrendInfoLayer:showLayer( var )
    self:setVisible(var)
end

--按钮事件
function TrendInfoLayer:onButtonClickedEvent(tag,ref)
    if tag == TrendInfoLayer.BT_MASK then
        self.m_parent.trendInfoLayer = nil;
        self:removeFromParent();
    end
end

--触摸回调
function TrendInfoLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self.m_parent.trendInfoLayer = nil;
        self:removeFromParent();
    end
end

function TrendInfoLayer:onTouchEnded(touch, event)

end

return TrendInfoLayer
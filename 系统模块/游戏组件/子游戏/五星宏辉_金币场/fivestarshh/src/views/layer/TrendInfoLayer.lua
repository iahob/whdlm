local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.fivestarshh.src";
local cmd = appdf.req(module_pre..".models.CMD_Game")

local TrendInfoLayer = class("TrendInfoLayer", cc.Layer)

TrendInfoLayer.BT_LEFT = 1;
TrendInfoLayer.BT_MASK = 2;

local invalidValue = 255;

--构造
function TrendInfoLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("ludan/WallBill.csb", self)
    self._csbNode:setPosition(0,0);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    -- --遮罩
    -- local btn = self._csbNode:getChildByName("Panel_1");
    -- btn:setTag(TrendInfoLayer.BT_MASK);
    -- btn:addTouchEventListener(btnEvent);

    --关闭
    local btn_close = self._csbNode:getChildByName("btn_close");
    btn_close:setTag(TrendInfoLayer.BT_MASK);
    btn_close:addTouchEventListener(btnEvent);

    local node_settle = self._csbNode:getChildByName("node_settle");
    self.node_settle = node_settle;

    --
    local node_ludan = self._csbNode:getChildByName("node_ludan");
    self.node_ludan = node_ludan;

    local node_record = self._csbNode:getChildByName("node_record");
    self.node_record = node_record;
    
end
-- 通过牌值获取花色
function TrendInfoLayer:getCardColorByCard(cardValue)
    if cardValue >= 1 and cardValue <=13 then
        return cmd.TYPE_1; --黑桃
    elseif cardValue >= 17 and cardValue <=29 then
        return cmd.TYPE_2; --红桃
    elseif cardValue >= 33 and cardValue <=45 then
        return cmd.TYPE_3; --梅花
    elseif cardValue >= 49 and cardValue <=61 then
        return cmd.TYPE_4; --方块
    elseif cardValue == 78 then
        return cmd.TYPE_5; --小王
    elseif cardValue == 79 then
        return cmd.TYPE_6; --大王
    end
end
function TrendInfoLayer:freshExchangeRecordShow()
    local node_settle = self.node_settle;

    local wheel_num = node_settle:getChildByName("wheel_num");
    local game_num = node_settle:getChildByName("game_num");

    local game_num_1 = node_settle:getChildByName("game_num_1");
    local percent_1 = node_settle:getChildByName("percent_1");
     local ludanData = self.m_parent.cbLuDanBigData;
    local count = 0
    local colorType = {0,0,0,0,0}
    for i = 1, #ludanData do
        local cardValue = ludanData[i];
        if cardValue > 0 then
            count = count+1

            local cardType = self:getCardColorByCard(cardValue);
            colorType[cardType] = colorType[cardType]+1
        end
    end
    local total = 0
    local nOpenColorCount = self.m_parent.nOpenColorCount;
    for i = 1, 5 do

        total = nOpenColorCount[i] + total
    end
    self.m_parent.nGameCount = count
    local nGameCount = total%100+1;
   
    local wheelNum = math.floor(total/100) + 1;

    wheel_num:setString(wheelNum);
    game_num:setString(nGameCount);
    if nGameCount > 1000 then
        game_num:setScale(0.9);
    end

     if isExchange then -- 是否切换到概率
        percent_1:setString("100%");
         record_num:setScale(0.8);
     else
        game_num_1:setString(self.m_parent.nGameCount);
         game_num_1:setScale(1.0);
     end


    for i = 1, 5 do
        local game_num = node_settle:getChildByName("game_num_" .. i+1);
        local percent = node_settle:getChildByName("percent_" .. i+1);

        local index = self.m_parent:getIndexByViewId(i);
        local openColorCount = colorType[index];
        
        local per = 0;
        if 0 ~= self.m_parent.nGameCount then
            per = (openColorCount / count) * 100;
        end

        -- if isExchange then -- 是否切换到概率
            percent:setString(string.format("%.1f%%", per));
            -- game_num:setScale(0.8);
        -- else
            game_num:setString(colorType[i]);
            -- game_num:setScale(1.0);
        -- end
    end
end

function TrendInfoLayer:trendInfoFresh()
    self:freshExchangeRecordShow();

    local ludanNum = self.m_parent.cbLuDanBigCount;
    local ludanData = self.m_parent.cbLuDanBigData;

    local beginX = 295;
    local beginY = 505;

    local disX = 80;
    local disY = 42;
    local posX = 30;
    local posX1 = 12;

    self.node_record:removeAllChildren();

    for i = 1, #ludanData do
        local cardValue = ludanData[i];
        if cardValue > 0 then
        local cardType = self.m_parent:getCardColorByCard(cardValue);

        local lineNum = math.mod(i-1,10);
        local rowNum = math.floor((i-1)/10);

        --花色
        local cardColor =nil;
        local str = string.format("record_type_%d.png", cardType)
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
        if nil ~= frame then
            cardColor = cc.Sprite:createWithSpriteFrame(frame);
        end
        cardColor:setPosition(beginX + lineNum * disX, beginY - rowNum * disY);
        self.node_record:addChild(cardColor);

        if cardType == cmd.TYPE_5 then
            cardColor:setPosition(beginX + lineNum * disX + posX1, beginY - rowNum * disY);
        elseif cardType == cmd.TYPE_6 then
            cardColor:setPosition(beginX + lineNum * disX + posX1, beginY - rowNum * disY);
        else
            --牌值
            local cardIcon = cc.Sprite:create("card/" .. cmd.ServerCardValueSprite[cardValue] .. ".png");
            cardIcon:setPosition(beginX + lineNum * disX + posX, beginY - rowNum * disY);
            cardIcon:setScale(0.45);
            self.node_record:addChild(cardIcon);
        end

        end
    end
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
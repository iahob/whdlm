local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local PlayerListLayer = class("PlayerListLayer", cc.Layer)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

PlayerListLayer.BT_MASK = 1

--构造
function PlayerListLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("playerlist/PlayerListLayer.csb", self)
    self._csbNode:setPosition(0,0);

    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(PlayerListLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    --
    local player_num = self._csbNode:getChildByName("player_num");
    player_num:setString(0);
   
    local panel = self._csbNode:getChildByName("panel");

    self.m_tabList = {};
    -- self._listView = cc.TableView:create(cc.size(600,280))
    self._listView = cc.TableView:create(panel:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_BOTTOMUP)  
    self._listView:setPosition(panel:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self._csbNode)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    -- self._listView:reloadData()
end

function PlayerListLayer:freshPlayersData(playersData)
    self.playersData = playersData;

    local playerNum = self.playersData.nPlayerCount;

    local player_num = self._csbNode:getChildByName("player_num");
    player_num:setString(playerNum or 0);

    
end

function PlayerListLayer:refreshList( userlist )
    -- self:setVisible(true)
    self.m_userlist = userlist;

    self:updateList(list);
end

function PlayerListLayer:updateList(list)
    
    self.m_tabList = {};
    local chairIdList = self.playersData.wChairID[1];

    for i = 1, #chairIdList do
        local chairId = chairIdList[i];
        if chairId ~= 255 then

            for j = 1, #self.m_userlist do
                if chairId == self.m_userlist[j].wChairID then
                    table.insert(self.m_tabList, chairId);
                end
            end
            
        end
    end

    if 0 < #self.m_tabList then
        self._listView:reloadData();
    end
end

-- 子视图大小
function PlayerListLayer:cellSizeForTable(view, idx)
    return view:getViewSize().width, 88;
end

function PlayerListLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    idx = #self.m_tabList - idx - 1;
    local iteminfo = self.m_tabList[idx + 1]
    if nil == iteminfo then
        return cell 
    end

    local csNode = ExternalFun.loadCSB("playerlist/PlayerListItem.csb", cell)
    csNode:setPosition(view:getViewSize().width * 0.5, 88 * 0.5)
    csNode:setName("csbNode") 

    if iteminfo then
        local index = idx + 1;
        local jetton = self.playersData.lBetRank20Jetton[1][index];
        local score = self.playersData.lBetRank20Score[1][index];
        local winCount = self.playersData.nWinCount[1][index];
        local wChairID = self.playersData.wChairID[1][index];

        local roleItem = self.m_userlist[1];
        for i = 1, #self.m_userlist do
            if wChairID == self.m_userlist[i].wChairID then
                roleItem = self.m_userlist[i];
            end
        end

        -- local roleItem = self.m_userlist[wChairID];
        -- dump(roleItem,"-------------roleItem---------------",6)

        -- print("-------index---------score-------",index, score)


        local item_title = csNode:getChildByName("item_title");

        local item_rank = csNode:getChildByName("item_rank");

        if index == 1 then
            item_title:setTexture("playerlist/title_first.png");
            item_rank:setVisible(false);
        else
            item_title:setTexture("playerlist/title_common.png");
            item_rank:setString(index);
        end

        local head_icon = csNode:getChildByName("head_icon");
        -- 创建头像
        local head = HeadSprite:createNormal(roleItem, head_icon:getContentSize().width)
        -- print("--------------------GlobalUserItem.tabAccountInfo-----------", GlobalUserItem.tabAccountInfo)
        if nil ~= head then
            -- head:setPosition(role_icon:getPosition())
            head:setPosition(cc.p(head_icon:getContentSize().width/2,head_icon:getContentSize().height/2))
            head_icon:addChild(head, 1)
        end

        local role_name = csNode:getChildByName("role_name");
        
        local name = string.EllipsisByConfig(roleItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 30));
        role_name:setString(name);
        
        local role_gold = csNode:getChildByName("role_gold");
        local icon = role_gold:getChildByName("role_gold_icon");
        
        local txt_bet = csNode:getChildByName("txt_bet");
        -- txt_bet:setString("下注：" .. 1.956 .. "亿");
        local txt_win = csNode:getChildByName("txt_win");
        txt_win:setString("获胜：" .. winCount .. "局");

        -- local score = 650000000;
        local scoreStr = "";
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/rich_wan.png")
        if score < 10000 then
            scoreStr = score;
            icon:setVisible(false);
        elseif score >=10000 and score < 100000000 then
            scoreStr = string.format("%.1f", score/10000)
            icon:setVisible(true);
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/rich_wan.png")
        else
            scoreStr = string.format("%.1f", score/100000000)
            icon:setVisible(true);
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/rich_yi.png")
        end
        
        if nil ~= frame then
            icon:setSpriteFrame(frame);
        end
        role_gold:setString(scoreStr);
        

        local jettonStr = "";
        if jetton < 10000 then
            jettonStr = jetton;
        elseif jetton >=10000 and jetton < 100000000 then
            jettonStr = string.format("%.1f", jetton/10000)
            jettonStr = jettonStr .. "万";
        else
            jettonStr = string.format("%.1f", jetton/100000000)
            jettonStr = jettonStr .. "亿";
        end

        -- print("------jetton---------jettonStr----------------",jetton , jettonStr)
        txt_bet:setString("下注：" .. jettonStr);

        return cell
    end
    return cell
end

-- function PlayerListLayer:tableCellTouched(view, cell)
--     local  csbNode = cell:getChildByName("csbNode")
--     if nil ~= csbNode then
--         local head = csbNode:getChildByName("head")
--         if nil ~= head then
--             head:onTouchHead()
--         end
--     end
-- end

-- 子视图数目
function PlayerListLayer:numberOfCellsInTableView(view)
    return #self.m_tabList
end

function PlayerListLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function PlayerListLayer:onButtonClickedEvent(tag, sender)
   
    if PlayerListLayer.BT_MASK == tag then
        self.m_parent.playerListLayer = nil;
        self:removeFromParent();
    end
end

--触摸回调
function PlayerListLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self.m_parent.playerListLayer = nil;
        self:removeFromParent();
    end
end

function PlayerListLayer:onTouchEnded(touch, event)

end

return PlayerListLayer
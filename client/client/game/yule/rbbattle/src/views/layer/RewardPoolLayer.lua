local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local RewardPoolLayer = class("RewardPoolLayer", cc.Layer)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

RewardPoolLayer.BT_MASK = 1;

--构造
function RewardPoolLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("rewardpool/RewardPoolLayer.csb", self)
    self._csbNode:setPosition(0,0)  

   
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(RewardPoolLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    local rewardpool_num = self._csbNode:getChildByName("rewardpool_num");

    local node_reward = self._csbNode:getChildByName("node_reward");
    node_reward:setVisible(false);
    local reward_type = node_reward:getChildByName("reward_type");
    local reward_num = node_reward:getChildByName("reward_num");
    local reward_time = node_reward:getChildByName("reward_time");

    local panel = node_reward:getChildByName("panel");
    self.m_fTwo = panel:getContentSize().height / 2;

    
    --  self.m_tabList = {}

    -- self.m_tabList = {1,2,3,4,5,6,2,3,4,5,6,2,3,4,5,6,8}
    -- self.m_nListCount =  math.ceil(#self.m_tabList/2);
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

function RewardPoolLayer:rewardPoolFresh(rewardData)
    self.rewardData = rewardData;

    local openType = rewardData.cbOpenType;

    local rewardpool_num = self._csbNode:getChildByName("rewardpool_num");
    local node_reward = self._csbNode:getChildByName("node_reward");
    local reward_type = node_reward:getChildByName("reward_type");
    local reward_num = node_reward:getChildByName("reward_num");
    local num_icon = node_reward:getChildByName("num_icon");
    local reward_time = node_reward:getChildByName("reward_time");
    local reward_time_1 = node_reward:getChildByName("reward_time_1");

    rewardpool_num:setString(rewardData.lpoolScore or 0);

    if openType == 0 then
        node_reward:setVisible(false);
    else
        node_reward:setVisible(true);

        local recordTypeStr = "game/cardtype/type_1_1.png";
        if openType == 1 then  
            recordTypeStr = "game/cardtype/type_1_1.png";
        elseif openType == 2 then --2到8
            recordTypeStr = "game/cardtype/type_1_2.png";
        elseif openType == 3 then -- 9到a
            recordTypeStr = "game/cardtype/type_2_2.png";
        elseif openType > 3 then
            recordTypeStr = "game/cardtype/type_2_" .. openType-1 .. ".png";
        else
        end
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(recordTypeStr);
        if frame then
            reward_type:setSpriteFrame(frame);
        end

        local nWinScore = rewardData.lTotalReward;
        local scoreStr = "";
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_wan.png")
        if nWinScore < 10000 then
            scoreStr = nWinScore;
            num_icon:setVisible(false);
        elseif nWinScore >=10000 and nWinScore < 100000000 then
            scoreStr = string.format("%.1f", nWinScore/10000)
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_wan.png")
            num_icon:setVisible(true);
        else
            scoreStr = string.format("%.1f", nWinScore/100000000)
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_yi.png")
            num_icon:setVisible(true);
        end
        
        if nil ~= frame then
            num_icon:setSpriteFrame(frame);
        end
        reward_num:setString(scoreStr);
        

        -- reward_num:setString(rewardData.lTotalReward);

        local openTime = rewardData.nOpenTime[1];

        reward_time:setString(openTime[1] .. "/" .. openTime[2] .. "/" .. openTime[3]);
        reward_time_1:setString(openTime[4] .. ":" .. openTime[5] .. ":" .. openTime[6]);



        self:updateList();
    end 

    
end

function RewardPoolLayer:updateList()
    
    self.m_tabList = {}

    local chairIdList = self.rewardData.dwUserID[1];

    for i = 1, #chairIdList do
        if chairIdList[i] ~= 255 then
            table.insert(self.m_tabList, i);
        end
    end

    -- dump(self.m_tabList,"--------self.m_tabList----------",6)
    self.m_nListCount =  math.ceil(#self.m_tabList/2);

    if 0 < #self.m_tabList then
        self._listView:reloadData();
    end
end

-- 子视图大小
function RewardPoolLayer:cellSizeForTable(view, idx)
    return view:getViewSize().width, 92;
end

function RewardPoolLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    idx = self.m_nListCount - (idx + 1)
    idx = idx * 2 + 1
    -- CMD_MB_S_QueryVideoInfoResult
    -- 第一个cell
    local iteminfo = self.m_tabList[idx]
    if nil == iteminfo then
        return cell 
    end

    if nil ~= iteminfo then
        local csb = self:createRecordItem(idx, cell)
        local imagecell = csb:getChildByName("bottom")
        local imagesize = imagecell:getContentSize()
        csb:setPosition(view:getViewSize().width * 0.5 - imagesize.width * 0.5 - 25, self.m_fTwo * 0.5)

        -- imagecell:setSwallowTouches(false)
        -- imagecell.info = iteminfo
        -- imagecell:addTouchEventListener(self.m_itemFunC)
    end

    -- 第二个cell
    iteminfo = self.m_tabList[idx + 1]
    if nil ~= iteminfo then
        local csb = self:createRecordItem(idx+1, cell)
        local imagecell = csb:getChildByName("bottom")
        local imagesize = imagecell:getContentSize()
        csb:setPosition(view:getViewSize().width * 0.5 + imagesize.width * 0.5 + 25, self.m_fTwo * 0.5)

        -- imagecell:setSwallowTouches(false)
        -- imagecell.info = iteminfo
        -- imagecell:addTouchEventListener(self.m_itemFunC)
    end

    return cell
end

function RewardPoolLayer:createRecordItem(idx, parent)
    local csb = ExternalFun.loadCSB("rewardpool/RewardPoolItem.csb", parent)

    local index = self.m_tabList[idx];
    local dwUserID = self.rewardData.dwUserID[1][index];
    local dwCustomID = self.rewardData.dwCustomID[1][index];
    local nWinScore = self.rewardData.nWinScore[1][index];
    local wFaceID = self.rewardData.wFaceID[1][index];
    local cbGender = self.rewardData.cbGender[1][index];
    local cbWinName = self.rewardData.cbWinName[1][index];

    if nil ~= parent then
        -- 房号
        local head_icon = csb:getChildByName("head_icon");
        -- txtID:setString(string.format("%06d", tabData.dwPersonalRoomID))

        local roleItem = {};
        roleItem.dwUserID = dwUserID;
        roleItem.dwCustomID = dwUserID;
        roleItem.wFaceID = wFaceID;
        -- 创建头像
        local head = HeadSprite:createNormal(roleItem, head_icon:getContentSize().width);
        if nil ~= head then
            -- head:setPosition(role_icon:getPosition())
            head:setPosition(cc.p(head_icon:getContentSize().width/2,head_icon:getContentSize().height/2));
            head_icon:addChild(head, 1);
        end

        local role_sex_icon = csb:getChildByName("role_sex_icon");
        if cbGender == 0 then
            role_sex_icon:setTexture("headinfo/icon_woman.png");
        else
            role_sex_icon:setTexture("headinfo/icon_man.png");
        end
        -- role_sex_icon:setTexture("playerlist/title_first.png");

        local role_name = csb:getChildByName("role_name");
        
        local name = string.EllipsisByConfig(cbWinName,120,string.getConfig("fonts/round_body.ttf" , 24));
        role_name:setString(name);

        local reward_num = csb:getChildByName("reward_num");
        local num_icon = csb:getChildByName("num_icon");

        local scoreStr = "";
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_wan.png");
        if nWinScore < 10000 then
            scoreStr = nWinScore;
            num_icon:setVisible(false);
        elseif nWinScore >=10000 and nWinScore < 100000000 then
            scoreStr = string.format("%.1f", nWinScore/10000);
            num_icon:setVisible(true);
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_wan.png");
        else
            scoreStr = string.format("%.1f", nWinScore/100000000);
            num_icon:setVisible(true);
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("font/font/self_bet_yi.png");
        end
        
        if nil ~= frame then
            num_icon:setSpriteFrame(frame);
        end
        reward_num:setString(scoreStr);
    end

    return csb
end

-- 子视图数目
function RewardPoolLayer:numberOfCellsInTableView(view)
    return self.m_nListCount;
    -- return 10;
end

function RewardPoolLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function RewardPoolLayer:onButtonClickedEvent(tag, sender)
    
    if RewardPoolLayer.BT_MASK == tag then
        self:removeFromParent();
    end
end

--触摸回调
function RewardPoolLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
       self:removeFromParent();
    end
end

function RewardPoolLayer:onTouchEnded(touch, event)

end

return RewardPoolLayer
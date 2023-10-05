local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")
local RecordLayer = class("RecordLayer", cc.Layer)

RecordLayer.BT_MASK = 1
RecordLayer.BT_ClOSE = 2
RecordLayer.BT_DISMISS = 3

--构造
function RecordLayer:ctor(viewParent)
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("record/ZhanjiLayer.csb", self);
    csbNode:setPosition(0,0);
    self._csbNode = csbNode;

    self.m_parent = viewParent;

    --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(RecordLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --遮罩
    local btn_close = self._csbNode:getChildByName("btn_close");
    btn_close:setTag(RecordLayer.BT_ClOSE);
    btn_close:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("sp_bg");
    self.m_spBg = sp_bg;

    local btn_close = self._csbNode:getChildByName("btn_close");

    --玩法
    local wanfa = self._csbNode:getChildByName("wanfa");
    
    self.wanfa = wanfa;

    local btn_dismiss = self.wanfa:getChildByName("btn_dismiss");
    btn_dismiss:setTag(RecordLayer.BT_DISMISS);
    btn_dismiss:addTouchEventListener(btnEvent);

    if GlobalUserItem.bPrivateGoldRoom then
        btn_dismiss:setVisible(false);
    end
    self:showWanfa();

    --战绩流水
    local node_title = self._csbNode:getChildByName("node_title");
    
    self.node_title = node_title;

    local content = self._csbNode:getChildByName("content");
    
    self.content = content;

    --切换按钮
    local tag_wanfa,tag_liushui=1,2;
    local check_wanfa = self._csbNode:getChildByName("CheckBox_1");
    check_wanfa:setTag(tag_wanfa);
    local check_liushui = self._csbNode:getChildByName("CheckBox_2");
    check_liushui:setTag(tag_liushui);
    function onSelectEvent(sender,eventType)
        if self.m_parent._scene.m_bLookOn then
            showToast(self.RoomInfoLayer, "观战中不可使用该功能!", 1)
            return
        end

        local tag=sender:getTag()
        check_wanfa:setSelected(tag==tag_wanfa)
        check_liushui:setSelected(tag==tag_liushui)
        self.wanfa:setVisible(tag==tag_wanfa)
        self.content:setVisible(tag==tag_liushui)
        self.node_title:setVisible(tag==tag_liushui)
    end
    check_wanfa:addEventListener(onSelectEvent)
    check_liushui:addEventListener(onSelectEvent)
    check_wanfa:setSelected(true)
    check_liushui:setSelected(false)

    self.content:setVisible(false);
    self.node_title:setVisible(false);
    self.wanfa:setVisible(true);

    local playerNum =0
    self.playerNum = 0;
    for i=1,cmd.GAME_PLAYER do --chairid
        local viewid=self.m_parent._scene:SwitchViewChairID(i-1)
        if self.m_parent._scene.m_tabUserItem[i] then
            self.playerNum=self.playerNum+1
            -- local strNickname = string.EllipsisByConfig(self.m_parent._scene.m_tabUserItem[viewid].szNickName, scoreWidth-5, 
            --                                 string.getConfig("fonts/round_body.ttf", (viewid==cmd.MY_VIEWID and 22 or 20)))
            local strNickname = self.m_parent._scene.m_tabUserItem[i].szNickName;
            node_title:getChildByName("player_"..i):setString(strNickname)
            if viewid==cmd.MY_VIEWID then  
                node_title:getChildByName("player_"..i):setTextColor(cc.c4b(0xbe,0x93,0xdb,0xff))
            end
            -- node_title:getChildByName("player_"..i):setPositionX((j-0.5)*scoreWidth)
        else
            node_title:getChildByName("player_"..i):setString("")
        end

        if GlobalUserItem.bPrivateGoldRoom then
            if viewid ~= cmd.MY_VIEWID then
                node_title:getChildByName("player_"..i):setString("")
            else
                -- node_title:getChildByName("jushu"):setPositionX(650/4)
                -- node_title:getChildByName("player_"..i):setPositionX(650*3/4)
            end
        end
    end


    --用户列表
    local m_tableView = cc.TableView:create(content:getContentSize())
    -- local m_tableView = cc.TableView:create(cc.size(1000,500))
    m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    m_tableView:setPosition(cc.p(0,0))
    m_tableView:setDelegate()
    m_tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    content:addChild(m_tableView)
    self.m_tableView = m_tableView;
    -- content:removeFromParent()

    self.listNum = 0;
    self.m_tabList = {{},{},{},{}};
    -- local userlist = {1};
    self:refreshList(self.m_tabList, self.listNum);
end

function RecordLayer:refreshList(m_tabList, nCount)
    self:setVisible(true);
    self.m_tabList = m_tabList;
    self.listNum = nCount;
    -- self.listNum = 30;
    self.m_tableView:reloadData();
end

--tableview
function RecordLayer.cellSizeForTable( view, idx )
    -- return g_var(UserItem).getSize()
    return 800, 70;
end

function RecordLayer:numberOfCellsInTableView( view )
    if nil == self.m_tabList then
        return 0
    else
        return self.listNum;
    end
end

function RecordLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    local item = nil
    local num = self.listNum - idx

    if nil == cell then
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    -- local userBar = self:createUser(self.m_tabList[idx+1], num)
    -- if not userBar then return cell end
    -- userBar:setAnchorPoint(0.5, 0.5)
    -- userBar:setPosition(cc.p(907/2,45))
    -- cell:addChild(userBar)
    local lDetailScore = self.m_tabList;
    local csNode = ExternalFun.loadCSB("record/Node_Zhanji.csb", cell);
    csNode:setPosition(35, 70 * 0.5 -10);
    -- csNode:setName("csbNode") 

    local game_num = csNode:getChildByName("game_num");
    game_num:setString(num .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit);
    
    if GlobalUserItem.bPrivateGoldRoom then
        game_num:setString(num);
    end

    for i = 1, cmd.GAME_PLAYER do
        local viewid=self.m_parent._scene:SwitchViewChairID(i-1)
        
        local player_score = csNode:getChildByName("player_score_" .. i);
        player_score:setVisible(true);
        player_score:setString(lDetailScore[i][num]);
        if i > self.playerNum then
            player_score:setVisible(false);
        end
        if GlobalUserItem.bPrivateGoldRoom then
            if viewid ~= cmd.MY_VIEWID then
                player_score:setString("");
            end
        end
        
    end

    return cell
end

function RecordLayer:onExit()
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:removeEventListener(self.listener)
end

--玩法显示
function RecordLayer:showWanfa()
    local node_wanfa_1 = self.wanfa:getChildByName("node_wanfa_1");
    local node_wanfa_2 = self.wanfa:getChildByName("node_wanfa_2");
    local node_wanfa_3 = self.wanfa:getChildByName("node_wanfa_3");
    local node_wanfa_4 = self.wanfa:getChildByName("node_wanfa_4");
    local node_wanfa_5 = self.wanfa:getChildByName("node_wanfa_5");

    --游戏模式 1大牌九，2小牌九，3加锅牌九
    local gameMode = {false, false, false};
    if self.m_parent.bJiaGuaPaiJiu then
        gameMode[3] = true;
    end
    if self.m_parent.bDaPaiJiu then
        gameMode[1] = true;
    else
        gameMode[2] = true;
    end
    
    for i=1, 3 do
        local check = node_wanfa_1:getChildByName("check_" .. i);
        check:setSelected(false);
        if gameMode[i] then
            check:setSelected(true);
        end
    end

    --牌型选择
    local cardType = {false, false, false, false};
    if self.m_parent.bZhaDan then
        cardType[1] = true;
    end
    if self.m_parent.bGuiZi then
        cardType[2] = true;
    end
    if self.m_parent.bTianJiuWang then
        cardType[3] = true;
    end
    if self.m_parent.bDiJiuNiangNiang then
        cardType[4] = true;
    end

    for i=1, 4 do
        local check = node_wanfa_2:getChildByName("check_" .. i);
        check:setSelected(false);
        if cardType[i] then
            check:setSelected(true);
        end
    end

    --道的选择 道的选择2,3
    local dao = {false, false};
    if self.m_parent.cbBetDaoShu == 2 then
        dao[1] = true;
    elseif self.m_parent.cbBetDaoShu == 3 then
        dao[2] = true;
    end
    
    for i=1, 2 do
        local check = node_wanfa_3:getChildByName("check_" .. i);
        check:setSelected(false);
        if dao[i] then
            check:setSelected(true);
        end
    end

    --庄的选择 0轮庄、1抢庄、2霸王庄
    local bankerType = {false, false, false};
    if self.m_parent.cbBankerMode == 0 then
        bankerType[1] = true;
    elseif self.m_parent.cbBankerMode == 1 then
        bankerType[2] = true;
    elseif self.m_parent.cbBankerMode == 2 then
        bankerType[3] = true;
    end
    
    for i=1, 3 do
        local check = node_wanfa_4:getChildByName("check_" .. i);
        check:setSelected(false);
        if bankerType[i] then
            check:setSelected(true);
        end
    end

    --庄家亮牌  0不亮、2亮2张、3亮3张
    local bankerShow = {false, false, false};
    if self.m_parent.cbBankerOpenCardCount == 0 then
        bankerShow[1] = true;
    elseif self.m_parent.cbBankerOpenCardCount == 2 then
        bankerShow[2] = true;
    elseif self.m_parent.cbBankerOpenCardCount == 3 then
        bankerShow[3] = true;
    end
    
    for i=1, 3 do
        local check = node_wanfa_5:getChildByName("check_" .. i);
        check:setSelected(false);
        if bankerShow[i] then
            check:setSelected(true);
        end
    end   
end

function RecordLayer:showLayer( var )
    self:setVisible(var)
end

function RecordLayer:hideLayer()
    self:removeFromParent();
    if self.m_parent and self.m_parent.zhanjiLayer and not tolua.isnull(self.m_parent.zhanjiLayer) then
        self.m_parent.zhanjiLayer = nil;
    end
    
end

--按钮回调方法
function RecordLayer:onButtonClickedEvent(tag, sender)
    if RecordLayer.BT_MASK == tag then
        -- self:hideLayer();
    elseif RecordLayer.BT_ClOSE == tag then
        self:hideLayer();
    elseif RecordLayer.BT_DISMISS == tag then
        self:hideLayer();
        PriRoom:getInstance():queryDismissRoom();
    end
end

--触摸回调
function RecordLayer:onTouchBegan(touch, event)
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

        self:hideLayer();
    end
end

function RecordLayer:onTouchEnded(touch, event)

end

return RecordLayer
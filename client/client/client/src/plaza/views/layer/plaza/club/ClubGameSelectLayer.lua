
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")




local ClubGameSelectLayer = class("ClubGameSelectLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",             -- 显示切换游戏
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_BACK",             -- 退出
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubGameSelectLayer:ctor( scene, param, level )
    local this = self

    ClubGameSelectLayer.super.ctor( self, scene, param, level )

    self.param = param

    dump(self.param, "ClubGameSelectLayer mmp", 5)

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_selectGame.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end


    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )
    mask:setTouchEnabled(true)
    self.m_mask = mask

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    self.m_tabGame = ClubFrame:getInstance().m_selectGameList
    

    dump(self.m_tabGame,"=====================1")
    local content = spbg:getChildByName("Panel_2")
    self._listView = cc.TableView:create(content:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)   
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setAnchorPoint(cc.p(0,0)) 
    self._listView:setPosition(cc.p(0,0))
    self._listView:setDelegate()
    self._listView:addTo(content)
    --self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --tmp:removeFromParent()


    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listView then
                return 
            end

            -- -- widget add 到 tableview 会在非可见区域触发touch事件
            -- local toViewPoint = ref:convertToNodeSpace(endPos)
            -- local rec = cc.rect(ref:getPositionX(), ref:getPositionY(), ref:getContentSize().width, ref:getContentSize().height)
            -- if true == cc.rectContainsPoint(rec, toViewPoint) then
                print(ref:getTag().."=============")
            --end  

            local gameInfo = self.m_tabGame[ref:getTag()]
            if nil == gameInfo then
                print("消息为空")
                return
            end
                        local runScene = cc.Director:getInstance():getRunningScene()


            if ClubFrame:getInstance().m_cbClubCurPlayMode ==0 then
                if not PriRoom:getInstance():isCurrentGameOpenPriScore(tonumber(gameInfo._KindID)) then
                    if nil ~= runScene then
                        showToast(runScene, "该游戏未开启积分房间！", 2)
                    end
                    return
                end
            else
                if not PriRoom:getInstance():isCurrentGameOpenPriGold(tonumber(gameInfo._KindID)) then
                    if nil ~= runScene then
                        showToast(runScene, "该游戏未开启勋章房间！", 2)
                    end
                    return
                end
            end

            --切换游戏
            -- 通知更新    
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
            eventListener.obj = yl.RY_CULB_PRIROOM_CHANGETYPE
            eventListener.msg = gameInfo._KindID 
            eventListener.cbMedalMode = self.param.cbMedalMode
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

            self:removeFromParent()
        end
    end   

    if #self.m_tabGame <= 6 then
        content:removeAllChildren()

        local count = #self.m_tabGame

        for i=1,count do
                local gameInfo = self.m_tabGame[i]
                local filePath = "club/configGame_"..gameInfo._KindID..".png"
                if not cc.FileUtils:getInstance():isFileExist(filePath) then
                    filePath = "club/configGame_200.png"
                end
                local gamecell = ccui.Button:create(filePath,filePath)
                gamecell:setAnchorPoint(cc.p(0.5,0.5))
                gamecell:setPressedActionEnabled(true)
                gamecell:setSwallowTouches(false)
                
                gamecell:setTag(i)
                gamecell:addTo(content)
                gamecell:addTouchEventListener(self.m_itemFunC)
                if count <= 3 then
                    local bpoint = {375,280,185}
                    gamecell:move(bpoint[count]+(i-1)*190 , 205 )
                elseif count == 4 then
                    gamecell:move(270+((i-1)%2)*190 , 295-math.floor((i-1)/2)*176 )
                else
                    gamecell:move(185+((i-1)%3)*190 , 295-math.floor((i-1)/3)*176 )
                end


        end



    else
        self._listView:reloadData()
    end


    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)

end

function ClubGameSelectLayer:onExit()
    ClubGameSelectLayer.super.onExit(self)

end

-- 按键监听
function ClubGameSelectLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.BTN_CLOSE or tag == TAG_ENUM.TAG_MASK then
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_SHOW then
        if self.m_spBg:isVisible() then
            self.m_spBg:setVisible(false)
            self.m_mask:setVisible(false)
            --self.m_mask:setTouchEnabled(false)
        else
            self.m_spBg:setVisible(true)
            --self.m_mask:setTouchEnabled(true)
            self.m_mask:setVisible(true)
        end
    end
end

-- function ClubGameSelectLayer:animationRemove()
--     self:scaleTransitionOut(self.m_spBg)
-- end

-- function ClubGameSelectLayer:onTransitionInFinish()
--     --刷新界面
--     print("onTransitionInFinish")
--     self:onRefreshInfo()
-- end

-- function ClubGameSelectLayer:onTransitionOutFinish()
--     self:removeFromParent()
-- end

-- function ClubGameSelectLayer:onOtherShowEvent()
--     -- if self:isVisible() then
--     --     self:setVisible(false)
--     -- end
-- end

-- function ClubGameSelectLayer:onOtherHideEvent()
--     if not self:isVisible() then
--         self:setVisible(true) 
--     end
--     print("onOtherHideEvent")
--     self:onRefreshInfo()
-- end

-- function ClubGameSelectLayer:onRefreshInfo()
--     print("ClubGameSelectLayer:onRefreshInfo")
-- end

function ClubGameSelectLayer:cellSizeForTable(view, idx)
    return 750 , 176
end

function ClubGameSelectLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end


        for i=1,3 do
            local gameInfo = self.m_tabGame[idx*3 +i]
            if nil == gameInfo then
                return cell
            end
            local filePath = "club/configGame_"..gameInfo._KindID..".png"
            if not cc.FileUtils:getInstance():isFileExist(filePath) then
                filePath = "club/configGame_200.png"
            end
            local gamecell = ccui.Button:create(filePath,filePath)
            gamecell:setAnchorPoint(cc.p(0.5,0.5))
            gamecell:setPressedActionEnabled(true)
            gamecell:setSwallowTouches(false)
            gamecell:move(185+(i-1)*190 , 88 )
            gamecell:setTag(idx*3 +i)
            gamecell:addTo(cell)
            gamecell:addTouchEventListener(self.m_itemFunC)

        end

    -- dump(gameInfo, "gameinfo", 10)

    -- local csb = ExternalFun.loadCSB("club/node_gameCell.csb", cell)
    -- local bg = csb:getChildByName("bg")
    -- csb:setPosition(179, 50)

    -- bg:setSwallowTouches(false)
    -- bg.info = gameInfo
    -- bg:addTouchEventListener(self.m_itemFunC)

    -- local img = csb:getChildByName("Sprite_2")

    -- if  gameInfo._KindID == GlobalUserItem.nCurGameKind then
    --     img:setVisible(true)
    -- else
    --     img:setVisible(false)
    -- end

    -- local txtGameName = csb:getChildByName("txt_name")
    -- if nil ~= txtGameName then
    --     txtGameName:setString(gameInfo._GameName)
    -- end

    return cell

end

-- 子视图数目
function ClubGameSelectLayer:numberOfCellsInTableView(view)

    return math.ceil(#self.m_tabGame/3)
end

-- function ClubGameSelectLayer:tableCellTouched(view, cell)
--     if view ~= self._listView then
--         return
--     end
--     local index = cell:getIdx()
--     --     print("tableCellTouched", index)
--     local gameInfo = self.m_tabGame[index +1]
--     if nil == gameInfo then
--         print("消息为空")
--         return
--     end


--     --切换游戏
--     -- 通知更新    
--     local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
--     eventListener.obj = yl.RY_CULB_PRIROOM_CHANGETYPE
--     eventListener.msg = gameInfo._KindID 
--     cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

 
--     --self._listView:removeFromParent()
--     --self:removeFromParent()
--     -- self.m_spBg:setVisible(false)
--     -- self.m_mask:setVisible(false)
-- end



return ClubGameSelectLayer
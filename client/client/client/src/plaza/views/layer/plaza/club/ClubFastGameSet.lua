
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubFastGameSet = class("ClubFastGameSet", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "BTN_SERCH",             -- 搜索
    "BTN_DATE",             -- 日期
    "BTN_DATEBG",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubFastGameSet:ctor( scene, param, level )
    local this = self

    ClubFastGameSet.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_wanfa.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene

    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myGroupInfo = GlobalUserItem.tabGroupUserInfo


    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    --spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    self.m_configList = param.configlist

    --成员列表
    local tmp = self.m_spBg:getChildByName("content")
    self._listConfigView = cc.TableView:create(tmp:getContentSize())
    self._listConfigView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listConfigView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listConfigView:setPosition(cc.p(0, 0))
    self._listConfigView:setDelegate()
    self._listConfigView:addTo(tmp)
    self._listConfigView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listConfigView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listConfigView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listConfigView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listConfigView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listConfigView:getViewSize().width, self._listConfigView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                this:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end
    self._listConfigView:reloadData()


    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)


end

function ClubFastGameSet:onExit()
    ClubFastGameSet.super.onExit(self)
end

-- 按键监听
function ClubFastGameSet:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        --self:scaleTransitionOut(self.m_spBg)
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_SERCH then
        self.m_editID:touchDownAction(self.m_editID, ccui.TouchEventType.ended)
        self.searchImg:setEnabled(false)
    elseif tag == TAG_ENUM.BTN_DATE then
        self.datePanel:setVisible(true)
    elseif tag == TAG_ENUM.BTN_DATEBG then
        self.datePanel:setVisible(false)
    end
end

function ClubFastGameSet:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubFastGameSet:onTransitionInFinish()
 

end

function ClubFastGameSet:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubFastGameSet:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubFastGameSet:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end


function ClubFastGameSet:cellSizeForTable(view, idx)
    return 761 , 138
end

function ClubFastGameSet:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local config = self.m_configList[idx +1]
    if nil == config then
        return cell
    end


    local csb = ExternalFun.loadCSB("club/node_wanfa.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    

    csb:getChildByName("wanfa"):setString("玩法"..((idx+1)>10 and (idx+1) or "0"..(idx+1)))


    csb:getChildByName("game"):setString(config.szConfigName.."")

    local rule = csb:getChildByName("Panel_1"):getChildByName("rule")
    rule:setString("基本玩法")

    local gameImg = csb:getChildByName("Sprite_2")
        local filePath = "club/configGame_"..config.dwKindID..".png"
        if cc.FileUtils:getInstance():isFileExist(filePath) then
            gameImg:setTexture(filePath)
            gameImg:setScale(0.8)
        end


    local tabRule = self._scene:getGameRule(config.cbGameRule[1], config.dwKindID)
    local str = ExternalFun.getGameNameByKind(config.dwKindID)..","..config.dwDrawCountLimit.."局,"
    if tabRule then
            for j=1, #tabRule do
                if j == #tabRule then
                    str = str ..  tabRule[j]
                else
                    str = str ..  tabRule[j] .. ","
                end
            end
    else
        str=str.."基本玩法"
    end

    rule:setString(ExternalFun.clipText(str,17,true))


    local btn = csb:getChildByName("Button_5")
    btn:addTouchEventListener(self.m_itemFunC)
    btn.config = config

    return cell

end

-- 子视图数目
function ClubFastGameSet:numberOfCellsInTableView(view)
    return #self.m_configList
end

function ClubFastGameSet:onItemButtonClickedEvent( tag, ref )
    local config = ref.config
    if nil == config then
        print("成员信息为空")
        return
    end
    ClubFrame:getInstance():setViewFrame(self._scene)
    ClubFrame:getInstance():sendFastConfig(ClubFrame:getInstance().m_nCurrentClub,config.dwConfigID)
    self:removeFromParent()
end


return ClubFastGameSet
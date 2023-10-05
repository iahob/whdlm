--
-- Author: zhong
-- Date: 2017-10-10 19:07:03
--
-- 流水界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local TurnoverInfoLayer = class("TurnoverInfoLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local TAG_START             = 100
local enumTable = 
{
    "CBT_SCORE_TURNOVER",   -- 金币流水
    "CBT_DIAMOND_TURNOVER", -- 钻石流水
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function TurnoverInfoLayer:ctor(scene, param, level)
    local this = self
    ExternalFun.registerNodeEvent(this)
    TurnoverInfoLayer.super.ctor( self, scene, param, level )

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("usercenter/TurnoverInfoLayer.csb", self)
    self.m_csbNode = csbNode

    -- 钻石
    local tmptxt = csbNode:getChildByName("txt_diamond")
    local clipdiamond = ClipText:createClipText(cc.size(170, 30), ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lDiamond), nil, 26)
    clipdiamond:setAnchorPoint(tmptxt:getAnchorPoint())
    clipdiamond:setPosition(tmptxt:getPosition())
    clipdiamond:setTextColor(cc.c4b(0xff,0xe5,0x89, 255))
    csbNode:addChild(clipdiamond)
    self.m_clipDiamond = clipdiamond
    --self.m_clipDiamond:setString(GlobalUserItem.tabAccountInfo.lDiamond)
    tmptxt:removeFromParent()

    -- 金币
    tmptxt = csbNode:getChildByName("txt_coin")
    local clipscore = ClipText:createClipText(cc.size(170, 30), ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore), nil, 26)
    clipscore:setAnchorPoint(tmptxt:getAnchorPoint())
    clipscore:setPosition(tmptxt:getPosition())
    clipscore:setTextColor(cc.c4b(0xff,0xe5,0x89, 255))
    csbNode:addChild(clipscore)
    self.m_clipScore = clipscore
    --self.m_clipScore:setString(GlobalUserItem.tabAccountInfo.lUserScore)
    tmptxt:removeFromParent()

    -- 提示标题
    --self.m_spTips = csbNode:getChildByName("content_title")
    self.m_spTips3 = csbNode:getChildByName("txt3")
    self.m_spTips5 = csbNode:getChildByName("txt5")

    local checkboxlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    -- 切换
    local cbtbox = csbNode:getChildByName("check_turnover")
    cbtbox:addEventListener(checkboxlistener)
    cbtbox:setSelected(true)
    self.m_checkBox = cbtbox

    self.m_tabList = {} --param
    self.m_nListCount = 0 --#param
    self.m_bEven = false -- 是否偶数列
    -- 列表
    self._listView = nil
end

function TurnoverInfoLayer:onEnterTransitionFinish()
    -- 公告列表
    local tmp = self.m_csbNode:getChildByName("content")
    self._listView = cc.TableView:create(tmp:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.m_csbNode)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tmp:removeFromParent()
    self._listView:reloadData()

    self:onRefreshInfo()
end

function TurnoverInfoLayer:onExit()
    self:dismissPopWait()
end

function TurnoverInfoLayer:onSelectedEvent(tag, sender)
    self:onRefreshInfo()
end

function TurnoverInfoLayer:onRefreshInfo()
    local param = ""
    if self.m_checkBox:isSelected() then
        -- if cc.FileUtils:getInstance():isFileExist("usercenter/ucenter_sp_coinlist_title.png") then
        --     self.m_spTips:setSpriteFrame(cc.Sprite:create("usercenter/ucenter_sp_coinlist_title.png"):getSpriteFrame())
        -- end
        self.m_spTips3:setString("操作前金币")
        self.m_spTips5:setString("操作后金币")
        param = "action=recordtreasuretrade&page=1&size=10&" .. GlobalUserItem:urlUserIdSignParam()
    else
        -- if cc.FileUtils:getInstance():isFileExist("usercenter/ucenter_sp_diamondlist_title.png") then
        --     self.m_spTips:setSpriteFrame(cc.Sprite:create("usercenter/ucenter_sp_diamondlist_title.png"):getSpriteFrame())
        -- end
        self.m_spTips3:setString("操作前钻石")
        self.m_spTips5:setString("操作后钻石")
        param = "action=recorddiamondstrade&page=1&size=10&" .. GlobalUserItem:urlUserIdSignParam()
    end
    self:showPopWait()
    -- 请求列表
    self:doHttpRequest(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param)
end

function TurnoverInfoLayer:onHttpRequestFinish( jstable, jsdata, allparam )
    self:dismissPopWait()
    dump(jstable, "TurnoverInfoLayer:onHttpRequestFinish", 6)
    local msg = nil
    if type(jstable) == "table" then
        local data = jstable["data"]
        dump(jstable, "流水信息", 7)
        msg = jstable["msg"]
        if type(data) == "table" then
            local valid = data["valid"]
            local list = data["list"]
            if valid and type(list) == "table" then
                self.m_tabList = list
                self.m_nListCount = #list
                self.m_bEven = (math.mod(self.m_nListCount ,2) == 0)

                self._listView:reloadData()
            end
        end
    end
    if type(msg) == "string" and "" ~= msg then
        showToast(this, msg, 1)
    end
end

function TurnoverInfoLayer:cellSizeForTable(view, idx)
    return 1110,60
end

function TurnoverInfoLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local info = self.m_tabList[ self.m_nListCount - idx ]
    if type(info) ~= "table" then
        return cell
    end
    local filepath = ""
    local evenIdx = (math.mod(idx ,2) == 0)
    if self.m_bEven then
        -- 偶数列
        if not evenIdx then
            filepath = "usercenter/ucenter_sp_turnoveritemcell.png"
        end
    else
        -- 奇数列
        if evenIdx then
            filepath = "usercenter/ucenter_sp_turnoveritemcell.png"
        end
    end

    -- 背景条
    local itemcell = ccui.ImageView:create(filepath)
    itemcell:setScale9Enabled(true)
    itemcell:setContentSize(cc.size(1110, 60))
    cell:addChild(itemcell)
    itemcell:setPosition(view:getViewSize().width * 0.5, 30)

    -- ID
    local txtid = cc.Label:createWithTTF(GlobalUserItem.tabAccountInfo.dwGameID, "fonts/round_body.ttf", 24)
    itemcell:addChild(txtid)
    txtid:setPosition(74, 30)

    -- time
    local sztime = info["SerialTime"] or ""
    local txttime = cc.Label:createWithTTF(sztime, "fonts/round_body.ttf", 24)
    itemcell:addChild(txttime)
    txttime:setPosition(262, 30)

    -- 操作前数量
    local szbefore = ""
    if self.m_checkBox:isSelected() then
        szbefore = info["BeforeGold"] or ""
    else
        szbefore = info["BeforeDiamond"] or ""
    end
    local txtbefore = ClipText:createClipText(cc.size(185, 30), ExternalFun.numberThousands(szbefore), nil, 24)
    txtbefore:setAnchorPoint(cc.p(0.5, 0.5))
    itemcell:addChild(txtbefore)
    txtbefore:setPosition(451, 30)

    -- 操作数量
    local szchange = ""
    if self.m_checkBox:isSelected() then
        szchange = info["ChangeGold"] or ""
    else
        szchange = info["ChangeDiamond"] or ""
    end
    local txtchange = ClipText:createClipText(cc.size(185, 30), ExternalFun.numberThousands(szchange), nil, 24)
    txtchange:setAnchorPoint(cc.p(0.5, 0.5))
    itemcell:addChild(txtchange)
    txtchange:setPosition(638, 30)

    -- 操作后数量
    local szafter = ""
    if self.m_checkBox:isSelected() then
        szafter = info["AfterGold"] or ""
    else
        szafter = info["AfterDiamond"] or ""
    end
    local txtafter = ClipText:createClipText(cc.size(185, 30), ExternalFun.numberThousands(szafter), nil, 24)
    txtafter:setAnchorPoint(cc.p(0.5, 0.5))
    itemcell:addChild(txtafter)
    txtafter:setPosition(826, 30)

    -- 操作方式
    local szaction = info["TypeName"] or ""
    local txtaction = cc.Label:createWithTTF(szaction, "fonts/round_body.ttf", 24)
    itemcell:addChild(txtaction)
    txtaction:setPosition(1015, 30)

    return cell
end

-- 子视图数目
function TurnoverInfoLayer:numberOfCellsInTableView(view)
    return self.m_nListCount
end

return TurnoverInfoLayer
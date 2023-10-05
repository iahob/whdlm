--
-- Author: zhong
-- Date: 2017-08-10 14:27:22
--
-- 战绩/流水/配置
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local MarkLayer = class("MarkLayer", TransitionLayer)

local TAG_START             = 100
local enumTable = 
{
    "TAG_MASK",             -- 遮罩
    "BTN_CLOSE",            -- 关闭
    "CHECK_CONFIG",         -- 玩法/配置
    "CHECK_MARK",           -- 战绩/流水
    "BTN_DISMISS",          -- 解散
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function MarkLayer:ctor( scene, param, level )
    MarkLayer.super.ctor( self, scene, param, level )

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/MarkLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("spbg")
    self.m_spBg = spbg

    -- 退出
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )

    self.m_tabBox = {}
    local selectlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),sender)
    end
    -- 玩法
    local checkbx = spbg:getChildByName("check_config")
    checkbx:setTag(TAG_ENUM.CHECK_CONFIG)
    checkbx:addEventListener(selectlistener)
    checkbx:setSelected(true)

    self.m_tabBox[TAG_ENUM.CHECK_CONFIG] = checkbx
    -- 战绩
    checkbx = spbg:getChildByName("check_mark")
    checkbx:setTag(TAG_ENUM.CHECK_MARK)
    checkbx:addEventListener(selectlistener)
    checkbx:setSelected(false)
    --checkbx:setEnabled(false)
    self.m_tabBox[TAG_ENUM.CHECK_MARK] = checkbx
    self.m_nSelectIdx = TAG_ENUM.CHECK_CONFIG

    -- 玩法
    self.goldNode = spbg:getChildByName("Node_gold")
    self.jifenNode = spbg:getChildByName("Node_jifen")
    if not GlobalUserItem.bPrivateGoldRoom then
        --机分场
        self.goldNode:setVisible(false) 
        self.jifenNode:setVisible(true)  
        self.m_spConfig = self.jifenNode:getChildByName("sp_configbg"):setVisible(true)
        self.m_spMark = self.jifenNode:getChildByName("sp_markbg"):setVisible(false)
    else
        self.jifenNode:setVisible(false) 
        self.goldNode:setVisible(true) 
        self.m_spConfig = self.goldNode:getChildByName("sp_configbg"):setVisible(true)
        self.m_spMark = self.goldNode:getChildByName("sp_markbg"):setVisible(false)
    end

    -- 底分
    local score = self._param or 0
    self.m_spConfig:getChildByName("text_score"):setString(score .. "分")

    if not GlobalUserItem.bPrivateGoldRoom then
        --局数
        local round = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0
        self.m_spConfig:getChildByName("text_round"):setString(round .. "局")

        --密码
        if not PriRoom:getInstance().m_tabPriData.bRoomPwd then
            self.m_spConfig:getChildByName("text_password"):setString("无需密码")
        else
            self.m_spConfig:getChildByName("text_password"):setString("需要密码")
        end

        --支付方式
            if PriRoom:getInstance().m_tabPriData.cbPayMode == 0 then
                self.m_spConfig:getChildByName("text_pay"):setString("房主支付")
            else
                self.m_spConfig:getChildByName("text_pay"):setString("AA制支付")
            end
    end

    -- 战绩
    self.m_spMark:setVisible(false)

    local nickpanel = self.m_spMark:getChildByName("title_panel")
    -- 局数
    self.m_fRoundPosX = nickpanel:getChildByName("Text_6"):getPositionX()
    self.m_layNickPanel = nickpanel

    self.m_tabClipNick = {}

    if not GlobalUserItem.bPrivateGoldRoom then
        -- 昵称
        for i = 1, 3 do
            local tmp = nickpanel:getChildByName("_text_nick_" .. i)
            local clipnick = ClipText:createClipText(tmp:getContentSize(), "", nil, 24)
            clipnick:setTextColor(cc.c4b(155, 57, 19, 255))
            clipnick:setAnchorPoint(tmp:getAnchorPoint())
            clipnick:setPosition(tmp:getPosition())
            nickpanel:addChild(clipnick)
            self.m_tabClipNick[i] = clipnick
            tmp:removeFromParent()
        end
    else
        --金币场
        local tmp = nickpanel:getChildByName("_text_nick_1" )
        local clipnick = ClipText:createClipText(tmp:getContentSize(), "", nil, 24)
        clipnick:setTextColor(cc.c4b(155, 57, 19, 255))
        clipnick:setAnchorPoint(tmp:getAnchorPoint())
        clipnick:setPosition(tmp:getPosition())
        nickpanel:addChild(clipnick)
        self.m_tabClipNick[1] = clipnick
        tmp:removeFromParent()
    end

    self.m_nListCount = 0
    self.m_tabList = {}
    -- 战绩列表
    local tmp = self.m_spMark:getChildByName("content")
    local tmpsize = tmp:getContentSize()
    self.m_nCellSize = tmpsize.height / 6
    self._listView = cc.TableView:create(tmpsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)      
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.m_spMark)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    if not GlobalUserItem.bPrivateGoldRoom then
        self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    else
        self._listView:registerScriptHandler(handler(self, self.tableCellAtIndexGold), cc.TABLECELL_SIZE_AT_INDEX)
    end
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tmp:removeFromParent()
    self._listView:reloadData()

    self:scaleTransitionIn(spbg)

    self.myChairId = self._scene:GetMeChairID()
end

function MarkLayer:onButtonClickedEvent(tag, ref)
    -- if TAG_ENUM.BTN_DISMISS == tag then
    --     PriRoom:getInstance():queryDismissRoom()
    -- end
    self:scaleTransitionOut(self.m_spBg)
end

function MarkLayer:onSelectedEvent(tag, sender)

    if self._scene.m_bLookOn and TAG_ENUM.CHECK_MARK == tag then
                sender:setSelected(false)
                showToast(self._scene, "观战中不可使用该功能!", 2)
                return 
    end

    if self.m_nSelectIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectIdx = tag
    for k,v in pairs(self.m_tabBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    
    if TAG_ENUM.CHECK_CONFIG == tag then
        -- 玩法
        self.m_spConfig:setVisible(true)
        self.m_spMark:setVisible(false)
    elseif TAG_ENUM.CHECK_MARK == tag then

        -- 战绩
        self.m_spConfig:setVisible(false)
        self.m_spMark:setVisible(true)
        -- 刷新昵称

        if not GlobalUserItem.bPrivateGoldRoom then
            for i = 1, 3 do
                self.m_tabClipNick[i]:setString("")
                local userItem = self._scene:getUserInfoByChairID(i - 1)
                if nil ~= userItem then
                    local sznick = userItem.szNickName or ""
                    local name =  string.EllipsisByConfig(sznick,180,string.getConfig("fonts/round_body.ttf" , 28))
                    self.m_tabClipNick[i]:setString(name)
                    -- if GlobalUserItem.bPrivateGoldRoom and userItem.dwUserID ~= GlobalUserItem.tabAccountInfo.dwUserID then
                        -- self.m_tabClipNick[i]:setString("")
                    -- end
                end
            end
        else
            local name = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
            self.m_tabClipNick[1]:setString(name)
        end

        -- 刷新战绩
        self.m_tabList = self._scene.m_tabMarkRecord or {}
        self.m_nListCount = #self.m_tabList
        self._listView:reloadData()
    end
end

function MarkLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function MarkLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function MarkLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function MarkLayer:onTransitionOutFinish()
    self:removeFromParent()
end

-- 子视图大小
function MarkLayer:cellSizeForTable(view, idx)
    return view:getViewSize().width, self.m_nCellSize
    -- return 670， 62
end

--积分场
function MarkLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local tabScore = self.m_tabList[self.m_nListCount -idx]
    if type(tabScore) ~= "table" then
        -- return cell
    end

    --背景条
    -- if (self.m_nListCount - idx)%2 == 0 then
    --     local bg = cc.Sprite:create("roominfo/grade_frame.png")
    --     bg:setAnchorPoint(cc.p(0.5, 0.5))
    --     bg:setPosition(cc.p(view:getViewSize().width*0.5 ,self.m_nCellSize * 0.5))
    --     cell:addChild(bg)
    -- end

    local lineSprite = cc.Sprite:create("privateroom/roominfo/line_1.png")
    lineSprite:setAnchorPoint(cc.p(0.5, 0.5))
    lineSprite:setPosition(cc.p(view:getViewSize().width*0.5, 6))
    cell:addChild(lineSprite)

    for i=1, 3 do
        local tmp = cell:getChildByName("round")
        if tmp then
            tmp:removeFromParent()
        end

        if tabScore and tabScore[i] then
            -- 局数
            local szround = ""
            -- if GlobalUserItem.bPrivateGoldRoom then
                -- szround = szround .. (idx + 1) 
            -- else
                szround = szround .. (idx + 1) .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            -- end
            local round = cc.Label:createWithTTF(szround, "fonts/round_body.ttf", 24)
            round:setTextColor(cc.c3b(155, 57, 19))
            round:setName("round")
            round:setPosition(self.m_fRoundPosX, self.m_nCellSize * 0.5)
            cell:addChild(round)

            -- 分数
            local score = tabScore[i]
            local szscore = score
            local txtscore
            local index
            if score >= 0 then
                txtscore = cc.LabelAtlas:create("/"..math.abs(score),"privateroom/no/grade1.png",20,26,string.byte("/"))
            else
                txtscore = cc.LabelAtlas:create("/"..math.abs(score),"privateroom/no/grade.png",20,26,string.byte("/"))
            end
            txtscore:setAnchorPoint(0.5, 0.5)
            index = self._scene:SwitchViewChairID(i-1)

            txtscore:setPosition(self.m_tabClipNick[i]:getPositionX(), self.m_nCellSize * 0.5)
            cell:addChild(txtscore)

            -- local userItem = self._scene:getUserInfoByChairID(i - 1)
            -- if nil ~= userItem then
            --     if GlobalUserItem.bPrivateGoldRoom and userItem.dwUserID ~= GlobalUserItem.tabAccountInfo.dwUserID then
            --         txtscore:setString("")
            --     end
            -- end
        end
    end
    return cell
end

--金币场
function MarkLayer:tableCellAtIndexGold(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local tabScore = self.m_tabList[self.m_nListCount -idx]
    if type(tabScore) ~= "table" then
        -- return cell
    end

    local lineSprite = cc.Sprite:create("privateroom/roominfo/line_1.png")
    lineSprite:setAnchorPoint(cc.p(0.5, 0.5))
    lineSprite:setPosition(cc.p(view:getViewSize().width*0.5, 6))
    cell:addChild(lineSprite)

    for i=1, 3 do
        if tabScore and tabScore[i] then
            if i-1 == self._scene:GetMeChairID() then
                -- 局数
                local tmp = cell:getChildByName("round")
                if tmp then
                    tmp:removeFromParent()
                end

                local szround = ""
                -- if GlobalUserItem.bPrivateGoldRoom then
                    szround = szround .. (idx + 1) 
                -- else
                    -- szround = szround .. (idx + 1) .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
                -- end
                local round = cc.Label:createWithTTF(szround, "fonts/round_body.ttf", 24)
                round:setTextColor(cc.c3b(155, 57, 19))
                round:setName("round")
                round:setPosition(self.m_fRoundPosX, self.m_nCellSize * 0.5)
                cell:addChild(round)

                -- 分数
                local score = tabScore[i]
                local szscore = score
                local txtscore
                local index
                if score >= 0 then
                    txtscore = cc.LabelAtlas:create("/"..math.abs(score),"privateroom/no/grade1.png",20,26,string.byte("/"))
                else
                    txtscore = cc.LabelAtlas:create("/"..math.abs(score),"privateroom/no/grade.png",20,26,string.byte("/"))
                end
                txtscore:setAnchorPoint(0.5, 0.5)
                index = self._scene:SwitchViewChairID(i-1)

                txtscore:setPosition(self.m_tabClipNick[1]:getPositionX(), self.m_nCellSize * 0.5)
                cell:addChild(txtscore)

                -- local userItem = self._scene:getUserInfoByChairID(i - 1)
                -- if nil ~= userItem then
                --     if GlobalUserItem.bPrivateGoldRoom and userItem.dwUserID ~= GlobalUserItem.tabAccountInfo.dwUserID then
                --         txtscore:setString("")
                --     end
                -- end
            end
        end
    end
    return cell
end

-- 子视图数目
function MarkLayer:numberOfCellsInTableView(view)
    if self.m_nListCount >= 6 then
        return self.m_nListCount
    else
        return 6
    end
end

return MarkLayer
--
-- Author: zhong
-- Date: 2017-08-10 14:27:22
--
-- 战绩/流水/配置
local cmd = appdf.req(appdf.GAME_SRC.."qipai.thirteenzj.src.models.cmd_game")
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local MarkLayer = class("MarkLayer", TransitionLayer)

local TAG_START             = 100
local BT_RULE  = 101
local BT_SCORELIST = 102


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
    -- scene = gamelayer
    MarkLayer.super.ctor( self, scene, param, level )

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/Layer_info.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    self.RoomInfoLayer = csbNode

    -- 遮罩
    local mask = csbNode:getChildByName("Panel")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    self.m_spBg = csbNode:getChildByName("Image_bg")

    local boxCallback = function(sender,eventType)
        self.m_tabBoxGame[BT_RULE]:setSelected(BT_RULE == sender:getTag())
        self.m_tabBoxGame[BT_SCORELIST]:setSelected(BT_SCORELIST == sender:getTag())

        self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(BT_RULE == sender:getTag())
        self.RoomInfoLayer:getChildByName("Node_score"):setVisible(BT_SCORELIST == sender:getTag())
        if BT_SCORELIST == sender:getTag() then
            self.m_tabList = self._scene.m_recordList.lDetailScore or {}
            self.m_nListCount = self._scene.m_recordList.nCount
            self._listView:reloadData()
            self:updataUserInfo()
        end
    end
    --按钮控制
    self.m_tabBoxGame = {}
    local checkbx = self.RoomInfoLayer:getChildByName("CheckBox_1")
    checkbx:setTag(BT_RULE)
    checkbx:addEventListener(boxCallback)
    checkbx:setSelected(true)
    self.m_tabBoxGame[BT_RULE] = checkbx

    checkbx = self.RoomInfoLayer:getChildByName("CheckBox_2")
    checkbx:setTag(BT_SCORELIST)
    checkbx:addEventListener(boxCallback)
    self.m_tabBoxGame[BT_SCORELIST] = checkbx


    local btnClose = self.RoomInfoLayer:getChildByName("Button_close")
    btnClose:addTouchEventListener( function (ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:scaleTransitionOut(self.m_spBg)        
        end
    end )

    -- 房间信息界面
    --解散按钮
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("btn_dismiss")
    if GlobalUserItem.bPrivateGoldRoom then
        btnDismiss:setVisible(false)
        self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("DisBg_15"):setVisible(false)
    end
    if nil ~= btnDismiss then
        btnDismiss:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
                self.RoomInfoLayer:setVisible(false)
            end
        end)
    end
    --游戏流水界面
    -- 列表
    self.m_tabList = self._scene.m_recordList.lDetailScore or {}
    self.m_nListCount = self._scene.m_recordList.nCount

    local tmpcontent = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("tableView")
    local contentsize = tmpcontent:getContentSize()
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setPosition(tmpcontent:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.RoomInfoLayer:getChildByName("Node_score"))
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
    tmpcontent:removeFromParent()
    
    self:updataUserInfo()

    self:showGameRule()
    --self:scaleTransitionIn(self.m_spBg)
end

function MarkLayer:showGameRule()
    local nodeRule = self.RoomInfoLayer:getChildByName("Node_rule")
    local modevalue = self._scene:getBankerMode()
    nodeRule:getChildByName("Text_wanfa"):setString(modevalue==0 and "通比" or "霸王庄")


    local playernum = self._scene._cbPlayCount
    print("playernum", playernum)
    if nil ~= playernum then
        nodeRule:getChildByName("Text_play"):setString(playernum.."人")
    end

    local cellscore = PriRoom:getInstance().m_tabPriData.lCellScore
    if cellscore ~= nil then
        nodeRule:getChildByName("Text_cellscore"):setString(cellscore.."")
    end
end

function MarkLayer:onButtonClickedEvent(tag, ref)
    if TAG_ENUM.BTN_DISMISS == tag then
        PriRoom:getInstance():queryDismissRoom()
    elseif TAG_ENUM.TAG_MASK == tag then

    end
    self:scaleTransitionOut(self.m_spBg)
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

function MarkLayer:updataUserInfo()
    print("updataUserInfo", i)
    local nodeScore = self.RoomInfoLayer:getChildByName("Node_score")
    for i=1,4 do
        if  GlobalUserItem.bPrivateGoldRoom  then
            if  i -1 == self._scene:GetMeChairID() then

                local userItem = self._scene:getUserInfoByChairID(i-1)
                local labName = nodeScore:getChildByName("Text_name1")
                 labName:setPositionX(500)

                if nil ~= userItem then
                    local strNickname = string.EllipsisByConfig(userItem.szNickName, 100, 
                                        string.getConfig("fonts/round_body.ttf", 25))
                    labName:setString(strNickname)
                    labName:setVisible(true)
                end
                nodeScore:getChildByName("Text_Round"):setPositionX(167)
                -- nodeScore:getChildByName("Sprite_1"):setPositionX(335)

            end
            -- if i ~=  1 then
            --     nodeScore:getChildByName("Sprite_"..i):setVisible(false)
            -- end

        else
            local userItem = self._scene:getUserInfoByChairID(i-1)
            local labName = nodeScore:getChildByName("Text_name"..i)
            if i -1 == self._scene:GetMeChairID() then
                --labName:setColor(cc.c3b(255, 255, 153))
            end
            if nil ~= userItem then
                print("updataUserInfo", userItem.szNickName)
               
                local strNickname = string.EllipsisByConfig(userItem.szNickName, 100, 
                                    string.getConfig("fonts/round_body.ttf", 25))
                labName:setString(strNickname)
                labName:setVisible(true)
            end
        end
    end
end

function MarkLayer:updataScoreList(scoreList)
    self.m_tabList = clone(scoreList.lDetailScore)
    self.m_nListCount = scoreList.nCount
    if 0 < self.m_nListCount then
        self._listView:reloadData()
    end
end

-- 子视图大小
function MarkLayer:cellSizeForTable(view, idx)
    return 691, 55
end

function MarkLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local iteminfo = {}
    for i=1,4 do
        table.insert(iteminfo, self.m_tabList[i][idx + 1])
    end
    dump(iteminfo, tableCellAtIndex, "10")
    if nil == iteminfo then
        return cell 
    end

    local csb = cc.CSLoader:createNode(cmd.RES_PATH.."privateroom/game/Node_cell.csb")
    cell:addChild(csb)
    -- csb:setPosition(-100, 55 * 0.5)
    csb:setPosition(-20, 55 * 0.5)

    --背景
    if 0 == math.mod(idx, 2) then
        -- local sprBg =  csb:getChildByName("Image")
        -- sprBg:setVisible(false) 
    end

    --局数
    local strRound = GlobalUserItem.bPrivateGoldRoom and (idx + 1) or (idx + 1).."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    csb:getChildByName("Text_round"):setString(strRound)

    if GlobalUserItem.bPrivateGoldRoom then
        csb:getChildByName("Text_round"):setPositionX(100)
    end

    --分数
    for i=1, 4 do
        local score = iteminfo[i]
        if GlobalUserItem.bPrivateGoldRoom then
                if i -1 == self._scene:GetMeChairID() then
                    local labScore = csb:getChildByName("Text_score1")
                    labScore:setPositionX(420)
                    labScore:setVisible(true)
                    local userItem = self._scene:getUserInfoByChairID(i-1)
                    if nil ~= score then
                        if userItem == nil  then
                            labScore:setString("")
                        else
                            labScore:setString(score.."")
                        end
                    
                        if score > 0 then 
                            labScore:setColor(cc.c3b(255, 99, 63))
                        else
                            labScore:setColor(cc.c3b(140, 58, 32))
                        end
                    
                    end
                
                -- csb:getChildByName("Sprite_1"):setPositionX(346)

            end
            -- if i ~=  1 then
            --     csb:getChildByName("Sprite_"..i):setVisible(false)
            -- end
        else

                local labScore = csb:getChildByName("Text_score"..i)
                labScore:setVisible(true)
                local userItem = self._scene:getUserInfoByChairID(i-1)
                if nil ~= score then
                    if userItem == nil  then
                        labScore:setString("")
                    else
                        labScore:setString(score.."")
                    end
                    

                    --判断是不是自己
                    --if i -1 == self._scene:GetMeChairID() then
                        if score > 0 then 
                            labScore:setColor(cc.c3b(255, 99, 63))
                        else
                            labScore:setColor(cc.c3b(140, 58, 32))
                        end
                    --end
                end
        end
    end
    return cell
end

-- 子视图数目
function MarkLayer:numberOfCellsInTableView(view)
    print("MarkLayer:numberOfCellsInTableView", self.m_nListCount)
    return self.m_nListCount
end

return MarkLayer
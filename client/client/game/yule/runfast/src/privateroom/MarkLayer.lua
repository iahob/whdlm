--
-- Author: zhong
-- Date: 2017-08-10 14:27:22
--
-- 战绩/流水/配置
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local MarkLayer = class("MarkLayer", cc.Layer)

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
function MarkLayer:ctor( scene )
	
	self._scene = scene

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("privateroom/game/Layer_info.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
	
	self._csbNode = csbNode
	
	--蒙板
	local mask = csbNode:getChildByName("Panel")
	mask:addTouchEventListener(function (sender,eventType)
		if eventType == ccui.TouchEventType.ended then
			self:removeFromParent()
		end
	end)

	--关闭
	local close = csbNode:getChildByName("Button_close")
	close:addTouchEventListener(function (sender,eventType)
		if eventType == ccui.TouchEventType.ended then
			self:removeFromParent()
		end
	end)
	--tab切换栏
	local btnListener = function (sender,eventType)
       for i=1 , 2 do
       	 local layer = self.m_tabLayer[i]
       	 local btn = self.m_tabCheckBox[i]
       	 layer:setVisible(i == sender:getTag())
       	 btn:setEnabled(sender:getTag() ~= i)
       end
    end
	

	
	self.m_tabCheckBox = {}
	--游戏配置
	local checkbx = self._csbNode:getChildByName("Button_wanfa")
	checkbx:setTag(1)
	checkbx:addTouchEventListener(btnListener)
	self.m_tabCheckBox[1] = checkbx
   
	--战绩流水
	checkbx = self._csbNode:getChildByName("Button_record")
	checkbx:setTag(2)
	checkbx:addTouchEventListener(btnListener)
	self.m_tabCheckBox[2] = checkbx

	self.m_tabLayer = {}

	local layer = self._csbNode:getChildByName("Node_rule")
	self.m_tabLayer[1] = layer

	layer = self._csbNode:getChildByName("Node_score")
	self.m_tabLayer[2] = layer
	-- --解散
	
	local dismissBtn = self._csbNode:getChildByName("Node_rule"):getChildByName("Button_1")
	
	dismissBtn:addTouchEventListener(function (sender,eventType)
		if eventType == ccui.TouchEventType.ended then
			PriRoom:getInstance():queryDismissRoom()
		end
	end)

	if PriRoom:getInstance().m_tabPriData.dwPlayCount==0 then
		dismissBtn:setEnabled(false)
	end

	    self.m_nListCount = 0
	    self.m_tabList = {}
    	-- 战绩列表
	local Panel_2 = self._csbNode:getChildByName("Node_score"):getChildByName("Panel_1")
	    local tmpsize = Panel_2:getContentSize()
	    self._listView = cc.TableView:create(tmpsize)
	    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)  
	    --self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)   
	    self._listView:setAnchorPoint(cc.p(0,0))
	    self._listView:setPosition(cc.p(0,0))
	    self._listView:setDelegate()
	    self._listView:addTo(Panel_2)
	    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
	    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

	    self._listView:reloadData()
dump(self.m_tabUserItem , "self.m_tabUserItem")
	    for i=1,3 do
	    	local name =self._csbNode:getChildByName("Node_score"):getChildByName("FileNode_title"):getChildByName("Text_content"..i) 
	    	if self._scene.m_tabUserItem[i] then
		    	name:setString(self._scene.m_tabUserItem[i].szNickName)
		    else
		    	name:setString("")
	    	end
	    end
	    dump(self._scene:getPriRecord() , "刚打开 战绩")
	    self:showGameRule()
end

function MarkLayer:onButtonClickedEvent(tag, ref)
    if TAG_ENUM.BTN_DISMISS == tag then
        PriRoom:getInstance():queryDismissRoom()
    end
end

function MarkLayer:onSelectedEvent(tag, sender)
		
	self:changeMode(tag)
end



function MarkLayer:showGameRule()

	local ruleNode = self._csbNode:getChildByName("Node_rule")
	ruleNode:getChildByName("rule1"):setString(self._scene.cbUserMaxCardCount.."张")

	ruleNode:getChildByName("rule2"):setString(self._scene.wPlayerCount.."人")

	ruleNode:getChildByName("rule3"):setString(self._scene.bForceOutCard and "有大必出" or "非有大必出")
	ruleNode:getChildByName("rule4"):setString(self._scene.bSpadeBanker and "黑桃3首出" or "赢家首出")
	ruleNode:getChildByName("rule5"):setString(self._scene.bEchelonConclude and "梯度结算" or "非梯度结算")
	if GlobalUserItem.bPrivateGoldRoom then
		ruleNode:getChildByName("Text_4"):setVisible(false)
		ruleNode:getChildByName("rule6"):setVisible(false)

	else
		ruleNode:getChildByName("rule6"):setString(PriRoom:getInstance().m_tabPriData.dwDrawCountLimit.."局")
	end

end


-- 子视图大小
function MarkLayer:cellSizeForTable(view, idx)
    return 770, 46
end

function MarkLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
	
    local recordList =  self._scene:getPriRecord().detail
    dump(recordList,"=====================11111111111111")
    if nil == recordList[1][idx+1] then
        return cell 
    end
	
    local csb = ExternalFun.loadCSB("privateroom/game/Node_cell.csb", cell)
    csb:setPosition(385, 23)

    local back = csb:getChildByName("Sprite_back")
    if idx%2==0 then
    	back:setSpriteFrame( cc.Sprite:create("privateroom/game/img_cellBack3.png"):getSpriteFrame() )
    else
    	back:setSpriteFrame( cc.Sprite:create("privateroom/game/img_cellBack2.png"):getSpriteFrame() )
    end
    local index = self._scene:getPriRecordCount() - idx
    local jushu = csb:getChildByName("Text_round")
    jushu:setString((index).."")


    local score1 = csb:getChildByName("Text_content1")
    local score2 = csb:getChildByName("Text_content2")
    local score3 = csb:getChildByName("Text_content3")
    	score1:setString(recordList[1][index].."")
    	score2:setString(recordList[2][index].."")
    	if self._scene.wPlayerCount == 2 then
    		score3:setVisible(false)
    	else
    		score3:setString(recordList[3][index].."")
    	end
    


    return cell
end

-- 子视图数目
function MarkLayer:numberOfCellsInTableView(view)
	return self._scene:getPriRecordCount()
end


return MarkLayer
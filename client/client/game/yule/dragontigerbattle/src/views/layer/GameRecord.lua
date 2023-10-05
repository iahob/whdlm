local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")
local GameLogicRecord = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.GameLogicRecord")
local GameLogicRecordEx = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.GameLogicRecordEx")

local GameRecord = class("GameRecord", cc.Layer)

local enumTable = {
    "BT_QUIT",
    "BT_MASK", 

    --预测标记
    "TABLEVIEW_1",
    "TABLEVIEW_2",
    "TABLEVIEW_3",
    "TABLEVIEW_4",
}
--大路
TrendListSize1 = cc.size(553, 165)
--大眼仔路，下路，自由路
TrendListSizeOther = cc.size(553/2, 165/2)

TrendListPos1 = cc.p(610.50, 284)
TrendListPos2 = cc.p(610.50, 201.30)
TrendListPos3 = cc.p(886.50, 201.30)
TrendListPos4 = cc.p(610.50, 119.63)

local TAG_ENUM = ExternalFun.declarEnumWithTable(0, enumTable)

function GameRecord:ctor(resultList, parent)
	local csbNode = ExternalFun.loadCSB("trend.csb", self)
	self.csbNode = csbNode
	self._gameView = parent

	--初始化
	self:paramInit()

	self:loadResource()

	--游戏记录
	self:saveGameRecord(resultList)

	--获得游戏数据
	self:freshGameData()

	table.insert(self.gameDataRecord, self.gameTrendData1)
	table.insert(self.gameDataRecord, self.gameTrendData2)
	table.insert(self.gameDataRecord, self.gameTrendData3)
	table.insert(self.gameDataRecord, self.gameTrendData4)

	--绘制趋势界面
	self:drawTrend()
end

function GameRecord:drawTrend()
	self:drawTrend1()
	self:drawTrend2()
	self:drawTrend3()
	self:drawTrend4()

	--局势预测
	self:drawNextRound()
end

function GameRecord:loadResource()
	local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

	--退出
	local btn = self.csbNode:getChildByName("Button_exit")
	btn:setTag(TAG_ENUM.BT_QUIT)
	btn:addTouchEventListener(btnEvent)

	--简洁路单
	self.simpleChartNode = self.csbNode:getChildByName("Image_1"):getChildByName("Sprite_9")

	--遮罩
	btn = self.csbNode:getChildByName("Panel_mask")
	btn:setTag(TAG_ENUM.BT_MASK)
	btn:addTouchEventListener(btnEvent)
end

function GameRecord:paramInit()
	--游戏记录
	self.gameData = {}

	--按列原始摆放
	self.gameTrendData1 = {}                --二维
	self.gameTrendData2 = {}                --一维
	self.gameTrendData3 = {}                --一维
	self.gameTrendData4 = {}                --一维

	--按界面的列摆放
	self.gameTrendDataByColumn1 = {}
	self.gameTrendDataByColumn2 = {}
	self.gameTrendDataByColumn3 = {}
	self.gameTrendDataByColumn4 = {}

	--按界面的行摆放
	self.gameTrendDataByRow1 = {}
	self.gameTrendDataByRow2 = {}
	self.gameTrendDataByRow3 = {}
	self.gameTrendDataByRow4 = {}

	--tableview
	self.resultList = nil
	self.trendList1 = nil
	self.trendList2 = nil
	self.trendList3 = nil
	self.trendList4 = nil

	--记录上一局游戏数据
	self.gameDataRecord = {}

	--简洁路单
	self.simpleChartNode = nil
end

function GameRecord:saveGameRecord(resultList)
	self.gameData = resultList
	self:updateShowRound()

	self.resultList = cc.TableView:create(cc.size(440, 329))
    self.resultList:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL) 
    self.resultList:setAnchorPoint(cc.p(0,0))
    self.resultList:setPosition(cc.p(170.83,119.63))
    self.resultList:setDelegate()
    self.resultList:addTo(self.csbNode)
    self.resultList:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self.resultList:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self.resultList:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    self.resultList:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    -- self.resultList:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    self.resultList:reloadData()
    -- self:scrollTableViewToBottom(self.resultList)
	-- self.resultList:scrollToPercentHorizontal(100)
    
end

function GameRecord:drawTrend1()
	self.trendList1 = cc.TableView:create(TrendListSize1)
    self.trendList1:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL) 
    self.trendList1:setAnchorPoint(cc.p(0,0))
    self.trendList1:setPosition(TrendListPos1)
    self.trendList1:setDelegate()
    self.trendList1:addTo(self.csbNode)
    self.trendList1:registerScriptHandler(handler(self, self.tableCellAtIndex1), cc.TABLECELL_SIZE_AT_INDEX)
    self.trendList1:registerScriptHandler(handler(self, self.tableCellTouched1), cc.TABLECELL_TOUCHED)
    self.trendList1:registerScriptHandler(self.cellSizeForTable1, cc.TABLECELL_SIZE_FOR_INDEX)
    self.trendList1:registerScriptHandler(handler(self, self.numberOfCellsInTableView1), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.trendList1:reloadData()
    -- self.trendList1:setContentOffset(cc.p(0,0))

    if self.gameTrendDataByColumn1 and #self.gameTrendDataByColumn1 > 24 then 
    	self.trendList1:setContentOffset(cc.p(-(#self.gameTrendDataByColumn1-24)*23,0))
    end

end

function GameRecord:drawTrend2()
	self.trendList2 = cc.TableView:create(TrendListSizeOther)
    self.trendList2:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL) 
    self.trendList2:setAnchorPoint(cc.p(0,0))
    self.trendList2:setPosition(TrendListPos2)
    self.trendList2:setDelegate()
    self.trendList2:addTo(self.csbNode)
    self.trendList2:registerScriptHandler(handler(self, self.tableCellAtIndex2), cc.TABLECELL_SIZE_AT_INDEX)
    self.trendList2:registerScriptHandler(handler(self, self.tableCellTouched2), cc.TABLECELL_TOUCHED)
    self.trendList2:registerScriptHandler(self.cellSizeForTable2, cc.TABLECELL_SIZE_FOR_INDEX)
    self.trendList2:registerScriptHandler(handler(self, self.numberOfCellsInTableView2), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.trendList2:reloadData()

    if self.gameTrendDataByColumn2 and #self.gameTrendDataByColumn2 > 12 then 
    	self.trendList2:setContentOffset(cc.p(-(#self.gameTrendDataByColumn2-12)*23,0))
    end
end

function GameRecord:drawTrend3()
	self.trendList3 = cc.TableView:create(TrendListSizeOther)
    self.trendList3:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL) 
    self.trendList3:setAnchorPoint(cc.p(0,0))
    self.trendList3:setPosition(TrendListPos3)
    self.trendList3:setDelegate()
    self.trendList3:addTo(self.csbNode)
    self.trendList3:registerScriptHandler(handler(self, self.tableCellAtIndex3), cc.TABLECELL_SIZE_AT_INDEX)
    self.trendList3:registerScriptHandler(handler(self, self.tableCellTouched3), cc.TABLECELL_TOUCHED)
    self.trendList3:registerScriptHandler(self.cellSizeForTable3, cc.TABLECELL_SIZE_FOR_INDEX)
    self.trendList3:registerScriptHandler(handler(self, self.numberOfCellsInTableView3), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.trendList3:reloadData()

    if self.gameTrendDataByColumn3 and #self.gameTrendDataByColumn3 > 12 then 
    	self.trendList3:setContentOffset(cc.p(-(#self.gameTrendDataByColumn3-12)*23,0))
    end

end

function GameRecord:drawTrend4()
	self.trendList4 = cc.TableView:create(TrendListSizeOther)
    self.trendList4:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL) 
    self.trendList4:setAnchorPoint(cc.p(0,0))
    self.trendList4:setPosition(TrendListPos4)
    self.trendList4:setDelegate()
    self.trendList4:addTo(self.csbNode)
    self.trendList4:registerScriptHandler(handler(self, self.tableCellAtIndex4), cc.TABLECELL_SIZE_AT_INDEX)
    self.trendList4:registerScriptHandler(handler(self, self.tableCellTouched4), cc.TABLECELL_TOUCHED)
    self.trendList4:registerScriptHandler(self.cellSizeForTable4, cc.TABLECELL_SIZE_FOR_INDEX)
    self.trendList4:registerScriptHandler(handler(self, self.numberOfCellsInTableView4), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.trendList4:reloadData()

    if self.gameTrendDataByColumn4 and #self.gameTrendDataByColumn4 > 12 then 
    	self.trendList4:setContentOffset(cc.p(-(#self.gameTrendDataByColumn4-12)*23,0))
    end

end

--下一局龙虎的预测显示
function GameRecord:drawNextRound()
	local function setNextRound(dataTable, areaIndex, no)
		local dataOld = {}
		local dataNew = {}
		local data = -1
		local str = string.format("win_little_%d", no)
		local str2 = "" 

		local a = self.gameDataRecord[areaIndex]
		if a == nil then
			return;
		end
		for i=1, #a do
			for j=1, #a[i] do
				table.insert(dataOld, a[i][j])
			end
		end

		if next(dataTable) == nil then
			self.csbNode:getChildByName(str):setTexture("") 
			return 
		end

		dataNew = dataTable
		data = dataNew[#dataNew]
		
		if #dataOld == #dataNew then
			self.csbNode:getChildByName(str):setTexture("") 
			return
		else
			if data == GAME_CMD.AREA_LONG then
				if areaIndex == 2 then
					str2 ="trend/blue_big1.png"
				elseif areaIndex == 3 then
					str2 ="trend/blue_big2.png"
				elseif areaIndex == 4 then
					str2 ="trend/blue_big3.png"
				end
				self.csbNode:getChildByName(str):setTexture(str2)
			elseif data == GAME_CMD.AREA_HU then
				if areaIndex == 2 then
					str2 ="trend/red_big1.png"
				elseif areaIndex == 3 then
					str2 ="trend/red_big2.png"
				elseif areaIndex == 4 then
					str2 ="trend/red_big3.png"
				end
				self.csbNode:getChildByName(str):setTexture(str2)
			else
				self.csbNode:getChildByName(str):setTexture("") 
			end
		end
	end

	local tmp = {}
	for k, v in ipairs(self.gameData) do
		local num = 0
		num = v
		table.insert(tmp, num)
	end

	table.insert(tmp, GAME_CMD.AREA_LONG)
	local data2,data3,data4 = self:getTrend(tmp)
	setNextRound(data2, 2, 1)
	setNextRound(data3, 3, 2)
	setNextRound(data4, 4, 3)
	

	--假如虎胜
	tmp = {}
	for k, v in ipairs(self.gameData) do
		local num = 0
		num = v
		table.insert(tmp, num)
	end
	table.insert(tmp, GAME_CMD.AREA_HU)

	local data2,data3,data4 = self:getTrend(tmp)
	setNextRound(data2, 2, 4)
	setNextRound(data3, 3, 5)
	setNextRound(data4, 4, 6)
end

function GameRecord:clearGameData()
	self.gameTrendData1 = {} 
	self.gameTrendDataByRow1 = {}
	self.gameTrendDataByColumn1 = {}
	self.gameTrendData2 = {} 
	self.gameTrendDataByRow2 = {}
	self.gameTrendDataByColumn2 = {}
	self.gameTrendData3 = {} 
	self.gameTrendDataByRow3 = {} 
	self.gameTrendDataByColumn = {}
	self.gameTrendData4 = {} 
	self.gameTrendDataByRow4 = {} 
	self.gameTrendDataByColumn = {}
end

--刷新游戏数据
function GameRecord:freshGameData()
	self:clearGameData()

	self.gameTrendData1, 
	self.gameTrendDataByRow1,
	self.gameTrendDataByColumn1 = GameLogicRecord.getTrendData1(self.gameData)

	self.gameTrendData2, 
	self.gameTrendDataByRow2,
	self.gameTrendDataByColumn2 = GameLogicRecord.getTrendData2(self.gameTrendDataByColumn1)

	self.gameTrendData3, 
	self.gameTrendDataByRow3, 
	self.gameTrendDataByColumn3 = GameLogicRecord.getTrendData3(self.gameTrendDataByColumn1)

	self.gameTrendData4, 
	self.gameTrendDataByRow4, 
	self.gameTrendDataByColumn4 = GameLogicRecord.getTrendData4(self.gameTrendDataByColumn1)

	--大路数据
	self.gameTrendData1,
	self.gameTrendDataByColumn1 = GameLogicRecordEx:getTrendData1(self.gameData)
	--大眼仔路数据
	self.gameTrendData2, 
	self.gameTrendDataByColumn2 = GameLogicRecordEx:getTrendData2(self.gameTrendDataByColumn1)
	--小路数据
	self.gameTrendData3, 
	self.gameTrendDataByColumn3 = GameLogicRecordEx:getTrendData3(self.gameTrendDataByColumn1)
	--甲由路数据
	self.gameTrendData4,
	self.gameTrendDataByColumn4 = GameLogicRecordEx:getTrendData4(self.gameTrendDataByColumn1)

end

--下一局预测
function GameRecord:getTrend(data)
	local a1,b1,c1 = GameLogicRecord.getTrendData1(data)

	local a2,b2,c2,d2 = GameLogicRecord.getTrendData2(c1)

	local a3,b3,c3,d3 = GameLogicRecord.getTrendData3(c1)

	local a4,b4,c4,d4 = GameLogicRecord.getTrendData4(c1)
	return d2, d3, d4
end

--更新所有界面信息
function GameRecord:freshGameRecord(resultList)
	self.gameData = resultList
	self.resultList:reloadData()

	self:freshGameData()
	self.trendList1:reloadData()
	self.trendList2:reloadData()
	self.trendList3:reloadData()
	self.trendList4:reloadData()

	--龙虎胜利局数显示
	self:updateShowRound()

	self.gameDataRecord = {}
	table.insert(self.gameDataRecord, self.gameTrendData1)
	table.insert(self.gameDataRecord, self.gameTrendData2)
	table.insert(self.gameDataRecord, self.gameTrendData3)
	table.insert(self.gameDataRecord, self.gameTrendData4)
	self:drawNextRound()
end

--简洁路单百分比
function GameRecord:longHuRate(rate)
	-- if rate ~= 0 and rate ~= 100 then
		local str1 = rate
		local str2 = (100-rate)
		self.csbNode:getChildByName("Text_longPercent"):setString(str1)
		self.csbNode:getChildByName("Text_huPercent"):setString(str2)
	-- end
	self.csbNode:getChildByName("LoadingBar_Long"):setPercent(rate)
	self.csbNode:getChildByName("LoadingBar_Hu"):setPercent(100-rate)
end

--更新简洁路单
function GameRecord:freshSimpleChart(data)
	self.simpleChartNode:removeAllChildren()
	local startX = 25
	local startY = 49/2
	local gap = 49
	local long = 0
	local ping = 0
	local hu = 0
    local index = #data >= 20 and 20 or #data

	for i=1, index do
		local str = ""
		local count = #data - index
        if data[i+count] == GAME_CMD.AREA_LONG then
        	long = long + 1
            str = "game/win_1.png"
        elseif data[i+count] == GAME_CMD.AREA_HU then
        	hu = hu + 1
            str = "game/win_3.png"
        elseif data[i+count] == GAME_CMD.AREA_PING then
        	ping = ping + 1
            str = "game/win_2.png"
        end

        local img = cc.Sprite:create(str)
        img:setPosition(cc.p(startX+gap*(i-1), startY))
        img:addTo(self.simpleChartNode)

        if index == i then
            local new = cc.Sprite:create("game/new.png")
            new:setPosition(cc.p(startX+gap*(i-1) - 6, startY + 6))
            new:addTo(self.simpleChartNode)

        end
        
	end

	--获得龙虎的比例
	local rate = math.floor(long/(#data-ping)* 100) 
	self:longHuRate(rate)
end

--龙虎胜利局数显示
function GameRecord:updateShowRound()
	local long = 0
	local hu = 0
	local ping = 0

	local data = self.gameData
	for i=1, #data do
		if data[i] == GAME_CMD.AREA_LONG then
        	long = long + 1
        elseif data[i] == GAME_CMD.AREA_HU then
        	hu = hu + 1
        elseif data[i] == GAME_CMD.AREA_PING then
        	ping = ping + 1
        end
	end
	self.csbNode:getChildByName("Text_Hu"):setString(hu)
	self.csbNode:getChildByName("Text_Long"):setString(long)
	self.csbNode:getChildByName("Text_Ping"):setString(ping)
end

----------------------------------------------------------------------------
--游戏记录(高级路单)
function GameRecord:tableCellAtIndex( view, idx )

	local cell = view:dequeueCell()

	if nil == cell then
		cell = cc.TableViewCell:new()
	else
		cell:removeAllChildren()
	end


	local startX = 27 
	local startY = 358
	local gap = 55

	for i = idx*6 +1,  idx*6 +6 do 
		local data = self.gameData[i]
		if data == nil then
			break
		end

		local str = ""
		if data == GAME_CMD.AREA_LONG then
        	str = "game/win_1.png"
        elseif data == GAME_CMD.AREA_HU then
            str = "game/win_3.png"
        elseif data == GAME_CMD.AREA_PING then
            str = "game/win_2.png"
        end
        local img = cc.Sprite:create(str)
        img:setPosition(cc.p(startX, startY-gap*(i-idx*6)))
        img:addTo(cell)
	end

	return cell
end

function GameRecord:scrollViewDidScroll(view)
	-- local num = math.ceil((#self.gameData)/6)
	-- local xOffset = self.resultList:getContentOffset().x

	-- if idx+1 == math.ceil((#self.gameData)/6) then
	-- 	print("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL")
	-- 	-- print(self.resultList:getContentOffset().x)
	-- end
	-- if idx+1 == math.ceil((#self.gameData)/6) then
		-- view:setContentOffset(cc.p(self.resultList:getViewSize().width - self.resultList:getContentSize().width, 0), true)
		
	-- end
	self.resultList:setContentOffset(cc.p(0, 0), true)
	-- view:setContentOffset(cc.p(view:maxContainerOffset().x, view:maxContainerOffset().y))
	self.resultList:setContentOffset(cc.p(0,0))
	-- view:setContentOffset(cc.p(view:maxContainerOffset().x, view:maxContainerOffset().y))
end

function GameRecord:scrollTableViewToBottom(list)
	local width = list:getInnerContainerSize().width
	list:setInnerContainerPosition(width, 0)
end

function GameRecord:tableCellTouched(view, cell)
end

function GameRecord.cellSizeForTable()
	-- local sprite = cc.Sprite:create("trend/1.png")
	-- local size = sprite:getContentSize()
	return 55, 329
end

function GameRecord:numberOfCellsInTableView()
	local num = math.ceil((#self.gameData)/6)
	if num < 8 then
		num = 8
	end
	return num
end

function GameRecord:onButtonClickedEvent(tag, ref)
	if tag == TAG_ENUM.BT_QUIT or tag == TAG_ENUM.BT_MASK then
		self:setVisible(false)
	end
end
----------------------------------------------------------------------
function writefile(path, content, mode)
	mode = mode or "w+b"
	local file = io.open(path, mode)
	if file then
		if file:write(content) == nil then return false end
		io.close(file)
		return true
	else
		return false
	end
end

function GameRecord:showResult(parent, tableData, x, y, gap, index, index2)
	if parent then
		parent:removeAllChildren()
	end
	
	if not tableData[index+1] or next(tableData[index+1]) == nil then
		return
	end
	local startX = x
	local startY = y
	local gap = gap
	
	local str = ""
	local num = 0
	for k, v in pairs(tableData[index+1]) do
		local label = nil
		if v == GAME_CMD.AREA_LONG then
			if index2 == 1 then
			 	str = "trend/blue_big.png"
			elseif index2 == 2 then
				str = "trend/blue1.png"
			elseif index2 == 3 then
				str = "trend/blue2.png"
			elseif index2 == 4 then
				str = "trend/blue3.png"
			end
		elseif v == GAME_CMD.AREA_HU then
			if index2 == 1 then
				str = "trend/red_big.png"
			elseif index2 == 2 then
				str = "trend/red1.png"
			elseif index2 == 3 then
				str = "trend/red2.png"
			elseif index2 == 4 then
				str = "trend/red3.png"
			end
		-- elseif v == GAME_CMD.AREA_PING then
			-- for k, v in pairs (tableData[index+1]) do
			-- 	if v == GAME_CMD.AREA_HU then
			-- 		str = "trend/red_big.png"
			-- 		break
			-- 	elseif v == GAME_CMD.AREA_LONG then
			-- 		str = "trend/blue_big.png"
			-- 		break
			-- 	end
			-- end
			-- num = num + 1
			-- label = cc.LabelAtlas:create(num,"no/he.png",10,14,string.byte("0"))
		end

		local img = cc.Sprite:create(str)
		if nil == img then break end
        img:setPosition(cc.p(startX, startY-gap*(k-1)))
        img:addTo(parent)
	end
end

--趋势tableview的处理
function GameRecord:tableCellAtIndex1( view, idx )
	local cell = view:dequeueCell()

	if nil == cell then
		cell = cc.TableViewCell:new()
	else
		cell:removeAllChildren()		
	end

	for j=1,#self.gameTrendDataByColumn1 do
		for i=#self.gameTrendDataByColumn1[j],1,-1 do
			if self.gameTrendDataByColumn1[j][i] == GAME_CMD.AREA_PING then 
				table.remove(self.gameTrendDataByColumn1[j],i)
			end
		end
	end
	-- assert(false)
	self:showResult(cell, self.gameTrendDataByColumn1, 12.5, 151, 27.3, idx, 1)
	return cell
end

function GameRecord:tableCellTouched1(view, cell)
end

function GameRecord.cellSizeForTable1()
	return 23,165
end

function GameRecord:numberOfCellsInTableView1()
	if self.gameTrendDataByColumn1 then
		local num = #self.gameTrendDataByColumn1
		if num < 24 then
			num = 24
		end
		return num
	end
end
------------------------------------------------------------------
function GameRecord:tableCellAtIndex2( view, idx )
	local cell = view:dequeueCell()

	if nil == cell then
		cell = cc.TableViewCell:new()
		
	else
		cell:removeAllChildren()
	end
	self:showResult(cell, self.gameTrendDataByColumn2, 12.5, 75, 13.5, idx, 2)

	return cell
end

function GameRecord:tableCellTouched2(view, cell)
end

function GameRecord.cellSizeForTable2()
	return 23,165/2
end

function GameRecord:numberOfCellsInTableView2()
	if self.gameTrendDataByColumn2 then
		local num = #self.gameTrendDataByColumn2
		if num < 12 then
			num = 12
		end
		return num
	end
end
-----------------------------------------------------------------------------
function GameRecord:tableCellAtIndex3( view, idx )
	local cell = view:dequeueCell()

	if nil == cell then
		cell = cc.TableViewCell:new()
	else
		cell:removeAllChildren()
	end
	self:showResult(cell, self.gameTrendDataByColumn3,12.5, 75, 13.5, idx, 3)
	return cell
end

function GameRecord:tableCellTouched3(view, cell)
end

function GameRecord.cellSizeForTable3()
	return 23,165/2
end

function GameRecord:numberOfCellsInTableView3()
	if self.gameTrendDataByColumn3 then
		local num = #self.gameTrendDataByColumn3
		if num < 12 then
			num = 12
		end
		return num
	end
end
----------------------------------------------------------------------------------
function GameRecord:tableCellAtIndex4( view, idx )
	local cell = view:dequeueCell()

	if nil == cell then
		cell = cc.TableViewCell:new()
	else
		cell:removeAllChildren()
	end
	self:showResult(cell, self.gameTrendDataByColumn4, 12.5, 75, 13.5, idx, 4)
	return cell
end

function GameRecord:tableCellTouched4(view, cell)
end

function GameRecord.cellSizeForTable4()
	return 23,165/2
end

function GameRecord:numberOfCellsInTableView4()
	if self.gameTrendDataByColumn4 then
		local num = #self.gameTrendDataByColumn4
		if num < 12 then
			num = 12
		end
		return num
	end
end

------------------------------------------------------------------------------

return GameRecord
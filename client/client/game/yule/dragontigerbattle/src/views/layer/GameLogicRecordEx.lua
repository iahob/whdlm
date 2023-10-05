local GameLogicRecordEx = {}
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")

--大路数据
local gameTrendData1 = {}

local Constant = 
{
	ComparedResult = {
		DragonWin 	= GAME_CMD.AREA_LONG,
		TigerWin 	= GAME_CMD.AREA_HU,
		Draw	  	= GAME_CMD.AREA_PING
	}
}

-- 填充路单数据
function GameLogicRecordEx:FillGameData(predictionData)
	-- body
	--大路数据
	self.gameTrendData1,
	self.gameTrendDataByColumn1 = self:getTrendData1(predictionData)
	--大眼仔路数据
	self.gameTrendData2, 
	self.gameTrendDataByColumn2 = self:getTrendData2(self.gameTrendDataByColumn1)
	--小路数据
	self.gameTrendData3, 
	self.gameTrendDataByColumn3 = self:getTrendData3(self.gameTrendDataByColumn1)
	--甲由路数据
	self.gameTrendData4,
	self.gameTrendDataByColumn4 = self:getTrendData4(self.gameTrendDataByColumn1)



	-- dump(self.gameTrendDataByColumn1,"gameTrendDataByColumn1",12)
	-- dump(self.gameTrendDataByColumn2,"gameTrendDataByColumn2",12)
	-- dump(self.gameTrendDataByColumn3,"gameTrendDataByColumn3",12)
	-- dump(self.gameTrendDataByColumn4,"gameTrendDataByColumn4",12)

end


--获得大路数据
function GameLogicRecordEx:getTrendData1(predictionData)
	local trendData = {}
	-- for i,v in ipairs(self.ZPLuTrendData.gameRecordList) do
	-- 	table.insert(trendData, v)
	-- end
	--如果是预测数据，则不使用原数据
	if predictionData then
		trendData = predictionData
	else
		-- assert(false)
		return {},{}
	end
	--不统计和
	for i=#trendData, 1, -1 do
		if trendData[i] == Constant.ComparedResult.Draw then
			table.remove(trendData, i)
		end
	end

	if next(trendData) == nil then
		-- assert(false)
		return {},{}
	end
	--按列摆放
	local gameTrendData1 = {}  
	local tmp = {}
	for i=1, #trendData do
		if trendData[i] ~= Constant.ComparedResult.Draw then
			for j=1, #tmp do
				if tmp[j] ~= Constant.ComparedResult.Draw and tmp[j] ~= trendData[i] then
					table.insert(gameTrendData1, tmp)
					tmp = {}
					break
				end
			end
		end
		table.insert(tmp, trendData[i])
	end

	if next(tmp) ~= nil then
		table.insert(gameTrendData1, tmp)
		tmp = {}
	end
	--按界面列摆放
	local gameTrendDataByColumn1 = self:orderByColmn(gameTrendData1)
	-- dump(gameTrendDataByColumn1,"你麻痹",12)
	return gameTrendData1 , gameTrendDataByColumn1
end

--获得大眼仔路数据
function GameLogicRecordEx:getTrendData2(gameTrendData)
	if not gameTrendData then
		return {}, {}
	end
	local data = self:getTrendData(2, gameTrendData)
	--按列摆放
	local gameTrendData2 = self:dealData(data)
    --按界面列摆放
    local gameTrendDataByColumn2 = self:orderByColmn(gameTrendData2)
    return 	gameTrendData2, gameTrendDataByColumn2

end

--获得小路数据
function GameLogicRecordEx:getTrendData3(gameTrendData)
	if not gameTrendData then
		return {}, {}
	end

	local data = self:getTrendData(3, gameTrendData)
	--按列摆放
	local gameTrendData3 = self:dealData(data)

	--按界面列摆放
	local gameTrendDataByColumn3 = self:orderByColmn(gameTrendData3)

	return 	gameTrendData3, gameTrendDataByColumn3
end


--获得甲由路数据
function GameLogicRecordEx:getTrendData4(gameTrendData)

	if not gameTrendData then
		return {}, {}
	end
	local data = self:getTrendData(4, gameTrendData)
	--按列摆放

	local gameTrendData4 = self:dealData(data)
    --按界面列摆放

    local gameTrendDataByColumn4 = self:orderByColmn(gameTrendData4)


    return 	gameTrendData4, gameTrendDataByColumn4

end

function GameLogicRecordEx:dealData(dataTable)
	local tmp = {}
	local tmp1 = {}
	for i=1, #dataTable do
		if next(tmp1) ~= nil then
			if tmp1[1] ~= dataTable[i] then
				table.insert(tmp, tmp1)
				tmp1 = {}
			end
		end
		table.insert(tmp1, dataTable[i])
	end

	if next(tmp1) ~= nil then
		table.insert(tmp, tmp1)
	end

	return tmp
end

--获得趋势数据
function GameLogicRecordEx:getTrendData(n, gameTrendData)

	local data = {}
	--大路数据为空，大路第n列数据为空
	if next(gameTrendData) == nil or not gameTrendData[n] or next(gameTrendData[n]) == nil then
		return data
	end

	--大路第n列第二个为空并且第三列为空
	if gameTrendData[n][2] == nil and (not gameTrendData[n+1] or next(gameTrendData[n+1]) == nil) then
		return data
	end

	for column = n, #gameTrendData do
		for row=1, #gameTrendData[column] do
			if column == n and gameTrendData[n][2] == nil then
				break
			elseif column == n and row == 1 then
				--参照列第一行不记录到路单，置为无效列
				row = -1
			end

			if row == 1 then
			--看齐整
			 	if n == 3 then
			 		--小路
			 		if self:Neat(gameTrendData[column], gameTrendData[column-2]) then
						table.insert(data, Constant.ComparedResult.TigerWin)		 		
					else
						table.insert(data,  Constant.ComparedResult.DragonWin)
					end
			 	elseif n == 4 then
			 		--甲由路
			 		if self:Neat(gameTrendData[column], gameTrendData[column-3]) then
			 			table.insert(data, Constant.ComparedResult.TigerWin)
			 		else
						table.insert(data,  Constant.ComparedResult.DragonWin)
					end
			 	else
					if self:Neat(gameTrendData[column-1], gameTrendData[column-2]) then
						table.insert(data, Constant.ComparedResult.TigerWin)
					else
						table.insert(data,  Constant.ComparedResult.DragonWin)
					end
				end
			elseif row == 2 then
			    --看有无
				if gameTrendData[column-1][2] then
					table.insert(data, Constant.ComparedResult.TigerWin)
				else
					table.insert(data, Constant.ComparedResult.DragonWin)
				end
			elseif row ~= -1 and gameTrendData[column][row] == gameTrendData[column][1] then --过滤长龙
			    --看直落
				if row == 6 then
					if gameTrendData[column-1][row-1] then
						table.insert(data, Constant.ComparedResult.TigerWin)
					else
						table.insert(data, Constant.ComparedResult.DragonWin)
					end
				else
					if gameTrendData[column-1][row-1] and gameTrendData[column-1][row+1] then
						table.insert(data, Constant.ComparedResult.TigerWin)
					else
						table.insert(data, Constant.ComparedResult.DragonWin)
					end
				end

				--处理长龙
				local currentColumnIndex = column + 1
				while(gameTrendData[currentColumnIndex])
				do
					if gameTrendData[currentColumnIndex][row] ~= gameTrendData[column][row] then
						break
					end
					if row == 6 then
						if gameTrendData[currentColumnIndex-1][row-1] then
							table.insert(data, Constant.ComparedResult.TigerWin)
						else
							table.insert(data, Constant.ComparedResult.DragonWin)
						end
					else
						if gameTrendData[currentColumnIndex-1][row-1] and gameTrendData[currentColumnIndex-1][row+1] then
							table.insert(data, Constant.ComparedResult.TigerWin)
						else
							table.insert(data, Constant.ComparedResult.DragonWin)
						end
					end
					currentColumnIndex = currentColumnIndex + 1
				end
			end
		end
	end
	return data
end

--看齐整
function GameLogicRecordEx:Neat(tab1, tab2)
	local dragonCount1 = 0
	local tigerCount1 = 0
	local dragonCount2 = 0
	local tigerCount2 = 0
	for k,v in pairs(tab1) do
		if v == Constant.ComparedResult.DragonWin then
			dragonCount1 = dragonCount1 + 1
		elseif v == Constant.ComparedResult.TigerWin then
			tigerCount1 = tigerCount1 + 1
		end
	end

	for k,v in pairs(tab2) do
		if v == Constant.ComparedResult.DragonWin then
			dragonCount2 = dragonCount2 + 1
		elseif v == Constant.ComparedResult.TigerWin then
			tigerCount2 = tigerCount2 + 1
		end
	end

	local isNeat = false
	if (dragonCount1 == dragonCount2 and tigerCount1 == tigerCount2) or (dragonCount1 == tigerCount2 and tigerCount1 == dragonCount2) then
		isNeat = true
	end
	return isNeat
end

function GameLogicRecordEx:orderByColmn(dataTable)
	local tmp = {}
	--当前数据列 数据个数
	local maxRowIndex = 0
	--当前插入数据列的 列索引
	local currentInsertColumnIndex = 0
	--当前插入数据列的 行索引
	local currentInsertRowIndex = 0

	local maxColumn = #dataTable
	for columnIndex = 1, maxColumn do
		if not tmp[columnIndex] then
			tmp[columnIndex] = {}
		end
		currentInsertColumnIndex = columnIndex
		currentInsertRowIndex = 0
		maxRowIndex = #dataTable[columnIndex]
		for rowIndex = 1, maxRowIndex do
			--第一列不需要判断，直接填充
			if rowIndex == 1 then
				tmp[columnIndex][rowIndex] = dataTable[columnIndex][rowIndex]
			else
				if rowIndex > 6 then
					-- 超过最大行数, 形成长龙
					currentInsertColumnIndex = currentInsertColumnIndex + 1
					if not tmp[currentInsertColumnIndex] then
						tmp[currentInsertColumnIndex] = {}
					end
					--向 被占用后的填充行 后一列 填充
					if currentInsertRowIndex ~= 0 then
						tmp[currentInsertColumnIndex][currentInsertRowIndex] = dataTable[columnIndex][rowIndex]
					else
						--向 最大行 后一列 填充
						tmp[currentInsertColumnIndex][6] = dataTable[columnIndex][rowIndex]
					end
				elseif tmp[columnIndex][rowIndex] then
					--被占用,则向同一行后一列 填充
					if currentInsertRowIndex == 0 then
						currentInsertRowIndex = rowIndex - 1
					end
					currentInsertColumnIndex = currentInsertColumnIndex + 1

					if not tmp[currentInsertColumnIndex] then
						tmp[currentInsertColumnIndex] = {}
					end

					tmp[currentInsertColumnIndex][currentInsertRowIndex] = dataTable[columnIndex][rowIndex]
				else
					if currentInsertRowIndex == 0 then
					    --没有被占用 且 小于等于最大行数，则直接填充
						tmp[columnIndex][rowIndex] = dataTable[columnIndex][rowIndex]
					else
						--已经形成长龙，向当前长龙列 后一列填充
						currentInsertColumnIndex = currentInsertColumnIndex + 1
						if not tmp[currentInsertColumnIndex] then
							tmp[currentInsertColumnIndex] = {}
						end
						tmp[currentInsertColumnIndex][currentInsertRowIndex] = dataTable[columnIndex][rowIndex]
					end
				end
			end
		end
	end
	return tmp
end

function GameLogicRecordEx:ClearRecordData()
	--珠盘路数据
	self.ZPLuTrendData = {}
	--大路 数据
	self.gameTrendData1 = {}
	self.gameTrendDataByColumn1 = {}
	--大眼仔路 数据
	self.gameTrendData2 = {}
	self.gameTrendDataByColumn2 = {}
	--小路 数据
	self.gameTrendData3 = {}	
	self.gameTrendDataByColumn3 = {}
	--甲由路 数据
	self.gameTrendData4 = {}
	self.gameTrendDataByColumn4 = {}
end

return GameLogicRecordEx
local GameLogicRecord = {}
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")

--大路数据
local gameTrendData1 = {}

--获得大路数据
function GameLogicRecord.getTrendData1(gameData)
	local gameTrendData1 = {}             --按列摆放      
	local gameTrendDataByRow1 = {}        --按界面行摆放
	local gameTrendDataByColumn1 = {}     --按界面列摆放

	if next(gameData) == nil then
		return gameTrendData1, gameTrendDataByRow1, gameTrendDataByColumn1
	end
	gameTrendData1 = {}
	local tmp = {}

	gameData = clone(gameData)

	for i=#gameData,1,-1 do
		if gameData[i] == GAME_CMD.AREA_PING then 
			table.remove(gameData,i)
		end
	end


	for i=1, #gameData do
		if gameData[i] == GAME_CMD.AREA_PING then
		else 
			for j=1, #tmp do
				if tmp[j] == GAME_CMD.AREA_PING then
				elseif tmp[j] ~= gameData[i] then
					table.insert(gameTrendData1, tmp)
					tmp = {}
					break
				end
			end
		end
		table.insert(tmp, gameData[i])
	end

	if next(tmp) ~= nil then
		table.insert(gameTrendData1, tmp)
		tmp = {}
	end
	--按行摆放
	gameTrendDataByRow1 = GameLogicRecord.orderByRow(gameTrendData1)
	--按列摆放
	if next(gameTrendDataByRow1) == nil then
		gameTrendDataByColumn1 = {}
	else
		gameTrendDataByColumn1 = GameLogicRecord.orderByColmn(gameTrendDataByRow1)
	end
	return gameTrendData1, gameTrendDataByRow1, gameTrendDataByColumn1
end

--获得大眼仔路数据
function GameLogicRecord.getTrendData2(gameTrendDataByColumn1)
	local gameTrendData2 = {}             --按列摆放      
	local gameTrendDataByRow2 = {}        --按界面行摆放
	local gameTrendDataByColumn2 = {}     --按界面列摆放
	local data = {}

	data = GameLogicRecord.getTrendData(2, gameTrendDataByColumn1)
	gameTrendData2 = GameLogicRecord.dealData(data)
	gameTrendDataByRow2 = GameLogicRecord.orderByRow(gameTrendData2)
	if next(gameTrendDataByRow2) == nil then
		gameTrendDataByColumn2 = {}
	else
		gameTrendDataByColumn2 = GameLogicRecord.orderByColmn(gameTrendDataByRow2)
	end
	
	return 	gameTrendData2, gameTrendDataByRow2, gameTrendDataByColumn2, data
end

--获得小路数据
function GameLogicRecord.getTrendData3(gameTrendDataByColumn1)
	local gameTrendData3 = {}             --按列摆放      
	local gameTrendDataByRow3 = {}        --按界面行摆放
	local gameTrendDataByColumn3 = {}     --按界面列摆放
	local data = {}

	data = GameLogicRecord.getTrendData(3, gameTrendDataByColumn1)
	gameTrendData3 = GameLogicRecord.dealData(data)
	gameTrendDataByRow3 = GameLogicRecord.orderByRow(gameTrendData3)
	if next(gameTrendDataByRow3) == nil then
		gameTrendDataByColumn3 = {}
	else
		gameTrendDataByColumn3 = GameLogicRecord.orderByColmn(gameTrendDataByRow3)
	end
	
	return 	gameTrendData3, gameTrendDataByRow3, gameTrendDataByColumn3, data
end

--获得甲由路数据
function GameLogicRecord.getTrendData4(gameTrendDataByColumn1)
	local gameTrendData4 = {}             --按列摆放      
	local gameTrendDataByRow4 = {}        --按界面行摆放
	local gameTrendDataByColumn4 = {}     --按界面列摆放
	local data = {}

	data = GameLogicRecord.getTrendData(4, gameTrendDataByColumn1)
	gameTrendData4 = GameLogicRecord.dealData(data)
	gameTrendDataByRow4 = GameLogicRecord.orderByRow(gameTrendData4)
	if next(gameTrendDataByRow4) == nil then
		gameTrendDataByColumn4 = {}
	else
		gameTrendDataByColumn4 = GameLogicRecord.orderByColmn(gameTrendDataByRow4)
	end
	
	return 	gameTrendData4, gameTrendDataByRow4, gameTrendDataByColumn4, data
end


function GameLogicRecord.dealData(dataTable)
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
function GameLogicRecord.getTrendData(n, gameTrendDataByColumn1)
	local data = {}
	--大路数据为空，大路第n列数据为空
	if not gameTrendDataByColumn1 or next(gameTrendDataByColumn1) == nil or not gameTrendDataByColumn1[n] or next(gameTrendDataByColumn1[n]) == nil then
		return data
	end

	--大路第n列第二个为空并且第三列为空
	if gameTrendDataByColumn1[n][2] == nil and (not gameTrendDataByColumn1[n+1] or next(gameTrendDataByColumn1[n+1]) == nil) then
		return data
	end
	for column = n, #gameTrendDataByColumn1 do
		for row=1, #gameTrendDataByColumn1[column] do
			if column == n and gameTrendDataByColumn1[n][2] == nil then
				break
			elseif column == n then
				row = 2
			end

			if row == 1 then
			--看齐整
			 	if n == 3 then
			 		--小路
			 		if GameLogicRecord.Neat(gameTrendDataByColumn1[column], gameTrendDataByColumn1[column-2]) then
						table.insert(data, GAME_CMD.AREA_HU)
					else
						table.insert(data,  GAME_CMD.AREA_LONG)
					end
			 	elseif n == 4 then
			 		--甲由路
			 		if GameLogicRecord.Neat(gameTrendDataByColumn1[column], gameTrendDataByColumn1[column-3]) then
						table.insert(data, GAME_CMD.AREA_HU)
					else
						table.insert(data,  GAME_CMD.AREA_LONG)
					end
			 	else
					if GameLogicRecord.Neat(gameTrendDataByColumn1[column-1], gameTrendDataByColumn1[column-2]) then
						table.insert(data, GAME_CMD.AREA_HU)
					else
						table.insert(data,  GAME_CMD.AREA_LONG)
					end
				end
			elseif row == 2 then
			--看有无
				if gameTrendDataByColumn1[column-1][2] then
					table.insert(data, GAME_CMD.AREA_HU)
				else
					table.insert(data, GAME_CMD.AREA_LONG)
				end
			else
			--看直落
				if row == 6 then
					if not gameTrendDataByColumn1[column-1][row-1] then
						table.insert(data, GAME_CMD.AREA_HU)
					else
						table.insert(data, GAME_CMD.AREA_LONG)
					end
				else
					if not gameTrendDataByColumn1[column-1][row-1] and not gameTrendDataByColumn1[column-1][row+1] then
						table.insert(data, GAME_CMD.AREA_HU)
					else
						table.insert(data, GAME_CMD.AREA_LONG)
					end
				end
			end
		end
	end
	return data
end

--看齐整
function GameLogicRecord.Neat(tab1, tab2)
	local count1 = 0
	local count2 = 0

	for i=1, #tab1 do
		if tab1[i] ~= GAME_CMD.AREA_PING then
			count1 = count1 + 1
		end
	end

	for i=1, #tab2 do
		if tab2[i] ~= GAME_CMD.AREA_PING then
			count2 = count2 + 1
		end
	end

	return count1 == count2
end

function GameLogicRecord.orderByRow(dataTable)
	local tmp = {}
	local row = 1
	local column = 1
	for i=1, #dataTable do
		local row = 1
		for j=1, #dataTable[i] do
			if row > 6 then
				row = 6
			end

			if not tmp[row] then
			    tmp[row] = {}
			end 

			local k = column 
			while true do
				if tmp[row][k] == nil then
					tmp[row][k] = dataTable[i][j]

					if k == column then
						row = row + 1
					end
					break
				else
					k = k + 1
				end
			end
		end
		column = column + 1
	end
	return tmp
end

function GameLogicRecord.orderByColmn(dataTableByRow)
	local tmp = {}
	local num = 0
	for m, n in pairs(dataTableByRow) do
		for k, v in pairs(n) do
			if num < k then
				num = k
			end
		end
	end

	for i=1, num do
		tmp[i] = {}
	end

	for m, n in pairs(dataTableByRow) do
		for k, v in pairs (n) do
			tmp[k][m] = v
		end
	end
	return tmp
end



return GameLogicRecord
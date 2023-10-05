local GameLogic = GameLogic or {}

--牌值掩码
GameLogic.MASK_VALUE			= 0X0F
--花色掩码
GameLogic.MASK_COLOR			= 0XF0
--最大手牌数目
GameLogic.MAX_CARDCOUNT			= 20
--牌库数目
GameLogic.FULL_COUNT			= 54
--正常手牌数目
GameLogic.NORMAL_COUNT			= 17

--排序类型
--大小排序
GameLogic.ST_ORDER				= 1
--数目排序
GameLogic.ST_COUNT				= 2
--自定排序
GameLogic.ST_CUSTOM				= 3

local RealVal = 
{
	[1] = 14,
	[2] = 2,
	[2] = 2,
	[2] = 2,
	[2] = 2,
	[2] = 2,
	[2] = 2,
	[2] = 2,
	[2] = 2,
	[2] = 2,
}
------------------------------------------------------------------
--类型函数

--获取类型
--param[cbCardDataTable] 扑克数据table
function GameLogic.GetCardType( cbCardDataTable )
	
end

--获取类型
function GameLogic.GetBackCardType( cbCardDataTable )
	-- body
end

--获取数值
function GameLogic.GetCardValue( cbCardData )
	return bit:_and(cbCardData, GameLogic.MASK_VALUE);
end

--获取花色
function GameLogic.GetCardColor( cbCardData )
	return bit:_and(cbCardData, GameLogic.MASK_COLOR) / 16;
end
------------------------------------------------------------------

------------------------------------------------------------------
function GameLogic:analyseCard(cards)
	if #cards ~= 5 then
		print(" 手牌数据错误 ")
		return
	end

	local color = {}
	local valList = {}

	for i, data in pairs(cards) do
		dump(data,"data")
		local clr = self.GetCardColor(data)
		if not color[clr] then
			color[clr] = {data}
		else
			table.insert(color[clr], data)
		end

		local val = self.GetCardValue(data)
		if not valList[val] then
			valList[val] = {data}
		else
			table.insert(valList[val], data)
		end
	end
	return color, valList
end

function GameLogic:GetCardType(cards)
	local clrList, valList = self:analyseCard(cards)

	-- 同花判断
	for k,v in pairs(clrList) do
		if #v == 5 then
			return cards
		end
	end

	local thr = {}
	local sec = {}
	local single = {}
	for val,dataList in pairs(valList) do
		if #dataList == 4 then
			return dataList
		elseif #dataList == 3 then
			thr = dataList
		elseif #dataList == 2 then
			sec[val] = dataList
		else
			table.insert(single, dataList[1])
		end
	end

	
	if #sec == 1 then
		if #thr ~= 0 then
			return cloneAB(thr,sec)
		else
			for val, data in pairs(sec) do
				if val >= 10 or val == 1 then
					return data
				end
			end
		end
	elseif #sec == 2 then
		local res = {}
		for val, data in pairs(sec) do
			res = cloneAB(data,res)
		end
		return res
	else
		if #thr ~= 0 then
			return thr
		end
	end

	if #single == 5 then
		if self:isSunZi(cards) then
			return cards
		end
	end
	return false
end

function GameLogic:isSunZi(cards)
	table.sort(cards,function(a,b)
		if b then
			local v1 = self.GetCardValue(a) == 1 and 14 or self.GetCardValue(a)
			local v2 = self.GetCardValue(b) == 1 and 14 or self.GetCardValue(b)
			return v1 < v2
		end
		return false
	end)
	local val1 = self.GetCardValue(cards[1])
	local val5 = self.GetCardValue(cards[5]) == 1 and 14 or self.GetCardValue(cards[5])
	return (val5 - val1) == 4
end

function GameLogic:cloneAB(a,b)
	local result = clone(a)
	for k,v in pairs(b) do
		if type(v) == "table" then
			for k,_v in pairs(v) do
				table.insert(result,_v)
			end
		else
			table.insert(result, v)
		end
	end
	return result
end

------------------------------------------------------------------

return GameLogic;
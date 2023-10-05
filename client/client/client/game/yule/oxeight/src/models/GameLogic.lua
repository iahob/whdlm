local GameLogic = {}


--**************    扑克类型    ******************--
--混合牌型
GameLogic.OX_VALUEO				= 0
--三条牌型
GameLogic.OX_THREE_SAME			= 102
--四条牌型
GameLogic.OX_FOUR_SAME			= 103
--天王牌型
GameLogic.OX_FOURKING			= 104
--天王牌型
GameLogic.OX_FIVEKING			= 105


--最大手牌数目
GameLogic.MAX_CARDCOUNT			= 5
--牌库数目
GameLogic.FULL_COUNT			= 52
--正常手牌数目
GameLogic.NORMAL_COUNT			= 5

GameLogic.KingValue				= 20

--经典模式
GameLogic.CT_CLASSIC_OX_VALUE0				= 0									--无牛
GameLogic.CT_CLASSIC_OX_VALUE1				= 1									--牛一
GameLogic.CT_CLASSIC_OX_VALUE2				= 2									--牛二
GameLogic.CT_CLASSIC_OX_VALUE3				= 3									--牛三
GameLogic.CT_CLASSIC_OX_VALUE4				= 4									--牛四
GameLogic.CT_CLASSIC_OX_VALUE5				= 5									--牛五
GameLogic.CT_CLASSIC_OX_VALUE6				= 6									--牛六
GameLogic.CT_CLASSIC_OX_VALUE7				= 7									--牛七
GameLogic.CT_CLASSIC_OX_VALUE8				= 8									--牛八
GameLogic.CT_CLASSIC_OX_VALUE9				= 9									--牛九
GameLogic.CT_CLASSIC_OX_VALUENIUNIU			= 10								--牛牛

GameLogic.CT_CLASSIC_OX_VALUE_FOURFLOWER	= 11								--四花牛
GameLogic.CT_CLASSIC_OX_VALUE_FIVEFLOWER	= 12								--五花牛
GameLogic.CT_CLASSIC_OX_VALUE_SHUNZI		= 13								--顺子
GameLogic.CT_CLASSIC_OX_VALUE_SAMEFLOWER	= 14								--同花
GameLogic.CT_CLASSIC_OX_VALUE_HULU			= 15								--葫芦
GameLogic.CT_CLASSIC_OX_VALUE_BOMB			= 16								--炸弹
GameLogic.CT_CLASSIC_OX_VALUE_TONGHUASHUN	= 17								--同花顺
GameLogic.CT_CLASSIC_OX_VALUE_FIVESNIUNIU	= 18								--五小牛

--加倍模式
GameLogic.CT_ADDTIMES_OX_VALUE0				= 0									--无牛
GameLogic.CT_ADDTIMES_OX_VALUE1				= 1									--牛一
GameLogic.CT_ADDTIMES_OX_VALUE2				= 2									--牛二
GameLogic.CT_ADDTIMES_OX_VALUE3				= 3									--牛三
GameLogic.CT_ADDTIMES_OX_VALUE4				= 4									--牛四
GameLogic.CT_ADDTIMES_OX_VALUE5				= 5									--牛五
GameLogic.CT_ADDTIMES_OX_VALUE6				= 6									--牛六
GameLogic.CT_ADDTIMES_OX_VALUE7				= 7									--牛七
GameLogic.CT_ADDTIMES_OX_VALUE8				= 8									--牛八
GameLogic.CT_ADDTIMES_OX_VALUE9				= 9									--牛九
GameLogic.CT_ADDTIMES_OX_VALUENIUNIU		= 10								--牛牛

GameLogic.CT_ADDTIMES_OX_VALUE_SHUNZI		= 11								--顺子
GameLogic.CT_ADDTIMES_OX_VALUE_SAMEFLOWER	= 12								--同花
GameLogic.CT_ADDTIMES_OX_VALUE_HULU			= 13								--葫芦
GameLogic.CT_ADDTIMES_OX_VALUE_FOURFLOWER	= 14								--四花牛
GameLogic.CT_ADDTIMES_OX_VALUE_FIVEFLOWER	= 15								--五花牛
GameLogic.CT_ADDTIMES_OX_VALUE_FIVESNIUNIU	= 16								--五小牛
GameLogic.CT_ADDTIMES_OX_VALUE_BOMB			= 17								--炸弹
GameLogic.CT_ADDTIMES_OX_VALUE_TONGHUASHUN	= 18								--同花顺


GameLogic.CT_OX_VALUE0  					= 0
GameLogic.CT_OX_VALUE1  					= 1
GameLogic.CT_OX_VALUE2  					= 2
GameLogic.CT_OX_VALUE3  					= 3
GameLogic.CT_OX_VALUE4  					= 4
GameLogic.CT_OX_VALUE5  					= 5
GameLogic.CT_OX_VALUE6  					= 6
GameLogic.CT_OX_VALUE7  					= 7
GameLogic.CT_OX_VALUE8  					= 8
GameLogic.CT_OX_VALUE9  					= 9
GameLogic.CT_OX_NIUNIU                      = 10


--经典模式
GameLogic.CT_OX_CLASSIC_LIST = 
{
 "无牛",
 "牛一",
 "牛二",
 "牛三",
 "牛四",
 "牛五",
 "牛六",
 "牛七",
 "牛八",
 "牛九",
 "牛牛",
 "四花牛",
 "五花牛",
 "顺子",
 "同花",
 "葫芦",
 "炸弹",
 "同花顺",
 "五小牛"
}

--加倍模式
GameLogic.CT_OX_ADDTIME_LIST = 
{
 "无牛",
 "牛一",
 "牛二",
 "牛三",
 "牛四",
 "牛五",
 "牛六",
 "牛七",
 "牛八",
 "牛九",
 "牛牛",
 "顺子",
 "同花",
 "葫芦",
 "四花牛",
 "五花牛",
 "五小牛",
 "炸弹",
 "同花顺",
}
--取模
function GameLogic:mod(a,b)
    if a and b then
    	return a - math.floor(a/b)*b
    else
    	return false
    end
    
end
--获得牌的数值（1 -- 13）
function GameLogic:getCardValue(cbCardData)
    return self:mod(cbCardData, 16)
end

--获得牌的颜色（0 -- 4）
function GameLogic:getCardColor(cbCardData)
    return math.floor(cbCardData/16)
end
--获得牌的逻辑值
function GameLogic:getCardLogicValue(cbCardData)

	local cbCardValue = self:getCardValue(cbCardData)
	if cbCardData == 0x4E or cbCardData == 0x4F then --王百搭
		cbCardValue = GameLogic.KingValue
		return cbCardValue
	end
	

	if cbCardValue > 10 then
		cbCardValue = 10
	end

	return cbCardValue
end


function GameLogic:sortCardList(data)
	--转换数值
	local  cbSortValue={}
	for i=1,#data do
		cbSortValue[i]=self:getCardLogicValue(data[i])
	end 	

	--排序操作
	local  bSorted=false
	local cbSwitchData=0
	local cbLast=#data -1

	while bSorted==false do
	
		bSorted=true
		for i=1,cbLast do
			
			if (cbSortValue[i]>cbSortValue[i+1]) or ((cbSortValue[i]==cbSortValue[i+1]) and (data[i]>data[i+1])) then
			
				--设置标志
				bSorted=false

				--扑克数据
				cbSwitchData=data[i]
				data[i]=data[i+1]
				data[i+1]=cbSwitchData

				--排序权位
				cbSwitchData=cbSortValue[i]
				cbSortValue[i]=cbSortValue[i+1]
				cbSortValue[i+1]=cbSwitchData
			end	
		end
		cbLast = cbLast -1
	end
end


--拷贝表
function GameLogic:copyTab(st)
    local tab = {}
    for k, v in pairs(st) do
        if type(v) ~= "table" then
            tab[k] = v
        else
            tab[k] = self:copyTab(v)
        end
    end
    return tab
 end
--获取牛牛
function GameLogic:getOxCard(data)

	local cbCardData = clone(data)
	
	--王百搭
	local bKing = false
	for i=1,#cbCardData do
		local value = self:getCardLogicValue(cbCardData[i])
		if value == GameLogic.KingValue then 
			bKing = true
			break
		end
	end

	if true == bKing then
		return true
	end

	local bTemp = {}
	local bTempData = clone(cbCardData)
	local bSum = 0
	for i = 1, 5 do
		bTemp[i] = self:getCardLogicValue(cbCardData[i])
		bSum = bSum + bTemp[i]
	end

	for i = 1, 5 - 1 do
		for j = i + 1, 5 do
			if self:mod(bSum - bTemp[i] - bTemp[j], 10) == 0 then
				local bCount = 1
				for k = 1, 5 do
					if k ~= i and k ~= j then
						cbCardData[bCount] = bTempData[k]
						bCount = bCount + 1
					end
				end
				cbCardData[4] = bTempData[i]
				cbCardData[5] = bTempData[j]
				return true
			end
		end
	end

	return false
end


--获取牛牛
function GameLogic:getAutoOxCard(cbCardData)


	--王牌统计
	local  cbKingCount = 0;
	for  i = 1,GameLogic.MAX_CARDCOUNT do
	
		if (cbCardData[i] == 0x4E or  cbCardData[i] == 0x4F) then
		
			cbKingCount = cbKingCount +1
		end
	end

	if cbKingCount == 2 then
	
		--拷贝扑克
		local  cbTempData = clone(cbCardData)
		self:sortCardList(cbTempData)

		cbCardData[1] = cbTempData[1]
		cbCardData[2] = cbTempData[2]
		cbCardData[3] = cbTempData[4]
		cbCardData[4] = cbTempData[3]
		cbCardData[5] = cbTempData[5]

		return true
	
	elseif cbKingCount == 1 then
	
		--拷贝扑克
		local  cbTempData = clone(cbCardData)
		self:sortCardList(cbTempData)

		local bTemp = {}
		local bSum = 0
		for i = 1, 4 do
			bTemp[i] = self:getCardLogicValue(cbTempData[i])
			bSum = bSum + bTemp[i]
		end

		
		--三张成十
		for i = 1, 4 do
		
			if ((bSum - bTemp[i])%10 == 0) then
			
				local  cbCount = 0
				for j=1 ,4 do
				
					if i ~= j then
						
						cbCount =cbCount+1
						cbCardData[cbCount] = cbTempData[j]

					end
				end
				

				cbCardData[4] = cbTempData[i]
				cbCardData[5] = cbTempData[5]

				return true
			end
		end

		--两张成十
		for i = 1,3 do
		
			for j = i + 1,4 do
			
				if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0) then
				
					
					cbCardData[1] = cbTempData[5]
					cbCardData[2] = cbTempData[i]
					cbCardData[3] = cbTempData[j]

					local cbCount = 3

					for k = 1,4 do
					
						if (i ~= k and  j ~= k) then
							cbCount = cbCount +1
							cbCardData[cbCount] = cbTempData[k];
						end
					end

					return true
				end
			end
		end

		--四中取大
		local  cbBigCount = 0
		local  cbBigIndex1 = 0
		local  cbBigIndex2 = 0
		for i = 1 ,3 do
		
			for  j = i + 1,4 do
			
				local  cbSumCount = (bTemp[i] + bTemp[j]) % 10
				if cbSumCount > cbBigCount then
				
					cbBigIndex1 = i
					cbBigIndex2 = j
					cbBigCount = cbSumCount
				end
			end
		end

		local  cbCount = 0
		for  i = 1,4 do
		
			if i ~= cbBigIndex1 and  i ~= cbBigIndex2 then
			
				cbCount = cbCount+1
				cbCardData[cbCount] = cbTempData[i]
			end
		end

		cbCardData[3] = cbTempData[5]
		cbCardData[4] = cbTempData[cbBigIndex1]
		cbCardData[5] = cbTempData[cbBigIndex2]

		return true
	
	elseif  cbKingCount == 0 then
	
		local bTemp = {}
		local bTempData = self:copyTab(cbCardData)
		local bSum = 0
		for i = 1, 5 do
			bTemp[i] = self:getCardLogicValue(cbCardData[i])
			bSum = bSum + bTemp[i]
		end

		for i = 1, 5 - 1 do
			for j = i + 1, 5 do
				if self:mod(bSum - bTemp[i] - bTemp[j], 10) == 0 then
					local bCount = 1
					for k = 1, 5 do
						if k ~= i and k ~= j then
							cbCardData[bCount] = bTempData[k]
							bCount = bCount + 1
						end
					end
					cbCardData[4] = bTempData[i]
					cbCardData[5] = bTempData[j]
					return true
				end
			end
		end
	end

	return false
end


--获取类型(特殊牌型服务器下发)
function GameLogic:getCardType(cbCardData)
	--cbCardData 5张扑克数据，前3张是手动选的
	local data = clone(cbCardData)
	local bKing = false

	for i=1,3 do
		local logicValue = self:getCardLogicValue(data[i])
 		if logicValue == GameLogic.KingValue  then
 			bKing = true
 			break
 		end
	end

	--判断前三张是否为10的倍数,若前三张中有王,必为整数
	local threeCount = self:getCardLogicValue(data[1])+self:getCardLogicValue(data[2])+self:getCardLogicValue(data[3])
	if (math.mod(threeCount,10) ~= 0) and (not bKing) then
		return GameLogic.CT_OX_VALUE0
	end

	--继续判断后2张中是否有KING
	bKing = false
 	for i=4,5 do
 		local logicValue = self:getCardLogicValue(data[i])
 		if logicValue == GameLogic.KingValue  then
 			bKing = true
 			break
 		end
 	end

 	if bKing then --只要有一个KING即为牛牛类型
 		return  GameLogic.CT_OX_NIUNIU
 	else
 		local oxType = math.mod(self:getCardLogicValue(data[4])+self:getCardLogicValue(data[5]),10)	
 		if oxType == 0 then
 			return  GameLogic.CT_OX_NIUNIU
 		else
 			return 	oxType
 		end
 	end


 	return GameLogic.CT_OX_VALUE0

	--[[for i = 1, 5 do
		if self:getCardValue(cbCardData[i]) > 10 then
			bKingCount = bKingCount + 1
		elseif self:getCardValue(cbCardData[i]) == 10 then
			bTenCount = bTenCount + 1
		end
	end
	if bKingCount == 5 then
		return GameLogic.OX_FIVEKING
	elseif bKingCount == 4 and bTenCount == 1 then
		return GameLogic.OX_FOURKING
	end

	local bTemp = {}
	local bSum = 0
	for i = 1, 5 do
		bTemp[i] = self:getCardLogicValue(cbCardData[i])
		bSum = bSum + bTemp[i]
	end

	for i = 1, 4 do
		for j = i + 1, 5 do
			if self:mod(bSum - bTemp[i] - bTemp[j], 10) == 0 then
				return bTemp[i] + bTemp[j] > 10 and bTemp[i] + bTemp[j] - 10 or bTemp[i] + bTemp[j]
			end
		end
	end

	return GameLogic.OX_VALUEO]]
end


-- 判断四张牌时的牌型
function GameLogic:analyseCardType(cbCardData)
	local data = {}
	local bKing = false

	for k , v in pairs(cbCardData) do
		if v > 0 then
			table.insert(data, v)
			local logicValue = self:getCardLogicValue(v)
			if logicValue == GameLogic.KingValue  then
	 			bKing = true
	 		end
		end
	end

	if #data ~= 4 then
		print("**********判断四张牌时的牌型------牌数据不对")
		return
	end

    if bKing then
    	local ox = 0
    	for i= 1, 4 do
    		if self:getCardLogicValue(data[i]) ~= GameLogic.KingValue then
				if ox < self:getCardLogicValue(data[i]) then
					ox = self:getCardLogicValue(data[i])
				end
			end
		end
		print("****************bKing******牛",ox)
		return ox
	end

	for i = 1, 4 do
		local ox = self:getCardLogicValue(data[i])
		local value = 0
		for k = 1, 4 do
			if k ~= i then
				value = self:getCardLogicValue(data[k]) + value
			end
		end
		if value > 0 and math.mod(value,10) == 0 then
			print("**********************牛",ox)
			return ox
		end
	end
    print("************************没牛")
	return GameLogic.CT_OX_VALUE0

end

return GameLogic
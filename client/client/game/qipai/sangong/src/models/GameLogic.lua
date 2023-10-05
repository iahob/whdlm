local GameLogic = {}

GameLogic.m_cbCardListData = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, -- 方块
                               0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, -- 梅花
                               0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, -- 红桃
                               0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, -- 黑桃
                             }

--数值掩码
GameLogic.LOGIC_MASK_COLOR			= 0xF0								--花色掩码
GameLogic.LOGIC_MASK_VALUE			= 0x0F								--数值掩码
GameLogic.MAX_COUNT					= 3                                 --最大数目

--扑克类型
GameLogic.OX_VALUE0					= 0									--混合牌型
GameLogic.OX_MIX_THREE_KING			= 11								--混三公牌型
GameLogic.OX_SMALL_THREE_KING		= 12								--小三公牌型
GameLogic.OX_BIG_THREE_KING			= 13								--大三公牌型

--分析结构
GameLogic.tagAnalyseResult = 
{
    {t = "byte", k = "cbFourCount"},					                --四张数目
    {t = "byte", k = "cbThreeCount"},					                --三张数目
    {t = "byte", k = "cbDoubleCount"},					                --两张数目
    {t = "byte", k = "cbSignedCount"},					                --单张数目
    {t = "byte", k = "cbFourLogicVolue", l = {1}},			            --四张列表
    {t = "byte", k = "cbThreeLogicVolue", l = {1}},		                --三张列表
    {t = "byte", k = "cbDoubleLogicVolue", l = {2}},		            --两张列表
    {t = "byte", k = "cbSignedLogicVolue", l = {5}},		            --单张列表
    {t = "byte", k = "cbFourCardData", l = {GameLogic.MAX_COUNT}},		    --四张列表
    {t = "byte", k = "cbThreeCardData", l = {GameLogic.MAX_COUNT}},		    --三张列表
    {t = "byte", k = "cbDoubleCardData", l = {GameLogic.MAX_COUNT}},		    --两张列表
    {t = "byte", k = "cbSignedCardData", l = {GameLogic.MAX_COUNT}},		    --单张数目
}

-- 获取牌值
function GameLogic:getCardValue(cbCardData)
    return bit:_and(cbCardData, 0X0F)    --数值掩码
end

-- 获取花色
function GameLogic:getCardColor(cbCardData)
    return bit:_and(cbCardData, 0XF0)    --花色掩码
end

--获取类型
function GameLogic:getCardType(cbCardData, cbCardCount)
    assert(cbCardCount == GameLogic.MAX_COUNT)

	--炸弹牌型
	local bSameCount = 0
	local cbLogicValue = GameLogic:sortCardList(cbCardData,cbCardCount)

	local cbKingCount = 0
	for i = 1,cbCardCount do
		if( GameLogic:getCardValue(cbLogicValue[i]) > 10 ) then
			cbKingCount = cbKingCount + 1
		end		
	end
	local cbSameCount = 1
	for i = 1,cbCardCount do
		if( GameLogic:getCardValue(cbLogicValue[i]) == GameLogic:getCardValue(cbLogicValue[(i == 3 and 0 or i)+1]) ) then
			cbSameCount = cbSameCount + 1
		end		
	end

	if(cbKingCount == GameLogic.MAX_COUNT and cbSameCount == GameLogic.MAX_COUNT) then return GameLogic.OX_BIG_THREE_KING end
	if(cbKingCount == GameLogic.MAX_COUNT and cbSameCount ~= GameLogic.MAX_COUNT) then return GameLogic.OX_MIX_THREE_KING end
	if(cbKingCount ~= GameLogic.MAX_COUNT and cbSameCount == GameLogic.MAX_COUNT) then return GameLogic.OX_SMALL_THREE_KING end
	

	local bTemp = {}
	local bSum = 0
	for i=1,cbCardCount do
        table.insert(bTemp, GameLogic:getCardLogicValue(cbLogicValue[i]))
		bSum = bSum + bTemp[i]
	end	
	return math.mod(bSum,10)
end

--获取倍数
function GameLogic:getTimes(cbCardData, cbCardCount)
	if (cbCardCount ~= GameLogic.MAX_COUNT) then return 0 end

	local bTimes = GameLogic:getCardType(cbCardData,GameLogic.MAX_COUNT)
	if (bTimes < 8) then return 1
	elseif (bTimes == 8) then return 2
	elseif (bTimes == 9) then return 3
	elseif (bTimes == 11) then return 5
	elseif (bTimes == 12) then return 7
	elseif (bTimes == 13) then return 9
    end
	return 1
end

--获取整数
function GameLogic:getAllCardValue(cbCardData, cbCardCount)
	local sum = 0
	for i=1,cbCardCount do
		sum = sum + GameLogic:getCardLogicValue(cbCardData[i])
	end
	assert(sum > 0)
	return math.mod(sum,10)
end

--排列扑克
function GameLogic:sortCardList(cbCardData, cbCardCount)
	--转换数值
	local cbLogicValue = {}
    for i = 1,cbCardCount do
        table.insert(cbLogicValue, GameLogic:getCardValue(cbCardData[i]))
    end

    table.sort(cbLogicValue,function(a,b) return a>b end)
--    dump(cbLogicValue)
	--排序操作
--	local bSorted=false
--	local cbTempData,bLast=cbCardCount,cbCardCount

--    while(not bSorted)
--    do
--        bSorted=true
--		for i = 1,bLast do
--            if i > (bLast-1) then return end
--			if ((cbLogicValue[i] < cbLogicValue[i+1]) or ((cbLogicValue[i] == cbLogicValue[i+1]) and (cbCardData[i] < cbCardData[i+1]))) then
--				--交换位置
--				cbTempData = cbCardData[i];
--				cbCardData[i] = cbCardData[i+1];
--				cbCardData[i+1] = cbTempData;
--				cbTempData = cbLogicValue[i];
--				cbLogicValue[i] = cbLogicValue[i+1];
--				cbLogicValue[i+1] = cbTempData;
--				bSorted = false;
--			end
--		end
--		bLast = bLast - 1
--    end
    return cbLogicValue
end

--逻辑数值
function GameLogic:getCardLogicValue(cbCardData)
	--扑克属性
	local bCardColor = GameLogic:getCardColor(cbCardData)
	local bCardValue = GameLogic:getCardValue(cbCardData)

	--转换数值
	return ((bCardValue>10) and (10) or bCardValue)
end

--获取公牌数量
function GameLogic:getFaceCardCount(cbCardData, cbCardCount)
	local cbCardNum = 0
	for i = 1,cbCardCount do
		if(GameLogic:getCardValue(cbCardData[i]) > 10) then cbCardNum = cbCardNum + 1 end
	end
	return cbCardNum
end

--对比扑克,为true则闲家赢，为false则庄家赢,First为闲家，Next为庄家
function GameLogic:compareCard(cbFirstData, cbNextData, cbCardCount)
	--获取点数
	local cbNextType = GameLogic:getCardType(cbNextData,cbCardCount)
	local cbFirstType = GameLogic:getCardType(cbFirstData,cbCardCount)

	if (cbNextType ~= cbFirstType) then
		return ((cbFirstType > cbNextType) and true or false)
	else
		--排序大小
		local bFirstTemp = GameLogic:sortCardList(bFirstTemp,cbCardCount)
		local bNextTemp = GameLogic:sortCardList(bNextTemp,cbCardCount)

		--比较公牌数量
		local cbFirstFaceCardNum = GameLogic:getFaceCardCount(bFirstTemp,cbCardCount)
		local cbNextFaceCardNum  = GameLogic:getFaceCardCount(bNextTemp,cbCardCount)

		--比较数值
		local cbNextMaxValue = GameLogic:getCardValue(bNextTemp[1])
		local cbFirstMaxValue = GameLogic:getCardValue(bFirstTemp[1])

		--都是混三公
		if(cbNextType == GameLogic.OX_MIX_THREE_KING and cbFirstType == GameLogic.OX_MIX_THREE_KING) then
			--比较最大牌大小
			if (cbFirstMaxValue > cbNextMaxValue) then return true 
			elseif(cbFirstMaxValue < cbNextMaxValue) then return false
			elseif(cbFirstMaxValue == cbNextMaxValue) then return ((GameLogic:getCardColor(bFirstTemp[1]) > GameLogic:getCardColor(bNextTemp[1])) and true or false) end			
		else
			--比较公牌数
			if(cbFirstFaceCardNum > cbNextFaceCardNum) then return true
			elseif(cbFirstFaceCardNum < cbNextFaceCardNum) then return false
			elseif(cbFirstFaceCardNum == cbNextFaceCardNum) then
				--比较最大牌大小
				if (cbFirstMaxValue > cbNextMaxValue) then return true
				elseif(cbFirstMaxValue < cbNextMaxValue) then return false
				elseif(cbFirstMaxValue == cbNextMaxValue) then return ((GameLogic:getCardColor(bFirstTemp[1]) > GameLogic:getCardColor(bNextTemp[1])) and true or false) end
			end			
		end			
	end
	return false
end

return GameLogic
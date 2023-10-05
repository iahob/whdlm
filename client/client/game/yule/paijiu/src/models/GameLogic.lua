local GameLogic={}

GameLogic.cardListData=
{
	0x66,0x66,0x11,0x11,0x44,0x44,0x13,0x13,0x55,0x55,0x33,
	0x33,0x22,0x22,0x56,0x56,0x46,0x46,0x16,0x16,0x15,0x15,
	0x36,0x45,0x35,0x26,0x34,0x25,0x32,0x14,0x24,0x12
}

local m_bCardListData=
{
	0x66,0x66,0x11,0x11,0x44,0x44,0x13,0x13,0x55,0x55,0x33,
	0x33,0x22,0x22,0x56,0x56,0x46,0x46,0x16,0x16,0x15,0x15,
	0x36,0x45,0x35,0x26,0x34,0x25,0x32,0x14,0x24,0x12
}


--扑克类型
local CT_P9_ZHIZHU		=			24								--至尊宝
local CT_P9_DUITIAN		=			23								--双天
local CT_P9_DUIDI		=				22								--双地
local CT_P9_DUIREN		=			21								--双人
local CT_P9_DUIE		=				20								--双鹅

local CT_P9_MEIPAI		=			19								--双梅
local CT_P9_CHANGSAN	=				18								--双长三
local CT_P9_BANDENG		=			17								--双板凳

local CT_P9_GAOJIAOQI	=				16								--双高脚
local CT_P9_TONGCHUILIU	=			15								--双零霖
local CT_P9_FUTOU		=				14								--双斧头
local CT_P9_HONGSHI		=			13								--双红头

local CT_P9_ZAWU		=				12								--杂五
local CT_P9_ZAQI		=				11								--杂七
local CT_P9_ZABA		=				10								--杂八
local CT_P9_ZAJIU		=				9								--杂九

local CT_P9_TIANWANG	=				8								--天王
local CT_P9_DIWANG		=			7								--地王
local CT_P9_TIANGANG	=				6								--天杠
local CT_P9_DIGANG		=			5								--地杠
local CT_P9_TIANGAOJIU	=			4								--天高九
local CT_P9_DIGAOJIU	=				3								--地高九
local CT_P9_NORMAL		=			2								--散牌
GameLogic.CT_P9_NORMAL=CT_P9_NORMAL
local CT_P9_BIESHI		=			1								--憋十
local CT_P9_INVALID		=			0								--无效

--数值掩码
local	LOGIC_MASK_COLOR	=			0xF0							--花色掩码
local	LOGIC_MASK_VALUE	=			0x0F							--数值掩码

--扑克数目
local MAX_CARD_COUNT		=			32								--扑克数目

--比较结果
local COMPCARD_WIN			=		1								--比较胜利
local COMPCARD_EQUAL		=			0								--比较相同
local COMPCARD_LOSE			=		-1								--比较失败

local INVALID_INDEX =255



--获取索引
function GameLogic.GetCardIndex( bCardData)

	for i=1,MAX_CARD_COUNT do
		if bCardData==m_bCardListData[i] then
			return i
		end
	end
	return INVALID_INDEX
end

--获取类型
function GameLogic.GetCardType( bCardData,  bCardCount)

	--数据校验
	assert(bCardCount==2)
	if(bCardCount~=2) then return CT_P9_INVALID end

	--牌组排序
	local bTmpCardData= {}
	bTmpCardData[1] = (GameLogic.GetCardIndex(bCardData[1]) <= GameLogic.GetCardIndex(bCardData[2]) and bCardData[1] or bCardData[2])
	bTmpCardData[2] = (GameLogic.GetCardIndex(bCardData[1]) < GameLogic.GetCardIndex(bCardData[2]) and bCardData[2] or bCardData[1])

	--牌型判断
	if (bTmpCardData[1] == 0x24 and bTmpCardData[2] == 0x12) then	--至尊
		return CT_P9_ZHIZHU
	elseif (bTmpCardData[1] == 0x66 and bTmpCardData[2] == 0x66) then	--对天
		return CT_P9_DUITIAN
	elseif (bTmpCardData[1] == 0x11 and bTmpCardData[2] == 0x11) then	--对地
		return CT_P9_DUIDI
	elseif (bTmpCardData[1] == 0x44 and bTmpCardData[2] == 0x44) then	--对人
		return CT_P9_DUIREN
	elseif (bTmpCardData[1] == 0x13 and bTmpCardData[2] == 0x13) then	--对鹅
		return CT_P9_DUIE

	elseif (bTmpCardData[1] == 0x55 and bTmpCardData[2] == 0x55) then	--双梅
		return CT_P9_MEIPAI		
	elseif (bTmpCardData[1] == 0x33 and bTmpCardData[2] == 0x33) then	--双长三
		return CT_P9_CHANGSAN	
	elseif (bTmpCardData[1] == 0x22 and bTmpCardData[2] == 0x22) then	--双板凳
		return CT_P9_BANDENG

	elseif (bTmpCardData[1] == 0x16 and bTmpCardData[2] == 0x16) then	--双高脚
		return CT_P9_GAOJIAOQI
	elseif (bTmpCardData[1] == 0x15 and bTmpCardData[2] == 0x15) then	--双零霖
		return CT_P9_TONGCHUILIU
	elseif (bTmpCardData[1] == 0x56 and bTmpCardData[2] == 0x56) then	--双斧头
		return CT_P9_FUTOU
	elseif (bTmpCardData[1] == 0x46 and bTmpCardData[2] == 0x46) then	--双红头
		return CT_P9_HONGSHI

	elseif (bTmpCardData[1] == 0x32 and bTmpCardData[2] == 0x14) then	--杂五
		return CT_P9_ZAWU
	elseif (bTmpCardData[1] == 0x34 and bTmpCardData[2] == 0x25) then	--杂七
		return CT_P9_ZAQI
	elseif (bTmpCardData[1] == 0x35 and bTmpCardData[2] == 0x26) then	--杂八
		return CT_P9_ZABA
	elseif (bTmpCardData[1] == 0x36 and bTmpCardData[2] == 0x45) then	--杂九
		return CT_P9_ZAJIU

	elseif (bTmpCardData[1] == 0x66 and bTmpCardData[2]==0x36) then	--天王
		return CT_P9_TIANWANG
	elseif (bTmpCardData[1] == 0x11 and bTmpCardData[2]==0x36) then	--地王
		return CT_P9_DIWANG
	elseif (bTmpCardData[1] == 0x66 and bTmpCardData[2]==0x35) then	--天杠
		return CT_P9_TIANGANG
	elseif (bTmpCardData[1] == 0x11 and bTmpCardData[2]==0x35) then	--地杠
		return CT_P9_DIGANG
	elseif (bTmpCardData[1] == 0x66 and bTmpCardData[2]==0x25) then	--天高九
		return CT_P9_TIANGAOJIU
	elseif (bTmpCardData[1] == 0x11 and bTmpCardData[2]==0x16) then	--地高九
		return CT_P9_DIGAOJIU
	else
		return CT_P9_NORMAL
	end
end

--获取点数
function GameLogic.GetSingleCardPoint( bCardData)
	assert(bCardData)
	return  bit:_and(bCardData,LOGIC_MASK_VALUE)+bit:_and(bCardData,LOGIC_MASK_COLOR)/16
end

--获取点数
function GameLogic.GetCardPoint( bCard,  bCardCount)

	--数据校验
	assert(bCardCount == 2 )
	assert(bCard[1])
	assert(bCard[2])
	if (bCardCount ~= 2) then
		return 0
	end
	local  bSpecCount = 0
	local bUnSpecData=0
	for i=1,bCardCount do
	
		if(bCard[i]==0x12 or bCard[i]==0x24) then
			bSpecCount=bSpecCount+1
		else
			bUnSpecData=bCard[i]	
		end		
	end

	--计算点数
	local bCardPoint=0

	--丁三和二四可以互换使用
	if (bSpecCount==1) then
		 bPoint1 = (GameLogic.GetSingleCardPoint(bUnSpecData)+GameLogic.GetSingleCardPoint(0x12))%10
		 bPoint2 = (GameLogic.GetSingleCardPoint(bUnSpecData)+GameLogic.GetSingleCardPoint(0x24))%10

		bCardPoint = math.max(bPoint1,bPoint2)
	else 
		bCardPoint = (GameLogic.GetSingleCardPoint(bCard[1]) + GameLogic.GetSingleCardPoint(bCard[2])) % 10
	end
		 
	return bCardPoint
end

--获取权值
function GameLogic.GetCardPower( bCard,  bCardCount)

	--数据校验
	assert(bCardCount == 2)
	if (bCardCount ~= 2) then		
		return -1 
	end

	local bCardType =GameLogic.GetCardType(bCard, bCardCount)

	local bCardPoint = 0
	if (CT_P9_NORMAL == bCardType) then
		 bCardPoint = GameLogic.GetCardPoint(bCard, bCardCount)
	end

	return bCardType*256+bCardPoint
end


function GameLogic.combine(cards)--返回牌索引集合，前两个为1对，后两个为1对
	assert(#cards==4)
	local comb={
		{1,2, 3,4},
		{1,3, 2,4},
		{1,4, 2,3}
	}
	local max=-1
	local index=1
	for i=1,#comb do
		local t1={cards[comb[i][1]],cards[comb[i][2]]}
		local t2={cards[comb[i][3]],cards[comb[i][4]]}
		local value=GameLogic.GetCardPower(t1,2)+GameLogic.GetCardPower(t2,2)
		if value>max then
			max=value
			index=i
		end
	end
	return comb[index]
end

return GameLogic
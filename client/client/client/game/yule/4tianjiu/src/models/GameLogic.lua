local GameLogic={}

GameLogic.cardListData=
{
	0x66,0x66,0x11,0x11,0x44,0x44,0x13,0x13,0x55,0x55,0x33,
	0x33,0x22,0x22,0x56,0x56,0x46,0x46,0x16,0x16,0x15,0x15,
	0x36,0x45,0x35,0x26,0x34,0x25,0x32,0x14,0x24,0x12
}

--牌值、10进制
GameLogic.m_bCardListData=
{
	102,102,17,17,68,68,19,19,85,85,51,
	51 ,34 ,34,86,86,70,70,22,22,21,21,
	54 ,69 ,53,38,52,37,50,20,36,18
}

--牌值、16进制
local m_bCardListData1=
{
	0x66,0x66,0x11,0x11,0x44,0x44,0x13,0x13,0x55,0x55,0x33,
	0x33,0x22,0x22,0x56,0x56,0x46,0x46,0x16,0x16,0x15,0x15,
	0x36,0x45,0x35,0x26,0x34,0x25,0x32,0x14,0x24,0x12
}

--牌值、10进制
local m_bCardListData=
{
	102,102,17,17,68,68,19,19,85,85,51,
	51 ,34 ,34,86,86,70,70,22,22,21,21,
	54 ,69 ,53,38,52,37,50,20,36,18
}

--牌值点数
local m_bCardPoint=
{
	12,12,2,2,8,8,4,4,10,10,6,
	6 ,4 ,4,11,11,10,10,7,7,6,6,
	9 ,9 ,8,8,7,7,5,5,6,3
}

--扑克类型
GameLogic.CT_SANPAI     =			2								--散牌
local CT_INVALID		=			0								--无效
local CT_BIE_SHI		=			1								--憋十
local CT_SANPAI		    =			2								--散牌
local CT_DIGANG		    =			3								--地杠
local CT_TIANGANG		=			4								--天杠
local CT_DUIZAWU		=			5								--对杂五
local CT_DUIZAQI		=			6								--对杂七
local CT_DUIZABA		=			7								--对杂八
local CT_DUIZAJIU		=			8								--对杂九
local CT_DUIDUAN		=			9								--对短
local CT_DUICHANG		=			10								--对长
local CT_DUIE		    =			11								--对鹅
local CT_DUIREN		    =			12								--对人
local CT_DUIDI		    =			13								--对地
local CT_DUITIAN		=			14								--对天
local CT_DIJIUNIANGNIANG=			15								--地九娘娘
local CT_TIANJIUWANG	=			16								--天九王
local CT_ZHIZUN		    =			17								--至尊
local CT_GUIZI		    =			18								--鬼子
local CT_ZHADAN		    =			19								--炸弹

--散牌类型
local CT_TIAN_X		=			0								--天X
local CT_DI_X		=			1								--地X
local CT_REN_X		=			2								--人X
local CT_E_X		=			3								--鹅X
local CT_CHANG_X	=			4								--长X
local CT_DUAN_X		=			5								--短X
local CT_ZA_X		=			6								--杂X
local CT_BISHI		=			7								--毕十


--数值掩码
local	LOGIC_MASK_COLOR	=			0xF0							--花色掩码
local	LOGIC_MASK_VALUE	=			0x0F							--数值掩码

--扑克数目
local MAX_CARD_COUNT		=			32								--扑克数目

--比较结果
local COMPCARD_WIN_3			=		3						--比较胜利(赢三道)
local COMPCARD_WIN_2		    =		2						--比较胜利(赢二道)
local COMPCARD_WIN_1			=		1						--比较胜利(赢一道)
local COMPCARD_EQUAL			=		0						--比较相同
local COMPCARD_LOSE_1		    =		-1						--比较失败(输一道)
local COMPCARD_LOSE_2			=		-2						--比较失败(输二道)
local COMPCARD_LOSE_3			=		-3						--比较失败(输三道)


--获取索引
function GameLogic.GetCardIndex(bCardData)

	for i=1,MAX_CARD_COUNT do
		if bCardData==m_bCardListData[i] then
			return i
		end
	end
	return yl.INVALID_WORD
end

--获取类型
function GameLogic.GetCardType(bCardData, bCardCount, viewParent)

	--数据校验
	assert(bCardCount==2)
	if(bCardCount~=2) then return yl.INVALID_WORD end

	--牌组排序
	local bTmpCardData= {}
	bTmpCardData[1] = (GameLogic.GetCardIndex(bCardData[1]) <= GameLogic.GetCardIndex(bCardData[2]) and bCardData[1] or bCardData[2])
	bTmpCardData[2] = (GameLogic.GetCardIndex(bCardData[1]) < GameLogic.GetCardIndex(bCardData[2]) and bCardData[2] or bCardData[1])

	--牌型判断
	if ((bTmpCardData[1] == 38 or bTmpCardData[1] == 53 or bTmpCardData[1] == 68) and bTmpCardData[2] == 18 and viewParent.bZhaDan) then	--炸弹
		return CT_ZHADAN;
	elseif (bTmpCardData[1] == 86 and (bTmpCardData[2] == 54 or bTmpCardData[2] == 69) and viewParent.bGuiZi) then	--鬼子
		return CT_GUIZI;
	elseif (bTmpCardData[1] == 36 and bTmpCardData[2] == 18) then	--至尊
		return CT_ZHIZUN;
	elseif (bTmpCardData[1] == 102 and (bTmpCardData[2] == 54 or bTmpCardData[2] == 69) and viewParent.bTianJiuWang) then	--天九王
		return CT_TIANJIUWANG;
	elseif (bTmpCardData[1] == 17 and (bTmpCardData[2] == 54 or bTmpCardData[2] == 69) and viewParent.bDiJiuNiangNiang) then	--地九娘娘
		return CT_DIJIUNIANGNIANG;
	elseif (bTmpCardData[1] == 102 and bTmpCardData[2] == 102) then	--对天
		return CT_DUITIAN;	
	elseif (bTmpCardData[1] == 17 and bTmpCardData[2] == 17) then	--对地
		return CT_DUIDI;
	elseif (bTmpCardData[1] == 68 and bTmpCardData[2] == 68) then	--对人
		return CT_DUIREN;
	elseif (bTmpCardData[1] == 19 and bTmpCardData[2] == 19) then	--对鹅
		return CT_DUIE;
	elseif ((bTmpCardData[1] == 85 and bTmpCardData[2] == 85) or (bTmpCardData[1] == 51 and bTmpCardData[2] == 51) or (bTmpCardData[1] == 34 and bTmpCardData[2] == 34)) then	--对长
		return CT_DUICHANG;
	elseif ((bTmpCardData[1] == 86 and bTmpCardData[2] == 86) or (bTmpCardData[1] == 70 and bTmpCardData[2] == 70) or (bTmpCardData[1] == 22 and bTmpCardData[2] == 22) or (bTmpCardData[1] == 21 and bTmpCardData[2] == 21)) then	--对短
		return CT_DUIDUAN;
	elseif ((bTmpCardData[1] == 54 or bTmpCardData[1] == 69) and (bTmpCardData[2] == 54 or bTmpCardData[2] == 69)) then	--对杂九
		return CT_DUIZAJIU;
	elseif ((bTmpCardData[1] == 38 or bTmpCardData[1] == 53) and (bTmpCardData[2] == 38 or bTmpCardData[2] == 53)) then	--对杂八
		return CT_DUIZABA;
	elseif ((bTmpCardData[1] == 37 or bTmpCardData[1] == 52) and (bTmpCardData[2] == 37 or bTmpCardData[2] == 52)) then	--对杂七
		return CT_DUIZAQI;
	elseif ((bTmpCardData[1] == 20 or bTmpCardData[1] == 50) and (bTmpCardData[2] == 20 or bTmpCardData[2] == 50)) then	--对杂五
		return CT_DUIZAWU;
	elseif (bTmpCardData[1] == 102 and (bTmpCardData[2] == 68 or bTmpCardData[2] == 53 or bTmpCardData[2] == 38)) then	--天杠
		return CT_TIANGANG;
	elseif (bTmpCardData[1] == 17 and (bTmpCardData[2] == 68 or bTmpCardData[2] == 53 or bTmpCardData[2] == 38)) then	--地杠
		return CT_DIGANG;
	else
		return CT_SANPAI;
	end
end

--获取散牌类型
function GameLogic.GetSanpaiType(bCardData, bCardCount, bCardPoint)
	--数据校验
	if(bCardCount~=2) then return yl.INVALID_WORD end

	--牌组排序
	local bTmpCardData= {}
	bTmpCardData[1] = (GameLogic.GetCardIndex(bCardData[1]) <= GameLogic.GetCardIndex(bCardData[2]) and bCardData[1] or bCardData[2])
	bTmpCardData[2] = (GameLogic.GetCardIndex(bCardData[1]) < GameLogic.GetCardIndex(bCardData[2]) and bCardData[2] or bCardData[1])

	--牌型判断
	if (bCardPoint == 0) then	--毕十
		return CT_BISHI;
	elseif (bTmpCardData[1] == 102) then	--天X
		return CT_TIAN_X;	
	elseif (bTmpCardData[1] == 17) then	--地X
		return CT_DI_X;
	elseif (bTmpCardData[1] == 68) then	--人X
		return CT_REN_X;
	elseif (bTmpCardData[1] == 19) then	--鹅X
		return CT_E_X;
	elseif (bTmpCardData[1] == 85 or bTmpCardData[1] == 51 or bTmpCardData[1] == 34) then	--长X
		return CT_CHANG_X;
	elseif (bTmpCardData[1] == 86 or bTmpCardData[1] == 70 or bTmpCardData[1] == 22 or bTmpCardData[1] == 21) then	--短X
		return CT_DUAN_X;
	elseif (bTmpCardData[1] == 54 or bTmpCardData[1] == 69 or bTmpCardData[1] == 53 or bTmpCardData[1] == 38 or bTmpCardData[1] == 52 or bTmpCardData[1] == 37 or bTmpCardData[1] == 50 or bTmpCardData[1] == 20) then	--杂X
		return CT_ZA_X;
	else
		return yl.INVALID_WORD;
	end
end

--获取点数
function GameLogic.GetCardPoint(bCardData, bCardCount)
	--数据校验
	if (bCardCount ~= 2) then
		return 0;
	end
	--计算点数
	local bCardPoint = 0;
	local bPoint1 = m_bCardPoint[GameLogic.GetCardIndex(bCardData[1])];
	local bPoint2 = m_bCardPoint[GameLogic.GetCardIndex(bCardData[2])];

	bCardPoint = (bPoint1 + bPoint2)%10;
		 
	return bCardPoint;
end

--获取权值
function GameLogic.GetCardPower(bCardData, bCardCount, viewParent)
	--数据校验
	if (bCardCount ~= 2) then		
		return -1;
	end

	local bCardType =GameLogic.GetCardType(bCardData, bCardCount, viewParent);

	local bCardPoint = 0;
	if (CT_SANPAI == bCardType) then
		 bCardPoint = GameLogic.GetCardPoint(bCardData, bCardCount);
	end

	return bCardType*256 + bCardPoint;
end

--返回牌索引集合，前两个为1对，后两个为1对
function GameLogic.combine(bCardData, viewParent)
	local comb={
		{1,2, 3,4},
		{1,3, 2,4},
		{1,4, 2,3}
	}
	local max = -1;
	local index = 1;
	for i = 1, #comb do
		local t1={bCardData[comb[i][1]], bCardData[comb[i][2]]};
		local t2={bCardData[comb[i][3]], bCardData[comb[i][4]]};
		local value=GameLogic.GetCardPower(t1, 2, viewParent) + GameLogic.GetCardPower(t2, 2, viewParent);
		if value > max then
			max = value;
			index = i;
		end
	end
	return comb[index];
end

return GameLogic
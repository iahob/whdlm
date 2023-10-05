local GameLogic = {}
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")
--牌库数目
GameLogic.TOTAL_COUNT				= 54

GameLogic.CardData = 
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,  --方块
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,  --梅花
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,  --红桃
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,  --黑桃
	0x4E, 0x4F,                                                                    --小大王
}

--获取牌值
function GameLogic.GetCardValue(nCardData)
	nCardData = nCardData or 0
	return bit:_and(nCardData, 0X0F)
end

--获取花色
function GameLogic.GetCardColor(nCardData)
	nCardData = nCardData or 0
	return bit:_and(nCardData, 0XF0)
end

--获取牌 0x4F大王 0x4E小王 nil牌背
function GameLogic:GetCardIndex(nCardData)
		if nCardData == 0x4E then
				return 53
		elseif nCardData	== 0x4F then
				return 54
		elseif nCardData == nil then
				return 55
		end

		local nCardValue = GameLogic.GetCardValue(nCardData)
		local nCardColor = GameLogic.GetCardColor(nCardData)

		nCardColor = bit:_rshift(nCardColor, 4)
		
		return nCardColor*13 + nCardValue
end

--比大小
function GameLogic.compare(card1, card2)
	local data1 = GameLogic.GetCardValue(card1)
	local data2 = GameLogic.GetCardValue(card2)

	if data1 > data2 then
		return GAME_CMD.AREA_LONG
	elseif data1 < data2 then
		return GAME_CMD.AREA_HU
	elseif data1 == data2 then
		return GAME_CMD.AREA_PING
	end
end

return GameLogic	
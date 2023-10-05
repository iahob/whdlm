local CardSprite = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.CardSprite")

local CardMg = class("CardMg")
CardMg.cardPos = {cc.p(558,658), cc.p(766,658)}

function CardMg:createCardBack(node)
	for i=1, 2 do
		local img = CardSprite:createCardBack()
		img:setPosition(CardMg.cardPos[i])

		if node then
			node:addChild(img)
		end
	end
end

function CardMg:createCard(cardData)
	
end








return CardMg
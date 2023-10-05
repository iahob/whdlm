local CardLayer = class("CardLayer", function(scene)
	local cardLayer = display.newLayer()
	return cardLayer
end)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.GameLogic")
local cmd = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.CMD_Game")



local posOutCard = {cc.p(235, 505), cc.p(667, 355), cc.p(1100, 505)}
local location = {cc.p(155,512),cc.p(667,210),cc.p(1180,512)}
local cardScale = {0.22,0.842,0.22}

local videoCard = {cc.p(153,615),cc.p(667,210),cc.p(1182,615),cc.p(1057,593)}

local chaY = 60
local vecOutCard = {0,-1,-1,0}--方向
--137  185
local cardW = 193
local cardH =  272
local cardDisW = 66




function CardLayer:ctor(scene)
	print("CardLayer=====================================>")
	self._scene = scene
	self:onInitData()

	--手牌
	--self.nodeHandCard = self._scene:getChildByName("cardNode")
	ExternalFun.registerTouchEvent(self, true)

	self.myCardNode  = cc.Node:create():move(0,0):addTo(self)

	self.nodeOutcard = {}
	self.otherCardNode = {}
	for i=1,cmd.GAME_PLAYER do
		self.nodeOutcard[i]=cc.Node:create():move(posOutCard[i]):addTo(self)
		
		self.otherCardNode[i] = cc.Node:create():move(videoCard[i]):addTo(self)
		
	end
	
	self.aniNode = cc.Node:create():move(0,0):addTo(self)
	
	self.cbCardCount = {0,0,0} 


	
end

function CardLayer:onInitData()
	--body
	math.randomseed(os.time())
	self.cbCardData = {}
	self.mSelectCard = {}
	self.mCurSelect = {}
	self.mLastIndex = {}
	self.outCardTable = {}

	self.mSelectCardData = {}
	self.makeCard = {}

	self.cbCardCount = {0,0,0}--索引 viewid 
	self.allPlayerCard = {{},{},{}}

	self.isTouchEnable = false -- 设置自己是否可以触摸
	
	self.beginPoint = nil
	self.mPointX = 800
	self.mPointY = 120

	--点击空白允许重置牌
	self.bClickZero = true
	
end

function CardLayer:onResetData()
	--body
	for i = 1, cmd.GAME_PLAYER  do
		self.nodeOutcard[i]:removeAllChildren()
		self.otherCardNode[i]:removeAllChildren()
	end
	self.cbCardData = {}
	--self.aniNode:removeAllChildren()
	local allChild = self.aniNode:getChildren()
	for k,v in pairs(allChild) do
		if v then
			v:stopAllActions()
			v:removeFromParent()
			--print("====================removeAllChildren()111")
		end
	end
	self:stopAllActions()

	self.mSelectCard = {}
	self.mCurSelect = {}
	self.mSelectCardData = {}
	self.mLastIndex = {}
	
	self.cbCardCount = {0,0,0}--索引 viewid 

	self.isTouchEnable = false

	self.isBreakTouch = false --是否中断选择
	self.beginPoint = nil
	self.myCardNode:removeAllChildren()

	--点击空白允许重置牌
	self.bClickZero = true

	print("====================removeAllChildren()222")
end





--设置自己是否可以触摸
function CardLayer:setMyCardTouchEnabled(enable)
	self.isTouchEnable = enable
end



function CardLayer:createCard(cardData)
	local x = cardData and math.mod(cardData, 16) or 3
	local y = cardData and math.floor(cardData/16) or 4
	local with = cardW
	local height = cardH
	local path = "rungame_res/card.png"

	if cardData == 0x4E then
		x= 2
		y=  4
	elseif cardData == 0x4F then
		x= 1
		y=  4
	elseif not cardData or cardData==0xff then
		x= 3
		y=  4
	end

	local card = cc.Sprite:createWithSpriteFrame(cc.SpriteFrame:create(path,cc.rect((x-1)*with,y*height,with,height))):setAnchorPoint(cc.p(0.5,0.5))


	return card
end


function CardLayer:showFirstOutTip(visible,viewId)
	local firstTipPoint = {cc.p(355,512),cc.p(668,430),cc.p(980,512)}
	if visible then
		local first = cc.Sprite:create("rungame_res/tip_first.png")
		first:setPosition(firstTipPoint[viewId])
		first:setName("_firstTip_")
		first:addTo(self)
	else
		self:removeChildByName("_firstTip_")
	end
end


function CardLayer:sortOtherPlayerHandCard(viewId,count,cardData)
	local scale = {0.5,1,0.5}
	self.otherCardNode[viewId]:removeAllChildren()
	if cardData then
		table.sort(cardData, function ( a,b )
				return GameLogic:GetCardLogicValue(a) < GameLogic:GetCardLogicValue(b)
			end )
	end

	for i=1,count do
		local card = self:createCard(cardData and cardData[i] or nil)
		card:setScale(scale[viewId])
		card:setLocalZOrder(viewId == 4 and 16-i or i)
		card:addTo(self.otherCardNode[viewId])
		card:setPositionX( (viewId == 1 and self._scene._scene.wPlayerCount == 2) and  (i-1)*40 or 0)
		card:setPositionY( (viewId == 1 and self._scene._scene.wPlayerCount == 2) and 0 or - (i-1)*20)
	end
end


function CardLayer:sendCardFinish(isAni)
	local viewId = self._scene._scene:SwitchViewChairID(self._scene._scene.wCurrentUser)
	
	self:sortHandCard()

	--dump(self.allPlayerCard,"=====================111222")
	if self._scene._scene.wPlayerCount == 2 then
		--print(self.cbCardCount[4].."===================33333333333")
		--self:sortOtherPlayerHandCard(1,self.cbCardCount[1],self._scene.bIsVideo and self.allPlayerCard[4] or nil)
		self._scene:setHandNum(true,{self.cbCardCount[1]})
	else
		if self._scene.bIsVideo then
			for i=1,3 do
				if i~= 2 then
					self:sortOtherPlayerHandCard(i,#self.allPlayerCard[i],self.allPlayerCard[i])
				end
			end
		else

			self._scene:setHandNum(true,{self.cbCardCount[1],self.cbCardCount[3]})
		end
	end

	if self._scene._scene.bFirstOutSpade and isAni then
		
		
		--print(viewId.."============9999========")
		local tabUserPos = {cc.p(80,523) , cc.p(80,93) ,cc.p(1255,523) }
		local tabUserRot = { -90, -120 , 90}
		self.firstAniCard:runAction(cc.Sequence:create(cc.RotateTo:create(0.5,tabUserRot[viewId]),cc.Spawn:create(cc.MoveTo:create(0.5,tabUserPos[viewId]),cc.ScaleTo:create(0.5,cardScale[viewId]),cc.FadeTo:create(0.5,50)),cc.CallFunc:create(function ( ... )
			self.firstAniCard:removeFromParent()
			self.firstAniCard = nil
			self:showFirstOutTip(true,viewId)
			if viewId == cmd.MY_VIEWID and not self._scene.bIsVideo then
				self.isTouchEnable = true
				self._scene:setGameBtnStatus(true,false,false)
			end
		end)))
	else
		if isAni then
			self:showFirstOutTip(true,viewId)
		end
		if viewId == cmd.MY_VIEWID and  not self._scene.bIsVideo then
			self.isTouchEnable = true
			self._scene:setGameBtnStatus(true,false,false)
		end
	end
end





function CardLayer:showSendCardAni(cardData, cardCount,isAni)
	

	local viewId = self._scene._scene:SwitchViewChairID(self._scene._scene.wCurrentUser)
	--print(viewId.."==============showSendCardAni==="..self._scene._scene.wPlayerCount)

	self.cbCardCount = clone(cardCount)

	self.firstAniCard = nil
	
	if isAni then
		if self._scene._scene.wPlayerCount == 3 and not self._scene.bIsVideo then

			self._scene:setHandNum(true)
		end

		if self._scene._scene.bFirstOutSpade then
			self.firstAniCard = self:createCard(self._scene._scene.cbFirstOutCard)

			self.firstAniCard:setPosition(667, 600)
			self.firstAniCard:setRotation(10)
			self.firstAniCard:addTo(self.aniNode)

			local icon = cc.Sprite:create("rungame_res/firstOut.png"):addTo(self.firstAniCard)
			icon:setAnchorPoint(cc.p(1,1))
			icon:setPosition(cc.p(190,270))

			self.firstAniCard:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.ScaleTo:create(0.5,0.6),cc.ScaleTo:create(0.5,1))))
		end
		--发牌音效
					self._scene._scene:PlaySound(cmd.RES_PATH.."sound/SEND_CARD.mp3")
		
		for i=1,cardCount[2] do
			for j=1,self._scene._scene.wPlayerCount do
				local  index = j
				if self._scene._scene.wPlayerCount ==2  then
					index= (j==1 and 2 or 1)
				end
				local card = self:createCard()
				card:setScale(0.3)
				card:addTo(self.aniNode)
				card:setPosition(667, 415)
				card:runAction(cc.Sequence:create(cc.DelayTime:create((i-1)*0.1),cc.Spawn:create(cc.MoveTo:create(0.1,location[index]),cc.ScaleTo:create(0.1,cardScale[index])) ,cc.CallFunc:create(function ()
					card:removeFromParent()

					self.cbCardData[i] = cardData[i]
					self:sortHandCard()
					
					

					if self._scene.bIsVideo then
						if self._scene._scene.wPlayerCount ==3 and j ~= 2 then
							self:sortOtherPlayerHandCard(j,i,self.allPlayerCard[j])
						end
					end

					if self._scene._scene.wPlayerCount ==2 and j==2 then
						--self:sortOtherPlayerHandCard(1,i,self._scene.bIsVideo and self.allPlayerCard[1] or nil)

					end	

					if i== cardCount[1] and j == self._scene._scene.wPlayerCount then
						self.cbCardData = clone(cardData)
						--dump(cardCount,"============123")
						self:sendCardFinish(isAni)
					end
				end)))
			end
		end
	else


		self.cbCardData = clone(cardData)

		self:sendCardFinish(isAni)

		if cardCount[1] ==cardCount[2] and  cardCount[3] ==cardCount[2] and cardCount[1] == self._scene._scene.cbUserMaxCardCount then
			self:showFirstOutTip(true,viewId)
		elseif self._scene._scene.wPlayerCount == 2 and cardCount[2] ==cardCount[4] and cardCount[2] == self._scene._scene.cbUserMaxCardCount then
			self:showFirstOutTip(true,viewId)
		end
		

	end
	
	
end



function CardLayer:showPass(viewId)

	if viewId == cmd.MY_VIEWID then
		self.isTouchEnable = false
		self:sortHandCard()
	end

	if self.nodeOutcard[viewId] then
		self.nodeOutcard[viewId]:removeAllChildren()
		local pass = cc.Sprite:create("rungame_res/tip_pass.png")
		pass:setName("_PASS_")
		self.nodeOutcard[viewId]:addChild(pass)

		--print("showPass========================="..viewId)

		if viewId == 4 then
	 		pass:setPosition(281, 50)
	 	elseif viewId == 2   then 
	 		if self._scene._scene.wPlayerCount == 2 then
	 			pass:setPosition(0, -60)
	 		else
	 			pass:setAnchorPoint(cc.p(0.5,0.5))
	 			pass:setPosition(0, 0)
	 		end
	 		
	 	elseif viewId ==1 then
	 		pass:setAnchorPoint(cc.p(0,0.5))
	 		pass:setPosition(60, 0)
	 	elseif viewId ==3 then
	 		pass:setAnchorPoint(cc.p(1,0.5))
	 		pass:setPosition(-60, 0)
		end
	end
end

function CardLayer:removeOutCard(viewId)
	--print("removeOutCard========================="..viewId)
	if viewId == yl.INVALID_CHAIR then
		return
	end
		self.nodeOutcard[viewId]:removeChildByName("_PASS_")
	

		if self.nodeOutcard[viewId] then
			self.nodeOutcard[viewId]:removeAllChildren()
		end
	
end




function CardLayer:gameOutCard(viewId, cbCardData, bOnlyShow, bEnded)

	if #cbCardData <= 0 then
		return
	end

	local outCardData = clone(cbCardData)
	dump(outCardData,"================hehe")

	self:showFirstOutTip(false)

	local outIndex =  viewId

	
	self.nodeOutcard[outIndex]:removeAllChildren()
	
	GameLogic:SortCardList(outCardData)
	local pointY= 0
	local pointX = 0
	if (viewId ==1 or viewId ==3) and #outCardData >8 then --1和3超过8张，两行显示
		pointY = 60
	end

	for i=1,#outCardData do
		local card = self:createCard(outCardData[i])
		card:setScale(0.44)
		
		
		if viewId ==1 then
			if #outCardData <= 8 then
				pointX = (i-1)*40
			else
				pointX = ((i > 8 and i-8 or i)-1)*40
			end
			pointY = (i<=8 and  pointY or 0)
		elseif viewId ==2 then
			pointX = -20*(#outCardData -1) +(i-1)*40
			pointY = 0
		elseif viewId ==3 then
			
			if #outCardData <= 8 then
				pointX = -40*(#outCardData -1) +(i-1)*40
			else
				pointX = -40*7 + ((i > 8 and i-8 or i)-1)*40
			end
			pointY = (i<=8 and  pointY or 0)			
		end
		
		
			card:setPosition(pointX,pointY)	
		

		card:addTo(self.nodeOutcard[outIndex])
	end


	local cbCardType = GameLogic:GetCardType(cbCardData)

	if viewId ==2 then
		if bEnded then--
			return
		end

		if bOnlyShow then
			self.nodeOutcard[outIndex]:setVisible(true)

		else
			self.isTouchEnable = false

			--self.nodeOutcard[outIndex]:setVisible(true)
			self.nodeOutcard[outIndex]:setVisible(false)
			local outTable = {}
			for i=1,#cbCardData do
				for j=1, #self.cbCardData do
					if cbCardData[i] == self.cbCardData[j]  then
						local card =  self.myCardNode:getChildByTag(j)
						--table.remove(self.cbCardData,j)
						table.insert(outTable,j)
						card:runAction(cc.Sequence:create(cc.Spawn:create(cc.MoveTo:create(0.2,posOutCard[outIndex]),cc.ScaleTo:create(0.2,0.63)),cc.CallFunc:create(function ( ... )
							card:setVisible(false)
							self.nodeOutcard[outIndex]:setVisible(true)

							if i == #cbCardData then
								table.sort(outTable, function ( a,b )
									return a > b
								end )
								dump(outTable,"================7777777")

								for n=1,#outTable do
									table.remove(self.cbCardData,outTable[n])
								end
								
								self:sortHandCard()
								self.bClickZero = true
								
								local typeAni = self:getCardTypeAni(cbCardType,#cbCardData,outIndex)

								typeAni:setPositionY(40)
								self.otherCardNode[outIndex]:addChild(typeAni)
							end
						end)))

					end
				end
		
			end
			self._scene:manageCardType(viewId, cbCardData)
		end

		
	else
		if not bOnlyShow then --场景消息不需要减
			self.cbCardCount[viewId] = self.cbCardCount[viewId] - #cbCardData
			self._scene:manageCardType(viewId, cbCardData)
		

		end
		self.nodeOutcard[outIndex]:setVisible(true)

		if self._scene._scene.wPlayerCount == 3 then
			if self._scene.bIsVideo then
				self:sortOtherPlayerHandCard(viewId,#self.allPlayerCard[viewId],self.allPlayerCard[viewId])
			else

				self._scene:setHandNum(true,{self.cbCardCount[1],self.cbCardCount[3]})
			end
		else
			self._scene:setHandNum(true,{self.cbCardCount[1]})
		end
		if not bEnded then
			local typeAni = self:getCardTypeAni(cbCardType,#cbCardData,outIndex)
			typeAni:setPositionY(-40)

			if outIndex ==1 then
				typeAni:setPositionX(180)
			elseif outIndex ==3 then
				typeAni:setPositionX(-180)
			end
			self.otherCardNode[outIndex]:addChild(typeAni)
		end
	end
	if self.cbCardCount[viewId] ==1 and viewId~=cmd.MY_VIEWID then
		self._scene:showBaoDan(true,viewId)
	end
		
end

function CardLayer:getCardTypeAni(cbCardType,count,viewid)
	local aniNode = cc.Node:create():move(0,0)
	local tabFileName = {"zd" , "feiji" , "paixing"}
	local tabSkinName = {"ld" , "3d1" , "3d2","3","33","4d3","shunzi"}
	local fileStr = ""
	local skinStr = ""
	print("播放的动画 牌型：",cbCardType,"牌数量：",count)
	if cbCardType == 16 then--炸弹
		fileStr = "zd"
		
	elseif cbCardType >= cmd.CT_TWO_THREE_LINE_TAKE_TWO_SINGLE  and cbCardType <= cmd.CT_FOUR_THREE_LINE_TAKE_FOUR_SINGLE then
		--飞机
		fileStr = "feiji"
	elseif cbCardType == cmd.CT_THREE_LINE_TAKE_DOUBLE or cbCardType == cmd.CT_THREE_TAKE_TWO_SINGLE then--三带一对
		fileStr = "paixing"
		skinStr = "3d2"
	elseif cbCardType == cmd.CT_DOUBLE_LINE then--一对或连对子
		fileStr = "paixing"
		if count ==2 then--一对 --对子不播动画
			fileStr = ""
		else--连对
			skinStr = "ld"
		end
	elseif cbCardType == cmd.CT_THREE_LINE then--3张或3连
		fileStr = "paixing"
		if count ==3 then--3张
			skinStr = "3"
		else--3连
			skinStr = "33"
		end
	elseif cbCardType == cmd.CT_SINGLE_LINE then--顺子
		fileStr = "paixing"
		skinStr = "shunzi"
	elseif cbCardType == cmd.CT_THREE_LINE_TAKE_SINGLE then --3带1单牌
		fileStr = "paixing"
		skinStr = "3d1"
	elseif cbCardType == cmd.CT_FOUR_TAKE_THREE then --4带3
		fileStr = "paixing"
		skinStr = "4d3"
	end

	if fileStr~="" then
		local skeleton = sp.SkeletonAnimation:create("skeleton/"..fileStr..".json", "skeleton/"..fileStr..".atlas")
		skeleton:setAnimation(0, "animation", false)
		if skinStr~="" then
			skeleton:setSkin(skinStr)
		end
		aniNode:addChild(skeleton)
	end

	
	aniNode:runAction(cc.Sequence:create(cc.DelayTime:create(1.2),cc.CallFunc:create(function ()
		if fileStr == "zd" then
			self._scene:shakeScreen()
		end
		end),cc.RemoveSelf:create()))

	return aniNode
end


--手牌排序self._scene._scene.bFirstOutSpade  self._scene._scene.cbFirstOutCard
function CardLayer:sortHandCard()

	self.myCardNode:removeAllChildren()
	self.mSelectCard = {}
	self.mCurSelect = {}
	self.mSelectCardData = {}

	--self._scene.mLogic:SortCardList(self.cbCardData,self.cbCardCount,true,self._scene.curSortType,isMagic,true)
	GameLogic:SortCardList(self.cbCardData)
	local  cardCount =#self.cbCardData
	self.mPointX = 740-(cardCount -1)*cardDisW/2


	for i=1, cardCount do
		--print("我是否观战。。",self._scene._scene.m_bLookOn)
		local card = self:createCard(self._scene._scene.m_bLookOn and 0xff or self.cbCardData[i])
		card:setScale(cardScale[2])
		card:setPosition(self.mPointX+(i-1)*cardDisW, self.mPointY)
		card:setLocalZOrder(i)
		card:setTag(i)
		card:addTo(self.myCardNode)

		if self._scene._scene.bFirstOutSpade and self._scene._scene.cbFirstOutCard==self.cbCardData[i] then
			local icon = cc.Sprite:create("rungame_res/firstOut.png"):addTo(card)
			icon:setAnchorPoint(cc.p(1,1))
			icon:setPosition(cc.p(190,270))
		end
	end

end


function CardLayer:showSelectCard(resultData)
	
	self.mSelectCard ={}
	local cardData = clone(self.cbCardData)

	for i=1,#resultData.rCardData do

		for j= 1,#self.cbCardData do

			if resultData.rCardData[i] == cardData[j] then
				
				table.insert(self.mSelectCard,j)

				local card =  self.myCardNode:getChildByTag(j)
				card:setPositionY(card:getPositionY() + 20)

				break

			end
		end

	end
	self.makeCard = clone(resultData.mCardData)
	self.mSelectCardData = clone(resultData.rCardData)
	-- local pass = false

	-- if self._scene.cbOutCardType ~= 0 then
	-- 	pass = true
	-- end

	--self._scene:showOperateButton(true,pass,true)--visible,pass,canout

	--dump(self.makeCard,"hhhhhhhhhhhhhhhhhhhhh")
	--dump(self.mSelectCardData,"hhhhhhhhhhhhhhhhhhhhh")
	--dump(self.mSelectCard,"hhhhhhhhhhhhhhhhhhhhh")

end





function CardLayer:deleteCard(cardData)
	
	for i=1,#cardData do
		for j=1,#self.cbCardData do
			if cardData[i]== self.cbCardData[j] then
				table.remove(self.cbCardData,j)
				break
			end
		end
	end
end

function CardLayer:getIndexByPos(point)
	local x= 1


	if point.y > self.mPointY + cardH*0.7/2 or  point.y < self.mPointY - cardH*0.7/2 then
		return nil
	end

	if point.x <  (self.mPointX -  cardW*0.7/2) or point.x > (self.mPointX + (#self.cbCardData -1)*cardDisW + cardW*0.7/2) then
		return nil
	else
		x = 1184 - point.x - self.mPointX <=0 and 1 or (math.floor((1184 - point.x - self.mPointX)/48) + 2)
	end

	x = math.floor((point.x - (self.mPointX -cardW*0.7/2))/cardDisW) +1
	x = x >#self.cbCardData and #self.cbCardData or x

	local card =  self.myCardNode:getChildByTag(x)
	if card and card:getChildByTag(123) then
		card = nil
	end
	if not card then
		return nil
	end

	
	return x
end


function CardLayer:popupCard(cardData)

	self:sortHandCard()
	self.mSelectCardData = clone(cardData)
	for i=1,#cardData do
		for j=1,#self.cbCardData do
			if cardData[i]== self.cbCardData[j] then
				table.insert(self.mSelectCard,j)
				local card =  self.myCardNode:getChildByTag(j)
				card:setPositionY(self.mPointY + 20)

				break
			end
		end
	end
	dump(self.mSelectCardData,"====================1")
	
end


function CardLayer:popSelectCard()
	local removeT = {}
	local addT= {}
	--dump(self.mCurSelect,"===============1")
	for i=1,#self.mCurSelect do

		if #self.mSelectCard == 0 then

			local card =  self.myCardNode:getChildByTag(self.mCurSelect[i])
				card:setPositionY(self.mPointY + 20)
				table.insert(addT,clone(self.mCurSelect[i]))
		else
			for j=1,#self.mSelectCard do 
				if self.mCurSelect[i]==self.mSelectCard[j] then
					local card =  self.myCardNode:getChildByTag(self.mCurSelect[i])
					card:setPositionY(self.mPointY)
					table.insert(removeT,j)
					break
				elseif j==#self.mSelectCard then
					local card =  self.myCardNode:getChildByTag(self.mCurSelect[i])
					card:setPositionY(self.mPointY + 20)
					table.insert(addT,self.mCurSelect[i])
					
				end
			end
		end
	end

	table.sort(removeT,function (a,b)
		return a<b
	end)

	for i=#removeT,1,-1 do

		table.remove(self.mSelectCard,removeT[i])
	end

	for i=1,#addT do
		table.insert(self.mSelectCard,addT[i])
	end
end

function CardLayer:addNotSelectEffect(tag)

	local card = self.myCardNode:getChildByTag(tag)
	if card and not (card:getChildByTag(123)) then 
		local greyBg = self:createCard(self.cbCardData[tag])
		greyBg:setColor(cc.BLACK)
      	greyBg:setOpacity(100)
		greyBg:setAnchorPoint(cc.p(0,0))
		greyBg:setPosition(cc.p(0, 0))
		greyBg:setTag(123)
		card:addChild(greyBg)
	end
end

-- while i < analyseResult.bThreeCount do
				
-- end
-- for j=1,#self.cbCardData do
-- 			if cardData[i]== self.cbCardData[j] then
-- 				table.insert(self.mSelectCard,j)
-- 				local card =  self.myCardNode:getChildByTag(j)
-- 				card:setPositionY(self.mPointY + 20)

-- 				break
-- 			end
-- 		end
-- resultOutCard.cbCardCount
function CardLayer:setNotSelectCardEffect(turnCardData)

	local bNextLastOne = false 
	if self.cbCardCount[cmd.MY_VIEWID+1] == 1 then  --如果下家只剩下最后一张牌,自己必须出最大牌
		bNextLastOne = true
	end


	for j=1,#self.cbCardData do
		self:addNotSelectEffect(j)
	end
	local resultOutCard = {}
	resultOutCard.cbCardCount = 1
	
	while resultOutCard.cbCardCount~=0 do
		--dump(turnCardData , "这组牌 我能不能压")
		resultOutCard = GameLogic:SearchOutCard(self.cbCardData, turnCardData,bNextLastOne,self._scene._scene.bFirstOutSpade,self._scene._scene.cbFirstOutCard)	
		--dump(resultOutCard , "找到了可以出的牌")
		for i=1 , resultOutCard.cbCardCount do
			for j=1,#self.cbCardData do
 				if GameLogic:GetCardLogicValue(self.cbCardData[j]) == GameLogic:GetCardLogicValue(resultOutCard.cbResultCard[i])  then
 					self.myCardNode:getChildByTag(j):removeChildByTag(123)
 				end
 			end
		end
		turnCardData = resultOutCard.cbResultCard
	end
end

function CardLayer:showSelectArea()
	if #self.mCurSelect  >0 then
		self:cleanSelectArea()

		for i=1,#self.mCurSelect do

			local card = self.myCardNode:getChildByTag(self.mCurSelect[i])
			if card:getChildByTag(888) then
				card:removeChildByTag(888)
			end
			local greyBg = self:createCard(self.cbCardData[self.mCurSelect[i]])
			greyBg:setColor(cc.BLACK)
      			greyBg:setOpacity(100)
			greyBg:setAnchorPoint(cc.p(0,0))
			greyBg:setPosition(cc.p(0, 0))
			greyBg:setTag(888)
			card:addChild(greyBg)
			
		end		
	end
end

function CardLayer:cleanSelectArea()
	local allcard = self.myCardNode:getChildren()
	for k,v in pairs(allcard) do
		if v:getChildByTag(888) then
			v:removeChildByTag(888)
		end
	end
end

function CardLayer:cleanNotSelectEffect()
	local allcard = self.myCardNode:getChildren()
	for k,v in pairs(allcard) do
		if v:getChildByTag(123) then
			v:removeChildByTag(123)
		end
	end
end

function CardLayer:resetMyCard()
	self:sortHandCard()
	if self._scene._scene.cbTurnCardData and #(self._scene._scene.cbTurnCardData)>0 and self._scene._scene.wCurrentUser==self._scene._scene:GetMeChairID() then
		local cardType = GameLogic:GetCardType(self._scene._scene.cbTurnCardData)
        if cardType==cmd.CT_SINGLE or 
            cardType==cmd.CT_SINGLE_LINE or 
            cardType==cmd.CT_DOUBLE_LINE or
            cardType==cmd.CT_THREE_LINE  or
            cardType==cmd.CT_BOMB then
                
            self:setNotSelectCardEffect(self._scene._scene.cbTurnCardData)
        end
	end
	self.mCurSelect = {}
	self.beginPoint = nil
	self.mSelectCardData = {}
	self:detectionPopupCard()
end

--触摸事件处理
function CardLayer:onTouchBegan(touch, event)
	local pos = touch:getLocation()
	print("touch begin1!", pos.x, pos.y)


	if self.isTouchEnable == false then
		--return false
	end

	if pos.y>230 and self.bClickZero then
		self:resetMyCard()
		return
	end
	

	local posIndex = self:getIndexByPos(pos)
	self.mCurSelect = {}
	if posIndex then
		self.beginPoint = pos
		self.mLastIndex = posIndex
		self.isBreakTouch = false
		table.insert(self.mCurSelect ,posIndex)
		--print("hhhhhhhhhhhhhhhhhhhhhhh,"..posIndex.x..","..posIndex.y)
		self:showSelectArea()

	else
		self.beginPoint  = nil
		return false
	end
	
	
	return true
end

function CardLayer:onTouchMoved(touch, event)
	if  self.isBreakTouch then

		return true
	end
	local pos = touch:getLocation()
	local posIndex = self:getIndexByPos(pos)
	if posIndex and self.beginPoint and math.abs(posIndex - self.mLastIndex)<=1 then
		
		self.mLastIndex = posIndex
		if self.mCurSelect[#self.mCurSelect]~=posIndex then
			local breakFlag = 0
			for k,v in pairs(self.mCurSelect) do
				if v == posIndex then
					breakFlag = k
					break
				end
			end
			if breakFlag~= 0 then
				for i= #self.mCurSelect,1, -1 do
					if i>breakFlag then
						table.remove(self.mCurSelect,i)
					end
				end
			else
				table.insert(self.mCurSelect ,posIndex)
			end

			
			self:showSelectArea()
		end
		
	else
		self.isBreakTouch = true
		self:cleanSelectArea()
		return true
	end

	return true
end

function CardLayer:onTouchEnded(touch, event)
	if  self.isBreakTouch then
		return 
	end
	local pos = touch:getLocation()
	
	self:popSelectCard()
	--判断选择的牌是否符合出牌条件
	self.mSelectCardData = {}
	for i=1,#self.mSelectCard do
		
		table.insert(self.mSelectCardData,self.cbCardData[self.mSelectCard[i]])
	end
	--local temp = clone(self.mSelectCardData)
	

	dump(self.mSelectCardData,"hhhhhhhhhhhhhhhhhhhhh")
	if self.isTouchEnable then
		self:detectionPopupCard()
	end

	self:cleanSelectArea()
end


function CardLayer:detectionPopupCard()

	for i=1,#self.mCurSelect do
		local card =  self.myCardNode:getChildByTag(self.mCurSelect[i])
		if card:getChildByTag(123) then
			self:resetMyCard()
			return;
		end
	end

	local bOk = self._scene._scene:detectionOutCard(self.mSelectCardData)
	self._scene:setGameBtnStatus(nil, nil, bOk)
end


function CardLayer:getPopupCard()
	
	if #self.mSelectCardData <=0 then
		return nil
	else
		return self.mSelectCardData
	end
end


return CardLayer
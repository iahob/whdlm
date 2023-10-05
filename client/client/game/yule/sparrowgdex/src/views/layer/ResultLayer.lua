local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.models.CMD_Game")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.views.layer.CardLayer")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.models.GameLogic")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local ResultLayer = class("ResultLayer", function(scene)
	local resultLayer = cc.CSLoader:createNode(cmd.RES_PATH.."gameResult/GameResultLayer.csb")
	return resultLayer
end)


ResultLayer.WINNER_ORDER					= 1


function ResultLayer:onInitData()
	--body
	self.winnerIndex = nil
	self.bShield = false
	self.myInfoNode = nil
end

function ResultLayer:onResetData()
	--body
	self.winnerIndex = nil
	self.bShield = false

	self:removeChildByTag(100)
	if nil ~= self.myInfoNode then
		self.myInfoNode:removeChildByTag(100)
		self.myInfoNode:removeChildByTag(101)
		self.myInfoNode:removeChildByTag(102)
		self.myInfoNode:removeChildByTag(103)
		self.myInfoNode:removeChildByTag(104)
		self.myInfoNode:removeChildByTag(105)
	end
end

function ResultLayer:ctor(scene)
	self._scene = scene
	self:onInitData()
	ExternalFun.registerTouchEvent(self, true)

	local btnExit = self:getChildByName("Button_exit")
	btnExit:addClickEventListener(function(ref)
		print("退出")
		self:hideLayer()
		self._scene:onButtonClickedEvent(self._scene.BT_EXIT)
	end)

	local btnChange = self:getChildByName("Button_change")
	btnChange:addClickEventListener(function(ref)
		print("换桌")
		--self._scene:onButtonClickedEvent(self._scene.BT_EXIT)
		self:hideLayer()
		self._scene._scene:onChangeDesk()
	end)

	local btContinue = self:getChildByName("Button_continue")
	btContinue:addClickEventListener(function(ref)
	  	if PriRoom and GlobalUserItem.bPrivateRoom then

	        if PriRoom:getInstance().m_tabPriData.dwPlayCount < PriRoom:getInstance().m_tabPriData.dwDrawCountLimit and not self._scene._scene.bIsPriEnd then
	            self._scene:onButtonClickedEvent(self._scene.BT_START)
	        else
	        	--self._scene._priView:onShowGameEnd()
	        	-- local endLayer = self._scene._priView:getChildByName("private_end_layer")
	        	-- if endLayer then
	        	-- 	endLayer:setVisible(true)
	        	-- end
	        	self._scene._priView:showEndLayer()
	        end--self._scene._priView
	        --self._scene._scene.openAllResult = true
	    else
	    	self._scene:onButtonClickedEvent(self._scene.BT_START)
	    end
		self:hideLayer()
		self._scene.btStart:setVisible(false)
	end)

	if GlobalUserItem.bPrivateRoom  then
		btnExit:setVisible(false)
		btnChange:setVisible(false)
		btContinue:setPositionX(667)
	end
end

function ResultLayer:onTouchBegan(touch, event)
	local pos = touch:getLocation()
	--print(pos.x, pos.y)
	local rect = cc.rect(122, 25, 976, 680)
	if not cc.rectContainsPoint(rect, pos) then
		self:hideLayer()
	end
	return self.bShield
end

function ResultLayer:showLayer(resultList, wBankerChairId, myChirID,cbHuCard, maData, validMaNum, validMaData, wFleeUser, isNoMagicCard)
	assert(type(resultList) == "table")
	local width = 44
	local height = 67

local btnExit = self:getChildByName("Button_exit")
	local btContinue = self:getChildByName("Button_continue")
local btnChange = self:getChildByName("Button_change")
	if GlobalUserItem.bPrivateRoom  then
		if self._scene._scene.playMode == 1 then
			btnExit:setVisible(true)
			btnChange:setVisible(false)
			btContinue:setVisible(true)

			btnExit:setPositionX(500)
			btContinue:setPositionX(1334-500)
		else
			btnExit:setVisible(false)
			btnChange:setVisible(false)
			btContinue:setVisible(true)

			btContinue:setPositionX(667)
		end 
	else
		btnExit:setVisible(true)
		btnChange:setVisible(true)
		btContinue:setVisible(true)

		btnExit:setPositionX(967)
		btnChange:setPositionX(667)
		btContinue:setPositionX(377)
	end

	
	

	----公共部分
	if wFleeUser ~= yl.INVALID_CHAIR then
		-- local sprWin = self:getChildByName("Sprite_win_or_lose")
		-- local sprFlag = cc.Sprite:create(cmd.RES_PATH.."gameResult/bg_win.png")
		-- print("获取玩家输赢标示", sprWin, sprFlag)
		-- if nil ~= sprFlag then
		-- 	sprWin:setSpriteFrame(sprFlag:getSpriteFrame())
		-- end

		local win_icon = self:getChildByName("win_icon")
		win_icon:setVisible(false)

		local Text_HuType = self:getChildByName("Text_HuType")
		Text_HuType:setVisible(false)
		local Text_HuNum = self:getChildByName("Text_HuNum")
		Text_HuNum:setVisible(false)
		
		local Sprite_win = self:getChildByName("Sprite_win")
		Sprite_win:setVisible(false)

		local labHuName = self:getChildByName("Text_HuNick")
		labHuName:setVisible(false)

		--提示玩家逃跑
		--先移除
		self:removeChildByTag(1)
		local FleeUserName = resultList[wFleeUser +1].userItem.szNickName
		local strTips = string.format("玩家%s逃跑了", FleeUserName)
		local labelTips = ccui.Text:create(strTips, "fonts/round_body.ttf", 24)
        :addTo(self)
        :setColor(cc.c4b(0x63,0x40,0x30,255))
        :setTag(1)
        :move(667, 550)
	end

	local huChair = yl.INVALID_CHAIR;
	for i = 1, cmd.GAME_PLAYER do
		if i <= #resultList then
			--输赢的标示(默认输)
			if i == myChirID +1 then
				--界面显示赢
				local sprWin = self:getChildByName("Sprite_win_or_lose")
				local sprFlag = nil
				if resultList[i].lHuScore > 0 or wFleeUser ~= yl.INVALID_CHAIR then
					sprFlag = cc.Sprite:create(cmd.RES_PATH.."gameResult/bg_win.png")
				else
					sprFlag = cc.Sprite:create(cmd.RES_PATH.."gameResult/bg_lose.png")
				end
				if nil ~= sprFlag then
					sprWin:setSpriteFrame(sprFlag:getSpriteFrame())
				end
			end

			--显示胡牌玩家
			if resultList[i].lHuScore > 0 then
				local labHuName = self:getChildByName("Text_HuNick")
				if nil ~= labHuName then
					labHuName :setString(resultList[i].userItem.szNickName)
				end
				--展示胡牌麻将
				local nodeWinCard = self:getChildByName("Node_winCard")
				--先移除
				nodeWinCard:removeAllChildren()
				local pos = cc.p(0, 0)
				for j=1,#resultList[i].cbCardData do
					local sprCard = self._scene._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, resultList[i].cbCardData[j], false)
					if nil ~= sprCard then
						nodeWinCard:addChild(sprCard)
						pos = cc.p(pos.x + width, 0)
						sprCard:setPosition(pos)
					end
				end
				--显示碰刚的牌
				pos = cc.p(pos.x + 20, 0)
				if nil ~=  resultList[i].cbActiveCardData then
					for j=1,#resultList[i].cbActiveCardData do
						local tagAvtiveCard = resultList[i].cbActiveCardData[j]
						for num=1,tagAvtiveCard.cbCardNum do
							local sprCard = self._scene._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, tagAvtiveCard.cbCardValue[1], false)
							if nil ~= sprCard then
								nodeWinCard:addChild(sprCard)
								pos = cc.p(pos.x + width, 0)
								sprCard:setPosition(pos)
							end
						end
					end
				end

				--显示胡的牌
				pos = cc.p(pos.x + 20, 0)
				local sprCard = self._scene._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, cbHuCard, false)
				if nil ~= sprCard then
					nodeWinCard:addChild(sprCard)
					pos = cc.p(pos.x + width, 0)
					sprCard:setPosition(pos)
				end
				
				huChair = i -1
			end


			local infoNode = self:getChildByName("FileNode_info"..string.format("%d", i))
			if nil ~= infoNode then

				if i == myChirID +1 then
					--底框（我）
					local bgMy = self:getChildByName("mine_bg_7")
					if nil ~=  bgMy then
						bgMy:setPositionY(infoNode:getPositionY())
					end
					--输赢（我）默认赢
					local flagWin = self:getChildByName("Sprite_win")
					if nil ~= flagWin then
						flagWin:setPositionY(infoNode:getPositionY())
						local sprFlag = nil
						if resultList[i].lHuScore < 0 then
							sprFlag = cc.Sprite:create(cmd.RES_PATH.."gameResult/icon_lose.png")
						else
							sprFlag = cc.Sprite:create(cmd.RES_PATH.."gameResult/icon_win.png")
						end
						if nil ~= sprFlag then
							flagWin:setSpriteFrame(sprFlag:getSpriteFrame())
						end
					end

					--总数赢
					local totleScore = resultList[i].lScore
					local labtotleScore = nil
					if totleScore > 0 then --胡牌特殊显示
						labtotleScore = cc.LabelAtlas:create(string.format("/%d", totleScore), cmd.RES_PATH.."gameResult/num_add.png", 26, 37, string.byte("/"))
					else
						labtotleScore = cc.LabelAtlas:create(string.format("/%d", math.abs(totleScore)), cmd.RES_PATH.."gameResult/num_reduction.png", 26, 37, string.byte("/"))
					end
					self:addChild(labtotleScore, 0, 100)
					labtotleScore:setPosition(cc.p(410, 635))

					self.myInfoNode = infoNode
				end


				--庄家标示
				local flag = infoNode:getChildByName("zhuang_2")
				if wBankerChairId +1 == i then
					if nil ~=  flag then
						flag:setVisible(true)
					end
				else
					if nil ~= flag then
						flag:setVisible(false)
					end
				end
				--昵称
				local name = infoNode:getChildByName("Text_name")
				if nil ~= name then
					local clipNick = ClipText:createClipText(cc.size(110, 20), resultList[i].userItem.szNickName)
			        clipNick:setAnchorPoint(cc.p(0, 0.5))
			        clipNick:setPosition(name:getPositionX(), 0)
			        infoNode:addChild(clipNick)
			        clipNick:setTextColor(cc.c3b(0x6b,0x49,0x38))
					--name:setString(resultList[i].userItem.szNickName)
					name:setVisible(false)
				end

				--胡分
				local huScore = infoNode:getChildByName("Text_winOrlose")
				if nil ~= huScore then
					local sprHu = nil 
					if resultList[i].lHuScore > 0 then --胡牌特殊显示
						sprHu = cc.Sprite:create(cmd.RES_PATH.."gameResult/hu.png")
					else
						sprHu = cc.Sprite:create(cmd.RES_PATH.."gameResult/lose.png")
					end
					if i == myChirID +1 then
						huScore:setVisible(false)
						sprHu:addTo(infoNode, 0, 100)
						sprHu:setPosition(huScore:getPosition())
					else
						if resultList[i].lHuScore > 0 then
							huScore:setString(string.format("胡了"))
						else
							huScore:setString(string.format("未胡"))
						end
					end	
				end
				local lab_huscore = infoNode:getChildByName("Text_huscore")
				if nil ~= lab_huscore then
					local labHu = nil 
					if resultList[i].lHuScore > 0 then --胡牌特殊显示
						labHu = cc.LabelAtlas:create(string.format("%d", resultList[i].lHuScore), cmd.RES_PATH.."gameResult/num_win.png", 15, 23, string.byte("/"))
					elseif resultList[i].lHuScore == 0 then
						labHu = cc.LabelAtlas:create(string.format("%d", 0), cmd.RES_PATH.."gameResult/num_lose.png", 15, 23, string.byte("/"))
					else
						labHu = cc.LabelAtlas:create(string.format("/%d", math.abs(resultList[i].lHuScore)), cmd.RES_PATH.."gameResult/num_lose.png", 15, 23, string.byte("/"))
					end
					if i == myChirID +1 then
						lab_huscore:setVisible(false)
						labHu:addTo(infoNode,0, 101)
						labHu:setPosition(lab_huscore:getPosition())
						labHu:setAnchorPoint(cc.p(0.5, 0.5))
					else
						lab_huscore:setString(string.format("%d", resultList[i].lHuScore))
					end	
				end

				--摸马
				local MaScore = infoNode:getChildByName("Text_ma")
				if nil ~= MaScore then
					local sprMa = nil 
					if resultList[i].lHuScore > 0 then --胡牌特殊显示
						sprMa = cc.Sprite:create(cmd.RES_PATH.."gameResult/ma_win.png")
					else
						sprMa = cc.Sprite:create(cmd.RES_PATH.."gameResult/ma_lose.png")
					end
					if i == myChirID +1 then
						MaScore:setVisible(false)
						sprMa:addTo(infoNode, 0, 102)
						sprMa:setPosition(MaScore:getPosition())
					end	
				end
				local lab_mascore = infoNode:getChildByName("Text_mascore")
				if nil ~= lab_mascore then
					local labMa = nil 
					if resultList[i].lHuScore > 0 then --胡牌特殊显示
						labMa = cc.LabelAtlas:create(string.format("%d", resultList[i].lMaScore), cmd.RES_PATH.."gameResult/num_win.png", 15, 23, string.byte("/"))
					else
						labMa = cc.LabelAtlas:create(string.format("/%d", math.abs(resultList[i].lMaScore)), cmd.RES_PATH.."gameResult/num_lose.png", 15, 23, string.byte("/"))
					end
					if 0 == resultList[i].lMaScore then
						labMa:setString(string.format("%d", 0))
					end
					if i == myChirID +1 then
						lab_mascore:setVisible(false)
						labMa:addTo(infoNode, 0, 103)
						labMa:setPosition(lab_mascore:getPosition())
						labMa:setAnchorPoint(cc.p(0.5, 0.5))
					else
						lab_mascore:setString(string.format("%d", resultList[i].lMaScore))
					end	
				end

				--杠分
				local gangScore = infoNode:getChildByName("Text_gang")
				if nil ~= gangScore then
					local sprGang = nil 
					if resultList[i].lHuScore > 0 then --胡牌特殊显示
						sprGang = cc.Sprite:create(cmd.RES_PATH.."gameResult/gang_win.png")
					else
						sprGang = cc.Sprite:create(cmd.RES_PATH.."gameResult/gang_lose.png")
					end
					if i == myChirID +1 then
						gangScore:setVisible(false)
						sprGang:addTo(infoNode, 0, 104)
						sprGang:setPosition(gangScore:getPosition())
					end	
				end
				local lab_gangscore = infoNode:getChildByName("Text_gangscore")
				if nil ~= lab_gangscore then
					local labGang = nil 
					if resultList[i].lGangScore >= 0 then --胡牌特殊显示
						labGang = cc.LabelAtlas:create(string.format("/%d", resultList[i].lGangScore), cmd.RES_PATH.."gameResult/num_win.png", 15, 23, string.byte("/"))
					else
						labGang = cc.LabelAtlas:create(string.format("/%d", math.abs(resultList[i].lGangScore)), cmd.RES_PATH.."gameResult/num_lose.png", 15, 23, string.byte("/"))
					end
					if resultList[i].lGangScore >= 0 then --胡分控制颜色，杠分控制加减
						labGang:setString(string.format("%d", resultList[i].lGangScore))
					else
						labGang:setString(string.format("/%d", math.abs(resultList[i].lGangScore)))
					end
					if i == myChirID +1 then
						lab_gangscore:setVisible(false)
						labGang:addTo(infoNode, 0, 105)
						labGang:setPosition(lab_gangscore:getPosition())
						labGang:setAnchorPoint(cc.p(0.5, 0.5))
					else
						lab_gangscore:setString(string.format("%d", resultList[i].lGangScore))
					end	
				end

				--胡牌类型
				print("结算界面胡牌类型1", resultList[i].cbChHuKind, resultList[i].dwChiHuRight[1])
				if GameLogic.WIK_CHI_HU == resultList[i].cbChHuKind then
					local huString, num, fan = GameLogic.getChiHuRightInfo(resultList[i].dwChiHuRight)
					if num > 0 then
						for j=1,num do
							local labType = self:getChildByName("Text_HuType")
							if nil ~= labType then
								labType:setString(huString[j])
								print("结算界面胡牌类型 牌型", huString[j])
							end
							local labFan = self:getChildByName("Text_HuNum")
							if nil ~= labFan then
								labFan:setString(string.format("X%d", fan[j]))
								print("结算界面胡牌类型 番数", fan[j])
							end
						end
					end
				end	
			end
		else
		end
	end

	--是否显示无鬼加倍
	local labMagic = self:getChildByName("Text_NoMagic")
	if nil ~= labMagic then
		if isNoMagicCard then
			labMagic:setVisible(true)
		else
			labMagic:setVisible(false)
		end
	end


	--码数
	local maNum = self:getChildByName("text_ma")
	if nil ~= maNum then
		maNum:setString(string.format("%d", validMaNum))
	end

	--码牌
	local nodeMa = self:getChildByName("Node_maCard")
	for i=1,#maData do
		--绘制码牌
		local sprCard = self._scene._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, maData[i], false)
		if nil ~= sprCard then
			nodeMa:addChild(sprCard, i)
			sprCard:setPosition(cc.p(width *(i -1), 0))
			-- if not self._scene:isValidMa(wBankerChairId, huChair, maData[i]) then
			-- 	sprCard:setColor(cc.c3b(127,127,127))
			-- end
			local isValidMa = false
			for j=1,validMaNum do
				if maData[i] == validMaData[j] then
					isValidMa = true
					break
				end
			end
			if not isValidMa then
				sprCard:setColor(cc.c3b(127,127,127))
			end
		end
	end


	self.bShield = true
	self:setVisible(true)
	self:setLocalZOrder(yl.MAX_INT)
end

function ResultLayer:hideLayer()
	if not self:isVisible() then
		return
	end
	self._scene._scene.openAllResult = true
	self:onResetData()
	self:setVisible(false)
	self._scene.btStart:setVisible(true)


end
return ResultLayer
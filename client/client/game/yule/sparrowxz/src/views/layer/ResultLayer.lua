local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.models.CMD_Game")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.CardLayer")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.models.GameLogic")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PlayerInfo = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.PlayerInfo")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")


local CARD_SCALE = 0.8

local ResultLayer = class("ResultLayer", function(scene)
	local resultLayer = cc.CSLoader:createNode(cmd.RES_PATH.."gameEnd/GameEnd.csb")
	return resultLayer
end)

function ResultLayer:createCard(value)
	local cardBg = cmd.RES_PATH.."card/dowm_small.png"
	local valueFile = cmd.RES_PATH.."card/my_small/tile_meUp_"
	local card = cc.Sprite:create(cardBg)
	--获取数值
	local cardIndex = GameLogic.SwitchToCardIndex(value)
	if cardIndex < 10 then
		valueFile = valueFile..string.format("0%d", cardIndex)..".png"
	else
		valueFile = valueFile..string.format("%d", cardIndex)..".png"
	end
	local spriteValue = display.newSprite(valueFile)
	card:addChild(spriteValue)
	spriteValue:setPosition(card:getContentSize().width/2,card:getContentSize().height/2 + 5)
	return card
end

function ResultLayer:createActiveCardWithPos(node, value, num, pos)

	for i=1,num do
		local card = self:createCard(value)
		card:setScale(CARD_SCALE)
		node:addChild(card,i)
		local cardSize = card:getContentSize()
		if i == 4 then
			card:setPosition(pos.x - CARD_SCALE*cardSize.width ,pos.y+10)
		else
			pos = cc.p(pos.x + CARD_SCALE*cardSize.width , pos.y)
		    card:setPosition(pos)
		end
	end
	pos = cc.p(pos.x + 5, pos.y)
	return pos

end

function ResultLayer:ctor(scene,tabUserItem,tabHupaiInfo,pGameEnd,leftUserViewId,bankerViewId)
	self._scene=scene
	local huCards = self._scene._cardLayer.huCard
dump(self._scene._cardLayer.huCard,"huCard")

    self.dwChiHuRight = pGameEnd.dwChiHuRight[1]
	self.m_tabPlayer={}
	for i=1,cmd.GAME_PLAYER do
		self.m_tabPlayer[i]=self:getChildByName("FileNode_"..i)
	end

	local activeCardDatas = self._scene._cardLayer:getActiveCardData()
	for chairId = 1, cmd.GAME_PLAYER do
		local removeHuCard = false
		local viewId = self._scene._scene:SwitchViewChairID(chairId-1)
		local cardNode = self.m_tabPlayer[viewId]:getChildByName("Node_card")

		local carddata = activeCardDatas[viewId]
		local pos = cc.p(-40,0)
		for i= 1, #carddata do
			print("carddata.cbCardValue, carddata.cbCardNum",carddata[i].cbCardValue[1], carddata[i].cbCardNum)
			pos = self:createActiveCardWithPos(cardNode, carddata[i].cbCardValue[1], carddata[i].cbCardNum, pos)
		end

		print("pGameEnd.cbCardCount[1][viewId]: ",pGameEnd.cbCardCount[1][chairId])
		local cardData = {}
		for i=1, pGameEnd.cbCardCount[1][chairId] do
			local value = pGameEnd.cbCardData[chairId][i]
			if value == huCards[viewId] and removeHuCard == false then
				removeHuCard = true
			else
				table.insert(cardData,pGameEnd.cbCardData[chairId][i])
			end
		end
		if huCards[viewId] > 0 then
			table.insert(cardData,huCards[viewId])
		end

		for i=1, #cardData do
			-- table.insert(cbCardData,pGameEnd.cbCardData[chairId][i])
			local card = self:createCard(cardData[i])
			card:setScale(CARD_SCALE)
			cardNode:addChild(card)
			card:setPosition(pos.x + card:getContentSize().width*CARD_SCALE*(i), 0)
			if i == #cardData and huCards[viewId] > 0 then
				card:setPositionX(card:getPositionX()+10)
			end
		end
	end

	self:showTopInfo()
	self:initPlayerInfo(tabUserItem,tabHupaiInfo,leftUserViewId,bankerViewId)
	self:initDetails(pGameEnd.tagHistoryChart, pGameEnd.dwChiHuRight)
	local btnExit = self:getChildByName("Button_exit")
	btnExit:addClickEventListener(function(ref)
		print("退出")
		self._scene:onButtonClickedEvent(self._scene.BT_EXIT)
	end)

	local btnChangeDesk = self:getChildByName("Button_changedesk")
	btnChangeDesk:addClickEventListener(function(ref)
		print("换桌")
		print("me cbUserStatus",self._scene._scene:GetMeUserItem().cbUserStatus )
		LogAsset:getInstance():logData("me cbUserStatus "..(self._scene._scene:GetMeUserItem().cbUserStatus or " nil"))
		--assert(self._scene._scene:GetMeUserItem().cbUserStatus<yl.US_PLAYING)
		self._scene._scene:queryChangeDesk()
		self._scene:removeExitedUsers()
		self._scene._scene:OnResetGameEngine()
		-- self:removeSelf()
	end)

	local btShare = self:getChildByName("Button_Share")
	btShare:addClickEventListener(function(ref)
		self:recodeShow()
	end)

	local btContinue = self:getChildByName("Button_Continue")
	btContinue:addClickEventListener(function(ref)
		if self._scene._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能", 1)
          	return
		end
		self._scene:removeExitedUsers()
		self._scene:onButtonClickedEvent(self._scene.BT_START)
		-- self:removeSelf()
	end)

	local btnExit = self:getChildByName("Button_exit")
	if not GlobalUserItem.bPrivateRoom then 
		btnExit:setEnabled(false)
		btnExit:setVisible(false)
		btnChangeDesk:setEnabled(true)
		btnChangeDesk:setVisible(true)
		btShare:setEnabled(true)
		btShare:setVisible(true)
	else
		btnExit:setEnabled(true)
		btnExit:setVisible(true)
		btnChangeDesk:setEnabled(false)
		btnChangeDesk:setVisible(false)
		btShare:setEnabled(false)
		btShare:setVisible(false)
	end

	ExternalFun.registerTouchEvent(self,true)
end

function ResultLayer:onTouchBegan()
	return true
end

function ResultLayer:initPlayerInfo(tabUserItem,tabHupaiInfo,leftUserViewId,bankerViewId)
	for i=1,cmd.GAME_PLAYER do
		local viewId = self._scene._scene:SwitchViewChairID(i -1)
		local node=self.m_tabPlayer[viewId]
		local head=PopupInfoHead:createNormal(tabUserItem[viewId], 76)
		--创建裁剪
	    local strClip = "game/head_mask.png"
	    clipSp = cc.Sprite:create(strClip)
	    local clip = cc.ClippingNode:create()
	        --裁剪
	    local headbg=node:getChildByName("playerNode")
	    local sz=headbg:getContentSize()
	    clip:setStencil(clipSp)
	    clip:setAlphaThreshold(0.01)
	    clip:addChild(head)
	    clip:addTo(headbg)
	    clip:setPosition(sz.width/2,sz.height/2)
	    print("i:",i)
	    assert(tabUserItem)
	    assert(tabUserItem[viewId])
		local clipText=ClipText:createClipText({width=50,height=20},tabUserItem[viewId].szNickName,"fonts/round_body.ttf",10)
		clipText:setTextFontSize(16)
		clipText:setAnchorPoint(cc.p(0,0.5))
	    clipText:addTo(node:getChildByName("name"))
	    clipText.m_text:setTextColor(cc.c3b(51,51,51))
	    local huTxt = node:getChildByName("bHupai")
	    if leftUserViewId==viewId then
	    	huTxt:setString(tabHupaiInfo[viewId].bHu==true and "已离开" or "逃跑")
	    else
			huTxt:setString(tabHupaiInfo[viewId].bHu==true and "胡牌" or "未胡牌")
			if bit:_and(GameLogic.CHR_ZIMO, self.dwChiHuRight[i]) ~= 0 then
				huTxt:setString("自摸")
			end
		end
		print("tabHupaiInfo[viewId].gameScore:",tabHupaiInfo[viewId].gameScore)
		local scorestr=tabHupaiInfo[viewId].gameScore
		if scorestr==nil then scorestr="0" 
		elseif scorestr>=0 then scorestr="+"..tabHupaiInfo[viewId].gameScore 
		else node:getChildByName("coinnum"):setTextColor(cc.c4b(0x58,0x6e,0xbe,0xff)) end
		node:getChildByName("coinnum"):setString( scorestr)

		node:getChildByName("banker"):setVisible(bankerViewId == viewId)
	end
end

function ResultLayer:initDetails(tagCharts,dwChiHuRight)
	--local tagChart={charttype=cmd.CHARTTYPE.HUJIAOZHUANYI_TYPE,lTimes=200,lScore=20000,bEffectChairID={true,true,true,true}}
	--tagCharts={tagChart,tagChart,tagChart,tagChart,tagChart,tagChart,tagChart,tagChart,tagChart,tagChart}

	for id=1,cmd.GAME_PLAYER do
		local viewId = self._scene._scene:SwitchViewChairID(id -1)

		local temp={}
		local text = ""
		for i=1,#tagCharts[id] do
			local tagChart=tagCharts[id][i]
			if tagChart and tagChart.lScore~=0 and tagChart.charttype~=cmd.CHARTTYPE.INVALID_CHARTTYPE then
				text = text .. cmd.CHARTTYPESTR[tagChart.charttype-21]
				local strs={}
				local objs={"东","北","西","南"}
				for j=1,cmd.GAME_PLAYER do
					local bEffectChairID = tagChart.bEffectChairID[j]
					if viewId~=cmd.MY_VIEWID then
						bEffectChairID = tagChart.bEffectChairID[1][j]
					end
					print("bEffectChairID = ",bEffectChairID)
					if bEffectChairID==true then
						local _viewid=self._scene._scene:SwitchViewChairID(j-1)
						-- if _viewid~=cmd.MY_VIEWID then table.insert(strs,_objs[_viewid]) table.insert(strs,objs[j])end
						-- table.insert(strs,_objs[_viewid]) 
						table.insert(strs,objs[j])
					end
				end
				text = text .. "("..table.concat(strs, "、")..") "
				-- table.insert(temp,tagChart)
			end
		end
		local node=self.m_tabPlayer[viewId]
		local details = node:getChildByName("details")
		local ChiHuRight = GameLogic.getChiHuRightInfo(dwChiHuRight[1][id])
		print("胡牌信息"..table.concat(ChiHuRight," "))
		details:setString(text.." "..table.concat(ChiHuRight,"、"))
	end
end

function ResultLayer:recodeShow()
	print("战绩炫耀")
	if not PriRoom then
		return
	end

    --GlobalUserItem.bAutoConnect = false
    PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
        bMyFriend = bMyFriend or false
        local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "战绩炫耀成功", 2)
            end
            --GlobalUserItem.bAutoConnect = true
        end
        local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
        -- 截图分享
        local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
        local area = cc.rect(0, 0, framesize.width, framesize.height)
        local imagename = "grade_share.jpg"
        if bMyFriend then
            imagename = "grade_share_" .. os.time() .. ".jpg"
        end
        ExternalFun.popupTouchFilter(0, false)
        captureScreenWithArea(area, imagename, function(ok, savepath)
            ExternalFun.dismissTouchFilter()
            if ok then
                if bMyFriend then
                    PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_FRIENDLIST, function( frienddata )
                        PriRoom:getInstance():imageShareToFriend(frienddata, savepath, "分享我的战绩")
                    end)
                elseif nil ~= target then
                    MultiPlatform:getInstance():shareToTarget(target, sharecall, "我的战绩", "分享我的战绩", url, savepath, "true")
                end
            end
        end)
    end, 3)
end

function ResultLayer:showTopInfo()

	local huawen = self:getChildByName("huawen_1")
	local text1 = huawen:getChildByName("Text_1")
    local ruleStr = "玩法："
	if self._scene._scene.bHuanSanZhang then --true为允许换三张
		ruleStr = ruleStr.."换三张 "
	end
	if self._scene._scene.bHuJiaoZhuanYi then --true允许呼叫转移
		ruleStr = ruleStr.."呼叫转移"
	end
	local txt = ""
    if GlobalUserItem.bPrivateRoom then
		if self._scene.cbPlayMode == 0 then
		    txt = txt .."房号："..PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
	    
		    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
			txt = txt .."  局数："..strcount
		end
    else
        local lvStr = {"新手场", "初级场", "进阶场", "中级场", "高级场", "精英场", "特级场", "顶级场", "大师场", "牌王场",}
    	txt = txt .."金币场："..lvStr[self._scene._scene.szServerName + 1].."  底分:"..self._scene._scene.lCellScore
    end
	text1:setString(txt .. "  "..ruleStr)

	local time = huawen:getChildByName("time")
	time:setString(os.date("%Y-%m-%d %H:%M:%S", os.time()))
end
return ResultLayer
--
-- Author: Tang
-- Date: 2016-10-13 16:08:13
--
local userlist = {
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
	{szNickName = "haha", lScore = 1000},
}
local BankerList = class("BankerList",  cc.Layer)

local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local g_var = ExternalFun.req_var

local multi = 1

function BankerList:ctor(_scene)
    self._scene = _scene
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    
	local rootLayer, csbNode = ExternalFun.loadRootCSB("image/DispatchBagLayer.csb",self)
	self._rootNode = csbNode


    self.m_bagMinScore = self._scene._bagMinScore
    self.m_bagMaxScore = self._scene._bagMaxScore
    self.m_bagCondition = self._scene._bagCondition
    print("红包限制",self.m_bagMinScore,self.m_bagMaxScore,self.m_bagCondition)

    self.m_thunder = 0
    self.m_bagScore = self.m_bagMinScore

    self.txt_thunder = self._rootNode:getChildByName("Text_Thunder")
    self.txt_thunder:setString("0")
    self.txt_bagScore = self._rootNode:getChildByName("Text_BagScore")
    self.txt_bagScore:setString(self.m_bagScore)

    self.txt_thunder:setVisible(true)
    self.txt_bagScore:setVisible(true)
    self.txt_thunder:setLocalZOrder(100)
    self.txt_bagScore:setLocalZOrder(100)

    self:initSlider()
    self:InitData()
    self:initButtons()
    self:initEditBox()
end

function BankerList:initButtons()
	local btn = self._rootNode:getChildByName("Button_1")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
                ExternalFun.playClickEffect()
            	if self.m_bagScore >= self.m_bagMinScore then
                    print(self.m_bagScore , (GlobalUserItem.tabAccountInfo.lUserScore / self._scene.nRatio))
                    if self.m_bagScore > (GlobalUserItem.tabAccountInfo.lUserScore / self._scene.nRatio) then

                        local runScene = cc.Director:getInstance():getRunningScene()
                        if nil ~= runScene then
                            showToast(runScene, "金币不足", 1)
                        end
                    else
            		    self._scene:sendUserBet(self.m_bagScore, self.m_thunder)
                    end
            	else
                    local runScene = cc.Director:getInstance():getRunningScene()
                    if nil ~= runScene then
                        showToast(runScene, "红包金额不能小于".. self.m_bagMinScore, 1)
                    end
            	end
            end
        end)

	local btn = self._rootNode:getChildByName("Button_close")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
                ExternalFun.playClickEffect()
           		self:removeFromParent()
            end
        end)

	-- 减
	local btn = self._rootNode:getChildByName("Button_3")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
                ExternalFun.playClickEffect()
	            if self.m_bagScore > self.m_bagMinScore then
	           		self.m_bagScore = self.m_bagScore - 10
                    if self.m_bagScore < self.m_bagMinScore then
                        self.m_bagScore = self.m_bagMinScore
                    end
	           		self.txt_bagScore:setString(self.m_bagScore)
                    local changePer = (self.m_bagScore / self.m_bagMaxScore)*100
                    self.m_sliderBg:setPercent(changePer)
                    self.txt_bagScore:setVisible(true)
	           	end
            end
        end)

	-- 加
	local btn = self._rootNode:getChildByName("Button_4")
	btn:addTouchEventListener(function (ref,eventType)
            if eventType == ccui.TouchEventType.ended then
                ExternalFun.playClickEffect()
           		self.m_bagScore = self.m_bagScore + 10
                if self.m_bagScore > self.m_bagMaxScore then
                    self.m_bagScore = self.m_bagMaxScore
                end
           		self.txt_bagScore:setString(self.m_bagScore)
           		local changePer = (self.m_bagScore / self.m_bagMaxScore)*100
				self.m_sliderBg:setPercent(changePer)
                self.txt_bagScore:setVisible(true)
            end
        end)
end

function BankerList:InitData()
 
	print("iInitData ==========================BankerList========================================= >")
	self.m_pTableView = cc.TableView:create(cc.size(300, 370))
	self.m_pTableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
	self.m_pTableView:setPosition(cc.p(279,116))
	self.m_pTableView:setDelegate()
    self.m_pTableView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self.m_pTableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self.m_pTableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self.m_pTableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	self._rootNode:addChild(self.m_pTableView)
	self.m_pTableView:reloadData()

end


function BankerList:reloadData()
	self.m_pTableView:reloadData()
end

--------------------------------------------------------------------tableview
function BankerList.cellSizeForTable( view, idx )
	return 300,40
end

function BankerList:numberOfCellsInTableView( view )
	return #self._scene.m_DispatchBagList
end

function BankerList:tableCellAtIndex( view, idx )
	print("idx is =================================================================== >" .. idx)
	local cell = view:dequeueCell()
	local userlist = self._scene.m_DispatchBagList
	
	local useritem = userlist[idx+1]
	if nil == useritem then
        return cell 
    end
	
	if nil == cell then
		cell = cc.TableViewCell:new()

	
		--玩家昵称
		local nick =  g_var(ClipText):createClipText(cc.size(150, 20),useritem.szNickName)
		nick:setTag(2)
		nick:setAnchorPoint(cc.p(0.0,0.5))
		nick:setPosition(cc.p(10,20))
		cell:addChild(nick)


		--玩家游戏币
		local coin =  g_var(ClipText):createClipText(cc.size(150, 20),ExternalFun.numberThousands(useritem.lScore))  
		coin:setAnchorPoint(cc.p(1,0.5))
		coin:setTag(4)
		coin:setPosition(290, 20)
		cell:addChild(coin)
	else

		if nil ~= cell:getChildByTag(2) then 
			local nick = cell:getChildByTag(2)
			nick:setString(useritem.szNickName)
		end

		if nil ~= cell:getChildByTag(4) then
			local coin = cell:getChildByTag(4)
			coin:setString(ExternalFun.numberThousands(useritem.lScore))
		end
	end

	return cell
end

-----------------------------------------------------------------------------

function BankerList:initEditBox()
    --设置金额
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    self.m_editThunder = ccui.EditBox:create(cc.size(100, 35),"image/public_blank.png")
            :setPosition(852,287)
            -- :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 26)
            :setFont("fonts/round_body.ttf", 26) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            -- :setTag(TAG_ENUM.EDIT_NAME)
            :setMaxLength(1)
    self._rootNode:addChild(self.m_editThunder)
    self.m_editThunder:registerScriptEditBoxHandler(editHanlder)

    self.m_editBagScore = ccui.EditBox:create(cc.size(150, 45),"image/public_blank.png")
            :setPosition(836,444)
            -- :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            -- :setTag(TAG_ENUM.EDIT_NAME)
            :setMaxLength(12)
    self._rootNode:addChild(self.m_editBagScore)
    self.m_editBagScore:registerScriptEditBoxHandler(editHanlder)
end

function BankerList:onEditEvent(event, editbox)

    if editbox == self.m_editThunder then
        local src = editbox:getText() 
        if event == "return" then
            print("m_editThunder", src)

            self.m_editThunder:setText("")
			self.m_thunder = tonumber(src) or 0
            self.txt_thunder:setString(self.m_thunder)
            self.txt_thunder:setVisible(true)
        elseif event == "changed" then
            -- self.m_editThunder:setText(src)

            -- self.txt_thunder:setString(src)
        elseif event == "began" then
            self.txt_thunder:setString("")
        end
    elseif editbox == self.m_editBagScore then
        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)

            self.m_editBagScore:setText("")

			self.m_bagScore = tonumber(src) or 0
            if self.m_bagScore > self.m_bagMaxScore then
                self.m_bagScore = self.m_bagMaxScore
            end
            self.txt_bagScore:setString(self.m_bagScore)
            self.txt_bagScore:setVisible(true)

            local changePer = self.m_bagScore / (self.m_bagMinScore * (self.m_bagMaxScore / self.m_bagMinScore/100) * multi)
            self.m_sliderBg:setPercent(changePer)

        elseif event == "changed" then

        elseif event == "began" then
            self.txt_bagScore:setString(src)
        end
    end
end

function BankerList:initSlider()
    local slider_bg = self._rootNode:getChildByName("Slider_2")
    slider_bg:addEventListener(function( sender, eventType )
        self:onSliderEvent( sender,eventType )
    end)

    self.m_sliderBg = slider_bg
    self.m_sliderBg:setPercent((self.m_bagScore/self.m_bagMaxScore)*100)
end

function BankerList:onSliderEvent( sender,eventType )
    local changePer = sender:getPercent()
    print("changePer",changePer)

	-- self.m_sliderBg:setPercent(changePer)
    local num = changePer * (self.m_bagMaxScore /100)
    self.m_bagScore = tonumber(string.format("%d",num))
	-- self.m_bagScore = changePer * (self.m_bagMaxScore /100)
	print("self.m_bagScore",self.m_bagScore)
    if self.m_bagScore < self.m_bagMinScore then
        self.m_bagScore = self.m_bagMinScore
        self.m_sliderBg:setPercent(self.m_bagScore/self.m_bagMaxScore*100)
    end
	self.txt_bagScore:setString(self.m_bagScore)
end

--触摸回调
function BankerList:onTouchBegan(touch, event)
    return self:isVisible()
end

function BankerList:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self._rootNode:getChildByName("applyUIBg_1")
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

function BankerList:onSendBagSuccess()
    local sp = cc.Sprite:create("image/spt_setBombSuccess.png")
    sp:setPosition(self._rootNode:getContentSize().width/2, self._rootNode:getContentSize().height/2)
    sp:addTo(self._rootNode)
    sp:setScale(0.1)
    sp:setLocalZOrder(101)
    sp:runAction( cc.Sequence:create(cc.ScaleTo:create(0.1, 0.5), cc.DelayTime:create(0.5), cc.CallFunc:create( function() sp:removeFromParent() end)))
end

return BankerList
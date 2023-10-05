--为了不破换原来的出牌选牌逻辑,这里新建一个亮倒的层,控制亮倒选牌逻辑
local ExternalFun = require(appdf.EXTERNAL_SRC.."ExternalFun")
local cmd = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")

local uiShowOut = env.uiBase:ctor{
    zorder = 1200,
}

local MODE_CHOOSE = {
    kou_pai = 1,
    hu_kou = 2,
}

function uiShowOut:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiShowOut:init(parent, cardLayer, gameLayer, callback)
    if parent:getChildByName("_uiShowOut_") then
        return
    end
    local csbNode = cc.CSLoader:createNode("LayerCardLisen.csb")
    csbNode:setName("_uiShowOut_")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode
    cc.funAddChildListener(csbNode, "sp_1/Button_cancel", function() self:onClickKouCancel() end)
    cc.funAddChildListener(csbNode, "sp_1/Button_fin", function() self:onClickKouOk() end)
    -- cc.funAddChildListener(csbNode, "sp_2/Button_cancel", function() self:onClickHuKouCancel() end)
    cc.funAddChildListener(csbNode, "sp_2/Button_ok", function() self:onClickHuKouOK() end)

    cc.find(self.widget, "sp_1"):setVisible(true)
    cc.find(self.widget, "sp_2"):setVisible(false)
    self:registerTouchEvent()

    self.kou_card_idx = {}
    self.kou_card_values = {}
    self._cardLayer = cardLayer
    self._gameLayer = gameLayer
    self._callback = callback

    local canKouListData = self._gameLayer:getCanKouListData()
    if next(canKouListData) then
        self:chooseMode(MODE_CHOOSE.kou_pai)
        self:showKouTip()
    else
        self:chooseMode(MODE_CHOOSE.hu_kou)
        self:setTingListVisible(true)
    end
    self._cardLayer:setMyCardTouchEnabled(false)

    self._selectCount = 0
end

function uiShowOut:showKouTip()
    local cardLayer = self._cardLayer
    local my_viewid = cmd.MY_VIEWID
    local handCardNode = cardLayer.nodeHandCard[my_viewid]
    local cardData = cardLayer.cbCardData[my_viewid]

    local canKouListData = self._gameLayer:getCanKouListData()

    for idx,cardvalue in pairs(cardData) do
        if canKouListData[cardvalue] then
            cardLayer:setKouTipVisible(idx, true)
        end
    end
end

function uiShowOut:registerTouchEvent()
    local function onTouchBegan( touch, event )
        return self:onTouchBegan(touch, event)
    end

    local function onTouchMoved(touch, event)
        self:onTouchMoved(touch, event)
    end

    local function onTouchEnded( touch, event )
        self:onTouchEnded(touch, event)
    end

    local function onTouchCanceled( touch, event )
        self:onTouchCanceled(touch, event)
    end

    local listener = cc.EventListenerTouchOneByOne:create()
    listener:setSwallowTouches(true)
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN)
    listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED)
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED)
    listener:registerScriptHandler(onTouchCanceled,cc.Handler.EVENT_TOUCH_CANCELLED)
    local eventDispatcher = self.widget:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener,self.widget)
end

function uiShowOut:onClickKouCancel()
    --print("onClickKouCancel")
    --退出亮倒,直接进入打牌模式
    self._cardLayer:setMyCardTouchEnabled(true)
    -- dump(self.kou_card_idx, "#self.kou_card_idx==")
    --隐藏麻将扣提示
    for cardTag,_ in pairs(self.kou_card_idx) do
        self._cardLayer:setKouTipVisible(cardTag, false)
        self._gameLayer._gameView._cardLayer:setKouTipVisible(cardTag, false)
    end
    --麻将都缩回去
    self._cardLayer:setCardStandBy()
    self:destroy()
    --继续显示听/弃牌?
end

function uiShowOut:onClickKouOk()
    self:chooseMode(MODE_CHOOSE.hu_kou)
    self:setTingListVisible(true)
    self._cardLayer:setCardStandBy()
    for cardTag,_ in pairs(self.kou_card_idx) do
        self._cardLayer:setKouTipVisible(cardTag, false)
    end
end

function uiShowOut:onClickHuKouCancel()
    self:destroy()

    self._cardLayer:setMyCardTouchEnabled(true)
    --麻将都缩回去
    self._cardLayer:setCardStandBy()
    --胡牌提示要关闭
    self._gameLayer:setHuTipLayerVisible(false)
end

function uiShowOut:onClickHuKouOK()
    --print("uiShowOut:onClickHuKouOK 点击确定出牌扣牌=====", self._curSelectValue, self._selectIdx)
    --扣牌的时候不能打牌
    if self._mode == MODE_CHOOSE.kou_pai then
        return
    end

    local ting_list = self._gameLayer:getTingData()
    local isCanTingAfterOut = ting_list[self._curSelectValue]
    if not isCanTingAfterOut then
        return
    end


    local cardLayer = self._cardLayer
    local my_viewid = cmd.MY_VIEWID

    --先发亮倒(扣牌)消息,再发打牌消息
    --听牌,点一张显示听哪张
    --判断打这张牌是不是可以胡,可以就打
    ---------------------------------------------------------亮倒打牌结束该流程
    local cards = {}
    for value,_ in pairs(self.kou_card_values) do
        table.insert(cards, value)
    end
    --dump(cards, "#扣牌=====")

    self._cardLayer:outCardEx(cmd.MY_VIEWID, self._selectIdx)
    self._gameLayer:sendUserShowOut(cards)
    self._gameLayer:sendOutCard(self._curSelectValue)

    self._gameLayer:setHuTipLayerVisible(false)
    self:destroy()
    ---------------------------------------------------------亮倒打牌结束该流程

    self._selectCount = self._selectCount + 1
    if self._selectCount >= 2 then
        self._selectCount = 0
    end
    self._preSelectIdx = self._selectIdx
end

function uiShowOut:onClickHuKouOk()
    self:destroy()
end

function uiShowOut:chooseMode(mode)
    self._mode = mode
    if self._mode == MODE_CHOOSE.kou_pai then
        cc.find(self.widget, "sp_1"):setVisible(true)
        cc.find(self.widget, "sp_2"):setVisible(false)
    else
        cc.find(self.widget, "sp_1"):setVisible(false)
        cc.find(self.widget, "sp_2"):setVisible(true)
        local btn_ok = cc.find(self.widget, "sp_2/Button_ok")

        local ting_list = self._gameLayer:getTingData()
        local isCanTingAfterOut = ting_list[self._curSelectValue]
        self:setIsCanTing(isCanTingAfterOut and true)
    end
end

function uiShowOut:setIsCanTing(isCan)
    local btn_ok = cc.find(self.widget, "sp_2/Button_ok")
    btn_ok:setEnabled(isCan)
end

--函数功能单一,不要跟chooseMode混在一起
function uiShowOut:setTingListVisible(isVisible)
    local ting_list = self._gameLayer:getTingData()
    local cardLayer = self._cardLayer
    local handCardNode = cardLayer.nodeHandCard[my_viewid]

    local my_viewid = cmd.MY_VIEWID
    local cardData = cardLayer.cbCardData[my_viewid]

    --已经扣掉的牌不能听
    local noTingList = {}
    for cardvalue,isKou in pairs(self.kou_card_values) do
        local leftNum = 0
        for k,value in pairs(cardData) do
            if isKou and value == cardvalue then
                leftNum = leftNum + 1
            end
        end
        if leftNum <= 3 then
            noTingList[cardvalue] = true
        end
    end

    for i = 1, cardLayer.cbCardCount[my_viewid] do
        local cardvalue = cardData[i]
        if cardvalue and cardvalue>0 and ting_list[cardvalue] and not noTingList[cardvalue] then
            cardLayer:setTingTipVisible(i, isVisible)
        end
    end
end

function uiShowOut:resetAllCardPosition()
    local cardLayer = self._cardLayer
    local my_viewid = cmd.MY_VIEWID
    local handCardNode = cardLayer.nodeHandCard[my_viewid]
    local children = handCardNode:getChildren()
    for k,child in pairs(children) do
        child:setPositionY(0)
    end
end

function uiShowOut:onTouchBegan(touch, event)
    local pos = touch:getLocation()
    local cardLayer = self._cardLayer
    local my_viewid = cmd.MY_VIEWID
    local handCardNode = cardLayer.nodeHandCard[my_viewid]
    local cardData = cardLayer.cbCardData[my_viewid]
    pos = cardLayer.nodeHandCard[my_viewid]:convertToNodeSpace(pos)

    local idx,cardvalue,card
    for i = 1, cardLayer.cbCardCount[my_viewid] do
        card = handCardNode:getChildByTag(i)
        if card then
            local cardRect = card:getBoundingBox()
            cardRect.x = cardRect.x - cardRect.width*0.5
            cardRect.y = cardRect.y - cardRect.height*0.5
            if cc.rectContainsPoint(cardRect, pos) then
                idx,cardvalue = i,cardData[i]
                self._boundingBox = cardRect
                self._selectIdx = idx
                self._curSelectValue = cardvalue
                break
            end
        end
    end

    --打牌的时候,要把所有牌都放下
    if self._mode ~= MODE_CHOOSE.kou_pai then
        self:resetAllCardPosition()
    end

    if cardvalue then
        --扣牌,判断是否有三张,有三张就做扣牌显示处理
        if self._mode == MODE_CHOOSE.kou_pai then
            local canKouListData = self._gameLayer:getCanKouListData()
            -- dump(canKouListData, "#canKouListData")

            local idxList = {}
            for idx,v in pairs(cardData) do
                if cardvalue and v == cardvalue then
                    idxList[idx] = true
                end
            end

            if canKouListData[cardvalue] then
                table.merge(self.kou_card_idx, idxList)
                --防重复,另外记录一份扣掉的牌,以防止扣牌操作修改手牌牌值
                self.kou_card_values[cardvalue] = true
                for cardTag,_ in pairs(idxList) do
                    cardLayer:setCardStandUp(cardTag)
                    cardLayer:setKouTipVisible(cardTag, true)
                end
            end
        else
            -- dump(self.kou_card_values, "#self.kou_card_values==")

            --先发亮倒(扣牌)消息,再发打牌消息
            --听牌,点一张显示听哪张
            --判断这张牌有没有被扣掉,如果扣掉了,判断手牌是不是还剩余一张
            --判断打这张牌是不是可以胡,可以就打
            if self.kou_card_values[cardvalue] then
                local leftNum = 0
                for k,value in pairs(cardData) do
                    if value == cardvalue then
                        leftNum = leftNum + 1
                    end
                end
                if leftNum <= 3 then
                    print("已经扣了这张牌,而且手上没有四张,不能再打了")
                    return false
                end
            end

            local ting_list = self._gameLayer:getTingData()

            local isCanTingAfterOut = ting_list[cardvalue]
            if isCanTingAfterOut then
                ---------------------------------------------------------听牌提示
                card:setPositionY(20)
                self._gameLayer:setHuTipLayerVisible(true, cardvalue)
                ---------------------------------------------------------听牌提示

                self:setIsCanTing(true)

                -- self._cardLayer:showUserTileMajong(my_viewid, self._cardLayer.cbCardData[my_viewid]) --测试亮倒
                -- self._cardLayer:createKouPai(viewid, cards) --测试亮倒
            else
                print("错误打这张是不能胡牌的,牌值:", cardvalue)
                return false
            end
        end
    end
    return true
end

function uiShowOut:onTouchMoved(touch, event)
end

function uiShowOut:onTouchEnded(touch, event)
    --扣牌的时候不能打牌
    if self._mode == MODE_CHOOSE.kou_pai then
        return
    end

    local cardLayer = self._cardLayer
    local my_viewid = cmd.MY_VIEWID

    --先发亮倒(扣牌)消息,再发打牌消息
    --听牌,点一张显示听哪张
    --判断打这张牌是不是可以胡,可以就打

    local pos = touch:getLocation()
    pos = cardLayer.nodeHandCard[my_viewid]:convertToNodeSpace(pos)
    if self._boundingBox and cc.rectContainsPoint(self._boundingBox, pos) then
        if self._preSelectIdx == self._selectIdx then
            self._selectCount = self._selectCount + 1
        else
            self._selectCount = 1
        end

        -- local ting_list = self._gameLayer:getTingData()
        -- local isCanTingAfterOut = ting_list[self._curSelectValue]
if self._selectCount >= 2 then
        -- if self._selectCount >= 2 and isCanTingAfterOut then
            ---------------------------------------------------------亮倒打牌结束该流程
            local cards = {}
            for value,_ in pairs(self.kou_card_values) do
                table.insert(cards, value)
            end

            --print("uiShowOut:onTouchEnded=====出牌扣牌",self._selectIdx, self._curSelectValue)
           --dump(cards, "#扣牌=====")
            self._cardLayer:outCardEx(cmd.MY_VIEWID, self._selectIdx)
            self._gameLayer:sendUserShowOut(cards)
            self._gameLayer:sendOutCard(self._curSelectValue)

            self._gameLayer:setHuTipLayerVisible(false)
            self:destroy()
            ---------------------------------------------------------亮倒打牌结束该流程

            self._selectCount = 0
        end

        self._preSelectIdx = self._selectIdx
    end
end

function uiShowOut:onTouchCanceled(touch, event)
    print("onTouchCanceled")
    self._boundingBox = nil
    self._selectCount = 0
    self._selectIdx = nil
end

function uiShowOut:onExit()
    self:setTingListVisible(false)
    self._boundingBox = nil
    self._selectCount = 0
    if type(self._callback) == "function" then
        self._callback()
    end
end

return uiShowOut
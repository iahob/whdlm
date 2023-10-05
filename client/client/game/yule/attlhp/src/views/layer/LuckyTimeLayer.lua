local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.attlhp.src";
local cmd = appdf.req(module_pre..".models.CMD_Game")
local GameLogic = appdf.req(module_pre..".models.GameLogic")

local LuckyTimeLayer = class("LuckyTimeLayer", cc.Layer)

LuckyTimeLayer.Da = 1;
LuckyTimeLayer.Xiao = 2;
LuckyTimeLayer.Finish = 3;
LuckyTimeLayer.Bibei = 4;
LuckyTimeLayer.Defen = 5;

local invalidValue = 255;

--构造
function LuckyTimeLayer:ctor(_sence,betNum)
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    self._scene = _sence
    self.betNum = betNum

    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end
    local csbNode = ExternalFun.loadCSB("game/lucky/Layer.csb",self)
    self.csbNode = csbNode
    math.randomseed(os.time())


    self.guessCard = csbNode:getChildByName("guss_poker")


    local btn_da = csbNode:getChildByName("Button_da")
    btn_da:setTag(LuckyTimeLayer.Da)
    btn_da:addTouchEventListener(btnEvent)

    local btn_xiao = csbNode:getChildByName("Button_xiao")
    btn_xiao:setTag(LuckyTimeLayer.Xiao)
    btn_xiao:addTouchEventListener(btnEvent)

    local btn_finish = csbNode:getChildByName("Button_finish")
    btn_finish:setTag(LuckyTimeLayer.Finish)
    btn_finish:addTouchEventListener(btnEvent)

    local btn_defen = csbNode:getChildByName("Button_close")
    btn_defen:setTag(LuckyTimeLayer.Defen)
    btn_defen:addTouchEventListener(btnEvent)

    btn_finish:setEnabled(false)

    self.btn_da = btn_da
    self.btn_xiao = btn_xiao
    self.btn_finish = btn_finish

    if self._scene.wGuessCount and self._scene.wGuessCount > 0 then
        for i = 1, self._scene.wGuessCount do
            self.csbNode:getChildByName("star_"..i):setTexture("game/lucky/star.png")
        end
    end

    self:refreshRecord()
end

function LuckyTimeLayer:guessStart()
    local value = math.random(1,13)
    self.csbNode:runAction(cc.Sequence:create(
        cc.DelayTime:create(0.1),
        cc.CallFunc:create(function()
            self.guessCard:loadTexture("poker/0_"..value..".png")
            self:guessStart()
        end)
        ))
end

function LuckyTimeLayer:stopGuessAction(CardData)
    self.guessCard:loadTexture("poker/0_"..self.resultData..".png")
    self.csbNode:stopAllActions()
    self.btn_da:setEnabled(false)
    self.btn_xiao:setEnabled(false)

    if self._scene.bguessBig == true and  self.resultData > 7 then
        self.csbNode:getChildByName("star_"..self._scene.wGuessCount):setTexture("game/lucky/star.png")
    elseif self._scene.bguessBig == false and  self.resultData < 7 then
        self.csbNode:getChildByName("star_"..self._scene.wGuessCount):setTexture("game/lucky/star.png")
    elseif self.resultData == 7 then
        
    end
    self.guessCard:runAction(cc.Sequence:create(
        cc.DelayTime:create(5),
        cc.CallFunc:create(function()
            -- self:guessStart()
            self.btn_da:setEnabled(true)
            self.btn_xiao:setEnabled(true)
        end)
        ))
end
--按钮事件
function LuckyTimeLayer:onButtonClickedEvent(tag,ref)
    if tag == LuckyTimeLayer.Da or tag == LuckyTimeLayer.Xiao then
        self._scene.bguessBig = tag == LuckyTimeLayer.Da
        print("****************88self._scene.bguessBig",self._scene.bguessBig)
        self._scene.wGuessCount = self._scene.wGuessCount + 1
        self:guessStart()
        self.btn_da:setEnabled(false)
        self.btn_xiao:setEnabled(false)
        self.btn_finish:setEnabled(true)
    elseif tag == LuckyTimeLayer.Finish then
        self._scene:sendClickGuess()
        self.btn_finish:setEnabled(false)
    elseif tag == LuckyTimeLayer.Defen then
        self._scene:sendGameEnd(self.betNum)
        self:removeFromParent()
    end
end

function LuckyTimeLayer:getGuessResult(CardData)
    self.resultData = CardData
    table.remove(self._scene.cbGuessCardResultRecord, 1)
    table.insert(self._scene.cbGuessCardResultRecord, CardData)
    self:stopGuessAction()
    self:refreshRecord()
end

function LuckyTimeLayer:refreshRecord()
    --猜大小记录
    local record = self._scene.cbGuessCardResultRecord
    dump(record)
    local idx = 1
    for i = 6, 1, -1 do
        if record[i] > 0 and idx < 6 then
            local card = self.csbNode:getChildByName("card_"..idx)
            local cardData = record[i]
            local value = GameLogic.GetCardValue( cardData )
            local color = GameLogic.GetCardColor( cardData )
            print(color,value)
            card:loadTexture("poker/"..color.."_"..value..".png")
            idx = idx +1
        end
    end
end

--触摸回调
function LuckyTimeLayer:onTouchBegan(touch, event)
    return true
end

function LuckyTimeLayer:onTouchEnded(touch, event)
end

return LuckyTimeLayer